#pragma once

#include "EngineCore.h"
#include "Rendering.h"

namespace Engine {

enum class BodyType2D {
    Dynamic, Static, Kinematic
};

struct PhysicsConfig {
    float gravityX = 0.0f;
    float gravityY = -9.81f;
    float gravityZ = 0.0f;
    int velocityIterations = 8;
    int positionIterations = 3;
    bool enableSleep = true;
};

class RigidBody2D : public Component {
public:
    RigidBody2D();
    
    void setMass(float mass) { this->mass = mass; }
    float getMass() const { return mass; }
    
    void setGravityScale(float scale) { gravityScale = scale; }
    float getGravityScale() const { return gravityScale; }
    
    void setVelocity(const Vector2& vel) { velocity = vel; }
    Vector2 getVelocity() const { return velocity; }
    
    void setAngularVelocity(float vel) { angularVelocity = vel; }
    float getAngularVelocity() const { return angularVelocity; }
    
    void applyForce(const Vector2& force);
    void applyImpulse(const Vector2& impulse);
    void applyTorque(float torque);
    
    void setLinearDamping(float damping) { linearDamping = damping; }
    float getLinearDamping() const { return linearDamping; }
    
    void setAngularDamping(float damping) { angularDamping = damping; }
    float getAngularDamping() const { return angularDamping; }
    
    void setFixedRotation(bool fixed) { fixedRotation = fixed; }
    bool isFixedRotation() const { return fixedRotation; }
    
    void setBodyType(BodyType2D type) { bodyType = type; }
    BodyType2D getBodyType() const { return bodyType; }
    
private:
    float mass = 1.0f;
    float gravityScale = 1.0f;
    Vector2 velocity;
    float angularVelocity = 0.0f;
    float linearDamping = 0.01f;
    float angularDamping = 0.01f;
    bool fixedRotation = false;
    BodyType2D bodyType = BodyType2D::Dynamic;
};

class BoxCollider2D : public Component {
public:
    BoxCollider2D();
    
    void setSize(const Vector2& size) { this->size = size; }
    Vector2 getSize() const { return size; }
    
    void setOffset(const Vector2& offset) { this->offset = offset; }
    Vector2 getOffset() const { return offset; }
    
    void setDensity(float density) { this->density = density; }
    float getDensity() const { return density; }
    
    void setFriction(float friction) { this->friction = friction; }
    float getFriction() const { return friction; }
    
    void setRestitution(float restitution) { this->restitution = restitution; }
    float getRestitution() const { return restitution; }
    
    void setTrigger(bool trigger) { m_isTrigger = trigger; }
    bool getTrigger() const { return m_isTrigger; }
    
private:
    Vector2 size = {1, 1};
    Vector2 offset = {0, 0};
    float density = 1.0f;
    float friction = 0.3f;
    float restitution = 0.0f;
    bool m_isTrigger = false;
};

class CircleCollider2D : public Component {
public:
    CircleCollider2D();
    
    void setRadius(float radius) { this->radius = radius; }
    float getRadius() const { return radius; }
    
    void setOffset(const Vector2& offset) { this->offset = offset; }
    Vector2 getOffset() const { return offset; }
    
    void setDensity(float density) { this->density = density; }
    float getDensity() const { return density; }
    
    void setFriction(float friction) { this->friction = friction; }
    float getFriction() const { return friction; }
    
    void setRestitution(float restitution) { this->restitution = restitution; }
    float getRestitution() const { return restitution; }
    
    void setTrigger(bool trigger) { m_isTrigger = trigger; }
    bool getTrigger() const { return m_isTrigger; }
    
private:
    float radius = 0.5f;
    Vector2 offset = {0, 0};
    float density = 1.0f;
    float friction = 0.3f;
    float restitution = 0.0f;
    bool m_isTrigger = false;
};

class Physics2D {
public:
    static Physics2D& get();
    
    void initialize(const PhysicsConfig& config = PhysicsConfig());
    void shutdown();
    
    void update(float deltaTime);
    
    void setGravity(const Vector2& gravity);
    Vector2 getGravity() const;
    
    void setPaused(bool paused) { this->paused = paused; }
    bool isPaused() const { return paused; }
    
    bool raycast(const Vector2& start, const Vector2& end, class RaycastHit2D& hit);
    bool circleCast(const Vector2& center, float radius, const Vector2& direction, float maxDistance, RaycastHit2D& hit);
    
    void addRigidBody(RigidBody2D* body);
    void removeRigidBody(RigidBody2D* body);
    
private:
    Physics2D() = default;
    
    PhysicsConfig config;
    bool initialized = false;
    bool paused = false;
};

class RaycastHit2D {
public:
    RigidBody2D* collider = nullptr;
    Vector2 point;
    Vector2 normal;
    float distance = 0.0f;
};

class Physics3D {
public:
    static Physics3D& get();
    
    void initialize(const PhysicsConfig& config = PhysicsConfig());
    void shutdown();
    
    void update(float deltaTime);
    
    void setGravity(const Vector3& gravity);
    Vector3 getGravity() const;
    
private:
    Physics3D() = default;
    
    PhysicsConfig config;
    bool initialized = false;
};

} // namespace Engine
