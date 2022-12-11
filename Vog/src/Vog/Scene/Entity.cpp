#include "vogpch.h"
#include "Entity.h"

namespace vog {
    Entity::Entity(ecs::EntityHandle handle_, Scene* pScene_)
        :
        m_pScene(pScene_), m_handle(handle_)
    {
    }
    Entity::Entity(ecs::EntityHandle handle_, Entity other_)
        :
        m_pScene(other_.m_pScene), m_handle(handle_)
    {

    }
    //Entity::Entity(ecs::EntityHandle handle_)
    //    :
    //    m_handle(handle_)
    //{
    //}
}