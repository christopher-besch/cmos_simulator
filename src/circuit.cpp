#include "circuit.h"
#include "helper.h"

#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/resource_loader.hpp>

using namespace godot;

void Circuit::_bind_methods()
{
    BIND_ATR(Circuit, tool, INT);
}

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

void Circuit::move_click(Vector2 pos)
{
    Part* part = get_part(pos);
    if(!part)
        return;
    PRT("click part " << part->get_type());
    Connector* connector = part->get_clicked_connector(pos);
    if(connector)
        PRT("click connector " << connector->get_type());
}

Part* Circuit::get_part(Vector2 pos)
{
    for(Part* part: m_parts)
        if(part->is_inside(pos))
            return part;
    return nullptr;
}

void Circuit::add_part(Ref<godot::PackedScene> scene, Vector2 pos)
{
    if(get_part(pos))
        return;
    Part* part = Object::cast_to<Part>(scene->instantiate());
    add_child(part);
    m_parts.insert(part);
    part->set_position(pos);
}

void Circuit::delete_part(Vector2 pos)
{
    Part* part = get_part(pos);
    if(!part)
        return;
    m_parts.erase(part);
    remove_child(part);
}
