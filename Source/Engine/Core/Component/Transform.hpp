#pragma once

#include "Component.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>

namespace Engine
{
class GameObject;

enum class RotationCoordinate
{
    Self,
    Parent,
    World,
};

class Transform : public Component
{
    DECLARE_OBJECT();

public:
    Transform();
    Transform(GameObject* gameObject);
    ~Transform() override{};

    const std::vector<Transform*>& GetChildren();
    Transform* GetParent();
    void SetParent(Transform* transform);
    void SetRotation(const glm::vec3& rotation);
    void SetRotation(const glm::quat& rotation);
    void SetPosition(const glm::vec3& position);
    void SetScale(const glm::vec3& scale);
    void Rotate(float angle, glm::vec3 axis, RotationCoordinate coord);
    void Translate(const glm::vec3& translate);
    const glm::vec3& GetPosition();
    const glm::vec3& GetScale();
    const glm::vec3& GetRotation();
    glm::vec3 GetForward();

    const glm::quat& GetRotationQuat() const;

    glm::mat4 GetModelMatrix() const;
    void SetModelMatrix(const glm::mat4& model);

    void Serialize(Serializer* s) const override;
    void Deserialize(Serializer* s) override;

    std::unique_ptr<Component> Clone(GameObject& owner) override;

    const std::string& GetName() override;
    void RemoveChild(Transform* child);

private:
    glm::quat rotation;
    glm::vec3 rotationEuler;
    glm::vec3 position;
    glm::vec3 scale;

    Transform* parent = nullptr;
    std::vector<Transform*> children;
};

} // namespace Engine
