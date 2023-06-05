#include "circuit.h"
#include "cable.h"
#include "helper.h"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/line2d.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/sprite2d.hpp>

using namespace godot;

Circuit::Circuit()
{
    ResourceLoader* relo = ResourceLoader::get_singleton();
    m_nmos_scene         = relo->load("res://nmos.tscn");
    m_pmos_scene         = relo->load("res://pmos.tscn");
    m_cable_scene        = relo->load("res://cable.tscn");
}
Circuit::~Circuit()
{
}

void Circuit::_ready()
{
}

void Circuit::_process(double delta)
{
    if(m_moving_part) {
        Vector2i new_pos = to_grid_pos(get_local_mouse_position() - m_moving_part_grab);
        move_part(m_moving_part, new_pos);
    }
    else if(m_new_cable) {
        Vector2i delta = to_grid_pos(get_local_mouse_position()) - m_new_cable->get_position();
        if(abs(delta.x) > abs(delta.y))
            delta = Vector2i(delta.x, 0);
        else
            delta = Vector2i(0, delta.y);
        m_new_cable->set_point_position(1, delta);
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
            auto [itr, range_end] = m_connectors.equal_range_square(to_grid_pos(mouse_pos), m_grid_size);
            if(itr != range_end && !itr->second->for_part) {
                m_new_cable = Object::cast_to<Cable>(m_cable_scene->instantiate());
                // doesn't work for cables attached to parts
                m_new_cable->set_position(itr->second->get_position());
                add_child(m_new_cable);
            }
            else {
                Part* part = get_part(mouse_pos);
                if(part) {
                    m_moving_part      = part;
                    m_moving_part_grab = part->get_local_mouse_position();
                }
            }
        } break;
        case Tool::DELETE: {
            Part* part = get_part(mouse_pos);
            if(part) {
                delete_part(part);
                break;
            }
            auto [itr, range_end] = m_connectors.equal_range_square(to_grid_pos(mouse_pos), m_grid_size);
            if(itr != range_end && !itr->second->for_part)
                delete_cable(itr->second);
            break;
        }
        case Tool::CREATE_NMOS:
            add_part(m_nmos_scene, mouse_pos);
            break;
        case Tool::CREATE_PMOS:
            add_part(m_pmos_scene, mouse_pos);
            break;
        }
    }
    else if(event->is_action_released("click") && m_tool == Tool::MOVE) {
        if(m_new_cable) {
            if(m_new_cable->get_point_position(1) == Vector2i(0, 0)) {
                remove_child(m_new_cable);
                m_new_cable->queue_free();
            }
            else {
                add_cable(m_new_cable);
            }
        }
        m_new_cable   = nullptr;
        m_moving_part = nullptr;
    }
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

    part->size = part->get_node<Sprite2D>("Sprite")->get_texture()->get_size();
    for(Cable* cable: part->cables)
        track_cable(cable);
    m_parts.insert(part);
}

void Circuit::delete_part(Part* part)
{
    m_parts.erase(part);

    for(Cable* cable: part->cables)
        untrack_cable(cable);
    remove_child(part);
    part->queue_free();
}

void Circuit::move_part(Part* part, Vector2i new_pos)
{
    if(new_pos == part->get_position())
        return;
    for(Cable* cable: part->cables)
        untrack_cable(cable);
    part->set_position(new_pos);
    for(Cable* cable: part->cables)
        track_cable(cable);
}

void Circuit::track_cable(Cable* cable)
{
    Vector2i origin    = to_local(cable->get_global_position());
    Vector2i local_end = cable->get_point_position(1);
    Vector2i end       = origin + local_end;

    Vector2i step = Vector2i(
        (0 < local_end.x) - (local_end.x < 0),
        (0 < local_end.y) - (local_end.y < 0));

    for(Vector2i pos {origin}; pos != end; pos += end)
        m_connectors.insert(pos, cable);
}

void Circuit::untrack_cable(Cable* cable)
{
    Vector2i origin    = to_local(cable->get_global_position());
    Vector2i local_end = cable->get_point_position(1);
    Vector2i end       = origin + local_end;

    Vector2i step = Vector2i(
        (0 < local_end.x) - (local_end.x < 0),
        (0 < local_end.y) - (local_end.y < 0));

    for(Vector2i pos {origin}; pos != end; pos += end)
        m_connectors.erase(pos, cable);
}

void Circuit::add_cable(Cable* cable)
{
    add_child(cable);
    track_cable(cable);
}
void Circuit::delete_cable(Cable* cable)
{
    untrack_cable(cable);
    remove_child(cable);
    cable->queue_free();
}
