#pragma once

#include "EngineCore.h"
#include <vector>

namespace Engine {

class Renderer;

struct Color {
    float r, g, b, a;
    
    Color() : r(1), g(1), b(1), a(1) {}
    Color(float r_, float g_, float b_, float a_ = 1.0f) 
        : r(r_), g(g_), b(b_), a(a_) {}
    
    static Color white() { return {1, 1, 1, 1}; }
    static Color black() { return {0, 0, 0, 1}; }
    static Color red() { return {1, 0, 0, 1}; }
    static Color green() { return {0, 1, 0, 1}; }
    static Color blue() { return {0, 0, 1, 1}; }
    static Color yellow() { return {1, 1, 0, 1}; }
    static Color transparent() { return {0, 0, 0, 0}; }
};

struct Vertex {
    Vector3 position;
    Color color;
    Vector2 texCoord;
};

class Mesh : public Object {
public:
    Mesh();
    
    void setVertices(const std::vector<Vertex>& verts);
    void setIndices(const std::vector<int>& inds);
    
    const std::vector<Vertex>& getVertices() const { return vertices; }
    const std::vector<int>& getIndices() const { return indices; }
    
private:
    std::vector<Vertex> vertices;
    std::vector<int> indices;
};

class Material : public Object {
public:
    Material();
    
    void setColor(const Color& color) { diffuseColor = color; }
    Color getColor() const { return diffuseColor; }
    
    void setEmissive(const Color& color) { emissive = color; }
    Color getEmissive() const { return emissive; }
    
    void setFloat(const std::string& name, float value);
    float getFloat(const std::string& name) const;
    
    void setTexture(const std::string& name, class Texture* tex);
    class Texture* getTexture(const std::string& name) const;
    
private:
    Color diffuseColor;
    Color emissive;
    std::unordered_map<std::string, float> floats;
    std::unordered_map<std::string, class Texture*> textures;
};

class Texture : public Object {
public:
    Texture();
    ~Texture();
    
    bool loadFromFile(const std::string& path);
    bool create(int width, int height, const Color& color = Color::transparent());
    
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    
    void setPixel(int x, int y, const Color& color);
    Color getPixel(int x, int y) const;
    
private:
    int width = 0;
    int height = 0;
    std::vector<Color> pixels;
};

class Shader : public Object {
public:
    Shader();
    ~Shader();
    
    bool loadFromFiles(const std::string& vertexPath, const std::string& fragmentPath);
    bool compile(const std::string& vertexSrc, const std::string& fragmentSrc);
    
    void use();
    void setInt(const std::string& name, int value);
    void setFloat(const std::string& name, float value);
    void setVector2(const std::string& name, const Vector2& value);
    void setVector3(const std::string& name, const Vector3& value);
    void setColor(const std::string& name, const Color& value);
    
private:
    unsigned int programID = 0;
};

class Sprite : public Component {
public:
    Sprite();
    
    void setTexture(Texture* tex) { texture = tex; }
    Texture* getTexture() const { return texture; }
    
    void setColor(const Color& c) { color = c; }
    Color getColor() const { return color; }
    
    void setFlipHorizontal(bool flip) { flipH = flip; }
    void setFlipVertical(bool flip) { flipV = flip; }
    bool isFlipHorizontal() const { return flipH; }
    bool isFlipVertical() const { return flipV; }
    
    void onUpdate(float deltaTime) override;
    
private:
    Texture* texture = nullptr;
    Color color = Color::white();
    bool flipH = false;
    bool flipV = false;
};

class Camera : public Component {
public:
    Camera();
    
    void setOrthographic(bool ortho) { orthographic = ortho; }
    bool isOrthographic() const { return orthographic; }
    
    void setSize(float size) { orthoSize = size; }
    float getSize() const { return orthoSize; }
    
    void setFOV(float fov_) { fov = fov_; }
    float getFOV() const { return fov; }
    
    void setNear(float near_) { nearClip = near_; }
    float getNear() const { return nearClip; }
    
    void setFar(float far_) { farClip = far_; }
    float getFar() const { return farClip; }
    
    Vector3 screenToWorld(const Vector2& screenPos);
    Vector2 worldToScreen(const Vector3& worldPos);
    
private:
    bool orthographic = true;
    float orthoSize = 5.0f;
    float fov = 60.0f;
    float nearClip = 0.1f;
    float farClip = 100.0f;
};

class Renderer {
public:
    static Renderer& get();
    
    void initialize();
    void shutdown();
    
    void clear(const Color& color = Color::black());
    void present();
    
    void drawMesh(Mesh* mesh, Material* material);
    void drawQuad(const Vector3& pos, const Vector2& size, const Color& color);
    void drawLine(const Vector3& start, const Vector3& end, const Color& color, float thickness = 1.0f);
    void drawCircle(const Vector2& center, float radius, const Color& color, int segments = 32);
    
    void setCamera(Camera* cam) { currentCamera = cam; }
    Camera* getCamera() const { return currentCamera; }
    
    void setViewMatrix(const class Matrix4& view);
    void setProjectionMatrix(const class Matrix4& proj);
    
private:
    Renderer() = default;
    
    Camera* currentCamera = nullptr;
};

class Matrix4 {
public:
    Matrix4();
    
    static Matrix4 identity();
    static Matrix4 translation(const Vector3& pos);
    static Matrix4 rotation(float angle, const Vector3& axis);
    static Matrix4 scale(const Vector3& scale);
    static Matrix4 perspective(float fov, float aspect, float near, float far);
    static Matrix4 orthographic(float left, float right, float bottom, float top, float near, float far);
    static Matrix4 lookAt(const Vector3& eye, const Vector3& target, const Vector3& up);
    
    Matrix4 operator*(const Matrix4& other) const;
    Vector3 operator*(const Vector3& v) const;
    
    float* getData() { return data; }
    const float* getData() const { return data; }
    
private:
    float data[16];
};

} // namespace Engine
