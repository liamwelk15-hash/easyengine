#include <windows.h>
#include <iostream>
#include <conio.h>

typedef void (*EngineInit)();
typedef void (*EngineUpdate)(float);
typedef void (*EngineRender)();
typedef void (*EngineShutdown)();
typedef const char* (*EngineVersion)();

int main() {
    std::cout << "=== EasyEngine Test ===" << std::endl;
    
    HMODULE hDLL = LoadLibraryA("C:\\Users\\welkl\\Desktop\\engine\\engine\\dll\\Engine.dll");
    if (!hDLL) {
        std::cout << "FEHLER: Engine.dll nicht gefunden!" << std::endl;
        _getch();
        return 1;
    }
    
    std::cout << "Engine.dll geladen!" << std::endl;
    
    EngineInit init = (EngineInit)GetProcAddress(hDLL, "Engine_Init");
    EngineUpdate update = (EngineUpdate)GetProcAddress(hDLL, "Engine_Update");
    EngineRender render = (EngineRender)GetProcAddress(hDLL, "Engine_Render");
    EngineShutdown shutdown = (EngineShutdown)GetProcAddress(hDLL, "Engine_Shutdown");
    EngineVersion version = (EngineVersion)GetProcAddress(hDLL, "Engine_Version");
    
    if (!init || !update || !render || !shutdown || !version) {
        std::cout << "FEHLER: Engine Funktionen nicht gefunden!" << std::endl;
        FreeLibrary(hDLL);
        _getch();
        return 1;
    }
    
    std::cout << "Engine Version: " << version() << std::endl;
    init();
    std::cout << "Engine gestartet!" << std::endl;
    
    std::cout << "Druecke eine Taste zum Beenden..." << std::endl;
    _getch();
    
    shutdown();
    std::cout << "Engine gestoppt!" << std::endl;
    
    FreeLibrary(hDLL);
    std::cout << "Fertig!" << std::endl;
    return 0;
}
