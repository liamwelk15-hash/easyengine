#pragma once
#define ENGINE_API __declspec(dllexport)
#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <memory>

namespace Engine {

struct Vec2 { float x, y; Vec2():x(0),y(0){} Vec2(float a,float b):x(a),y(b){} };
struct Vec3 { float x, y, z; Vec3():x(0),y(0),z(0){} Vec3(float a,float b,float c=0):x(a),y(b),z(c){} };
struct Color { float r, g, b, a; Color(float R=1,float G=1,float B=1,float A=1):r(R),g(G),b(B),a(A){} 
    static Color white(){return Color(1,1,1,1);}
    static Color red(){return Color(1,0,0,1);}
    static Color green(){return Color(0,1,0,1);}
    static Color blue(){return Color(0,0,1,1);}
    static Color yellow(){return Color(1,1,0,1);}
    static Color orange(){return Color(1,0.5f,0,1);}
};

struct Matrix4 {
    float m[16];
    static Matrix4 identity() { Matrix4 r; for(int i=0;i<16;i++)r.m[i]=0; r.m[0]=r.m[5]=r.m[10]=r.m[15]=1; return r; }
    static Matrix4 perspective(float fov, float aspect, float nearPlane, float farPlane) { 
        Matrix4 r = identity(); 
        float f = 1.0f / tan(fov * 0.5f); 
        r.m[0] = f / aspect; 
        r.m[5] = f; 
        r.m[10] = (farPlane + nearPlane) / (nearPlane - farPlane); 
        r.m[11] = -1.0f; 
        r.m[14] = (2.0f * farPlane * nearPlane) / (nearPlane - farPlane); 
        return r; 
    }
    static Matrix4 translation(float x, float y, float z) { Matrix4 r = identity(); r.m[12]=x; r.m[13]=y; r.m[14]=z; return r; }
    static Matrix4 scale(float x, float y, float z) { Matrix4 r = identity(); r.m[0]=x; r.m[5]=y; r.m[10]=z; return r; }
    static Matrix4 rotationX(float a) { Matrix4 r = identity(); float c=cos(a),s=sin(a); r.m[5]=c; r.m[6]=s; r.m[9]=-s; r.m[10]=c; return r; }
    static Matrix4 rotationY(float a) { Matrix4 r = identity(); float c=cos(a),s=sin(a); r.m[0]=c; r.m[2]=-s; r.m[8]=s; r.m[10]=c; return r; }
    static Matrix4 rotationZ(float a) { Matrix4 r = identity(); float c=cos(a),s=sin(a); r.m[0]=c; r.m[1]=s; r.m[4]=-s; r.m[5]=c; return r; }
    Vec3 transformPoint(const Vec3& p) const { 
        return Vec3(
            m[0]*p.x + m[4]*p.y + m[8]*p.z + m[12],
            m[1]*p.x + m[5]*p.y + m[9]*p.z + m[13],
            m[2]*p.x + m[6]*p.y + m[10]*p.z + m[14]
        ); 
    }
    Matrix4 operator*(const Matrix4& o) const { 
        Matrix4 r; 
        for(int i=0;i<4;i++) for(int j=0;j<4;j++){ 
            r.m[i*4+j]=0; 
            for(int k=0;k<4;k++) r.m[i*4+j] += m[k*4+j] * o.m[i*4+k]; 
        } 
        return r; 
    }
};

class Input {
public:
    enum Key { W='W', A='A', S='S', D='D', Q='Q', E='E', SPACE=VK_SPACE, ESCAPE=VK_ESCAPE, UP=VK_UP, DOWN=VK_DOWN, LEFT=VK_LEFT, RIGHT=VK_RIGHT, SHIFT=VK_SHIFT, F1=VK_F1, F2=VK_F2, F3=VK_F3, F4=VK_F4, F5=VK_F5, R='R' };
    static bool getKey(Key k){ return currentKeys[k]; }
    static bool getKeyDown(Key k){ return currentKeys[k] && !previousKeys[k]; }
    static void setKey(Key k, bool pressed){ previousKeys[k] = currentKeys[k]; currentKeys[k] = pressed; }
    static void update(){ for(int i=0;i<256;i++) previousKeys[i] = currentKeys[i]; }
private:
    static bool currentKeys[256];
    static bool previousKeys[256];
};
bool Input::currentKeys[256]={0}, Input::previousKeys[256]={0};

class Window {
public:
    static Window& get() { static Window w; return w; }
    Window():hwnd(0),closed(0){}
    static LRESULT CALLBACK wndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam){
        if(msg==WM_KEYDOWN || msg==WM_SYSKEYDOWN){ 
            Input::setKey((Input::Key)wparam, true); 
        }
        if(msg==WM_KEYUP || msg==WM_SYSKEYUP){ 
            Input::setKey((Input::Key)wparam, false); 
        }
        if(msg==WM_QUIT || msg==WM_CLOSE){ Window::get().closed = 1; }
        if(msg==WM_SIZE){ Window::get().updateSize(); }
        return DefWindowProcA(hwnd,msg,wparam,lparam);
    }
    bool create(const char* title, int width, int height) {
        WNDCLASSA wc = {}; wc.lpfnWndProc = wndProc; wc.hInstance = GetModuleHandleA(0); wc.lpszClassName = "EE3D"; RegisterClassA(&wc);
        hwnd = CreateWindowA("EE3D",title,WS_OVERLAPPEDWINDOW|WS_MAXIMIZE,CW_USEDEFAULT,CW_USEDEFAULT,width,height,0,0,GetModuleHandleA(0),0);
        if(hwnd){ ShowWindow(hwnd,SW_SHOWMAXIMIZED); return true; }
        return false;
    }
    void destroy(){ if(hwnd){ DestroyWindow(hwnd); hwnd=0; } }
    HWND getHWND() const { return hwnd; }
    void updateSize(){}
    int getWidth() const { if(!hwnd) return 0; RECT r; GetClientRect(hwnd,&r); return r.right - r.left; }
    int getHeight() const { if(!hwnd) return 0; RECT r; GetClientRect(hwnd,&r); return r.bottom - r.top; }
    bool isClosed() const { return closed; }
    void pumpMessages(){ MSG msg; while(PeekMessageA(&msg,0,0,0,PM_REMOVE)){ TranslateMessage(&msg); DispatchMessageA(&msg); } }
private:
    HWND hwnd;
    int closed;
};

