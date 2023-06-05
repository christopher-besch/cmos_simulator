#include "connector_container.h"
#include "godot_cpp/classes/object.hpp"
#include "helper.h"

using namespace godot;

void ConnectorContainer::insert(Vector2i pos, Cable* cable)
{
    m_connectors.insert({pos, cable});
}

void ConnectorContainer::erase(Vector2i pos, Cable* cable)
{
    for(auto [itr, range_end] = equal_range(pos); itr != range_end; ++itr)
        if(itr->second == cable) {
            m_connectors.erase(itr);
            return;
        }
}

std::pair<std::unordered_multimap<Vector2i, Cable*, Vector2iHash>::iterator,
          std::unordered_multimap<Vector2i, Cable*, Vector2iHash>::iterator>
    ConnectorContainer::equal_range_square(Vector2i pos, int grid_size)
{
    {
        auto [itr, end] = equal_range(pos + Vector2i(0, 0));
        if(itr != end)
            return {itr, end};
    }
    {
        auto [itr, end] = equal_range(pos + Vector2i(0, grid_size));
        if(itr != end)
            return {itr, end};
    }
    {
        auto [itr, end] = equal_range(pos + Vector2i(0, -grid_size));
        if(itr != end)
            return {itr, end};
    }
    {
        auto [itr, end] = equal_range(pos + Vector2i(grid_size, 0));
        if(itr != end)
            return {itr, end};
    }
    {
        auto [itr, end] = equal_range(pos + Vector2i(grid_size, grid_size));
        if(itr != end)
            return {itr, end};
    }
    {
        auto [itr, end] = equal_range(pos + Vector2i(grid_size, -grid_size));
        if(itr != end)
            return {itr, end};
    }
    {
        auto [itr, end] = equal_range(pos + Vector2i(-grid_size, 0));
        if(itr != end)
            return {itr, end};
    }
    {
        auto [itr, end] = equal_range(pos + Vector2i(-grid_size, grid_size));
        if(itr != end)
            return {itr, end};
    }
    {
        auto [itr, end] = equal_range(pos + Vector2i(-grid_size, -grid_size));
        if(itr != end)
            return {itr, end};
    }
    return {m_connectors.end(), m_connectors.end()};
}
