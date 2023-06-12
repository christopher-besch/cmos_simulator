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
    Node2D*                   m_cursor {nullptr};
    bool                      m_show_cursor {false};
    Ref<godot::PackedScene>   m_nmos_scene;
    Ref<godot::PackedScene>   m_pmos_scene;
    Ref<godot::PackedScene>   m_cable_scene;
    std::unordered_set<Part*> m_parts;
    ConnectorContainer        m_connectors;
    Tool                      m_tool {Tool::MOVE};
    PartType                  m_create_part_type {PartType::NONE};
    Part*                     m_moving_part {nullptr};
    Vector2                   m_moving_part_grab;
    Cable*                    m_new_cable {nullptr};
    int                       m_grid_size {20};

protected:
    static void _bind_methods()
    {
        BIND_ATR(Circuit, tool, INT);
        BIND_ATR(Circuit, create_part_type, INT);
        ClassDB::bind_method(D_METHOD("to_json"), &Circuit::to_json);
        ClassDB::bind_method(D_METHOD("load_json", "json"), &Circuit::load_json);
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
    int get_create_part_type() const
    {
        return static_cast<int>(m_create_part_type);
    }
    void set_create_part_type(int create_part_type)
    {
        m_create_part_type = static_cast<PartType>(create_part_type);
    }

    void _ready() override;
    void _process(double delta) override;
    void _input(const Ref<InputEvent>& event) override;

    String to_json() const;
    void   load_json(String json);

private:
    Vector2i mouse_to_grid(Vector2i pos) const;

    bool  is_part_clicked(Part* part, Vector2 pos) const;
    Part* get_part(Vector2 pos) const;

    void add_part(Ref<godot::PackedScene> scene, Vector2 pos, double rotation = 0.0);
    void delete_part(Part* part);
    void move_part(Part* part, Vector2i new_pos);

    void track_cable(Cable* cable);
    void untrack_cable(Cable* cable);
    void delete_cable(Cable* cable);

    void add_cable(Vector2i pos0, Vector2i pos1);

    void delete_all();

    void create_part_of_type(PartType type, Vector2i pos, double rotation = 0.0)
    {
        switch(type) {
        case PartType::NONE:
            break;
        case PartType::NMOS:
            add_part(m_nmos_scene, pos, rotation);
        case PartType::PMOS:
            add_part(m_pmos_scene, pos, rotation);
        }
    }
};

} // namespace godot