class Time {
public:
    static float getDeltaTime(){ return deltaTime; }
    static float getTime(){ return totalTime; }
    static int getFrameCount(){ return frameCount; }
    static int getDisplayFPS(){ return displayFPS; }
    static void setDeltaTime(float dt){ deltaTime = dt; totalTime += dt; frameCount++; if(dt>0) displayFPS = (int)(1.0f/dt+0.5f); }
private:
    static float deltaTime;
    static float totalTime;
    static int frameCount;
    static int displayFPS;
};
float Time::deltaTime=0, Time::totalTime=0;
int Time::frameCount=0, Time::displayFPS=60;

class Transform {
public:
    Transform():position(0,0,0),rotation(0,0,0),scale(1,1,1){}
    void setPosition(float x, float y, float z=0){ position = Vec3(x,y,z); }
    void setRotation(float x, float y, float z){ rotation = Vec3(x,y,z); }
    void setScale(float x, float y, float z=1){ scale = Vec3(x,y,z); }
    Vec3 getPosition() const{ return position; }
    Vec3 getRotation() const{ return rotation; }
    Vec3 getScale() const{ return scale; }
    Matrix4 getMatrix() const{
        Matrix4 m = Matrix4::translation(position.x,position.y,position.z);
        m = m * Matrix4::rotationY(rotation.y);
        m = m * Matrix4::rotationX(rotation.x);
        m = m * Matrix4::rotationZ(rotation.z);
        m = m * Matrix4::scale(scale.x,scale.y,scale.z);
        return m;
    }
private:
    Vec3 position, rotation, scale;
};

