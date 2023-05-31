#include "part.h"
#include "connector.h"
#include "godot_cpp/classes/sprite2d.hpp"
#include "helper.h"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/input_event_mouse_button.hpp>
#include <godot_cpp/classes/os.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void Part::_bind_methods()
{
    BIND_ATR(Part, type, INT);
}

Part::Part()
{
}
Part::~Part()
{
}

void Part::_ready()
{
    auto children = get_children();
    for(int i {0}; i < children.size(); ++i) {
        Connector* child = Object::cast_to<Connector>(children[i]);
        if(child)
            m_connectors.push_back(child);
    }

    Input*  input  = Input::get_singleton();
    Engine* engine = Engine::get_singleton();
    if(engine->is_editor_hint())
        return;

    m_size = get_node<Sprite2D>("Sprite")->get_texture()->get_size();
}

bool Part::is_inside(Vector2 pos) const
{
    Vector2 upper_left   = get_position() - 0.5 * m_size;
    Vector2 bottom_right = get_position() + 0.5 * m_size;
    if(pos.x < upper_left.x || pos.y < upper_left.y)
        return false;
    if(pos.x > bottom_right.x || pos.y > bottom_right.y)
        return false;
    return true;
}

Connector* Part::get_clicked_connector(Vector2 pos)
{
    Vector2 upper_left = get_position() - 0.5 * m_size;
    for(Connector* connector: m_connectors)
        if(((get_position() + connector->get_position()) - pos).length_squared() <= m_min_dist_sqrd)
            return connector;
    return nullptr;
}
