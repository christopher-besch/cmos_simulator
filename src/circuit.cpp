#include "circuit.h"
#include "trace_cables.h"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/input_event_mouse_motion.hpp>
#include <godot_cpp/classes/line2d.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/sprite2d.hpp>
#include <godot_cpp/variant/transform2d.hpp>

using namespace godot;

Circuit::Circuit()
{
}
Circuit::~Circuit()
{
    if(m_next_part)
        m_next_part->queue_free();
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
        Vector2i a     = m_new_cable->get_global_a();
        Vector2i delta = mouse_to_grid(mouse_pos) - a;
        if(abs(delta.x) > abs(delta.y))
            delta = Vector2i(delta.x, 0);
        else
            delta = Vector2i(0, delta.y);
        m_new_cable->set_global_b(a + delta);
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
    if(!mouse_event.is_null()) {
        if(m_select_area) {
            m_cursor->set_visible(false);
            Transform2D trans = m_select_area->get_transform();

            trans.set_origin((m_select_area_start + mouse_pos) / 2);
            Vector2 scale = mouse_pos - m_select_area_start;
            scale.x       = scale.x ? scale.x : 1;
            scale.y       = scale.y ? scale.y : 1;
            trans.set_scale(scale);
            m_select_area->set_transform(trans);
        }
        else {
            m_cursor->set_position(mouse_to_grid(mouse_pos));
            m_cursor->set_visible(true);
        }
    }

    if(event->is_action_pressed("click")) {
        switch(m_tool) {
        case Tool::MOVE: {
            bool handled {false};
            // // find cable
            // auto [itr, range_end] = m_connectors.equal_range_square(mouse_to_grid(mouse_pos), m_grid_size);
            // for(; itr != range_end; ++itr) {
            //     // only use cables that have length != 0 and are not for a part
            //     // or cables that have length == 0 and are for a part
            //     // -> ignore cables that are for parts but not of length 0
            //     if((itr->second->for_part != nullptr) ==
            //        (itr->second->get_global_a() == itr->second->get_global_b())) {
            //         m_new_cable = Object::cast_to<Cable>(m_cable_scene->instantiate());
            //         add_child(m_new_cable);
            //         m_new_cable->set_global_a(itr->first);
            //         m_new_cable->set_global_b(itr->first);
            //         handled = true;
            //         break;
            //     }
            // }
            // area selection
            if(!handled) {
                m_select_area = Object::cast_to<Area2D>(m_select_area_scene->instantiate());
                add_child(m_select_area);
                m_select_area->set_position(mouse_pos);
                m_select_area_start = mouse_pos;
            }
            break;
        }
        case Tool::DELETE: {
            // auto [itr, range_end] = m_connectors.equal_range(mouse_to_grid(mouse_pos));
            // if(itr != range_end && !itr->second->for_part) {
            //     delete_cable(itr->second);
            //     break;
            // }
            // Part* part = get_part(mouse_pos);
            // if(part)
            //     delete_part(part);
            // break;
        }
        case Tool::ROTATE: {
            // Part* part = get_part(mouse_pos);
            // if(part) {
            //     for(Cable* cable: part->cables)
            //         untrack_cable(cable);

            //     part->rotate(M_PI / 2);
            //     for(Node2D* no_rotate_note: part->no_rotate_nodes)
            //         no_rotate_note->rotate(-M_PI / 2);

            //     for(Cable* cable: part->cables)
            //         track_cable(cable);
            // }
            // break;
        }
        case Tool::CREATE:
            if(!m_next_part) {
                // TODO:
                // what the fuck?
                PRT("dafuq?");
                break;
            }
            Part* part = Object::cast_to<Part>(m_next_part->duplicate());
            add_part(part, mouse_pos);
            break;
        }
    }
    else if(event->is_action_released("click") && m_tool == Tool::MOVE) {
        if(m_new_cable) {
            if(m_new_cable->get_global_a() == m_new_cable->get_global_b()) {
                remove_child(m_new_cable);
                m_new_cable->queue_free();
            }
            else
                track_cable(m_new_cable);
        }

        if(m_select_area) {
            m_cur_selection = get_selection();
            for(Node2D* node: m_cur_selection) {
                Part* part = Object::cast_to<Part>(node);
                if(part) {
                    // TODO: highlight
                    continue;
                }
            }

            remove_child(m_select_area);
            m_select_area->queue_free();
        }

        m_select_area = nullptr;
        m_new_cable   = nullptr;
        m_moving_part = nullptr;
    }
}

Vector2i Circuit::mouse_to_grid(Vector2i pos) const
{
    pos += 0.5 * m_grid_size * trace_cables::sgn(pos);
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

std::vector<Node2D*> Circuit::get_selection() const
{
    TypedArray<Area2D> overlaps = m_select_area->get_overlapping_areas();
    PRT(overlaps.size());
    std::vector<Node2D*> parents;
    for(int i {0}; i < overlaps.size(); ++i)
        parents.push_back(Object::cast_to<Node2D>(Object::cast_to<Area2D>(overlaps[i])->get_parent()));

    return parents;
}

void Circuit::add_part(Part* part, Vector2i pos)
{
    part->load();

    add_child(part);
    part->set_position(mouse_to_grid(pos));
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
    for(Vector2i con: trace_cables::get_cable_cons(cable, m_grid_size, m_connectors))
        m_connectors.insert(con, cable);
}
void Circuit::untrack_cable(Cable* cable)
{
    for(Vector2i con: trace_cables::get_cable_cons(cable, m_grid_size, m_connectors))
        m_connectors.erase(con, cable);
}

void Circuit::delete_cable(Cable* cable)
{
    untrack_cable(cable);
    remove_child(cable);
    cable->queue_free();
}

void Circuit::delete_all()
{
    TypedArray<Node> children = get_children();
    for(int i {0}; i < children.size(); ++i) {
        // go through cables first -> don't delete part before going over part's cable
        {
            Cable* cable = Object::cast_to<Cable>(children[i]);
            if(cable && !cable->for_part) {
                delete_cable(cable);
                continue;
            }
        }
        {
            Part* part = Object::cast_to<Part>(children[i]);
            if(part)
                delete_part(part);
        }
    }
}

void Circuit::add_cable(Vector2i pos0, Vector2i pos1)
{
    Cable* cable = Object::cast_to<Cable>(m_cable_scene->instantiate());
    add_child(cable);
    cable->set_global_a(pos0);
    cable->set_global_b(pos1);
    track_cable(cable);
}
