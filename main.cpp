#include <iostream>
#include <fstream>
#include <queue>
#include <filesystem>
#include <dirent.h>
#include <mutex>
#include <thread>
#include "include/rapidjson/document.h"
#include <chrono>
#include <sstream>

std::mutex queue_mutex;
std::queue<std::string> filenames;

void processFiles(std::string path) {
    while(!filenames.empty()) {
        queue_mutex.lock();
        std::string filename = filenames.front();
        filenames.pop();
        queue_mutex.unlock();

        std::ifstream file(path + filename);
        if (!file.is_open()) {
            std::cout << "Could not open file: " << filename << std::endl;
            continue;
        }
        rapidjson::Document document;
        try {

            std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            document.Parse(content.c_str());
        } catch (std::exception& e) {
            std::cout << "Could not read file: " << filename << std::endl;
            continue;
        }
        file.close();


        assert(document.IsObject());
        assert(document.HasMember("title"));
        assert(document["title"].IsString());
        assert(document["text"].IsString());

        // loop through every word in text
        std::string text = document["text"].GetString();
        std::istringstream iss(text);

        // Iterate the istringstream
        // using do-while loop
        do {
            std::string subs;

            // Get the word from the istringstream
            iss >> subs;

            // Print the word fetched
            // from the istringstream
            if (subs[0] > 96 && subs[0] < 123) {
            }

        } while (iss);
    }
}

int main() {
    std::string path = "../data/articles/2018_01_112b52537b67659ad3609a234388c50a/";

    if (auto dir = opendir(path.c_str())) {
        while (auto f = readdir(dir)) {
            // if begins with dots, skip
            if (f->d_name[0] == '.') {
                continue;
            }
            filenames.push(f->d_name);
        }
        closedir(dir);
    }


    std::thread t1(processFiles, path);

    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;

    auto timer1 = high_resolution_clock::now();
    t1.join();
    auto timer2 = high_resolution_clock::now();

    /* Getting number of milliseconds as an integer. */
    auto ms_int = duration_cast<milliseconds>(timer2 - timer1);

    /* Getting number of milliseconds as a double. */
    duration<double, std::milli> ms_double = timer2 - timer1;

    std::cout << ms_int.count() << "ms\n";



    return 0;
}
