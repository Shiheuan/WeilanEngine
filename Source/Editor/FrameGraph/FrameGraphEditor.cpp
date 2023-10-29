#include "FrameGraphEditor.hpp"
#include "ThirdParty/imgui/GraphEditor.h"
#include <spdlog/spdlog.h>
namespace Engine::Editor
{
namespace ed = ax::NodeEditor;
namespace fg = FrameGraph;

void FrameGraphEditor::Init()
{
    ax::NodeEditor::Config config;
    config.SettingsFile = "Frame Graph Editor.json";

    graphContext = ax::NodeEditor::CreateEditor(&config);

    testGraph = std::make_unique<FrameGraph::Graph>();
    graph = testGraph.get();
}

void FrameGraphEditor::Destory()
{
    ax::NodeEditor::DestroyEditor(graphContext);
}

void FrameGraphEditor::Draw()
{
    ed::SetCurrentEditor(graphContext);
    auto cursorPosition = ImGui::GetCursorScreenPos();

    ImGui::Begin("Frame Graph Editor");
    ed::Begin("Frame Graph");
    {
        int uniqueId = 1;
        if (graph)
        {
            for (std::unique_ptr<FrameGraph::Node>& node : graph->GetNodes())
            {
                ed::BeginNode(node->GetID());
                ImGui::Text("%s", node->GetName().c_str());
                for (fg::Property& input : node->GetInput())
                {
                    DrawProperty(input, ed::PinKind::Input);
                }

                for (fg::Property& input : node->GetOutput())
                {
                    DrawProperty(input, ed::PinKind::Output);
                }
                ed::EndNode();
            }
        }

        if (ed::BeginCreate())
        {
            ed::PinId inputPinId, outputPinId;
            if (ed::QueryNewLink(&inputPinId, &outputPinId))
            {
                if (inputPinId && outputPinId)
                {
                    if (ed::AcceptNewItem())
                    {
                        if (!graph->Connect(inputPinId.Get(), outputPinId.Get()))
                        {
                            ed::RejectNewItem();
                        }
                    }
                }
            }
            ed::EndCreate();
        }

        if (ed::ShowBackgroundContextMenu())
        {
            ed::Suspend();
            ImGui::OpenPopup("Create New Node");
            ed::Resume();
        }

        // make links
        for (auto c : graph->GetConnections())
        {
            ed::Link(c.id, c.src, c.dst);
        }
    }

    ImGui::SetCursorScreenPos(cursorPosition);
    // content
    ed::Suspend();
    if (ImGui::BeginPopup("Create New Node"))
    {
        auto popupMousePos = ImGui::GetMousePosOnOpeningCurrentPopup();
        for (fg::NodeBlueprint& bp : fg::NodeBlueprintRegisteration::GetNodeBlueprints())
        {
            if (ImGui::MenuItem(bp.GetName().c_str()))
            {
                FrameGraph::Node& node = graph->AddNode(bp);
                ed::SetNodePosition(node.GetID(), ed::ScreenToCanvas(popupMousePos));
            }
        }
        ImGui::EndPopup();
    }
    ed::Resume();

    ed::End();
    ed::SetCurrentEditor(nullptr);
    ImGui::End();
}

void FrameGraphEditor::ShowFloatProp(FrameGraph::Property& p)
{
    float* v = (float*)p.GetData();
    if (ImGui::InputFloat("", v))
    {}
}

void FrameGraphEditor::DrawProperty(FrameGraph::Property& p, ax::NodeEditor::PinKind kind)
{
    ed::BeginPin(p.GetID(), kind);
    ImGui::Text("%s", p.GetName().c_str());
    if (p.GetType() == fg::PropertyType::Float)
        ShowFloatProp(p);
    ed::EndPin();
}
} // namespace Engine::Editor
