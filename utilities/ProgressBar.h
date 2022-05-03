//
// Created by Adam Escobedo on 4/30/2022.
//

#ifndef INC_22S_FINAL_PROJ_PROGRESSBAR_H
#define INC_22S_FINAL_PROJ_PROGRESSBAR_H

#include <iomanip>
#include <iostream>
#include <future>

template<class T>
struct ProgressBar {
    void printProgressBar(double progress) {
        int barWidth = 70;
        std::cout << "[";
        int pos = barWidth * progress;
        for (int i = 0; i < barWidth; ++i) {
            if (i < pos) {
                std::cout << "=";
            } else if (i == pos) {
                std::cout << ">";
            } else {
                std::cout << " ";
            }
        }
        std::cout << "] " << std::setprecision(3) << std::fixed << double(progress * 100.0) << " %\r";
        std::cout.flush();
    }

    template<class ...Args>
    void initiate(Args ...args) {
        std::future<void> fut = std::async(std::launch::async, args...);

        // Poll the progress of the invoker every 400 milliseconds
        while (fut.wait_for(std::chrono::milliseconds(40)) != std::future_status::ready) {
            double progress = getProgress(invoker);
            if (progress > 0) {
                printProgressBar(progress);
            }
        }
        printProgressBar(1);
        std::cout << std::endl;
    }

    T *invoker = nullptr;

    double (*getProgress)(T *) = nullptr;
};

#endif //INC_22S_FINAL_PROJ_PROGRESSBAR_H
