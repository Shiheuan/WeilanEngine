#pragma once

#include "Core/Graphics/RenderContext.hpp"
#include "Core/AssetObject.hpp"
#include "Core/GameObject.hpp"
namespace Engine
{
    class GameScene : public AssetObject
    {
        public:
            GameScene();
            ~GameScene() {}
            RefPtr<GameObject> CreateGameObject();

            const std::vector<RefPtr<GameObject>>& GetRootObjects();

            void Tick();

            void MoveGameObjectToRoot(RefPtr<GameObject> obj);
            void RemoveGameObjectFromRoot(RefPtr<GameObject> obj);
            void Deserialize(AssetSerializer& serializer, ReferenceResolver& refResolver) override;

        private:
            EDITABLE(std::vector<UniPtr<GameObject>>, gameObjects);
            std::vector<RefPtr<GameObject>> roots;

            void TickGameObject(RefPtr<GameObject> obj);
    };
}
