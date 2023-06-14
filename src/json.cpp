#include "circuit.h"
#include "godot_cpp/classes/label.hpp"
#include "godot_cpp/variant/char_string.hpp"

#include <godot_cpp/variant/typed_array.hpp>
#include <string>
#include <vendor/json.hpp>

#include <sstream>

using namespace godot;

std::string to_std_str(String str)
{
    std::string std_str;
    std_str.reserve(str.length());
    for(int i {0}; i < str.length(); ++i)
        std_str.push_back(str[i]);
    return std_str;
}

String to_godot_str(std::string str)
{
    return str.c_str();
}

String Circuit::to_json() const
{
    std::vector<nlohmann::json> parts;
    std::vector<nlohmann::json> cables;

    TypedArray<Node> children = get_children();
    for(int i {0}; i < children.size(); ++i) {
        {
            Part* part = Object::cast_to<Part>(children[i]);
            if(part) {
                Vector2i pos = part->get_position().round();
                parts.push_back(nlohmann::json {
                    {"x", pos.x},
                    {"y", pos.y},
                    {"rotation", part->get_rotation()},
                    {"type", static_cast<int>(part->type)},
                    {"label", part->type == PartType::LABEL ?
                                  to_std_str(part->get_node<Label>("Label")->get_text()) :
                                  ""},
                });
                continue;
            }
        }
        {
            Cable* cable = Object::cast_to<Cable>(children[i]);
            if(cable && !cable->for_part) {
                Vector2i pos0 = cable->to_global(cable->get_point_position(0)).round();
                Vector2i pos1 = cable->to_global(cable->get_point_position(1)).round();
                cables.push_back(nlohmann::json {
                    {"x0", pos0.x},
                    {"y0", pos0.y},
                    {"x1", pos1.x},
                    {"y1", pos1.y},
                });
            }
        }
    }

    nlohmann::json final_json {
        {"parts", parts},
        {"cables", cables},
    };
    return final_json.dump().c_str();
}

void Circuit::load_json(String json)
{
    std::string json_str = to_std_str(json);
    delete_all();

    // TODO: please don't crash on malformed json
    nlohmann::json json_obj = nlohmann::json::parse(json_str);
    for(nlohmann::json& part: json_obj["parts"])
        create_part_of_type(static_cast<PartType>(part["type"]), Vector2i(part["x"], part["y"]), part["rotation"], to_godot_str(part["label"]));
    for(nlohmann::json& cable: json_obj["cables"])
        add_cable(Vector2i(cable["x0"], cable["y0"]), Vector2i(cable["x1"], cable["y1"]));
}

void Circuit::create_part_of_type(PartType type, Vector2i pos, double rotation, String text)
{
    Part* part {nullptr};
    switch(type) {
    case PartType::NONE:
        return;
    case PartType::NMOS:
        part = Object::cast_to<Part>(m_nmos_scene->instantiate());
        break;
    case PartType::PMOS:
        part = Object::cast_to<Part>(m_pmos_scene->instantiate());
        break;
    case PartType::GND:
        part = Object::cast_to<Part>(m_gnd_scene->instantiate());
        break;
    case PartType::LABEL:
        part = Object::cast_to<Part>(m_label_scene->instantiate());
        PRT(m_next_part->get_children().size());
        part->get_node<Label>("Label")->set_text(text);
        break;
    }
    m_next_part->set_rotation(rotation);
    add_part(part, pos);
}
