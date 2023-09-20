#include "Libs/Serialization/Serializable.hpp"
#include "Libs/Serialization/Serializer.hpp"
#include "Object.hpp"
#include <functional>
#include <memory>
#pragma once

namespace Engine
{

class Asset : public Object, public Serializable
{
public:
    void SetName(std::string_view name)
    {
        this->name = name;
    }
    const std::string& GetName()
    {
        return name;
    }
    virtual ~Asset(){};

    virtual void Reload(Asset&& asset)
    {
        uuid = std::move(asset.uuid);
        name = std::move(asset.name);
    }

    // asset format that is not serializable and deserializable
    virtual bool IsExternalAsset()
    {
        return false;
    }

    // return false if loading failed
    virtual bool LoadFromFile(const char* path)
    {
        return false;
    }

    void Serialize(Serializer* s) const override
    {
        s->Serialize("uuid", uuid);
        s->Serialize("name", name);
    }

    void Deserialize(Serializer* s) override
    {
        s->Deserialize("uuid", uuid);
        s->Deserialize("name", name);
    }

    virtual const char* GetExtension() = 0;

protected:
    std::string name;
};

struct AssetRegistry
{
public:
    using Extension = std::string;
    using Creator = std::function<std::unique_ptr<Asset>()>;
    static std::unique_ptr<Asset> CreateAsset(const ObjectTypeID& id);
    static std::unique_ptr<Asset> CreateAssetByExtension(const Extension& id);
    template <class T>
    static std::unique_ptr<T> CreateAsset(const ObjectTypeID& id);
    static char RegisterAsset(const ObjectTypeID& assetID, const char* ext, const Creator& creator);
    static char RegisterExternalAsset(const ObjectTypeID& assetID, const char* ext, const Creator& creator);

private:
    static std::unordered_map<ObjectTypeID, std::function<std::unique_ptr<Asset>()>>* GetAssetTypeRegistery();
    static std::unordered_map<Extension, std::function<std::unique_ptr<Asset>()>>* GetAssetExtensionRegistry();
};

template <class T>
concept IsAsset = requires { std::derived_from<T, Asset>; };

#define DECLARE_ASSET()                                                                                                \
    DECLARE_OBJECT()                                                                                                   \
public:                                                                                                                \
    const char* GetExtension() override;                                                                               \
    static const char* StaticGetExtension();                                                                           \
                                                                                                                       \
private:                                                                                                               \
    static char _register;

#define DEFINE_ASSET(Type, ObjectID, Extension)                                                                        \
    DEFINE_OBJECT(Type, ObjectID)                                                                                      \
    char Type::_register = AssetRegistry::RegisterAsset(                                                               \
        Type::StaticGetObjectTypeID(),                                                                                 \
        StaticGetExtension(),                                                                                          \
        []() { return std::unique_ptr<Asset>(new Type()); }                                                            \
    );                                                                                                                 \
    const char* Type::GetExtension()                                                                                   \
    {                                                                                                                  \
        return StaticGetExtension();                                                                                   \
    }                                                                                                                  \
    const char* Type::StaticGetExtension()                                                                             \
    {                                                                                                                  \
        return "." Extension;                                                                                          \
    }
} // namespace Engine