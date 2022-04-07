#include <iostream>
#include <fstream>
#include "include/rapidjson/document.h"

int main() {
    std::string path = "../data/articles/2018_01_112b52537b67659ad3609a234388c50a/news_0037810.json";

    std::ifstream ifs(path);
    if (!ifs.is_open()) {
        std::cout << "open file failed" << std::endl;
        return -1;
    }
    std::string json((std::istreambuf_iterator<char>(ifs)),
                     (std::istreambuf_iterator<char>()));
    rapidjson::Document document;

    document.Parse(json.c_str());

    assert(document.IsObject());
    assert(document["title"].IsString());
    assert(document["uuid"].IsString());

    std::cout << "Title: " << document["title"].GetString() << std::endl;
    std::cout << "UUID: " << document["uuid"].GetString() << std::endl;



    return 0;
}
