#pragma once
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>
namespace glm
{
void to_json(nlohmann::json& j, const glm::mat4& v);
void from_json(const nlohmann::json& j, glm::mat4& v);
} // namespace glm

namespace Engine
{
struct AssetObjectMeta;
void to_json(nlohmann::json& j, const Engine::AssetObjectMeta& v);
void from_json(const nlohmann::json& j, Engine::AssetObjectMeta& v);
} // namespace Engine
