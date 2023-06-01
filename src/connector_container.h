#pragma once
#include "connector.h"

#include <stdint.h>
#include <unordered_map>
#include <vector>

namespace godot {

struct Vector2iHash {
public:
    size_t operator()(const Vector2i& v) const
    {
        uint64_t key = static_cast<uint64_t>(static_cast<uint32_t>(v.x)) |
                       static_cast<uint64_t>(static_cast<uint32_t>(v.y)) << 32;
        // Vector2i(static_cast<uint32_t>(key), static_cast<uint32_t>(key >> 32));
        return std::hash<uint64_t> {}(key);
    }
};

class ConnectorContainer {
private:
    std::unordered_multimap<Vector2i, Connector*, Vector2iHash> m_connectors {};

public:
    void insert(Vector2i pos, Connector* connector);
    void insert(Connector* connector);
    void erase(Vector2i pos, Connector* connector);
    void erase(Connector* connector);
    auto equal_range(Vector2i pos)
    {
        return m_connectors.equal_range(pos);
    }
    auto equal_range(Connector* connector)
    {
        return m_connectors.equal_range(to_pos(connector));
    }
    auto size()
    {
        return m_connectors.size();
    }
    std::pair<std::unordered_multimap<Vector2i, Connector*, Vector2iHash>::iterator,
              std::unordered_multimap<Vector2i, Connector*, Vector2iHash>::iterator>
        equal_range_square(Vector2i pos, int grid_size);

private:
    Vector2i to_pos(Connector* connector);
};
} // namespace godot
