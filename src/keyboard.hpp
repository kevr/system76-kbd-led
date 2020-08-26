#ifndef KEYBOARD_HPP
#define KEYBOARD_HPP

#include "color.hpp"

namespace color
{

class keyboard
{
private:
    region<color::left, rgb> m_left;
    region<color::center, rgb> m_center;
    region<color::right, rgb> m_right;
    region<color::extra, rgb> m_extra;

public:
    std::array<rgb, 4> regions(void) const;

    region<left, rgb> &left_region(void);
    const region<color::left, rgb> &left_region(void) const;

    region<center, rgb> &center_region(void);
    const region<color::center, rgb> &center_region(void) const;

    region<right, rgb> &right_region(void);
    const region<color::right, rgb> &right_region(void) const;

    region<extra, rgb> &extra_region(void);
    const region<color::extra, rgb> &extra_region(void) const;

    void set_color(const color::rgb &color);
};

}; // namespace color

#endif /* KEYBOARD_HPP */
