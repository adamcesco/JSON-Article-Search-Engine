//
// Created by dshar on 4/27/2022.
//

#include "QueryBuilder.h"
#include "./utils.h"

QueryBuilder::QueryBuilder(ArticleTable *articleTable, WordTree *wordTree) {
    this->articleTable = articleTable;
    this->wordTree = wordTree;
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
            this->root = new OrNode(this->articleTable, this->wordTree);
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

            // Special "Head nodes"
        } else if (word == "NOT") {
            // replace the root with a not node and add the old root as a child
            std::vector<std::string> wordsToAdd;
            it++;
            if (it == words.end()) {
                return;
            }
            while (!wordIsSpecial(*it)) {
                wordsToAdd.push_back(*it);
                // check if at end of vector
                it++;
                if (it == words.end()) {
                    break;
                }
            }

            QueryNode *oldRoot = this->root;
            this->root = new NotNode(this->articleTable, this->wordTree, wordsToAdd);
            this->root->addChild(oldRoot);
        } else if (word == "ORG") {
            std::vector<std::string> wordsToAdd;
            it++;
            if (it == words.end()) {
                return;
            }
            while (!wordIsSpecial(*it)) {
                wordsToAdd.push_back(*it);
                // check if at end of vector
                it++;
                if (it == words.end()) {
                    break;
                }
            }

            QueryNode *oldRoot = this->root;
            this->root = new OrgNode(this->articleTable, this->wordTree, wordsToAdd);
            this->root->addChild(oldRoot);

        } else if (word == "PERSON") {
            std::string person;
            it++;
            person = *it;

            QueryNode *oldRoot = this->root;
            this->root = new PeopleNode(this->articleTable, this->wordTree, person);
            this->root->addChild(oldRoot);
            it++;
        } else {
            this->root = new SingleWordNode(this->articleTable, this->wordTree, word);
            it++;
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

std::vector<Article> QueryBuilder::executeQuery() {
    if (this->root == nullptr) {
        return {};
    }
    std::vector<ScoredId> result = this->root->execute();
    // Compress the result by adding the scores
    std::vector<ScoredId> compressedResult;
    for (ScoredId &scored: result) {
        // Search compressedResult for the uuid
        // If found : add the score to the existing score
        // If not found : add the uuid and score to the compressed result
        bool found = false;
        for (ScoredId &compressed: compressedResult) {
            if (compressed.first.compare(scored.first) == 0) {
                compressed.second += scored.second;
                found = true;
                break;
            }
        }
        if (!found) {
            compressedResult.push_back(scored);
        }
    }

    // Sort the result by score
    std::sort(compressedResult.begin(), compressedResult.end(), [](ScoredId &a, ScoredId &b) {
        return a.second > b.second;
    });

    // convert the result to a vector of articles
    std::vector<Article> articles;
    for (ScoredId &scored: compressedResult) {
        articles.push_back(this->articleTable->operator[](scored.first));
    }
    return articles;
}

QueryNode::QueryNode(ArticleTable *table, WordTree *tree) {
    this->table = table;
    this->tree = tree;
}

QueryNode::~QueryNode() {
    for (auto &it: this->children) {
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
    Porter2Stemmer::stem(word);
    std::transform(word.begin(), word.end(), word.begin(), ::tolower);
    std::vector<ScoredId> result;
    try {
        result = this->tree->operator[](word);
    } catch (std::exception &e) {
        std::cout << "Word not found in tree" << std::endl;
    }
    return result;
}

std::vector<ScoredId> AndNode::execute() {
    // execute children and merge results if they are shared
    std::vector<ScoredId> result;
    for (auto &it: this->children) {
        std::vector<ScoredId> childResult = it->execute();
        result.insert(result.end(), childResult.begin(), childResult.end());
    }

    int requiredTimes = this->children.size();
    std::vector<ScoredId> finalResult;

    for (ScoredId &scored: result) {
        int times = 0;
        for (ScoredId &scored2: result) {
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

std::vector<ScoredId> OrNode::execute() {
    // Execute children and merge results
    std::vector<ScoredId> result;
    for (auto &it: this->children) {
        std::vector<ScoredId> childResult = it->execute();
        result.insert(result.end(), childResult.begin(), childResult.end());
    }
    return result;
}

std::vector<ScoredId> NotNode::execute() {
    // Gets result of children
    if (this->children.size() != 1) {
        std::cout << "NotNode only supports one child" << std::endl;
        return {};
    }
    std::vector<ScoredId> result = this->children[0]->execute();
    for (auto &word: this->words) {
        Porter2Stemmer::stem(word);
        std::transform(word.begin(), word.end(), word.begin(), ::tolower);
        std::vector<ScoredId> toRemove;
        try {
            toRemove = this->tree->operator[](word);
        } catch (std::exception &e) {
            std::cout << "Word not found in tree" << std::endl;
        }
        for (ScoredId &scored: result) {
            for (ScoredId &scored2: toRemove) {
                if (scored.first == scored2.first) {
                    result.erase(std::remove(result.begin(), result.end(), scored), result.end());
                }
            }
        }
    }
    return result;
}

std::vector<ScoredId> OrgNode::execute() {
    std::string orgToSearch = this->orgs[0];
    orgToSearch = cleanPropnoun(orgToSearch);
    std::vector<ScoredId> result = this->children[0]->execute();

    std::vector<ScoredId> passed;

    for (ScoredId &scored: result) {
        Article article = this->table->operator[](scored.first);
        // if article.orglist contains an org from  this->org
        if (std::find(article.orgList.begin(), article.orgList.end(), orgToSearch) != article.orgList.end()) {
            passed.push_back(scored);
        }
    }

    return passed;

}

std::vector<ScoredId> PeopleNode::execute() {
    std::string person = this->person;
    person = cleanPropnoun(person);
    std::vector<ScoredId> result = this->children[0]->execute();

    std::vector<ScoredId> passed;

    for (ScoredId &scored: result) {
        Article article = this->table->operator[](scored.first);
        if (article.author == person) {
            passed.push_back(scored);
        }
    }

    return passed;
}
