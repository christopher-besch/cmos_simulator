#pragma once

#include "cable.h"
#include "connector_container.h"

#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <godot_cpp/core/math.hpp>

namespace godot::trace_cables {

struct PairPartIdConTypeHash {
public:
    size_t operator()(const std::pair<int, ConnectorType>& pair) const
    {
        return std::hash<int> {}(pair.first) ^ std::hash<int> {}(static_cast<int>(pair.second));
    }
};
using PartConGraph = std::unordered_map<std::pair<int, ConnectorType>, std::pair<int, ConnectorType>, PairPartIdConTypeHash>;
using PartConSet   = std::unordered_set<std::pair<int, ConnectorType>, PairPartIdConTypeHash>;

std::vector<Vector2i> get_cable_cons(Cable* cable, int grid_size, ConnectorContainer& connectors);

void trace_cables(std::unordered_set<Part*> parts_set, int grid_size, ConnectorContainer& connectors);

// TODO: put somewhere else
Vector2i sgn(Vector2 vec);

} // namespace godot::trace_cables