struct Mesh {
    std::vector<Vec3> vertices;
    std::vector<int> indices;
    Color color;
    static Mesh cube(){
        Mesh m;
        m.vertices = {Vec3(-0.5f,-0.5f,0.5f), Vec3(0.5f,-0.5f,0.5f), Vec3(0.5f,0.5f,0.5f), Vec3(-0.5f,0.5f,0.5f),
                       Vec3(-0.5f,-0.5f,-0.5f), Vec3(0.5f,-0.5f,-0.5f), Vec3(0.5f,0.5f,-0.5f), Vec3(-0.5f,0.5f,-0.5f)};
        m.indices = {0,1,2,0,2,3, 1,5,6,1,6,2, 5,4,7,5,7,6, 4,0,3,4,3,7, 3,2,6,3,6,7, 4,5,1,4,1,0};
        return m;
    }
    static Mesh pyramid(){
        Mesh m;
        m.vertices = {Vec3(0,0.5f,0), Vec3(-0.5f,-0.5f,0.5f), Vec3(0.5f,-0.5f,0.5f), Vec3(0.5f,-0.5f,-0.5f), Vec3(-0.5f,-0.5f,-0.5f)};
        m.indices = {0,1,2, 0,3,4, 0,1,4, 0,3,2, 1,2,4, 2,3,4};
        return m;
    }
    static Mesh plane(){
        Mesh m;
        m.vertices = {Vec3(-0.5f,0,-0.5f), Vec3(0.5f,0,-0.5f), Vec3(0.5f,0,0.5f), Vec3(-0.5f,0,0.5f)};
        m.indices = {0,1,2, 0,2,3};
        return m;
    }
};

class GameObject {
public:
    GameObject():name("Object"),active(true){}
    explicit GameObject(const char* n):name(n),active(true){}
    const char* getName() const{ return name.c_str(); }
    void setName(const char* n){ name = n; }
    bool isActive() const{ return active; }
    void setActive(bool a){ active = a; }
    Transform& getTransform(){ return transform; }
    const Transform& getTransform() const{ return transform; }
    Mesh& getMesh(){ return mesh; }
    const Mesh& getMesh() const{ return mesh; }
    void setMesh(const Mesh& m){ mesh = m; }
    void setColor(const Color& c){ mesh.color = c; }
    Color getColor() const{ return mesh.color; }
private:
    std::string name;
    bool active;
    Transform transform;
    Mesh mesh;
};

class Scene {
public:
    Scene(){}
    GameObject* createObject(const char* name="Object"){
        auto obj = std::make_shared<GameObject>(name);
        objects.push_back(obj);
        return obj.get();
    }
    const std::vector<std::shared_ptr<GameObject>>& getObjects() const{ return objects; }
private:
    std::vector<std::shared_ptr<GameObject>> objects;
};

class Camera {
public:
    Camera():position(0,2,-5),rotation(0,0,0),fov(60),nearPlane(0.1f),farPlane(100){}
    void setPosition(float x, float y, float z){ position = Vec3(x,y,z); }
    void setRotation(float x, float y, float z){ rotation = Vec3(x,y,z); }
    Vec3 getPosition() const{ return position; }
    Vec3 getRotation() const{ return rotation; }
    Matrix4 getViewMatrix() const{
        Matrix4 m = Matrix4::identity();
        m = m * Matrix4::rotationX(-rotation.x);
        m = m * Matrix4::rotationY(-rotation.y);
        m = m * Matrix4::rotationZ(-rotation.z);
        m = m * Matrix4::translation(-position.x,-position.y,-position.z);
        return m;
    }
    Matrix4 getProjectionMatrix(int width, int height) const{
        return Matrix4::perspective(fov * 3.14159f/180.0f, (float)width/height, nearPlane, farPlane);
    }
    void rotate(float dx, float dy){ rotation.x += dx; rotation.y += dy; if(rotation.x>1.5f)rotation.x=1.5f; if(rotation.x<-1.5f)rotation.x=-1.5f; }
private:
    Vec3 position, rotation;
    float fov, nearPlane, farPlane;
};

