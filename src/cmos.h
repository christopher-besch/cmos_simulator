#pragma once

#include <godot_cpp/classes/input_event.hpp>
#include <godot_cpp/classes/node2d.hpp>

namespace godot {

class CMOS: public Node2D {
    GDCLASS(CMOS, Node2D)

private:
    double m_time_passed;
    double m_amplitude;

protected:
    static void _bind_methods();

public:
    CMOS();
    ~CMOS();

    void set_amplitude(const double amplitude)
    {
        m_amplitude = amplitude;
    }
    double get_amplitude() const
    {
        return m_amplitude;
    }

    // void _input(const Ref<InputEvent>& event) override;
    // void CMOS::_input(const Ref<InputEvent>& event)
    // {
    //     Ref<InputEventMouseButton> mouse_event = event;
    //     if(!mouse_event.is_null())
    //         PRT("click");
    // }
    void _process(double delta) override;

    void _ready() override;
};

} // namespace godot
