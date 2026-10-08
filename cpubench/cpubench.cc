#include <cstdlib>
#include <thread>
#include <iostream>
#include <atomic>

int main(){

    std::string command = "./bench";

    std::atomic<uint> active_threads = 0;
    std::atomic<uint> total_thread = std::thread::hardware_concurrency();

    for (uint i = 0;i <= total_thread;i++){
        active_threads += 1;
        std::thread([&command, &active_threads](){
            std::system(
                std::string(
                    command + " 2>/dev/null"
                ).c_str()
            );
            active_threads -= 1;
        }).detach();
        continue;
    }

}