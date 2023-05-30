#include "cmos.h"
#include "godot_cpp/variant/variant.hpp"
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void CMOS::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("get_amplitude"), &CMOS::get_amplitude);
    ClassDB::bind_method(D_METHOD("set_amplitude", "p_amplitude"), &CMOS::set_amplitude);
    ClassDB::add_property("CMOS", PropertyInfo(Variant::FLOAT, "amplitude"), "set_amplitude", "get_amplitude");
}

CMOS::CMOS()
{
    m_time_passed = 0.0;
    m_amplitude   = 10.0;
}

CMOS::~CMOS()
{
}

void CMOS::set_amplitude(const double amplitude)
{
    m_amplitude = amplitude;
}
double CMOS::get_amplitude() const
{
    return m_amplitude;
}

void CMOS::_process(double delta)
{
    m_time_passed += delta;
    Vector2 new_pos = Vector2(
        m_amplitude + (m_amplitude * sin(m_time_passed * 2.0)),
        m_amplitude + (m_amplitude * cos(m_time_passed * 2.0)));
    set_position(new_pos);
}
