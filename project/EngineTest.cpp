// EngineTest.cpp - Testprogramm für die Engine
#include <windows.h>
#include <iostream>
#include <conio.h>

// Engine Funktionen laden
typedef void (*EngineInit)();
typedef void (*EngineUpdate)(float);
typedef void (*EngineRender)();
typedef void (*EngineShutdown)();
typedef const char* (*EngineVersion)();

int main() {
    std::cout << "=== EasyEngine Test ===" << std::endl;
    
    // DLL laden
    HMODULE hDLL = LoadLibraryA("engine\\dll\\Engine.dll");
    if (!hDLL) {
        std::cout << "FEHLER: Engine.dll nicht gefunden!" << std::endl;
        std::cout << "Druecke eine Taste..." << std::endl;
        _getch();
        return 1;
    }
    
    std::cout << "Engine.dll geladen!" << std::endl;
    
    // Funktionen holen
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
    
    // Engine starten
    init();
    std::cout << "Engine gestartet!" << std::endl;
    
    // Game Loop
    std::cout << "Druecke eine Taste zum Beenden..." << std::endl;
    _getch();
    
    // Engine stoppen
    shutdown();
    std::cout << "Engine gestoppt!" << std::endl;
    
    // DLL entladen
    FreeLibrary(hDLL);
    
    std::cout << "Fertig!" << std::endl;
    return 0;
}
