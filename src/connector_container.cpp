#include "connector_container.h"
#include "godot_cpp/classes/object.hpp"
#include "helper.h"

using namespace godot;

void ConnectorContainer::insert(Vector2i pos, Connector* connector)
{
    m_connectors.insert({pos, connector});
}
void ConnectorContainer::insert(Connector* connector)
{
    insert(to_pos(connector), connector);
}

void ConnectorContainer::erase(Vector2i pos, Connector* connector)
{
    for(auto [itr, range_end] = equal_range(pos); itr != range_end; ++itr)
        if(itr->second == connector) {
            m_connectors.erase(itr);
            return;
        }
}
void ConnectorContainer::erase(Connector* connector)
{
    erase(to_pos(connector), connector);
}

Vector2i ConnectorContainer::to_pos(Connector* connector)
{
    Node2D* parent = Object::cast_to<Node2D>(connector->get_parent());
    return parent->get_position() + connector->get_position();
}
