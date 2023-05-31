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
            m_parts.push_back(child);
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
        case Tool::CREATE_NMOS:
            add_part(m_nmos_scene, mouse_pos);
            break;
        case Tool::CREATE_PMOS:
            add_part(m_pmos_scene, mouse_pos);
            break;
        }
    }
}

void Circuit::move_click(Vector2 mouse_pos)
{
    for(Part* part: m_parts) {
        if(!part->is_inside(mouse_pos))
            continue;
        PRT("click part " << part->get_type());
        Connector* connector = part->get_clicked_connector(mouse_pos);
        if(connector)
            PRT("click connector " << connector->get_type());
    }
}

void Circuit::add_part(Ref<godot::PackedScene> scene, Vector2 mouse_pos)
{
    Part* part = Object::cast_to<Part>(scene->instantiate());
    add_child(part);
    m_parts.push_back(part);
    part->set_position(mouse_pos);
}
