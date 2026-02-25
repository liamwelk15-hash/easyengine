#include "EngineCore.h"
#include <iostream>
#include <chrono>
#include <cmath>

namespace Engine {

float Time::deltaTime = 0.0f;
float Time::totalTime = 0.0f;
int Time::fps = 0;

std::unordered_map<Input::Key, bool> Input::currentKeys;
std::unordered_map<Input::Key, bool> Input::previousKeys;
Vector2 Input::mousePos;
float Input::mouseScroll = 0.0f;

void Logger::log(LogLevel level, const std::string& message) {
    const char* levelStr = "DEBUG";
    switch (level) {
        case LogLevel::INFO: levelStr = "INFO"; break;
        case LogLevel::WARNING: levelStr = "WARN"; break;
        case LogLevel::ERROR: levelStr = "ERROR"; break;
    }
    std::cout << "[" << levelStr << "] " << message << std::endl;
}

void Logger::debug(const std::string& message) { log(LogLevel::DEBUG, message); }
void Logger::info(const std::string& message) { log(LogLevel::INFO, message); }
void Logger::warning(const std::string& message) { log(LogLevel::WARNING, message); }
void Logger::error(const std::string& message) { log(LogLevel::ERROR, message); }

Object::Object() : name("Object") {}
Object::~Object() {}

void Object::destroy() {
    destroyed = true;
}

Transform::Transform() {}

void Transform::setPosition(float x, float y, float z) {
    position[0] = x;
    position[1] = y;
    position[2] = z;
}

void Transform::setRotation(float x, float y, float z) {
    rotation[0] = x;
    rotation[1] = y;
    rotation[2] = z;
}

void Transform::setScale(float x, float y, float z) {
    scale[0] = x;
    scale[1] = y;
    scale[2] = z;
}

float Vector3::length() const {
    return std::sqrt(x * x + y * y + z * z);
}

Vector3 Vector3::normalized() const {
    float len = length();
    if (len > 0.0001f) {
        return {x / len, y / len, z / len};
    }
    return {0, 0, 0};
}

float Vector3::dot(const Vector3& other) const {
    return x * other.x + y * other.y + z * other.z;
}

Vector3 Vector3::cross(const Vector3& other) const {
    return {
        y * other.z - z * other.y,
        z * other.x - x * other.z,
        x * other.y - y * other.x
    };
}

float Vector2::length() const {
    return std::sqrt(x * x + y * y);
}

Vector2 Vector2::normalized() const {
    float len = length();
    if (len > 0.0001f) {
        return {x / len, y / len};
    }
    return {0, 0};
}

Component::Component() : Object() {}

GameObject::GameObject() : Object() {
    setName("GameObject");
}

GameObject::GameObject(const std::string& name) : Object() {
    setName(name);
}

void GameObject::onStart() {
    for (auto& comp : components) {
        comp->onStart();
    }
}

void GameObject::onUpdate(float deltaTime) {
    for (auto& comp : components) {
        comp->onUpdate(deltaTime);
    }
}

DLLModule::DLLModule(const std::string& path) : path(path) {
    handle = LoadLibraryA(path.c_str());
    if (!handle) {
        Logger::error("Failed to load DLL: " + path);
    }
}

DLLModule::~DLLModule() {
    if (handle) {
        FreeLibrary(handle);
    }
}

void* DLLModule::getFunction(const std::string& name) {
    if (!handle) return nullptr;
    return reinterpret_cast<void*>(GetProcAddress(handle, name.c_str()));
}

DLLManager& DLLManager::get() {
    static DLLManager instance;
    return instance;
}

bool DLLManager::loadDLL(const std::string& name, const std::string& path) {
    auto dll = std::make_unique<DLLModule>(path);
    if (dll->isLoaded()) {
        modules[name] = std::move(dll);
        Logger::info("Loaded DLL: " + name);
        return true;
    }
    return false;
}

void DLLManager::unloadDLL(const std::string& name) {
    modules.erase(name);
}

void* DLLManager::getFunction(const std::string& dllName, const std::string& funcName) {
    auto it = modules.find(dllName);
    if (it != modules.end()) {
        return it->second->getFunction(funcName);
    }
    return nullptr;
}

std::vector<std::string> DLLManager::getLoadedDLLs() const {
    std::vector<std::string> names;
    for (const auto& pair : modules) {
        names.push_back(pair.first);
    }
    return names;
}

bool Input::isKeyPressed(Key key) {
    return currentKeys[key] && !previousKeys[key];
}

bool Input::isKeyDown(Key key) {
    return currentKeys[key];
}

bool Input::isKeyReleased(Key key) {
    return !currentKeys[key] && previousKeys[key];
}

Vector2 Input::getMousePosition() {
    return mousePos;
}

float Input::getMouseX() {
    return mousePos.x;
}

float Input::getMouseY() {
    return mousePos.y;
}

float Input::getMouseScroll() {
    return mouseScroll;
}

void Input::setKeyPressed(Key key, bool pressed) {
    previousKeys[key] = currentKeys[key];
    currentKeys[key] = pressed;
}

void Input::setMousePosition(float x, float y) {
    mousePos.x = x;
    mousePos.y = y;
}

void Input::setMouseScroll(float scroll) {
    mouseScroll = scroll;
}

Scene::Scene() : Object() {
    setName("Scene");
}

GameObject* Scene::createGameObject(const std::string& name) {
    auto obj = std::make_shared<GameObject>(name);
    gameObjects.push_back(obj);
    return obj.get();
}

void Scene::destroyGameObject(GameObject* obj) {
    for (auto it = gameObjects.begin(); it != gameObjects.end(); ++it) {
        if (it->get() == obj) {
            obj->destroy();
            gameObjects.erase(it);
            return;
        }
    }
}

void Scene::onStart() {
    for (auto& obj : gameObjects) {
        obj->onStart();
    }
}

void Scene::onUpdate(float deltaTime) {
    for (auto& obj : gameObjects) {
        obj->onUpdate(deltaTime);
    }
}

EngineCore& EngineCore::get() {
    static EngineCore instance;
    return instance;
}

bool EngineCore::initialize() {
    Logger::info("Engine initializing...");
    Logger::info("Version: 0.1.0");
    running = true;
    return true;
}

void EngineCore::shutdown() {
    Logger::info("Engine shutting down...");
    currentScene = nullptr;
    running = false;
}

void EngineCore::loadScene(Scene* scene) {
    currentScene = scene;
    if (currentScene) {
        currentScene->onStart();
    }
}

void EngineCore::update(float deltaTime) {
    Time::setDeltaTime(deltaTime);
    Time::setTime(Time::getTime() + deltaTime);
    
    if (currentScene) {
        currentScene->onUpdate(deltaTime);
    }
}

void EngineCore::render() {
    
}

void EngineCore::loadPluginDLL(const std::string& name) {
    std::string path = "dll/" + name + ".dll";
    if (DLLManager::get().loadDLL(name, path)) {
        loadedPlugins.push_back(name);
    }
}

extern "C" {
    ENGINE_API void Engine_Initialize() {
        EngineCore::get().initialize();
    }
    
    ENGINE_API void Engine_Update(float deltaTime) {
        EngineCore::get().update(deltaTime);
    }
    
    ENGINE_API void Engine_Render() {
        EngineCore::get().render();
    }
    
    ENGINE_API void Engine_Shutdown() {
        EngineCore::get().shutdown();
    }
    
    ENGINE_API const char* Engine_GetVersion() {
        return "0.1.0";
    }
}

} // namespace Engine