class Renderer {
public:
    Renderer(){}
    void init(Window* w){ window = w; }
    void shutdown(){ 
        if(memDC){ DeleteDC(memDC); memDC = 0; }
        if(memBitmap){ DeleteObject(memBitmap); memBitmap = 0; }
        window = nullptr; 
    }
    void clear(Color bg=Color(0.1f,0.1f,0.2f)){
        if(!window || !window->getHWND()) return;
        int w = window->getWidth();
        int h = window->getHeight();
        if(w<=0 || h<=0) return;
        
        if(!memDC || curW!=w || curH!=h){
            if(memDC){ DeleteDC(memDC); }
            if(memBitmap){ DeleteObject(memBitmap); }
            HDC hdc = GetDC(window->getHWND());
            memDC = CreateCompatibleDC(hdc);
            memBitmap = CreateCompatibleBitmap(hdc,w,h);
            SelectObject(memDC,memBitmap);
            ReleaseDC(window->getHWND(),hdc);
            curW = w; curH = h;
        }
        
        RECT rect = {0,0,w,h};
        HBRUSH brush = CreateSolidBrush(RGB((int)(bg.r*255),(int)(bg.g*255),(int)(bg.b*255)));
        FillRect(memDC,&rect,brush);
        DeleteObject(brush);
    }
    void present(){
        if(!window || !window->getHWND() || !memDC) return;
        HDC hdc = GetDC(window->getHWND());
        BitBlt(hdc,0,0,curW,curH,memDC,0,0,SRCCOPY);
        ReleaseDC(window->getHWND(),hdc);
    }
    void drawMesh(const GameObject& obj, const Matrix4& view, const Matrix4& proj, int width, int height){
        if(!memDC) return;
        HPEN pen = CreatePen(PS_SOLID,2,RGB((int)(obj.getColor().r*255),(int)(obj.getColor().g*255),(int)(obj.getColor().b*255)));
        SelectObject(memDC,pen);
        Matrix4 mvp = proj * view * obj.getTransform().getMatrix();
        const Mesh& mesh = obj.getMesh();
        for(size_t i=0;i<mesh.indices.size();i+=3){
            Vec3 v1 = mvp.transformPoint(mesh.vertices[mesh.indices[i]]);
            Vec3 v2 = mvp.transformPoint(mesh.vertices[mesh.indices[i+1]]);
            Vec3 v3 = mvp.transformPoint(mesh.vertices[mesh.indices[i+2]]);
            if(v1.z<0 && v2.z<0 && v3.z<0){
                float x1 = (v1.x/(-v1.z)+1)*(float)width/2, y1 = (1-v1.y/(-v1.z))*(float)height/2;
                float x2 = (v2.x/(-v2.z)+1)*(float)width/2, y2 = (1-v2.y/(-v2.z))*(float)height/2;
                float x3 = (v3.x/(-v3.z)+1)*(float)width/2, y3 = (1-v3.y/(-v3.z))*(float)height/2;
                MoveToEx(memDC,(int)x1,(int)y1,0); LineTo(memDC,(int)x2,(int)y2);
                LineTo(memDC,(int)x3,(int)y3); LineTo(memDC,(int)x1,(int)y1);
            }
        }
        DeleteObject(pen);
    }
    void drawText(float x, float y, const char* text, Color c=Color::white()){
        if(!memDC) return;
        SetTextColor(memDC,RGB((int)(c.r*255),(int)(c.g*255),(int)(c.b*255)));
        SetBkMode(memDC,TRANSPARENT);
        TextOutA(memDC,(int)x,(int)y,text,(int)strlen(text));
    }
private:
    Window* window = nullptr;
    HDC memDC = 0;
    HBITMAP memBitmap = 0;
    int curW = 0, curH = 0;
};

