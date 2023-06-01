#pragma once

#include "helper.h"
#include <godot_cpp/classes/input_event.hpp>
#include <godot_cpp/classes/marker2d.hpp>

namespace godot {

enum class ConnectorType : int {
    NONE = 0,
    // nmos
    Source = 1,
    Drain  = 2,
    Gate   = 3,
    Bulk   = 4,
};

struct Connector: public Marker2D {
    GDCLASS(Connector, Node2D)

    ConnectorType type {ConnectorType::NONE};

protected:
    static void _bind_methods()
    {
        BIND_ATR(Connector, type, INT);
    }

public:
    int get_type() const
    {
        return static_cast<int>(type);
    }
    void set_type(int p_type)
    {
        type = static_cast<ConnectorType>(p_type);
    }
};

} // namespace godot
