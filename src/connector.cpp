#include "connector.h"
#include "helper.h"

using namespace godot;

void Connector::_bind_methods()
{
    BIND_ATR(Connector, type, INT);
}

Connector::Connector()
{
}

Connector::~Connector()
{
}
