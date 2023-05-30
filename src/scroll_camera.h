#pragma once

#include <godot_cpp/classes/camera2d.hpp>

namespace godot {

class ScrollCamera: public Camera2D {
    GDCLASS(ScrollCamera, Camera2D)

private:
    float m_zoom_factor;
    float m_min_zoom;
    float m_cur_zoom;

    Vector2 m_mouse_click_pos;
    Vector2 m_camera_click_pos;

protected:
    static void _bind_methods();

public:
    ScrollCamera();
    ~ScrollCamera();

    float get_cur_zoom() const
    {
        return m_cur_zoom;
    }
    void set_cur_zoom(float zoom)
    {
        m_cur_zoom = std::max(zoom, m_min_zoom);
    }

    void _process(double delta) override;
};

} // namespace godot
