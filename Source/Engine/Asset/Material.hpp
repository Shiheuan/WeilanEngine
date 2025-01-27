#pragma once

#include "Core/Asset.hpp"
#include "Core/Texture.hpp"
#include "GfxDriver/Image.hpp"
#include "GfxDriver/ShaderConfig.hpp"
#include "Shader.hpp"
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace Engine
{
namespace Gfx
{
class ShaderResource;
}

class AssetDatabase;
class AssetFileData;
class Material : public Asset
{
    DECLARE_ASSET();

public:
    Material(RefPtr<Shader> shader);
    Material();
    Material(const Material& other);
    ~Material() override;

    void SetShader(RefPtr<Shader> shader);
    Shader* GetShader()
    {
        return shader;
    }

    Gfx::ShaderProgram* GetShaderProgram();

    Gfx::ShaderResource* GetShaderResource()
    {
        return ValidateGetShaderResource();
    }

    std::unique_ptr<Asset> Clone() override;

    void SetMatrix(const std::string& param, const std::string& member, const glm::mat4& value);
    void SetFloat(const std::string& param, const std::string& member, float value);
    void SetVector(const std::string& param, const std::string& member, const glm::vec4& value);
    void SetTexture(const std::string& param, Gfx::Image* image);
    void SetTexture(const std::string& param, Texture* texture);
    void SetTexture(const std::string& param, std::nullptr_t);
    void EnableFeature(const std::string& name);
    void DisableFeature(const std::string& name);

    void UploadDataToGPU();

    glm::mat4 GetMatrix(const std::string& param, const std::string& membr);
    Texture* GetTexture(const std::string& param);
    float GetFloat(const std::string& param, const std::string& membr);
    glm::vec4 GetVector(const std::string& param, const std::string& membr);

    // const UUID& Serialize(RefPtr<AssetFileData> assetFileData) override;
    // void        Deserialize(RefPtr<AssetFileData> assetFileData, RefPtr<AssetDatabase> assetDatabase) override;

    const Gfx::ShaderConfig& GetShaderConfig()
    {
        if (overrideShaderConfig || shader == nullptr)
            return shaderConfig;
        else
            return shader->GetDefaultShaderConfig();
    }

    void SetShaderConfig(const Gfx::ShaderConfig& shaderConfig)
    {
        overrideShaderConfig = true;
        this->shaderConfig = shaderConfig;
    }

    void Serialize(Serializer* s) const override;
    void Deserialize(Serializer* s) override;

private:
    struct UBO
    {
        bool dirty = false;
        std::unique_ptr<Gfx::Buffer> buffer;
        std::unordered_map<std::string, float> floats;
        std::unordered_map<std::string, glm::vec4> vectors;
        std::unordered_map<std::string, glm::mat4> matrices;

        void Serialize(Serializer* ser) const;
        void Deserialize(Serializer* ser);
    };

    union UpdateVal
    {
        float f;
        glm::vec4 vec;
        glm::mat4 mat;
    };

    struct ScheduledUpdate
    {
        size_t offset;
        size_t size;
        UpdateVal val;
    };

    struct Schedule
    {
        bool scheduled;
        std::vector<ScheduledUpdate> updates;
    };

    Shader* shader = nullptr;
    uint32_t shaderContentHash = -1;
    std::unique_ptr<Gfx::ShaderResource> shaderResource = nullptr;
    Gfx::ShaderConfig shaderConfig;
    Gfx::ShaderProgram* cachedShaderProgram = nullptr;
    uint64_t globalShaderFeaturesHash;
    bool overrideShaderConfig = false;

    std::unordered_map<std::string, UBO> ubos;
    std::unordered_map<std::string, Texture*> textureValues;
    std::unordered_set<std::string> enabledFeatures;
    std::unique_ptr<Gfx::Buffer> buffer;
    std::shared_ptr<Schedule> schedule;

    void SetShaderNoProtection(RefPtr<Shader> shader);
    Gfx::ShaderResource* ValidateGetShaderResource();
};
} // namespace Engine
