//
// Created by dshar on 4/27/2022.
//

#include "QueryBuilder.h"

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
    return this->root->execute();
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

std::vector<ScoredId> SingleWordNode::execute() {
    return std::vector<ScoredId>();
}
