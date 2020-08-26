#include "keyboard.hpp"
using namespace color;

std::array<rgb, 4> keyboard::regions(void) const
{
    return {left_region().color(), center_region().color(),
            right_region().color(), extra_region().color()};
}

region<left, rgb> &keyboard::left_region(void)
{
    return m_left;
}

const region<color::left, rgb> &keyboard::left_region(void) const
{
    return m_left;
}

region<center, rgb> &keyboard::center_region(void)
{
    return m_center;
}

const region<color::center, rgb> &keyboard::center_region(void) const
{
    return m_center;
}

region<right, rgb> &keyboard::right_region(void)
{
    return m_right;
}

const region<color::right, rgb> &keyboard::right_region(void) const
{
    return m_right;
}

region<extra, rgb> &keyboard::extra_region(void)
{
    return m_extra;
}

const region<color::extra, rgb> &keyboard::extra_region(void) const
{
    return m_extra;
}

void keyboard::set_color(const color::rgb &color)
{
    m_left.set_color(color);
    m_center.set_color(color);
    m_right.set_color(color);
    m_extra.set_color(color);
}
