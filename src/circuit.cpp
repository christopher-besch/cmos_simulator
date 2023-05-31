#include "circuit.h"
#include "helper.h"

#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/sprite2d.hpp>

#include <bitset>

using namespace godot;

Circuit::Circuit()
{
    ResourceLoader* relo = ResourceLoader::get_singleton();
    m_nmos_scene         = relo->load("res://nmos.tscn");
    m_pmos_scene         = relo->load("res://pmos.tscn");
}
Circuit::~Circuit()
{
}

void Circuit::_ready()
{
    auto children = get_children();
    for(int i {0}; i < children.size(); ++i) {
        Part* child = Object::cast_to<Part>(children[i]);
        if(child)
            m_parts.insert(child);
    }
}

void Circuit::_process(double delta)
{
    if(m_moving_part) {
        Vector2 new_pos = to_grid_pos(get_local_mouse_position() - m_moving_part_grab);
        Vector2 delta   = m_moving_part->get_position();
        m_moving_part->set_position(new_pos);
        // TODO: move connectors
    }
}

void Circuit::_input(const Ref<InputEvent>& event)
{
    Input*  input  = Input::get_singleton();
    Engine* engine = Engine::get_singleton();
    if(engine->is_editor_hint())
        return;
    if(event->is_action_pressed("click")) {
        Vector2 mouse_pos = get_local_mouse_position();
        switch(m_tool) {
        case Tool::MOVE: {
            prt(to_grid_pos(mouse_pos));
            auto [itr, range_end] = m_connectors.equal_range(to_grid_pos(mouse_pos));
            if(itr != range_end)
                for(; itr != range_end; ++itr)
                    PRT("click connector " << itr->second->get_parent() << " " << static_cast<int>(itr->second->type));
            else {
                Part* part = get_part(mouse_pos);
                if(part) {
                    m_moving_part      = part;
                    m_moving_part_grab = part->get_local_mouse_position();
                }
            }
        } break;
        case Tool::DELETE:
            delete_part(mouse_pos);
            break;
        case Tool::CREATE_NMOS:
            add_part(m_nmos_scene, mouse_pos);
            break;
        case Tool::CREATE_PMOS:
            add_part(m_pmos_scene, mouse_pos);
            break;
        }
    }
    else if(event->is_action_released("click") && m_tool == Tool::MOVE)
        m_moving_part = nullptr;
}

Vector2i Circuit::to_grid_pos(Vector2i pos) const
{
    return Vector2i(pos / m_grid_size) * m_grid_size;
}

bool Circuit::is_part_clicked(Part* part, Vector2 pos) const
{
    Vector2 upper_left   = part->get_position() - 0.5 * part->size;
    Vector2 bottom_right = part->get_position() + 0.5 * part->size;
    if(pos.x < upper_left.x || pos.y < upper_left.y)
        return false;
    if(pos.x > bottom_right.x || pos.y > bottom_right.y)
        return false;
    return true;
}

Part* Circuit::get_part(Vector2 pos)
{
    for(Part* part: m_parts)
        if(is_part_clicked(part, pos))
            return part;
    return nullptr;
}

void Circuit::add_part(Ref<godot::PackedScene> scene, Vector2 pos)
{
    if(get_part(pos))
        return;
    Part* part = Object::cast_to<Part>(scene->instantiate());
    part->set_position(to_grid_pos(pos));
    add_child(part);

    part->size    = part->get_node<Sprite2D>("Sprite")->get_texture()->get_size();
    auto children = part->get_children();
    for(int i {0}; i < children.size(); ++i) {
        Connector* connector = Object::cast_to<Connector>(children[i]);
        if(!connector)
            continue;
        part->connectors.push_back(connector);

        Node2D* parent = Object::cast_to<Node2D>(connector->get_parent());
        prt(parent->get_position() + connector->get_position());
        prt(to_local(connector->get_global_position()));
        prt(part->get_position() + connector->get_position());
        prt(to_grid_pos(part->get_position() + connector->get_position()));

        auto [itra, enda] = m_connectors.equal_range(connector);
        if(itra == enda)
            PRT("buh");
        else
            PRT("yay");
        m_connectors.insert(connector);

        auto [itr, end] = m_connectors.equal_range(connector);
        if(itr == end)
            PRT("buh");
        else
            PRT("yay");
    }

    m_parts.insert(part);
}

void Circuit::delete_part(Vector2 pos)
{
    Part* part = get_part(pos);
    if(!part)
        return;

    m_parts.erase(part);

    for(Connector* connector: part->connectors)
        m_connectors.erase(connector);
    remove_child(part);
}