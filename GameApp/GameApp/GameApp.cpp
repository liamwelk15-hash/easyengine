#include <windows.h>
#include <windowsx.h>
#include <string>
#include <vector>
#include <wininet.h>

#define HUB_CLASS "EasyEngineHub"

typedef void (*EngineInit)();
typedef void (*EngineUpdate)(float);
typedef void (*EngineRender)();
typedef void (*EngineShutdown)();
typedef const char* (*EngineVersion)();

HWND hMainWnd = NULL;
HINSTANCE hInst = NULL;
bool engineRunning = false;
HMODULE hEngineDLL = NULL;
std::string currentVersion = "1.1.0";
std::string latestVersion = "";

bool checkForUpdates();
void showUpdateNotification(HWND parent);
void runEngine();

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    static HFONT hFontTitle, hFontButton, hFontSmall;
    static HWND hTitle, hSubtitle, hBtnNew, hBtnOpen, hBtnDemo, hBtnExit;
    static HWND hVersion, hUpdateBtn;
    
    switch (msg) {
        case WM_CREATE: {
            hFontTitle = CreateFontA(48, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Segoe UI");
            hFontButton = CreateFontA(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Segoe UI");
            hFontSmall = CreateFontA(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Segoe UI");
            
            hTitle = CreateWindowA("STATIC", "EasyEngine", WS_VISIBLE | WS_CHILD | SS_CENTER,
                50, 50, 700, 60, hwnd, NULL, hInst, NULL);
            SendMessageA(hTitle, WM_SETFONT, (WPARAM)hFontTitle, TRUE);
            
            hSubtitle = CreateWindowA("STATIC", "Create Amazing 3D Games", WS_VISIBLE | WS_CHILD | SS_CENTER,
                50, 110, 700, 30, hwnd, NULL, hInst, NULL);
            SendMessageA(hSubtitle, WM_SETFONT, (WPARAM)hFontButton, TRUE);
            
            int btnY = 190;
            int btnGap = 55;
            
            hBtnNew = CreateWindowA("BUTTON", "+ New Project", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                200, btnY, 400, 50, hwnd, (HMENU)1, hInst, NULL);
            SendMessageA(hBtnNew, WM_SETFONT, (WPARAM)hFontButton, TRUE);
            
            hBtnOpen = CreateWindowA("BUTTON", "Open Project", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                200, btnY + btnGap, 400, 50, hwnd, (HMENU)2, hInst, NULL);
            SendMessageA(hBtnOpen, WM_SETFONT, (WPARAM)hFontButton, TRUE);
            
            hBtnDemo = CreateWindowA("BUTTON", "Start Demo Scene", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                200, btnY + btnGap*2, 400, 50, hwnd, (HMENU)3, hInst, NULL);
            SendMessageA(hBtnDemo, WM_SETFONT, (WPARAM)hFontButton, TRUE);
            
            hBtnExit = CreateWindowA("BUTTON", "Exit", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                200, btnY + btnGap*3, 400, 50, hwnd, (HMENU)5, hInst, NULL);
            SendMessageA(hBtnExit, WM_SETFONT, (WPARAM)hFontButton, TRUE);
            
            hUpdateBtn = CreateWindowA("BUTTON", "Check for Updates", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                200, btnY + btnGap*4 + 20, 400, 40, hwnd, (HMENU)6, hInst, NULL);
            SendMessageA(hUpdateBtn, WM_SETFONT, (WPARAM)hFontSmall, TRUE);
            
            std::string versionText = "Version " + currentVersion;
            hVersion = CreateWindowA("STATIC", versionText.c_str(), WS_VISIBLE | WS_CHILD | SS_CENTER,
                50, 530, 700, 25, hwnd, NULL, hInst, NULL);
            SendMessageA(hVersion, WM_SETFONT, (WPARAM)hFontSmall, TRUE);
            
            break;
        }
        
        case WM_COMMAND: {
            int id = LOWORD(wparam);
            if (id == 1) {
                MessageBoxA(hwnd, "New Project - Coming soon!", "EasyEngine", MB_OK);
            } else if (id == 2) {
                MessageBoxA(hwnd, "Open Project - Coming soon!", "EasyEngine", MB_OK);
            } else if (id == 3) {
                ShowWindow(hwnd, SW_MINIMIZE);
                runEngine();
            } else if (id == 5) {
                PostQuitMessage(0);
            } else if (id == 6) {
                if (checkForUpdates()) {
                    showUpdateNotification(hwnd);
                } else {
                    MessageBoxA(hwnd, "Du hast bereits die neueste Version!", "EasyEngine", MB_OK | MB_ICONINFORMATION);
                }
            }
            break;
        }
        
        case WM_ERASEBKGND: {
            HDC hdc = (HDC)wparam;
            RECT rect;
            GetClientRect(hwnd, &rect);
            HBRUSH brush = CreateSolidBrush(RGB(26, 26, 37));
            FillRect(hdc, &rect, brush);
            DeleteObject(brush);
            return 1;
        }
        
        case WM_DESTROY: {
            if (hFontTitle) DeleteObject(hFontTitle);
            if (hFontButton) DeleteObject(hFontButton);
            if (hFontSmall) DeleteObject(hFontSmall);
            PostQuitMessage(0);
            break;
        }
        
        default:
            return DefWindowProcA(hwnd, msg, wparam, lparam);
    }
    return 0;
}

bool checkForUpdates(){
    latestVersion = "1.1.0";
    return latestVersion > currentVersion;
}

void showUpdateNotification(HWND parent){
    std::string msg = "EasyEngine " + latestVersion + " ist verfuegbar!\n\n";
    msg += "Aktuelle Version: " + currentVersion + "\n";
    msg += "Neue Version: " + latestVersion + "\n\n";
    msg += "Besuche unsere Website fuer das Update.";
    
    int result = MessageBoxA(parent, msg.c_str(), "Update verfuegbar!", MB_YESNO | MB_ICONINFORMATION);
    if(result == IDYES){
        ShellExecuteA(NULL, "open", "https://github.com/easyengine/engine", NULL, NULL, SW_SHOWNORMAL);
    }
}

void runEngine() {
    if (engineRunning) return;
    
    hEngineDLL = LoadLibraryA("C:\\Users\\welkl\\Desktop\\engine\\project\\Engine\\x64\\Debug\\Engine.dll");
    if (!hEngineDLL) {
        MessageBoxA(NULL, "Engine.dll not found!", "Error", MB_OK | MB_ICONERROR);
        ShowWindow(hMainWnd, SW_RESTORE);
        return;
    }
    
    auto init = (EngineInit)GetProcAddress(hEngineDLL, "Engine_Init");
    auto update = (EngineUpdate)GetProcAddress(hEngineDLL, "Engine_Update");
    auto render = (EngineRender)GetProcAddress(hEngineDLL, "Engine_Render");
    auto shutdown = (EngineShutdown)GetProcAddress(hEngineDLL, "Engine_Shutdown");
    
    if (!init || !update || !render || !shutdown) {
        MessageBoxA(NULL, "Engine functions not found!", "Error", MB_OK | MB_ICONERROR);
        FreeLibrary(hEngineDLL);
        hEngineDLL = NULL;
        ShowWindow(hMainWnd, SW_RESTORE);
        return;
    }
    
    init();
    engineRunning = true;
    
    MSG msg;
    float lastTime = GetTickCount() / 1000.0f;
    
    while (engineRunning) {
        while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                engineRunning = false;
                break;
            }
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }
        
        if (!engineRunning) break;
        
        float currentTime = GetTickCount() / 1000.0f;
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        
        if (deltaTime > 0.1f) deltaTime = 0.1f;
        
        update(deltaTime);
        render();
        
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            break;
        }
    }
    
    shutdown();
    FreeLibrary(hEngineDLL);
    hEngineDLL = NULL;
    engineRunning = false;
    
    ShowWindow(hMainWnd, SW_RESTORE);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    hInst = hInstance;
    
    WNDCLASSA wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = HUB_CLASS;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = CreateSolidBrush(RGB(26, 26, 37));
    
    RegisterClassA(&wc);
    
    hMainWnd = CreateWindowA(HUB_CLASS, "EasyEngine Hub", 
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL, NULL, hInst, NULL);
    
    ShowWindow(hMainWnd, SW_SHOW);
    UpdateWindow(hMainWnd);
    
    MSG msg;
    while (GetMessageA(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
    
    return 0;
}
