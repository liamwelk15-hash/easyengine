#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sys/stat.h>

#ifdef _WIN32
#include <windows.h>
#endif

namespace EasyEngineHub {

struct Project {
    std::string name;
    std::string path;
    std::string createdDate;
};

bool folderExists(const std::string& path) {
    struct stat info;
    return (stat(path.c_str(), &info) == 0 && (info.st_mode & _S_IFDIR));
}

void createFolder(const std::string& path) {
    #ifdef _WIN32
    CreateDirectoryA(path.c_str(), NULL);
    #else
    mkdir(path.c_str(), 0777);
    #endif
}

class Hub {
public:
    static Hub& get() { static Hub instance; return instance; }
    
    void run() {
        showBanner();
        while (running) {
            showMainMenu();
            int choice = getChoice();
            handleChoice(choice);
        }
    }
    
private:
    void showBanner() {
        std::cout << "===========================================\n";
        std::cout << "      EasyEngine Hub v1.0\n";
        std::cout << "   Your Unity-like Game Engine\n";
        std::cout << "===========================================\n";
        std::cout << "\n";
    }
    
    void showMainMenu() {
        std::cout << "--- Main Menu ---\n";
        std::cout << "1. Create New Project\n";
        std::cout << "2. Open Existing Project\n";
        std::cout << "3. List All Projects\n";
        std::cout << "4. Settings\n";
        std::cout << "5. Exit\n";
        std::cout << "\nSelect: ";
    }
    
    int getChoice() {
        int choice;
        std::cin >> choice;
        return choice;
    }
    
    void handleChoice(int choice) {
        switch (choice) {
            case 1: createProject(); break;
            case 2: openProject(); break;
            case 3: listProjects(); break;
            case 4: showSettings(); break;
            case 5: running = false; break;
            default: std::cout << "Invalid choice!\n";
        }
    }
    
    void createProject() {
        std::cout << "\n--- Create New Project ---\n";
        std::cout << "Project name: ";
        std::string name;
        std::cin >> name;
        
        std::cout << "Project location: ";
        std::string location;
        std::cin >> location;
        
        if (location.empty()) {
            location = "./projects/" + name;
        }
        
        createFolder(location);
        createFolder(location + "/scripts");
        createFolder(location + "/assets");
        createFolder(location + "/scenes");
        
        std::ofstream config(location + "/project.eep");
        config << "name=" << name << "\n";
        config << "version=1.0\n";
        config << "created=2026-02-25\n";
        config.close();
        
        std::cout << "Project '" << name << "' created at " << location << "\n\n";
    }
    
    void openProject() {
        std::cout << "\n--- Open Project ---\n";
        std::cout << "Enter project path: ";
        std::string path;
        std::cin >> path;
        
        if (folderExists(path)) {
            std::cout << "Opening project...\n";
        } else {
            std::cout << "Project not found!\n";
        }
    }
    
    void listProjects() {
        std::cout << "\n--- Projects ---\n";
        std::cout << "No projects found.\n\n";
    }
    
    void showSettings() {
        std::cout << "\n--- Settings ---\n";
        std::cout << "1. Set Default Project Location\n";
        std::cout << "2. Set Engine Path\n";
        std::cout << "3. Back\n\n";
    }
    
    bool running = true;
};

} // namespace

int main() {
    EasyEngineHub::Hub::get().run();
    return 0;
}
