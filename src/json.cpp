#include "circuit.h"

#include <godot_cpp/variant/typed_array.hpp>
#include <vendor/json.hpp>

#include <sstream>

using namespace godot;

String Circuit::to_json() const
{
    std::vector<nlohmann::json> parts;
    std::vector<nlohmann::json> cables;

    TypedArray<Node> children = get_children();
    for(int i {0}; i < children.size(); ++i) {
        Part* part = Object::cast_to<Part>(children[i]);
        if(part) {
            Vector2i pos = part->get_position().round();
            parts.push_back(nlohmann::json {
                {"x", pos.x},
                {"y", pos.y},
                {"rotation", part->get_rotation()},
                {"type", static_cast<int>(part->type)},
            });
            // continue;
        }
        Cable* cable = Object::cast_to<Cable>(children[i]);
        if(cable && !cable->for_part) {
            Vector2i pos0 = cable->to_global(cable->get_point_position(0)).round();
            Vector2i pos1 = cable->to_global(cable->get_point_position(1)).round();
            cables.push_back(nlohmann::json {
                {"x0", pos0.x},
                {"y0", pos0.y},
                {"x1", pos1.x},
                {"y1", pos1.y},
                {"type", static_cast<int>(part->type)},
            });
            continue;
        }
    }

    nlohmann::json final_json {
        {"parts", parts},
        // {"cable", cables},
    };
    return (std::stringstream {} << final_json).str().c_str();
}

void Circuit::load_json(String json) const
{
}