class Engine {
public:
    static Engine& get(){ static Engine e; return e; }
    bool init(const char* title="EasyEngine3D", int width=800, int height=600){
        running = true;
        window = new Window();
        if(!window->create(title,width,height)) return false;
        renderer = new Renderer();
        renderer->init(window);
        scene = std::make_shared<Scene>();
        createDefaultScene();
        std::cout << "=== EasyEngine 3D v1.1 ===\n";
        std::cout << "F1-F5=Select R+WASD=Move Obj\n";
        std::cout << "Camera: QE=Rotate Arrows=Look Space/Shift=Up/Down\n";
        return true;
    }
    void createDefaultScene(){
        auto ground = scene->createObject("Ground");
        ground->setMesh(Mesh::plane());
        ground->getTransform().setPosition(0,-1,0);
        ground->getTransform().setScale(10,1,10);
        ground->setColor(Color::green());
        
        auto cube = scene->createObject("Cube");
        cube->setMesh(Mesh::cube());
        cube->getTransform().setPosition(0,0,0);
        cube->setColor(Color::red());
        
        auto pyramid = scene->createObject("Pyramid");
        pyramid->setMesh(Mesh::pyramid());
        pyramid->getTransform().setPosition(-2,0.5f,0);
        pyramid->setColor(Color::yellow());
        
        auto sphere = scene->createObject("Sphere");
        sphere->setMesh(Mesh::cube());
        sphere->getTransform().setPosition(2,0,0);
        sphere->setColor(Color::blue());
        
        for(int i=0;i<5;i++){
            auto box = scene->createObject("Box");
            box->setMesh(Mesh::cube());
            box->getTransform().setPosition(-4.0f + i*2.0f, -0.5f, 3.0f);
            box->getTransform().setScale(0.5f,0.5f,0.5f);
            box->setColor(Color::orange());
        }
    }
    GameObject* createObject(const char* name){ return scene->createObject(name); }
    void shutdown(){
        if(scene) scene.reset();
        if(renderer){ renderer->shutdown(); delete renderer; renderer = nullptr; }
        if(window){ window->destroy(); delete window; window = nullptr; }
        running = false;
    }
    void update(float dt){
        Time::setDeltaTime(dt);
        if(window) window->pumpMessages();
        Input::update();
        handleCameraInput(dt);
        handleObjectInput(dt);
    }
    void handleObjectInput(float dt){
        auto& objs = scene->getObjects();
        if(Input::getKeyDown(Input::F1) && objs.size()>0){ selectedObject = 0; std::cout << "[SELECT] Object 0 selected: " << objs[0]->getName() << std::endl; }
        if(Input::getKeyDown(Input::F2) && objs.size()>1){ selectedObject = 1; std::cout << "[SELECT] Object 1 selected: " << objs[1]->getName() << std::endl; }
        if(Input::getKeyDown(Input::F3) && objs.size()>2){ selectedObject = 2; std::cout << "[SELECT] Object 2 selected: " << objs[2]->getName() << std::endl; }
        if(Input::getKeyDown(Input::F4) && objs.size()>3){ selectedObject = 3; std::cout << "[SELECT] Object 3 selected: " << objs[3]->getName() << std::endl; }
        if(Input::getKeyDown(Input::F5) && objs.size()>4){ selectedObject = 4; std::cout << "[SELECT] Object 4 selected: " << objs[4]->getName() << std::endl; }
        
        if(selectedObject >= 0 && selectedObject < (int)objs.size() && Input::getKey(Input::R)){
            float speed = 2.0f * dt;
            auto& obj = objs[selectedObject];
            Vec3 pos = obj->getTransform().getPosition();
            if(Input::getKey(Input::A)){ obj->getTransform().setPosition(pos.x - speed, pos.y, pos.z); std::cout << "[MOVE] " << obj->getName() << " left" << std::endl; }
            if(Input::getKey(Input::D)){ obj->getTransform().setPosition(pos.x + speed, pos.y, pos.z); std::cout << "[MOVE] " << obj->getName() << " right" << std::endl; }
            if(Input::getKey(Input::W)){ obj->getTransform().setPosition(pos.x, pos.y, pos.z + speed); std::cout << "[MOVE] " << obj->getName() << " forward" << std::endl; }
            if(Input::getKey(Input::S)){ obj->getTransform().setPosition(pos.x, pos.y, pos.z - speed); std::cout << "[MOVE] " << obj->getName() << " back" << std::endl; }
            if(Input::getKey(Input::SPACE)){ obj->getTransform().setPosition(pos.x, pos.y + speed, pos.z); std::cout << "[MOVE] " << obj->getName() << " up" << std::endl; }
            if(Input::getKey(Input::SHIFT)){ obj->getTransform().setPosition(pos.x, pos.y - speed, pos.z); std::cout << "[MOVE] " << obj->getName() << " down" << std::endl; }
        }
    }
    void handleCameraInput(float dt){
        if(Input::getKey(Input::R)) return;
        
        float speed = 3.0f * dt;
        float rotSpeed = 2.0f * dt;
        float cy = cos(camera.getRotation().y), sy = sin(camera.getRotation().y);
        if(Input::getKey(Input::W)) camera.setPosition(camera.getPosition().x + sy*speed, camera.getPosition().y, camera.getPosition().z + cy*speed);
        if(Input::getKey(Input::S)) camera.setPosition(camera.getPosition().x - sy*speed, camera.getPosition().y, camera.getPosition().z - cy*speed);
        if(Input::getKey(Input::A)) camera.setPosition(camera.getPosition().x - cy*speed, camera.getPosition().y, camera.getPosition().z + sy*speed);
        if(Input::getKey(Input::D)) camera.setPosition(camera.getPosition().x + cy*speed, camera.getPosition().y, camera.getPosition().z - sy*speed);
        if(Input::getKey(Input::Q)) camera.rotate(0,-rotSpeed);
        if(Input::getKey(Input::E)) camera.rotate(0,rotSpeed);
        if(Input::getKey(Input::UP)) camera.rotate(-rotSpeed,0);
        if(Input::getKey(Input::DOWN)) camera.rotate(rotSpeed,0);
        if(Input::getKey(Input::LEFT)) camera.rotate(0,-rotSpeed);
        if(Input::getKey(Input::RIGHT)) camera.rotate(0,rotSpeed);
    }
    void render(){
        if(!renderer) return;
        int width = window ? window->getWidth() : 800;
        int height = window ? window->getHeight() : 600;
        renderer->clear();
        Matrix4 view = camera.getViewMatrix();
        Matrix4 proj = camera.getProjectionMatrix(width,height);
        if(scene){
            for(auto& obj : scene->getObjects()){
                if(obj->isActive()){
                    renderer->drawMesh(*obj, view, proj, width, height);
                }
            }
        }
        renderer->drawText(10,10,"EasyEngine3D v1.1");
        char fpsText[32]; sprintf_s(fpsText,32,"FPS: %d",Time::getDisplayFPS());
        renderer->drawText(10,30,fpsText);
        char camText[64]; sprintf_s(camText,64,"Cam: %.1f,%.1f,%.1f",camera.getPosition().x,camera.getPosition().y,camera.getPosition().z);
        renderer->drawText(10,50,camText);
        char selText[64]; sprintf_s(selText,64,"Selected: %s",selectedObject>=0 && scene ? scene->getObjects()[selectedObject]->getName() : "None");
        renderer->drawText(10,70,selText);
        renderer->drawText(10,90,"F1-F5=Select R+WASD=MoveObj");
        renderer->drawText(10,110,"Camera: QE=Rot Arrows=Look Space=Up");
        renderer->present();
    }
    bool isRunning() const{ return running && !window->isClosed(); }
private:
    bool running = false;
    Window* window = nullptr;
    Renderer* renderer = nullptr;
    std::shared_ptr<Scene> scene;
    Camera camera;
    int selectedObject = -1;
};

extern "C" {
    ENGINE_API void Engine_Init(){ Engine::get().init(); }
    ENGINE_API void Engine_Update(float dt){ Engine::get().update(dt); }
    ENGINE_API void Engine_Render(){ Engine::get().render(); }
    ENGINE_API void Engine_Shutdown(){ Engine::get().shutdown(); }
    ENGINE_API const char* Engine_Version(){ return "EasyEngine3D v1.1"; }
}

}
