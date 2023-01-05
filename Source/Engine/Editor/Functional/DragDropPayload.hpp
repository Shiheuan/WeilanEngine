#pragma once
#include "Libs/Ptr.hpp"
#include "Core/Object.hpp"
namespace Engine
{
    enum class PayloadType
    {
        Mesh,
    };
    struct DragDropPayload
    {
        void* data;
        PayloadType type;
    };
}