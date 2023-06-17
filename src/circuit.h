#pragma once

#include "cable.h"
#include "connector_container.h"
#include "helper.h"
#include "part.h"
#include "scroll_camera.h"
#include "trace_cables.h"

#include <godot_cpp/classes/area2d.hpp>
#include <godot_cpp/classes/input_event.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <map>
#include <unordered_set>

namespace godot {

enum class Tool : int {
    MOVE   = 0,
    DELETE = 1,
    CREATE = 2,
    ROTATE = 4,
};

class Circuit: public Node2D {
    GDCLASS(Circuit, Node2D)

private:
    int m_grid_size {20};

    Ref<godot::PackedScene> m_cable_scene;
    Ref<godot::PackedScene> m_nmos_scene;
    Ref<godot::PackedScene> m_pmos_scene;
    Ref<godot::PackedScene> m_gnd_scene;
    Ref<godot::PackedScene> m_label_scene;
    Ref<godot::PackedScene> m_select_area_scene;

    ConnectorContainer        m_connectors;
    std::unordered_set<Part*> m_parts;

    Node2D* m_cursor {nullptr};
    bool    m_show_cursor {false};

    Tool    m_tool {Tool::MOVE};
    Part*   m_next_part {nullptr};
    Part*   m_moving_part {nullptr};
    Vector2 m_moving_part_grab;
    Cable*  m_new_cable {nullptr};

    Area2D*              m_select_area {nullptr};
    Vector2              m_select_area_start;
    std::vector<Node2D*> m_cur_selection;

protected:
    static void _bind_methods()
    {
        BIND_ATR(Circuit, tool, INT);
        BIND_ATR(Circuit, next_part, OBJECT);
        ClassDB::bind_method(D_METHOD("to_json"), &Circuit::to_json);
        ClassDB::bind_method(D_METHOD("load_json", "json"), &Circuit::load_json);
        ClassDB::bind_method(D_METHOD("trace_circuit"), &Circuit::trace_circuit);
        ClassDB::bind_method(D_METHOD("set_scenes", "cable_scene", "nmos_scene", "pmos_scene", "gnd_scene", "label_scene", "select_area_scene"), &Circuit::set_scenes);
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
    Part* get_next_part()
    {
        return m_next_part;
    }
    void set_next_part(Part* next_part)
    {
        if(m_next_part)
            m_next_part->queue_free();
        m_next_part = next_part;
    }
    void set_scenes(Ref<PackedScene> cable_scene, Ref<PackedScene> nmos_scene, Ref<PackedScene> pmos_scene, Ref<PackedScene> gnd_scene, Ref<PackedScene> label_scene, Ref<PackedScene> select_area_scene)
    {
        m_cable_scene       = cable_scene;
        m_nmos_scene        = nmos_scene;
        m_pmos_scene        = pmos_scene;
        m_gnd_scene         = gnd_scene;
        m_label_scene       = label_scene;
        m_select_area_scene = select_area_scene;
    }

    void _ready() override;
    void _process(double delta) override;
    void _input(const Ref<InputEvent>& event) override;

    String to_json() const;
    void   load_json(String json);

    void trace_circuit()
    {
        trace_cables::trace_cables(m_parts, m_grid_size, m_connectors);
    }

private:
    Vector2i mouse_to_grid(Vector2i pos) const;

    bool is_part_clicked(Part* part, Vector2 pos) const;

    void add_part(Part* part, Vector2i pos);
    void create_part_of_type(PartType type, Vector2i pos, double rotation = 0.0, String text = "");
    void delete_part(Part* part);
    void move_part(Part* part, Vector2i new_pos);

    void track_cable(Cable* cable);
    void untrack_cable(Cable* cable);
    void delete_cable(Cable* cable);

    void add_cable(Vector2i pos0, Vector2i pos1);

    void delete_all();

    std::vector<Node2D*> get_selection() const;
};

} // namespace godot
