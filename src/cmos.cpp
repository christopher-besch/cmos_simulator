#include "cmos.h"
#include "helper.h"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/input_event_mouse_button.hpp>
#include <godot_cpp/classes/os.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/variant.hpp>

using namespace godot;

void CMOS::_bind_methods()
{
    BIND_ATR(CMOS, amplitude);
}

CMOS::CMOS()
{
    m_time_passed = 0.0;
    m_amplitude   = 10.0;
}

CMOS::~CMOS()
{
}

void CMOS::_ready()
{
    auto children = get_children();
    for(int i {0}; i < children.size(); ++i) {
        prt(children[i].stringify());
    }
}

void CMOS::_process(double delta)
{
    Input*  input  = Input::get_singleton();
    Engine* engine = Engine::get_singleton();
    if(engine->is_editor_hint())
        return;

    m_time_passed += delta;
    Vector2 new_pos = Vector2(
        m_amplitude + (m_amplitude * sin(m_time_passed * 2.0)),
        m_amplitude + (m_amplitude * cos(m_time_passed * 2.0)));
    set_position(new_pos);
}
