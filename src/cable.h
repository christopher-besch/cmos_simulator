#pragma once

#include "connector.h"
#include "helper.h"

#include <godot_cpp/classes/input_event.hpp>
#include <godot_cpp/classes/line2d.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <vector>

namespace godot {

struct Cable: public Line2D {
    GDCLASS(Cable, Line2D)

    Connector* connector_origin;
    Connector* connector_end;

protected:
    static void _bind_methods()
    {
        godot::ClassDB::bind_method(D_METHOD("set_connector_origin", "connector"), &Cable::set_connector_origin);
        godot::ClassDB::bind_method(D_METHOD("set_connector_end", "connector"), &Cable::set_connector_end);
    }

public:
    void set_connector_origin(Connector* con)
    {
        con->for_part    = true;
        connector_origin = con;
    }
    void set_connector_end(Connector* con)
    {
        con->for_part = true;
        connector_end = con;
    }
};

} // namespace godot
