# EasyEngine

Ein Unity-ähnliches 3D Game Engine in C++

## Version

**1.1.0**

## Features

- 3D Wireframe Rendering
- Kamera Steuerung (WASD, QE, Pfeile, Space, Shift)
- Objekt Auswahl und Bewegung
- Plugin System
- GUI Hub (grafisches Menü)
- Update Benachrichtigungen

## Installation

1. **GameApp.exe** starten
2. Auf "Start Demo Scene" klicken

## Steuerung

### Kamera
- **WASD** - Bewegen
- **Q/E** - Drehen links/rechts
- **Pfeiltasten** - Schauen
- **Space** - Hoch
- **Shift** - Runter

### Objekte
- **F1-F5** - Objekt auswählen
- **R + WASD** - Ausgewähltes Objekt bewegen

### Sonstiges
- **ESC** - Zurück zum Hub

## Ordner Struktur

```
engine/
├── dll/
│   └── EasyEngine.h        # Engine Header (DLL)
├── source/
│   ├── EngineCore.cpp      # Engine Core
│   ├── Rendering.cpp       # Rendering System
│   └── Physics.cpp         # Physics System
└── include/
    └── *.h                # Header Dateien

plugins/
├── PluginSystem.h          # Plugin Manager
├── example/                # Beispiel Plugin
└── test_plugin/            # Test Plugin

GameApp/
├── GameApp.cpp            # Hub + Engine Launcher
└── x64/Debug/GameApp.exe # Kompiliertes Programm
```

## Plugin erstellen

1. Kopiere `plugins/test_plugin` und benenne es um
2. Ändere den Namespace in allen Dateien
3. Füge deine Klassen in `.h` hinzu
4. Implementiere die Logik in `start.cpp`
5. Compiliere als DLL

### Plugin API

```cpp
extern "C" {
    __declspec(dllexport) void Plugin_Init();      // Start
    __declspec(dllexport) void Plugin_Update(float); // Frame
    __declspec(dllexport) void Plugin_Shutdown();   // Ende
    __declspec(dllexport) const char* Plugin_GetName();
    __declspec(dllexport) const char* Plugin_GetVersion();
}
```

## Build

### Engine DLL
```
Visual Studio -> Engine.vcxproj -> Build
```

### GameApp (Hub)
```
Visual Studio -> GameApp.vcxproj -> Build
```

## Lizenz

MIT License - Siehe LICENSE Datei

---

Made with ❤️
