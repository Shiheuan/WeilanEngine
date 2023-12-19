#pragma once
#include "Window.hpp"
#include <filesystem>
class AssetWindow : public Window
{
public:
    void SetPath(const std::filesystem::path& path)
    {
        assetRoot = path;
    };
    void Tick() override;

private:
    std::filesystem::path assetRoot;
};
