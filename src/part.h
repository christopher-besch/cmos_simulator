#pragma once

#include "connector.h"
#include "helper.h"
#include <vector>

#include <godot_cpp/classes/input_event.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

namespace godot {

enum class PartType : int {
    NONE = 0,
    NMOS = 1,
    PMOS = 2,
};

class Part: public Node2D {
    GDCLASS(Part, Node2D)

private:
    std::vector<Connector*> m_connectors {};
    PartType                m_type {PartType::NONE};
    bool                    m_active {false};
    Vector2                 m_size;
    float                   m_min_dist_sqrd {40.0};

protected:
    static void _bind_methods();

public:
    Part();
    ~Part();

    int get_type() const
    {
        return static_cast<int>(m_type);
    }
    void set_type(int type)
    {
        m_type = static_cast<PartType>(type);
    }
    bool       is_inside(Vector2 pos) const;
    Connector* get_clicked_connector(Vector2 pos);

    void _ready() override;
};

} // namespace godot
