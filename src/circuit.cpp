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
    m_viewport = get_node<SubViewport>("..");
}

void Circuit::_process(double delta)
{
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
        case Tool::MOVE:
            move_click(mouse_pos);
            break;
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
}

Vector2i Circuit::get_grid_pos(Vector2 pos) const
{
    return Vector2i((pos + Vector2(0.5 * m_grid_size, 0.5 * m_grid_size)) / m_grid_size) * m_grid_size;
}
uint64_t Circuit::get_con_key(Vector2i pos) const
{
    return static_cast<uint64_t>(static_cast<uint32_t>(pos.x)) | static_cast<uint64_t>(static_cast<uint32_t>(pos.y)) << 32;
}
Vector2i Circuit::get_pos_from_con_key(uint64_t key) const
{
    return Vector2i(static_cast<uint32_t>(key), static_cast<uint32_t>(key >> 32));
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

void Circuit::move_click(Vector2 pos)
{
    Part* part = get_part(pos);
    if(part)
        PRT("click part " << part->get_type());

    prt(get_grid_pos(pos));
    for(auto [itr, range_end] = m_connectors.equal_range(get_con_key(get_grid_pos(pos))); itr != range_end; ++itr)
        PRT("click connector " << itr->second->part);
}

void Circuit::add_part(Ref<godot::PackedScene> scene, Vector2 pos)
{
    if(get_part(pos))
        return;
    Part* part = Object::cast_to<Part>(scene->instantiate());
    part->set_position(get_grid_pos(pos));

    part->size    = part->get_node<Sprite2D>("Sprite")->get_texture()->get_size();
    auto children = part->get_children();
    for(int i {0}; i < children.size(); ++i) {
        Connector* connector = Object::cast_to<Connector>(children[i]);
        if(!connector)
            continue;
        connector->part = part;
        part->connectors.push_back(connector);
        uint64_t con_key = get_con_key(part->get_position() + connector->get_position());
        m_connectors.insert({con_key, connector});
        prt(part->get_position() + connector->get_position());
    }

    add_child(part);
    m_parts.insert(part);
}

void Circuit::delete_part(Vector2 pos)
{
    Part* part = get_part(pos);
    if(!part)
        return;

    m_parts.erase(part);

    for(Connector* connector: part->connectors) {
        uint64_t con_key = get_con_key(part->get_position() + connector->get_position());
        for(auto [itr, range_end] = m_connectors.equal_range(con_key); itr != range_end; ++itr)
            if(itr->second == connector)
                m_connectors.erase(itr);
    }
    remove_child(part);
}
