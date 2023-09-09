#include "GameEditor.hpp"
#include "Core/Time.hpp"
#include "GfxDriver/GfxDriver.hpp"
#include "ThirdParty/imgui/imgui_impl_sdl.h"
#include "Tools/EnvironmentBaker.hpp"
#include "Tools/GameView.hpp"
#include "WeilanEngine.hpp"
#include "spdlog/spdlog.h"
#include <unordered_map>

namespace Engine::Editor
{
class EnvironmentBaker;

GameEditor::GameEditor(const char* path)
{
    engine = std::make_unique<WeilanEngine>();
    engine->Init({.projectPath = path});

    ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true;

    gameEditorRenderer = std::make_unique<Editor::Renderer>();

    auto [editorRenderNode, editorRenderNodeOutputHandle] = gameEditorRenderer->BuildGraph();
    gameEditorRenderer->Process(editorRenderNode, editorRenderNodeOutputHandle);

    editorCameraGO = std::make_unique<GameObject>();
    editorCameraGO->SetName("editor camera");
    editorCamera = editorCameraGO->AddComponent<Camera>();

    toolList.emplace_back(new EnvironmentBaker());

    for (auto& t : toolList)
    {
        RegisteredTool rt;
        rt.tool = t.get();
        rt.isOpen = false;
        registeredTools.push_back(rt);
    }

    uint32_t mainQueueFamilyIndex = GetGfxDriver()->GetQueue(QueueType::Main)->GetFamilyIndex();
};

GameEditor::~GameEditor()
{
    engine->gfxDriver->WaitForIdle();
}

void SceneTree(Transform* transform)
{
    if (ImGui::TreeNode(transform->GetGameObject()->GetName().c_str()))
    {
        GameObject* go = transform->GetGameObject();
        for (auto& c : go->GetComponents())
        {
            if (c->GetName() == "Transform")
            {
                auto pos = transform->GetPosition();
                if (ImGui::InputFloat3("Position", &pos[0]))
                {
                    transform->SetPosition(pos);
                }

                auto rotation = transform->GetRotation();
                if (ImGui::InputFloat3("rotation", &rotation[0]))
                {
                    transform->SetRotation(rotation);
                }
            }
            if (c->GetName() == "Light")
            {
                Light* light = static_cast<Light*>(c.Get());
                float intensity = light->GetIntensity();
                ImGui::DragFloat("intensity", &intensity);
                light->SetIntensity(intensity);
            }
        }

        for (auto child : transform->GetChildren())
        {
            SceneTree(child.Get());
        }
        ImGui::TreePop();
    }
}

static void SceneTree(Scene& scene)
{
    ImGui::Begin("Scene");
    for (auto root : scene.GetRootObjects())
    {
        SceneTree(root->GetTransform().Get());
    }
    ImGui::End();
}

void MenuVisitor(std::vector<std::string>::iterator iter, std::vector<std::string>::iterator end, bool& clicked)
{
    if (iter == end)
    {
        return;
    }
    else if (iter == end - 1)
    {
        if (ImGui::MenuItem(iter->c_str()))
        {
            clicked = true;
        }
        return;
    }

    if (ImGui::BeginMenu(iter->c_str()))
    {
        iter += 1;
        MenuVisitor(iter, end, clicked);
        ImGui::EndMenu();
    }
}

void GameEditor::MainMenuBar()
{
    ImGui::BeginMainMenuBar();
    if (ImGui::MenuItem("Editor Camera"))
    {
        // gameCamera = scene->GetMainCamera();
        // scene->SetMainCamera(editorCamera);
    }
    if (ImGui::MenuItem("Game Camera"))
    {
        // scene->SetMainCamera(gameCamera);
        gameCamera = nullptr;
    }

    static bool openSceneWindow = false;
    if (ImGui::BeginMenu("Assets"))
    {
        if (ImGui::MenuItem("Open Scene"))
        {
            openSceneWindow = !openSceneWindow;
        }
        ImGui::EndMenu();
    }
    if (openSceneWindow)
    {
        ImGui::Begin(
            "Open Scene...",
            nullptr,
            ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings
        );

        static char openScenePath[1024];
        ImGui::InputText("Path", openScenePath, 1024);
        if (ImGui::Button("Open"))
        {}

        ImGui::SameLine();

        if (ImGui::Button("Close"))
        {}

        ImGui::End();
    }

    for (auto& registeredTool : registeredTools)
    {
        int index = 0;
        auto items = registeredTool.tool->GetToolMenuItem();
        bool clicked = false;
        MenuVisitor(items.begin(), items.end(), clicked);
        if (clicked)
        {
            registeredTool.isOpen = !registeredTool.isOpen;
            if (registeredTool.isOpen)
                registeredTool.tool->Open();
            else
                registeredTool.tool->Close();
        }

        if (registeredTool.isOpen)
        {
            registeredTool.isOpen = registeredTool.tool->Tick();
            if (!registeredTool.isOpen)
            {
                registeredTool.tool->Close();
            }
        }
    }

    if (ImGui::BeginMenu("Scene"))
    {
        if (ImGui::BeginMenu("Tools"))
        {
            if (ImGui::MenuItem("Scene Tree"))
                sceneTree = !sceneTree;
            ImGui::EndMenu();
        }
        if (ImGui::MenuItem("Scene Info"))
            sceneInfo = !sceneInfo;

        ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
}

static void EditorCameraWalkAround(Camera& editorCamera)
{
    auto tsm = editorCamera.GetGameObject()->GetTransform();
    auto pos = tsm->GetPosition();
    glm::mat4 model = tsm->GetModelMatrix();
    glm::vec3 right = glm::normalize(model[0]);
    glm::vec3 up = glm::normalize(model[1]);
    glm::vec3 forward = glm::normalize(model[2]);

    ImGui::Begin("Test");
    static float testSpeed = 100;
    ImGui::DragFloat("Speed", &testSpeed);
    auto camPos = tsm->GetPosition();
    if (ImGui::InputFloat3("Editor Cam Pos", &camPos[0]))
    {
        tsm->SetPosition(camPos);
    }

    if (ImGui::Button("Reset"))
    {
        editorCamera.GetGameObject()->GetTransform()->SetRotation({0, 0, 0});
    }
    ImGui::End();

    if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) || ImGui::IsAnyItemHovered())
    {
        return;
    }

