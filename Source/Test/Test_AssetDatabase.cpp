#include "AssetDatabase/AssetDatabase.hpp"
#include <gtest/gtest.h>

namespace Engine
{
class IAmSerializable : public Resource
{
public:
    int x;
};

template <>
struct AssetFactory<IAmSerializable> : public AssetRegister
{
    inline static AssetTypeID assetTypeID = GENERATE_SERIALIZABLE_FILE_ID("IAmSerializable");
    inline static char reg =
        RegisterAsset(assetTypeID, []() { return std::unique_ptr<Resource>(new IAmSerializable()); });
};

} // namespace Engine

TEST(AssetDatabase, SaveFile)
{
    Engine::AssetDatabase adb(TEMP_FILE_DIR);

    auto v = std::make_unique<Engine::IAmSerializable>();
    Engine::IAmSerializable* vb =
        (Engine::IAmSerializable*)adb.CreateAsset(std::move(v), "serializable.asset")->GetResource();
}

TEST(AssetDatabse, LoadFile) {}
