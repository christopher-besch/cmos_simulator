#pragma once

#include "cable.h"
#include "connector_container.h"
#include "helper.h"
#include "part.h"
#include "scroll_camera.h"

#include <godot_cpp/classes/input_event.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <unordered_set>

namespace godot {

enum class Tool : int {
    MOVE        = 0,
    DELETE      = 1,
    CREATE_NMOS = 2,
    CREATE_PMOS = 3,
    ROTATE      = 4,
};

class Circuit: public Node2D {
    GDCLASS(Circuit, Node2D)

private:
    Node2D*                   m_cursor {nullptr};
    bool                      m_show_cursor {false};
    Ref<godot::PackedScene>   m_nmos_scene;
    Ref<godot::PackedScene>   m_pmos_scene;
    Ref<godot::PackedScene>   m_cable_scene;
    std::unordered_set<Part*> m_parts;
    ConnectorContainer        m_connectors;
    Tool                      m_tool {Tool::MOVE};
    Part*                     m_moving_part {nullptr};
    Vector2                   m_moving_part_grab;
    Cable*                    m_new_cable {nullptr};
    int                       m_grid_size {20};

protected:
    static void _bind_methods()
    {
        BIND_ATR(Circuit, tool, INT);
    }

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
    Vector2i mouse_to_grid(Vector2i pos) const;

    bool  is_part_clicked(Part* part, Vector2 pos) const;
    Part* get_part(Vector2 pos) const;

    void add_part(Ref<godot::PackedScene> scene, Vector2 pos);
    void delete_part(Part* part);
    void move_part(Part* part, Vector2i new_pos);

    void track_cable(Cable* cable);
    void untrack_cable(Cable* cable);
    void delete_cable(Cable* cable);

    Vector2i sgn(Vector2 vec) const;
};

} // namespace godot
