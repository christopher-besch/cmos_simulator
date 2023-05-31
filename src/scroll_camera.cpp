#include "scroll_camera.h"
#include "godot_cpp/classes/input_event_mouse_button.hpp"
#include "helper.h"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/input_event.hpp>
#include <godot_cpp/classes/sub_viewport.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/variant.hpp>

using namespace godot;

void ScrollCamera::_bind_methods()
{
}

ScrollCamera::ScrollCamera()
{
    m_mouse_click_pos  = Vector2 {-1.0, -1.0};
    m_camera_click_pos = Vector2 {-1.0, -1.0};
    m_zoom_factor      = 1.1;
    m_min_zoom         = 0.01;
    m_target_zoom      = 1.0;
}

ScrollCamera::~ScrollCamera()
{
}

void ScrollCamera::_process(double delta)
{
    Engine* engine = Engine::get_singleton();
    if(engine->is_editor_hint())
        return;

    if(m_pan) {
        Vector2 mouse_delta = get_local_mouse_position() - m_mouse_click_pos;
        Vector2 new_pos     = m_camera_click_pos - mouse_delta;
        set_position(new_pos);
    }
    if(m_zoom) {
        Vector2 target_zoom = Vector2(m_target_zoom, m_target_zoom);
        set_zoom(target_zoom);
        m_zoom = false;
    }
}

void ScrollCamera::_input(const Ref<InputEvent>& event)
{
    Input*  input  = Input::get_singleton();
    Engine* engine = Engine::get_singleton();
    if(engine->is_editor_hint())
        return;
    if(event->is_action_pressed("pan_click")) {
        m_mouse_click_pos  = get_local_mouse_position();
        m_camera_click_pos = get_position();
        m_pan              = true;
        input->set_mouse_mode(Input::MOUSE_MODE_HIDDEN);
    }
    else if(event->is_action_released("pan_click")) {
        m_pan = false;
        input->set_mouse_mode(Input::MOUSE_MODE_VISIBLE);
    }
    else if(event->is_action_released("zoom_in")) {
        m_zoom        = true;
        m_target_zoom = std::max(m_target_zoom * m_zoom_factor, m_min_zoom);
    }
    else if(event->is_action_released("zoom_out")) {
        m_zoom        = true;
        m_target_zoom = std::max(m_target_zoom / m_zoom_factor, m_min_zoom);
    }
}
