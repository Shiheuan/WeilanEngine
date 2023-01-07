#include "RenderGraph.hpp"

namespace Engine::RGraph
{
bool RenderGraph::Compile()
{
    std::sort(nodes.begin(), nodes.end(), [](auto& left, auto& right) { return left->GetDepth() < right->GetDepth(); });

    ResourceStateTrack stateTrack;
    for (auto& n : nodes)
    {
        if (n->Preprocess(stateTrack) == false) return false;
    }

    for (auto& n : nodes)
    {
        if (n->Compile(stateTrack) == false) return false;
    }

    return true;
}
bool RenderGraph::Execute(CommandBuffer* cmdBuf, RGraph::ResourceStateTrack& stateTrack)
{
    // handle memory depenednecy before the node is executed
    for (auto& n : nodes)
    {
        if (n->Execute(cmdBuf, stateTrack) == false) return false;
    }

    return true;
}
} // namespace Engine::RGraph