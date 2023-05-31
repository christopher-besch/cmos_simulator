#pragma once

#include "helper.h"
#include <godot_cpp/classes/input_event.hpp>
#include <godot_cpp/classes/node2d.hpp>

namespace godot {

enum class ConnectorType : int {
    NONE = 0,
    // nmos
    Source = 1,
    Drain  = 2,
    Gate   = 3,
    Bulk   = 4,
};

class Connector: public Node2D {
    GDCLASS(Connector, Node2D)

private:
    ConnectorType m_type {ConnectorType::NONE};

protected:
    static void _bind_methods();

public:
    Connector();
    ~Connector();

    int get_type() const
    {
        return static_cast<int>(m_type);
    }
    void set_type(int type)
    {
        m_type = static_cast<ConnectorType>(type);
    }
};

} // namespace godot
