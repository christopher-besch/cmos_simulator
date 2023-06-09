#include "circuit.h"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/input_event_mouse_motion.hpp>
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
    m_cursor     = get_node<Node2D>("Cursor");
    Input* input = Input::get_singleton();
    input->set_use_accumulated_input(false);
}

void Circuit::_process(double delta)
{
    Vector2 mouse_pos = get_local_mouse_position();
    m_cursor->set_position(mouse_to_grid(mouse_pos));

    if(m_moving_part) {
        Vector2i new_pos = mouse_to_grid(mouse_pos - m_moving_part_grab);
        move_part(m_moving_part, new_pos);
    }
    else if(m_new_cable) {
        Vector2i delta = mouse_to_grid(mouse_pos) - m_new_cable->get_position();
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

    Vector2              mouse_pos   = get_local_mouse_position();
    Ref<InputEventMouse> mouse_event = event;
    if(!mouse_event.is_null())
        m_cursor->set_position(mouse_to_grid(mouse_pos));

    if(event->is_action_pressed("click")) {
        switch(m_tool) {
        case Tool::MOVE: {
            bool found {false};
            auto [itr, range_end] = m_connectors.equal_range_square(mouse_to_grid(mouse_pos), m_grid_size);
            for(; itr != range_end; ++itr) {
                // only use cables that have length != 0 and are not for a part
                // or cables that have length == 0 and are for a part
                // -> ignore cables that are for parts but not of length 0
                if((itr->second->for_part != nullptr) ==
                   (itr->second->get_point_position(0) == itr->second->get_point_position(1))) {
                    m_new_cable = Object::cast_to<Cable>(m_cable_scene->instantiate());
                    m_new_cable->set_position(itr->first);
                    add_child(m_new_cable);
                    found = true;
                    break;
                }
            }
            if(!found) {
                Part* part = get_part(mouse_pos);
                if(part) {
                    m_moving_part      = part;
                    m_moving_part_grab = mouse_pos - part->get_position();
                }
            }
            break;
        }
        case Tool::DELETE: {
            Part* part = get_part(mouse_pos);
            if(part) {
                delete_part(part);
                break;
            }
            auto [itr, range_end] = m_connectors.equal_range(mouse_to_grid(mouse_pos));
            if(itr != range_end && !itr->second->for_part)
                delete_cable(itr->second);
            break;
        }
        case Tool::ROTATE: {
            Part* part = get_part(mouse_pos);
            if(part) {
                for(Cable* cable: part->get_cables())
                    untrack_cable(cable);
                part->rotate(M_PI / 2);
                for(Cable* cable: part->get_cables())
                    track_cable(cable);
            }
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
            if(m_new_cable->get_point_position(0) == m_new_cable->get_point_position(1)) {
                remove_child(m_new_cable);
                m_new_cable->queue_free();
            }
            else
                track_cable(m_new_cable);
        }
        m_new_cable   = nullptr;
        m_moving_part = nullptr;
    }
}

Vector2i Circuit::mouse_to_grid(Vector2i pos) const
{
    pos += 0.5 * m_grid_size * sgn(pos);
    Vector2i grid_pos = Vector2i(pos / m_grid_size) * m_grid_size;
    return grid_pos;
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

Part* Circuit::get_part(Vector2 pos) const
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
    part->set_position(mouse_to_grid(pos));
    add_child(part);

    part->size = part->get_node<Sprite2D>("Sprite")->get_texture()->get_size();
    for(Cable* cable: part->get_cables())
        track_cable(cable);
    m_parts.insert(part);
}

void Circuit::delete_part(Part* part)
{
    m_parts.erase(part);

    for(Cable* cable: part->get_cables())
        untrack_cable(cable);
    remove_child(part);
    part->queue_free();
}

void Circuit::move_part(Part* part, Vector2i new_pos)
{
    if(new_pos == part->get_position())
        return;
    for(Cable* cable: part->get_cables())
        untrack_cable(cable);
    part->set_position(new_pos);
    for(Cable* cable: part->get_cables())
        track_cable(cable);
}

void Circuit::track_cable(Cable* cable)
{
    // round in case of rotation
    Vector2i start = cable->to_global(cable->get_point_position(0)).round();
    Vector2i end   = cable->to_global(cable->get_point_position(1)).round();
    Vector2i step  = m_grid_size * sgn(end - start);

    if(step == Vector2i(0, 0)) {
        m_connectors.insert(start, cable);
        return;
    }
    for(Vector2i pos {start}; pos != end + step; pos += step)
        m_connectors.insert(pos, cable);
}

void Circuit::untrack_cable(Cable* cable)
{
    Vector2i start = cable->to_global(cable->get_point_position(0)).round();
    Vector2i end   = cable->to_global(cable->get_point_position(1)).round();
    Vector2i step  = m_grid_size * sgn(end - start);

    if(step == Vector2i(0, 0)) {
        m_connectors.erase(start, cable);
        return;
    }
    for(Vector2i pos {start}; pos != end + step; pos += step)
        m_connectors.erase(pos, cable);
}

void Circuit::delete_cable(Cable* cable)
{
    untrack_cable(cable);
    remove_child(cable);
    cable->queue_free();
}

Vector2i Circuit::sgn(Vector2 vec) const
{
    return Vector2i(
        (0 < vec.x) - (vec.x < 0),
        (0 < vec.y) - (vec.y < 0));
}
