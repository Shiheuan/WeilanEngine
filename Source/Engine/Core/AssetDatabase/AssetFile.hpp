#pragma once

#include "Core/AssetObject.hpp"
#include <unordered_map>
#include <filesystem>
namespace Engine
{
    class AssetFile
    {
        public:
            AssetFile(AssetFile&& other) : path(other.path), root(std::exchange(other.root, nullptr)), containedAssetObjects(std::move(other.containedAssetObjects)){};
            AssetFile() : root(nullptr) {};
            AssetFile(UniPtr<AssetObject>&& root, const std::filesystem::path& path, const std::filesystem::path& relativeBase);
            ~AssetFile(){}
            RefPtr<AssetObject> GetRoot() { return root; }
            std::vector<RefPtr<AssetObject>> GetAllContainedAssets();
            const std::filesystem::path& GetPath() { return path; }
            const std::filesystem::path& GetRelativeBase() { return relativeBase; }
            std::filesystem::path GetFullPath() { return relativeBase / path; }

            void Reload(UniPtr<AssetObject>&& obj);
            std::filesystem::file_time_type GetLastWriteTime() { return lastWriteTime; }
            std::filesystem::file_time_type GetLatestWriteTime()
            {
                auto fullPath = relativeBase / path;
                return std::filesystem::last_write_time(fullPath);
            }

            void Save();
        private:

            std::filesystem::path fullPath; // relativeBase / path
            std::filesystem::path path;
            std::filesystem::path relativeBase;
            UniPtr<AssetObject> root;
            std::vector<RefPtr<AssetObject>> containedAssetObjects;
            std::filesystem::file_time_type lastWriteTime;

            void UpdateLastWriteTime();
    };
}
