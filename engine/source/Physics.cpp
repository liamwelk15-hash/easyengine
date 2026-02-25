#include "Physics.h"
#include <cmath>

namespace Engine {

RigidBody2D::RigidBody2D() : Component() {}

void RigidBody2D::applyForce(const Vector2& force) {
    velocity = velocity + force / mass;
}

void RigidBody2D::applyImpulse(const Vector2& impulse) {
    velocity = velocity + impulse / mass;
}

void RigidBody2D::applyTorque(float torque) {
    angularVelocity += torque;
}

BoxCollider2D::BoxCollider2D() : Component() {}

CircleCollider2D::CircleCollider2D() : Component() {}

Physics2D& Physics2D::get() {
    static Physics2D instance;
    return instance;
}

void Physics2D::initialize(const PhysicsConfig& config) {
    this->config = config;
    initialized = true;
    Logger::info("Physics2D initialized");
}

void Physics2D::shutdown() {
    initialized = false;
    Logger::info("Physics2D shutdown");
}

void Physics2D::update(float deltaTime) {
    if (!initialized || paused) return;
    
    Vector2 gravity = {config.gravityX, config.gravityY};
    
    for (int iter = 0; iter < config.velocityIterations; iter++) {
    }
}

void Physics2D::setGravity(const Vector2& gravity) {
    config.gravityX = gravity.x;
    config.gravityY = gravity.y;
}

Vector2 Physics2D::getGravity() const {
    return {config.gravityX, config.gravityY};
}

bool Physics2D::raycast(const Vector2& start, const Vector2& end, RaycastHit2D& hit) {
    return false;
}

bool Physics2D::circleCast(const Vector2& center, float radius, const Vector2& direction, float maxDistance, RaycastHit2D& hit) {
    return false;
}

void Physics2D::addRigidBody(RigidBody2D* body) {
}

void Physics2D::removeRigidBody(RigidBody2D* body) {
}

Physics3D& Physics3D::get() {
    static Physics3D instance;
    return instance;
}

void Physics3D::initialize(const PhysicsConfig& config) {
    this->config = config;
    initialized = true;
    Logger::info("Physics3D initialized");
}

void Physics3D::shutdown() {
    initialized = false;
}

void Physics3D::update(float deltaTime) {
    if (!initialized) return;
    
    Vector3 gravity = {config.gravityX, config.gravityY, config.gravityZ};
}

void Physics3D::setGravity(const Vector3& gravity) {
    config.gravityX = gravity.x;
    config.gravityY = gravity.y;
    config.gravityZ = gravity.z;
}

Vector3 Physics3D::getGravity() const {
    return {config.gravityX, config.gravityY, config.gravityZ};
}

} // namespace Engine
