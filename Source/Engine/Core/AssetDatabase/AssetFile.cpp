#include "AssetFile.hpp"
#include <fstream>
#include <nlohmann/json.hpp>
namespace Engine
{
static void GetAssets(RefPtr<AssetObject> asset, std::vector<RefPtr<AssetObject>>& assets)
{
    auto containedAssets = asset->GetAssetObjectMembers();
    assets.insert(assets.end(), containedAssets.begin(), containedAssets.end());

    for (RefPtr<AssetObject> a : containedAssets)
    {
        GetAssets(a, assets);
    }
}
AssetFile::AssetFile(UniPtr<AssetObject>&& root,
                     const std::filesystem::path& path,
                     const std::filesystem::path& relativeBase)
    : path(path), relativeBase(relativeBase), root(std::move(root)), containedAssetObjects()
{
    fullPath = relativeBase / path;
    GetAssets(this->root, containedAssetObjects);
    UpdateLastWriteTime();
}

void AssetFile::UpdateLastWriteTime()
{
    try
    {
        lastWriteTime = std::filesystem::last_write_time(fullPath);
    }
    catch (std::filesystem::filesystem_error e)
    {
        // pass
    }
}

void AssetFile::Reload(UniPtr<AssetObject>&& obj)
{
    root->Reload(std::move(*obj));
    UpdateLastWriteTime();
}

std::vector<RefPtr<AssetObject>> AssetFile::GetAllContainedAssets() { return containedAssetObjects; }

void AssetFile::Save()
{
    auto metaPath = fullPath;
    metaPath.replace_extension(path.extension().string() + ".meta");
    std::ofstream metaFileOutput(metaPath);

    if (metaFileOutput.good())
    {
        nlohmann::json j;
        j["uuid"] = root->GetUUID().ToString();
        for (auto asset : containedAssetObjects)
        {
            j["contained"][asset->GetName()] = asset->GetUUID().ToString();
        }
        metaFileOutput << j.dump();
    }

    AssetSerializer ser;
    if (root->Serialize(ser))
    {
        ser.WriteToDisk(fullPath);
    }
}
} // namespace Engine
