#include "scroll_camera.h"
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/input.hpp>
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
    m_cur_zoom         = 1.0;
}

ScrollCamera::~ScrollCamera()
{
}

void ScrollCamera::_process(double delta)
{
    Input*  input  = Input::get_singleton();
    Engine* engine = Engine::get_singleton();
    if(engine->is_editor_hint())
        return;

    if(input->is_action_just_pressed("pan_click")) {
        m_mouse_click_pos  = get_local_mouse_position();
        m_camera_click_pos = get_position();
        input->set_mouse_mode(Input::MOUSE_MODE_HIDDEN);
    }
    if(input->is_action_just_released("pan_click")) {
        input->set_mouse_mode(Input::MOUSE_MODE_VISIBLE);
    }
    if(input->is_action_pressed("pan_click")) {
        Vector2 mouse_delta = get_local_mouse_position() - m_mouse_click_pos;
        Vector2 new_pos     = m_camera_click_pos - mouse_delta;
        set_position(new_pos);
    }

    if(input->is_action_just_released("zoom_in"))
        set_cur_zoom(m_cur_zoom * m_zoom_factor);
    if(input->is_action_just_released("zoom_out"))
        set_cur_zoom(m_cur_zoom / m_zoom_factor);
    set_zoom(Vector2(m_cur_zoom, m_cur_zoom));
}