    float speed = 10 * Time::DeltaTime();
    glm::vec3 dir = glm::vec3(0);
    if (ImGui::IsKeyDown(ImGuiKey_D))
    {
        dir += right * speed;
    }
    if (ImGui::IsKeyDown(ImGuiKey_A))
    {
        dir -= right * speed;
    }
    if (ImGui::IsKeyDown(ImGuiKey_W))
    {
        dir -= forward * speed;
    }
    if (ImGui::IsKeyDown(ImGuiKey_S))
    {
        dir += forward * speed;
    }
    if (ImGui::IsKeyDown(ImGuiKey_E))
    {
        dir += up * speed;
    }
    if (ImGui::IsKeyDown(ImGuiKey_Q))
    {
        dir -= up * speed;
    }
    pos += dir;
    tsm->SetPosition(pos);

    static ImVec2 lastMouseDelta = ImVec2(0, 0);
    if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
    {
        auto rotation = tsm->GetRotationQuat();
        auto mouseLastClickDelta = ImGui::GetMouseDragDelta(0, 0);
        glm::vec2 mouseDelta = {mouseLastClickDelta.x - lastMouseDelta.x, mouseLastClickDelta.y - lastMouseDelta.y};
        lastMouseDelta = mouseLastClickDelta;
        auto upDown = glm::radians(mouseDelta.y * testSpeed) * Time::DeltaTime();
        auto leftRight = glm::radians(mouseDelta.x * testSpeed) * Time::DeltaTime();

        auto eye = tsm->GetPosition();
        auto lookAtDelta = leftRight * right - upDown * up;
        auto final = glm::lookAt(eye, eye - forward + lookAtDelta, glm::vec3(0, 1, 0));
        tsm->SetModelMatrix(glm::inverse(final));
    }
    else
    {
        lastMouseDelta = ImVec2(0, 0);
    }
}

void GameEditor::OnWindowResize(int32_t width, int32_t height)
{
    editorCamera->SetProjectionMatrix(glm::radians(45.0f), width / (float)height, 0.01f, 1000.f);
}

void GameEditor::Start()
{
    while (true)
    {
        engine->BeginFrame();

        if (engine->event->GetWindowClose().state)
        {
            return;
        }
        if (engine->event->GetWindowSizeChanged().state)
        {
            auto [editorRenderNode, editorRenderNodeOutputHandle] = gameEditorRenderer->BuildGraph();
            gameEditorRenderer->Process(editorRenderNode, editorRenderNodeOutputHandle);
        }

        GUIPass();

        auto& cmd = engine->GetActiveCmdBuffer();
        Render(cmd);

        engine->EndFrame();
    }
}

void GameEditor::GUIPass()
{
    MainMenuBar();
    bool isEditorCameraActive = gameCamera != nullptr;
    if (isEditorCameraActive)
        EditorCameraWalkAround(*editorCamera);

    Scene* scene = nullptr;

    if (scene)
    {
        if (sceneTree)
            SceneTree(*scene);
    }
}

void GameEditor::Render(Gfx::CommandBuffer& cmd)
{
    // make sure we don't have sync issue with game rendering
    Gfx::GPUBarrier barrier{
        .buffer = nullptr,
        .image = nullptr,
        .srcStageMask = Gfx::PipelineStage::All_Commands,
        .dstStageMask = Gfx::PipelineStage::All_Commands,
        .srcAccessMask = Gfx::AccessMask::Memory_Read | Gfx::AccessMask::Memory_Write,
        .dstAccessMask = Gfx::AccessMask::Memory_Read | Gfx::AccessMask::Memory_Write,
    };
    cmd.Barrier(&barrier, 1);

    for (auto& t : registeredTools)
    {
        if (t.isOpen)
            t.tool->Render(cmd);
    }

    // make sure we don't have  sync issue with tool rendering
    cmd.Barrier(&barrier, 1);

    gameEditorRenderer->Execute(cmd);
}

void GameEditor::OpenWindow() {}
} // namespace Engine::Editor