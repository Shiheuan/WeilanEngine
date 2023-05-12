#include "GameScene.hpp"
namespace Engine
{
GameScene::GameScene() : Resource()
{
    name = "New GameScene";
}

RefPtr<GameObject> GameScene::CreateGameObject()
{
    UniPtr<GameObject> newObj = MakeUnique<GameObject>(this);
    gameObjects.push_back(std::move(newObj));
    RefPtr<GameObject> refObj = gameObjects.back();
    roots.push_back(refObj);
    return refObj;
}

void GameScene::AddGameObject(GameObject* newGameObject)
{
    newGameObject->SetGameScene(this);
    roots.push_back(newGameObject);
    externalGameObjects.push_back(newGameObject);
}

const std::vector<RefPtr<GameObject>>& GameScene::GetRootObjects() { return roots; }

void GameScene::Tick()
{
    for (auto obj : roots)
    {
        TickGameObject(obj);
    }
}

void GameScene::MoveGameObjectToRoot(RefPtr<GameObject> obj) { roots.push_back(obj); }

void GameScene::RemoveGameObjectFromRoot(RefPtr<GameObject> obj)
{
    auto it = roots.begin();
    while (it != roots.end())
    {
        if (*it == obj)
        {
            roots.erase(it);
            return;
        }
        it += 1;
    }
}

void GameScene::TickGameObject(RefPtr<GameObject> obj)
{
    obj->Tick();

    for (auto child : obj->GetTransform()->GetChildren())
    {
        TickGameObject(child->GetGameObject());
    }
}

void GetLights(RefPtr<Transform> tsm, std::vector<RefPtr<Light>>& lights)
{
    for (auto& child : tsm->GetChildren())
    {
        GetLights(child, lights);
    }

    auto light = tsm->GetGameObject()->GetComponent<Light>();
    if (light != nullptr)
    {
        lights.push_back(light);
    }
}

std::vector<RefPtr<Light>> GameScene::GetActiveLights()
{
    std::vector<RefPtr<Light>> lights;
    for (auto child : roots)
    {
        GetLights(child->GetTransform(), lights);
    }

    return lights;
}

void SerializableField<GameScene>::Serialize(GameScene* v, Serializer* s)
{
    s->Serialize(v->gameObjects);
    s->Serialize(v->externalGameObjects);
}

void SerializableField<GameScene>::Deserialize(GameScene* v, Serializer* s)
{
    s->Deserialize(v->gameObjects);
    s->Deserialize(v->externalGameObjects, [v](void* resource){
            if (auto go = static_cast<GameObject*>(resource))
            {
                go->SetGameScene(v);
                Transform* goParent = go->GetTransform()->GetParent().Get();
                if (!goParent)
                {
                    v->roots.push_back(go);
                }
            }
    });

    // find all the root object
    for (auto& obj : v->gameObjects)
    {
        obj->SetGameScene(v);
        if (obj->GetTransform()->GetParent() == nullptr)
        {
            v->roots.push_back(obj);
        }
    }

    for (auto obj : v->externalGameObjects)
    {
        if (obj)
        {
            obj->SetGameScene(v);
            if (obj->GetTransform()->GetParent() == nullptr)
            {
                v->roots.push_back(obj);
            }
        }
    }
}

// const UUID& World::Serialize(RefPtr<Serializer> serializer)
// {
//     auto& infoJson = serializer->GetInfo();
//     auto& gameObjectsJson = infoJson["members"]["gameObjects"];

//     for(auto& obj : gameObjects)
//     {
//         UUID refID = obj->Serialize(serializer);
//         gameObjectsJson.push_back(refID.ToString());
//     }

//     auto& rootsJson = infoJson["members"]["roots"];
//     for(auto root : roots)
//     {
//         rootsJson.push_back(root->GetUUID().ToString());
//     }

//     return GetUUID();
// }

// void World::Deserialize(RefPtr<Deserializer> deserializer, RefPtr<AssetDatabase> assetDatabase)
// {
//     auto& infoJson = deserializer->GetInfo();
//     auto& gameObjectsJson = infoJson["members"]["gameObjects"];
//     auto& rootsJson = infoJson["members"]["roots"];
// }
} // namespace Engine
