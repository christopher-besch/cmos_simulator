#pragma once

#include <godot_cpp/classes/sprite2d.hpp>

namespace godot {

class CMOS: public Sprite2D {
    GDCLASS(CMOS, Sprite2D)

private:
    double m_time_passed;
    double m_amplitude;

protected:
    static void _bind_methods();

public:
    CMOS();
    ~CMOS();

    void   set_amplitude(const double amplitude);
    double get_amplitude() const;

    void _process(double delta) override;
};

} // namespace godot
