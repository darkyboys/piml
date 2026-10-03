#include <chrono>
#include <cstdlib>
#include <iostream>
#include <filesystem>
#include <atomic>
#include <thread>

std::atomic <int> active_threads = 0;
std::atomic <int> total_thread_count = std::thread::hardware_concurrency();

void scan_and_build_object(const std::string& name, const std::string& compiler);
int main(){

    std::cout << "Building the sub-libraries.\n";
    
    std::string compiler = "g++";
    std::system("rm -rf objects && mkdir objects");

    scan_and_build_object("src", compiler);
    scan_and_build_object("test", compiler);
}


// Implementation junk goes here!
void scan_and_build_object(const std::string& name, const std::string& compiler){
    
    std::string n = name;
    if (n.rfind('/') != std::string::npos)
        n = n.substr(n.rfind('/') + 1);

    std::cout<<"\n\n\n";
    std::cout << "\033[36mBuilding \033[93m" << n << "\033[36m with compiler \033[93m"<<compiler<<"\033[0m\n";
    std::cout << "\n";

    for (auto& file : std::filesystem::directory_iterator(name)){
        if (file.is_regular_file()){
            std::cout << "\033[32mBuilding Object For: \033[93m"<<file.path()<<"\033[0m\n";
            std::string command = compiler + " ";
            command += file.path();
            command += " -c -O3 -o objects/";
            if (file.path().string().rfind('/') != std::string::npos)
                command += file.path().string().substr(file.path().string().rfind('/') + 1);
            else command += file.path();

            if (command.rfind(".cc") == std::string::npos){
               continue; 
            }
            
            command = command.substr(0, command.rfind(".cc"));
            command += ".o";

            std::cout << "    \033[95m" << command << "\033[0m\n";
            // std::system(command.c_str());
            active_threads += 1;
            std::thread([command](){

                std::system(command.c_str());
                active_threads -= 1;

            }).detach();

            while (active_threads >= total_thread_count){
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
            
        }
        else if (file.is_directory()){
            scan_and_build_object(file.path().string(), compiler);
        }
    }

    while (active_threads != 0){
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}