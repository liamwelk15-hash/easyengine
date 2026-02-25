#include "Rendering.h"
#include <cstring>

namespace Engine {

Mesh::Mesh() : Object() {}

void Mesh::setVertices(const std::vector<Vertex>& verts) {
    vertices = verts;
}

void Mesh::setIndices(const std::vector<int>& inds) {
    indices = inds;
}

Material::Material() : Object() {
    diffuseColor = Color::white();
}

void Material::setFloat(const std::string& name, float value) {
    floats[name] = value;
}

float Material::getFloat(const std::string& name) const {
    auto it = floats.find(name);
    if (it != floats.end()) return it->second;
    return 0.0f;
}

void Material::setTexture(const std::string& name, Texture* tex) {
    textures[name] = tex;
}

Texture* Material::getTexture(const std::string& name) const {
    auto it = textures.find(name);
    if (it != textures.end()) return it->second;
    return nullptr;
}

Texture::Texture() : Object() {}

Texture::~Texture() {}

bool Texture::loadFromFile(const std::string& path) {
    return false;
}

bool Texture::create(int width_, int height_, const Color& color) {
    width = width_;
    height = height_;
    pixels.resize(width * height, color);
    return true;
}

void Texture::setPixel(int x, int y, const Color& color) {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        pixels[y * width + x] = color;
    }
}

Color Texture::getPixel(int x, int y) const {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        return pixels[y * width + x];
    }
    return Color::transparent();
}

Shader::Shader() : Object() {}

Shader::~Shader() {
    
}

bool Shader::loadFromFiles(const std::string& vertexPath, const std::string& fragmentPath) {
    return false;
}

bool Shader::compile(const std::string& vertexSrc, const std::string& fragmentSrc) {
    return false;
}

void Shader::use() {
    
}

void Shader::setInt(const std::string& name, int value) {
    
}

void Shader::setFloat(const std::string& name, float value) {
    
}

void Shader::setVector2(const std::string& name, const Vector2& value) {
    
}

void Shader::setVector3(const std::string& name, const Vector3& value) {
    
}

void Shader::setColor(const std::string& name, const Color& value) {
    
}

Sprite::Sprite() : Component() {}

void Sprite::onUpdate(float deltaTime) {
    
}

Camera::Camera() : Component() {}

Vector3 Camera::screenToWorld(const Vector2& screenPos) {
    return Vector3(screenPos.x, screenPos.y, 0);
}

Vector2 Camera::worldToScreen(const Vector3& worldPos) {
    return {worldPos.x, worldPos.y};
}

Renderer& Renderer::get() {
    static Renderer instance;
    return instance;
}

void Renderer::initialize() {
    Logger::info("Renderer initialized");
}

void Renderer::shutdown() {
    Logger::info("Renderer shutdown");
}

void Renderer::clear(const Color& color) {
    
}

void Renderer::present() {
    
}

void Renderer::drawMesh(Mesh* mesh, Material* material) {
    
}

void Renderer::drawQuad(const Vector3& pos, const Vector2& size, const Color& color) {
    
}

void Renderer::drawLine(const Vector3& start, const Vector3& end, const Color& color, float thickness) {
    
}

void Renderer::drawCircle(const Vector2& center, float radius, const Color& color, int segments) {
    
}

void Renderer::setViewMatrix(const Matrix4& view) {
    
}

void Renderer::setProjectionMatrix(const Matrix4& proj) {
    
}

Matrix4::Matrix4() {
    memset(data, 0, sizeof(data));
}

Matrix4 Matrix4::identity() {
    Matrix4 m;
    m.data[0] = m.data[5] = m.data[10] = m.data[15] = 1.0f;
    return m;
}

Matrix4 Matrix4::translation(const Vector3& pos) {
    Matrix4 m = identity();
    m.data[12] = pos.x;
    m.data[13] = pos.y;
    m.data[14] = pos.z;
    return m;
}

Matrix4 Matrix4::rotation(float angle, const Vector3& axis) {
    Matrix4 m = identity();
    return m;
}

Matrix4 Matrix4::scale(const Vector3& s) {
    Matrix4 m = identity();
    m.data[0] = s.x;
    m.data[5] = s.y;
    m.data[10] = s.z;
    return m;
}

Matrix4 Matrix4::perspective(float fov, float aspect, float near, float far) {
    Matrix4 m;
    float tanHalfFov = 1.0f / (float)tan(fov * 0.5f * 3.14159f / 180.0f);
    
    m.data[0] = 1.0f / (aspect * tanHalfFov);
    m.data[5] = 1.0f / tanHalfFov;
    m.data[10] = -(far + near) / (far - near);
    m.data[11] = -1.0f;
    m.data[14] = -(2.0f * far * near) / (far - near);
    return m;
}

Matrix4 Matrix4::orthographic(float left, float right, float bottom, float top, float near, float far) {
    Matrix4 m = identity();
    m.data[0] = 2.0f / (right - left);
    m.data[5] = 2.0f / (top - bottom);
    m.data[10] = -2.0f / (far - near);
    m.data[12] = -(right + left) / (right - left);
    m.data[13] = -(top + bottom) / (top - bottom);
    m.data[14] = -(far + near) / (far - near);
    return m;
}

Matrix4 Matrix4::lookAt(const Vector3& eye, const Vector3& target, const Vector3& up) {
    Matrix4 m;
    Vector3 z = (eye - target).normalized();
    Vector3 x = up.cross(z).normalized();
    Vector3 y = z.cross(x);
    
    m.data[0] = x.x; m.data[4] = x.y; m.data[8] = x.z;
    m.data[1] = y.x; m.data[5] = y.y; m.data[9] = y.z;
    m.data[2] = z.x; m.data[6] = z.y; m.data[10] = z.z;
    m.data[12] = -x.dot(eye);
    m.data[13] = -y.dot(eye);
    m.data[14] = -z.dot(eye);
    m.data[15] = 1.0f;
    return m;
}

Matrix4 Matrix4::operator*(const Matrix4& other) const {
    Matrix4 result;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result.data[j * 4 + i] = 
                data[i] * other.data[j * 4] +
                data[i + 4] * other.data[j * 4 + 1] +
                data[i + 8] * other.data[j * 4 + 2] +
                data[i + 12] * other.data[j * 4 + 3];
        }
    }
    return result;
}

Vector3 Matrix4::operator*(const Vector3& v) const {
    float w = data[3] * v.x + data[7] * v.y + data[11] * v.z + data[15];
    if (w != 0) {
        return {
            (data[0] * v.x + data[4] * v.y + data[8] * v.z + data[12]) / w,
            (data[1] * v.x + data[5] * v.y + data[9] * v.z + data[13]) / w,
            (data[2] * v.x + data[6] * v.y + data[10] * v.z + data[14]) / w
        };
    }
    return v;
}

} // namespace Engine
