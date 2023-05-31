#pragma once

#include <godot_cpp/classes/camera2d.hpp>

namespace godot {

class ScrollCamera: public Camera2D {
    GDCLASS(ScrollCamera, Camera2D)

private:
    float m_zoom_factor;
    float m_min_zoom;
    float m_target_zoom;

    Vector2 m_mouse_click_pos;
    Vector2 m_camera_click_pos;
    bool    m_pan {false};
    bool    m_zoom {false};

protected:
    static void _bind_methods();

public:
    ScrollCamera();
    ~ScrollCamera();

    void _process(double delta) override;
    void _input(const Ref<InputEvent>& event) override;
};

} // namespace godot
