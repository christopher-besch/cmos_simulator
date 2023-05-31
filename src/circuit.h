#pragma once

#include "connector.h"
#include "connector_container.h"
#include "helper.h"
#include "part.h"
#include "scroll_camera.h"

#include <godot_cpp/classes/input_event.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <set>

namespace godot {

enum class Tool : int {
    MOVE        = 0,
    DELETE      = 1,
    CREATE_NMOS = 2,
    CREATE_PMOS = 3,
};

class Circuit: public Node2D {
    GDCLASS(Circuit, Node2D)

private:
    Ref<godot::PackedScene> m_nmos_scene;
    Ref<godot::PackedScene> m_pmos_scene;
    std::set<Part*>         m_parts;
    ConnectorContainer      m_connectors;
    Tool                    m_tool {Tool::MOVE};
    Part*                   m_moving_part {nullptr};
    Vector2                 m_moving_part_grab;
    int                     m_grid_size {20};

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
        PRT(m_connectors.size());
        m_tool = static_cast<Tool>(tool);
    }

    void _ready() override;
    void _process(double delta) override;
    void _input(const Ref<InputEvent>& event) override;

private:
    Vector2i to_grid_pos(Vector2i pos) const;

    bool  is_part_clicked(Part* part, Vector2 pos) const;
    Part* get_part(Vector2 pos);

    void add_part(Ref<godot::PackedScene> scene, Vector2 pos);
    void delete_part(Vector2 pos);
};

} // namespace godot