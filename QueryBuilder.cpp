//
// Created by dshar on 4/27/2022.
//

#include "QueryBuilder.h"
#include "include/porter2_stemmer/porter2_stemmer.h"

QueryBuilder::QueryBuilder(ArticleTable *articleTable, WordTree *wordTree, std::mutex *pMutex) {
    this->articleTable = articleTable;
    this->wordTree = wordTree;
    this->treeMutex = pMutex;
}

bool wordIsSpecial(std::string word) {
    if (word == "AND" || word == "OR" || word == "NOT" || word == "ORG" || word == "PERSON") {
        return true;
    }
    return false;
}

void QueryBuilder::buildQuery(std::string query) {
    // Split query into words
    std::vector<std::string> words = split(query, ' ');
    auto it = words.begin();
    while (it != words.end()) {
        std::string word = *it;
        if (word == "AND") {
            this->root = new AndNode(this->articleTable, this->wordTree);
            it++;
            if (it == words.end()) {
                return;
            }
            while (!wordIsSpecial(*it)) {
                this->root->addChild(new SingleWordNode(this->articleTable, this->wordTree, *it));
                it++;
                if (it == words.end()) {
                    return;
                }
            }
        } else if (word == "OR") {
        } else if (word == "NOT") {
        } else if (word == "ORG"){
        } else if (word == "PERSON") {
        } else {
            this->root = new SingleWordNode(this->articleTable, this->wordTree,  word);
        }

    }


}

std::vector<std::string> QueryBuilder::split(std::string basicString, char i) {
    std::vector<std::string> result;
    std::stringstream ss(basicString);
    std::string item;
    while (std::getline(ss, item, i)) {
        result.push_back(item);
    }
    return result;
}

std::vector<ScoredId> QueryBuilder::executeQuery() {
    if (this->root == nullptr) {
        return {};
    }
    std::vector<ScoredId> result = this->root->execute();
    for (ScoredId& scored : result) {
        std::cout << scored.first << " " << scored.second << std::endl;
    }
    return result;
}

QueryNode::QueryNode(ArticleTable *table, WordTree *tree) {
    this->table = table;
    this->tree = tree;
}

QueryNode::~QueryNode() {
    for (auto & it : this->children) {
        delete it;
    }
}

void QueryNode::addChild(QueryNode *child) {
    this->children.push_back(child);
}

SingleWordNode::SingleWordNode(ArticleTable *table, WordTree *tree, std::string word) : QueryNode(table, tree) {
    this->word = word;
}

//TODO: Feels like something is missing
std::vector<ScoredId> SingleWordNode::execute() {
    std::cout << "Executing single word node: " << this->word << std::endl;
    Porter2Stemmer::stem(word);
    std::transform(word.begin(), word.end(), word.begin(), ::tolower);
    std::vector<ScoredId> result = this->tree->operator[](word);
    return result;
}

std::vector<ScoredId> AndNode::execute() {
    std::cout << "Executing AND" << std::endl;
    // execute children and merge results if they are shared
    std::vector<ScoredId> result;
    for (auto & it : this->children) {
        std::vector<ScoredId> childResult = it->execute();
        result.insert(result.end(), childResult.begin(), childResult.end());
    }

    int requiredTimes = this->children.size();
    std::vector<ScoredId> finalResult;

    for (ScoredId & scored : result) {
        int times = 0;
        for (ScoredId & scored2 : result) {
            if (scored.first == scored2.first) {
                times++;
            }
        }
        if (times >= requiredTimes) {
            finalResult.push_back(scored);
        }
    }
    return finalResult;
}
