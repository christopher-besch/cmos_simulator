#include "trace_cables.h"
#include "part.h"

#include <queue>
#include <unordered_set>

using namespace godot;
using namespace godot::trace_cables;

Vector2i trace_cables::sgn(Vector2 vec)
{
    return Vector2i(
        (0 < vec.x) - (vec.x < 0),
        (0 < vec.y) - (vec.y < 0));
}

std::vector<Vector2i> trace_cables::get_cable_cons(Cable* cable, int grid_size, ConnectorContainer& connectors)
{
    std::vector<Vector2i> cons;

    // round in case of rotation
    Vector2i start = cable->to_global(cable->get_point_position(0)).round();
    Vector2i end   = cable->to_global(cable->get_point_position(1)).round();
    Vector2i step  = grid_size * sgn(end - start);

    if(step == Vector2i(0, 0)) {
        cons.push_back(start);
        return cons;
    }
    // TODO: prevent endless loop check
    for(Vector2i pos {start}; pos != end + step; pos += step)
        cons.push_back(pos);
    return cons;
}

void trace_cable(
    Cable*              start_cable,
    Part*               start_part,
    PartConGraph&       graph,
    PartConSet&         visited_parts,
    int                 grid_size,
    ConnectorContainer& connectors)
{
    std::queue<Cable*> q;
    q.push(start_cable);
    std::unordered_set<Cable*> visited;

    while(!q.empty()) {
        Cable* cable = q.front();
        q.pop();

        if(visited.find(cable) != visited.end())
            continue;
        visited.insert(cable);

        for(Vector2i con: get_cable_cons(cable, grid_size, connectors))
            for(auto [con_itr, con_end] = connectors.equal_range(con); con_itr != con_end; ++con_itr) {
                Cable* new_cable = con_itr->second;
                // found itself?
                if(new_cable == cable)
                    continue;

                // not final cable for part
                // could also check if cable type is 0
                if(new_cable->get_point_position(0) != new_cable->get_point_position(1)) {
                    q.push(new_cable);
                    continue;
                }
                int p = new_cable->for_part->id;

                // TODO: remove
                if(visited_parts.find({p, cable->type}) != visited_parts.end())
                    continue;
                visited_parts.insert({p, cable->type});

                graph.insert(std::make_pair(
                    std::make_pair(start_part->id, start_cable->type),
                    std::make_pair(p, cable->type)));
                graph.insert(std::make_pair(
                    std::make_pair(p, cable->type),
                    std::make_pair(start_part->id, start_cable->type)));
            }
    }
}

void trace_cables::trace_cables(std::unordered_set<Part*> parts_set, int grid_size, ConnectorContainer& connectors)
{
    std::vector<Part*> parts;
    for(Part* part: parts_set) {
        part->id = parts.size();
        parts.push_back(part);
    }

    PartConGraph graph;
    PartConSet   visited_parts;

    for(Part* part: parts)
        for(auto [type, cable]: part->start_cables) {
            if(visited_parts.find({part->id, type}) != visited_parts.end())
                continue;
            visited_parts.insert({part->id, type});
            trace_cable(cable, part, graph, visited_parts, grid_size, connectors);
        }
}
