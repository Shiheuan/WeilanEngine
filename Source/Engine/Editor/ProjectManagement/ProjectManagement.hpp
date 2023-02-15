#pragma once

#include "Libs/Ptr.hpp"
#include "Libs/UUID.hpp"
#include <filesystem>
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>

namespace Engine
{
class GameScene;
}
namespace Engine::Editor
{
std::filesystem::path GetSysConfigPath();
class ProjectManagement
{
public:
    enum class ResultCode
    {
        Success,
        FilePathError
    };

    UUID GetLastActiveScene() { return gameProj.value("lastActiveScene", UUID::empty.ToString()); }
    ResultCode CreateNewProject(const std::filesystem::path& path);
    bool IsInitialized() { return initialized; }
    std::vector<std::filesystem::path> GetProjectLists();
    static std::filesystem::path GetInternalRootPath();
    void SetLastActiveScene(RefPtr<GameScene> scene);
    void Save();
    glm::vec3 GetLastEditorCameraPos();
    void GetLastEditorCameraRotation(float& theta, float& phi);
    void SetLastEditorCameraPos(glm::vec3 pos);
    void SetLastEditorCameraRotation(float theta, float phi);
    ProjectManagement::ResultCode LoadProject(const std::filesystem::path& root);

    static RefPtr<ProjectManagement> instance;

private:
    void InitializeProject(const std::filesystem::path& root);
    bool initialized = false;
    nlohmann::json gameProj;
};
} // namespace Engine::Editor
