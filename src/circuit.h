#pragma once

#include "connector.h"
#include "helper.h"
#include "part.h"
#include "scroll_camera.h"

#include <godot_cpp/classes/input_event.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/sub_viewport.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <vector>

namespace godot {

enum class Tool : int {
    MOVE        = 0,
    CREATE_NMOS = 1,
    CREATE_PMOS = 2,
};

class Circuit: public Node2D {
    GDCLASS(Circuit, Node2D)

private:
    SubViewport*            m_viewport;
    Ref<godot::PackedScene> m_nmos_scene;
    Ref<godot::PackedScene> m_pmos_scene;
    std::vector<Part*>      m_parts {};
    Tool                    m_tool {Tool::MOVE};

protected:
    static void _bind_methods();

public:
    Circuit();
    ~Circuit();

    int get_tool() const
    {
        return static_cast<int>(m_tool);
    }
    void set_tool(int tool)
    {
        m_tool = static_cast<Tool>(tool);
    }

    void _ready() override;
    void _process(double delta) override;
    void _input(const Ref<InputEvent>& event) override;

private:
    void move_click(Vector2 mouse_pos);

    void add_part(Ref<godot::PackedScene> scene, Vector2 mouse_pos);
};

} // namespace godot
