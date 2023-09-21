#include "InspectorRegistry.hpp"
#include "Inspector.hpp"

namespace Engine::Editor
{

InspectorRegistry::Registry& InspectorRegistry::GetRegistry()
{
    static Registry registry;
    return registry;
}

InspectorBase* InspectorRegistry::GetInspector(Object& obj)
{
    Registry& re = GetRegistry();
    auto iter = re.find(typeid(obj));
    if (iter != re.end())
    {
        iter->second->SetTarget(obj);
        return iter->second.get();
    }

    auto defaultInspector = re.find(typeid(Object))->second.get();
    defaultInspector->SetTarget(obj);
    return defaultInspector;
}

const char InspectorRegistry::_defaultInspector = InspectorRegistry::Register<Inspector<Object>, Object>();
} // namespace Engine::Editor
