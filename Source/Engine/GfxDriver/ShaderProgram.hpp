#pragma once

#include "Libs/Ptr.hpp"
#include "ShaderConfig.hpp"
#include "ShaderInfo.hpp"
#include <string>
#include <vector>
namespace Engine::Gfx
{
struct ShaderResourceLayout
{
    std::string name;
    uint32_t size;
    uint32_t offset;
};

class ShaderProgram
{
public:
    virtual ~ShaderProgram(){};
    virtual const ShaderConfig& GetDefaultShaderConfig() = 0;
    virtual const std::string& GetName() = 0;
    virtual const ShaderInfo::ShaderInfo& GetShaderInfo() = 0;

protected:
};
} // namespace Engine::Gfx
