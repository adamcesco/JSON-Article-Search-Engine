//
// Created by dshar on 4/27/2022.
//

#ifndef INC_22S_FINAL_PROJ_QUERYBUILDER_H
#define INC_22S_FINAL_PROJ_QUERYBUILDER_H


#include "typedefs.h"

class QueryNode {

protected:
    WordTree *tree = nullptr;
    ArticleTable *table = nullptr;
    std::vector<QueryNode*> children;
public:
    QueryNode(ArticleTable *table, WordTree *tree);
    void addChild(QueryNode *child);

    virtual std::vector<ScoredId> execute() = 0;
    ~QueryNode();
};

class SingleWordNode : public QueryNode {
private:
    std::string word;

public:
    SingleWordNode(ArticleTable *table, WordTree *tree, std::string word);
    std::vector<ScoredId> execute() override;
};

class AndNode : public QueryNode {
public:
    AndNode(ArticleTable *table, WordTree *tree) : QueryNode(table, tree) {};
    std::vector<ScoredId> execute() override;
};

class OrNode : public QueryNode {
public:
    OrNode(ArticleTable *table, WordTree *tree) : QueryNode(table, tree) {};
    std::vector<ScoredId> execute() override;
};

class NotNode : public QueryNode {
private:
    std::vector<std::string> words;
public:
    NotNode(ArticleTable *table, WordTree *tree, std::vector<std::string>words) : QueryNode(table, tree) {
        this->words = words;
    };
    std::vector<ScoredId> execute() override;
};

class OrgNode : public QueryNode {
private:
    std::vector<std::string> orgs;
public:
    OrgNode(ArticleTable *table, WordTree *tree, std::vector<std::string>orgs) : QueryNode(table, tree) {
        this->orgs = orgs;
    };
    std::vector<ScoredId> execute() override;
};

class QueryBuilder {
private:
    ArticleTable *articleTable = nullptr;
    WordTree *wordTree = nullptr;
    QueryNode* root = nullptr;

public:
    QueryBuilder(ArticleTable *articleTable, WordTree *wordTree);
    void buildQuery(std::string query);
    std::vector<Article> executeQuery();

    std::vector<std::string> split(std::string basicString, char i);
};


#endif //INC_22S_FINAL_PROJ_QUERYBUILDER_H
