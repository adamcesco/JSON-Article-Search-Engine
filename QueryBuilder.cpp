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

void QueryBuilder::buildQuery(std::string query) {
    // Split query into words
    std::vector<std::string> words = split(query, ' ');
    for (auto it = words.begin(); it != words.end(); ++it) {
        std::string word = *it;
        if (word == "AND") {
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

SingleWordNode::SingleWordNode(ArticleTable *table, WordTree *tree, std::string word) : QueryNode(table, tree) {
    this->word = word;
}

//TODO: Feels like something is missing
std::vector<ScoredId> SingleWordNode::execute() {
    Porter2Stemmer::stem(word);
    std::vector<ScoredId> result = this->tree->operator[](word);
    return result;
}
