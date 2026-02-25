#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <windows.h>

namespace Engine {

enum class LogLevel {
    DEBUG, INFO, WARNING, ERROR
};

class Logger {
public:
    static void log(LogLevel level, const std::string& message);
    static void debug(const std::string& message);
    static void info(const std::string& message);
    static void warning(const std::string& message);
    static void error(const std::string& message);
};

class Object {
public:
    Object();
    virtual ~Object();
    
    virtual void destroy();
    bool isDestroyed() const { return destroyed; }
    
    std::string getName() const { return name; }
    void setName(const std::string& n) { name = n; }
    
    template<typename T>
    T* as() { return dynamic_cast<T*>(this); }
    
private:
    std::string name;
    bool destroyed = false;
};

class Transform : public Object {
public:
    Transform();
    
    void setPosition(float x, float y, float z = 0.0f);
    void setRotation(float x, float y, float z);
    void setScale(float x, float y, float z = 1.0f);
    
    float getPositionX() const { return position[0]; }
    float getPositionY() const { return position[1]; }
    float getPositionZ() const { return position[2]; }
    
    float getRotationX() const { return rotation[0]; }
    float getRotationY() const { return rotation[1]; }
    float getRotationZ() const { return rotation[2]; }
    
    float getScaleX() const { return scale[0]; }
    float getScaleY() const { return scale[1]; }
    float getScaleZ() const { return scale[2]; }
    
private:
    float position[3] = {0, 0, 0};
    float rotation[3] = {0, 0, 0};
    float scale[3] = {1, 1, 1};
};

struct Vector3 {
    float x, y, z;
    
    Vector3() : x(0), y(0), z(0) {}
    Vector3(float x_, float y_, float z_ = 0) : x(x_), y(y_), z(z_) {}
    
    Vector3 operator+(const Vector3& other) const { return {x + other.x, y + other.y, z + other.z}; }
    Vector3 operator-(const Vector3& other) const { return {x - other.x, y - other.y, z - other.z}; }
    Vector3 operator*(float scalar) const { return {x * scalar, y * scalar, z * scalar}; }
    Vector3 operator/(float scalar) const { return {x / scalar, y / scalar, z / scalar}; }
    
    float length() const;
    Vector3 normalized() const;
    float dot(const Vector3& other) const;
    Vector3 cross(const Vector3& other) const;
};

struct Vector2 {
    float x, y;
    
    Vector2() : x(0), y(0) {}
    Vector2(float x_, float y_) : x(x_), y(y_) {}
    
    Vector2 operator+(const Vector2& other) const { return {x + other.x, y + other.y}; }
    Vector2 operator-(const Vector2& other) const { return {x - other.x, y - other.y}; }
    Vector2 operator*(float scalar) const { return {x * scalar, y * scalar}; }
    
    float length() const;
    Vector2 normalized() const;
};

class Component : public Object {
public:
    Component();
    virtual void onStart() {}
    virtual void onUpdate(float deltaTime) {}
    virtual void onDestroy() {}
    
    Transform* getTransform() const { return transform; }
    void setTransform(Transform* t) { transform = t; }
    
private:
    Transform* transform = nullptr;
};

class GameObject : public Object {
public:
    GameObject();
    explicit GameObject(const std::string& name);
    
    template<typename T>
    T* addComponent() {
        auto comp = std::make_shared<T>();
        comp->setTransform(&transform);
        components.push_back(comp);
        return comp.get();
    }
    
    template<typename T>
    T* getComponent() {
        for (auto& comp : components) {
            if (comp->as<T>()) {
                return comp->as<T>();
            }
        }
        return nullptr;
    }
    
    Transform& getTransform() { return transform; }
    
    void onStart();
    void onUpdate(float deltaTime);
    
private:
    Transform transform;
    std::vector<std::shared_ptr<Component>> components;
};

class DLLModule {
public:
    DLLModule(const std::string& path);
    ~DLLModule();
    
    bool isLoaded() const { return handle != nullptr; }
    void* getFunction(const std::string& name);
    
private:
    HMODULE handle = nullptr;
    std::string path;
};

class DLLManager {
public:
    static DLLManager& get();
    
    bool loadDLL(const std::string& name, const std::string& path);
    void unloadDLL(const std::string& name);
    void* getFunction(const std::string& dllName, const std::string& funcName);
    std::vector<std::string> getLoadedDLLs() const;
    
private:
    DLLManager() = default;
    std::unordered_map<std::string, std::unique_ptr<DLLModule>> modules;
};

class Time {
public:
    static float getDeltaTime() { return deltaTime; }
    static float getTime() { return totalTime; }
    static int getFPS() { return fps; }
    static void setDeltaTime(float dt) { deltaTime = dt; }
    static void setTime(float t) { totalTime = t; }
    static void setFPS(int f) { fps = f; }
    
private:
    static float deltaTime;
    static float totalTime;
    static int fps;
};

class Input {
public:
    enum class Key {
        A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
        NUM0, NUM1, NUM2, NUM3, NUM4, NUM5, NUM6, NUM7, NUM8, NUM9,
        SPACE, ENTER, ESCAPE, SHIFT, CTRL, ALT,
        UP, DOWN, LEFT, RIGHT,
        MOUSE_LEFT, MOUSE_RIGHT, MOUSE_MIDDLE
    };
    
    static bool isKeyPressed(Key key);
    static bool isKeyDown(Key key);
    static bool isKeyReleased(Key key);
    
    static Vector2 getMousePosition();
    static float getMouseX();
    static float getMouseY();
    static float getMouseScroll();
    
    static void setKeyPressed(Key key, bool pressed);
    static void setMousePosition(float x, float y);
    static void setMouseScroll(float scroll);
    
private:
    static std::unordered_map<Key, bool> currentKeys;
    static std::unordered_map<Key, bool> previousKeys;
    static Vector2 mousePos;
    static float mouseScroll;
};

class Scene : public Object {
public:
    Scene();
    
    GameObject* createGameObject(const std::string& name = "");
    void destroyGameObject(GameObject* obj);
    
    void onStart();
    void onUpdate(float deltaTime);
    
    const std::vector<std::shared_ptr<GameObject>>& getGameObjects() const { return gameObjects; }
    
private:
    std::vector<std::shared_ptr<GameObject>> gameObjects;
};

class EngineCore {
public:
    static EngineCore& get();
    
    bool initialize();
    void shutdown();
    
    void loadScene(Scene* scene);
    Scene* getCurrentScene() const { return currentScene; }
    
    void update(float deltaTime);
    void render();
    
    bool isRunning() const { return running; }
    void quit() { running = false; }
    
    void loadPluginDLL(const std::string& name);
    
private:
    EngineCore() = default;
    
    bool running = false;
    Scene* currentScene = nullptr;
    std::vector<std::string> loadedPlugins;
};

#define ENGINE_API __declspec(dllexport)

extern "C" {
    ENGINE_API void Engine_Initialize();
    ENGINE_API void Engine_Update(float deltaTime);
    ENGINE_API void Engine_Render();
    ENGINE_API void Engine_Shutdown();
    ENGINE_API const char* Engine_GetVersion();
}

} // namespace Engine
