#include "rgb.hpp"
#include <iomanip>
#include <sstream>
using namespace color;

rgb::rgb(const std::string &rgb_s)
{
    uint32_t c;

    sscanf(rgb_s.substr(0, 2).c_str(), "%02X", &c);
    m_value.r = c;

    sscanf(rgb_s.substr(2, 2).c_str(), "%02X", &c);
    m_value.g = c;

    sscanf(rgb_s.substr(4, 2).c_str(), "%02X", &c);
    m_value.b = c;
}

rgb::rgb(const rgb &other)
    : m_value(other.m_value)
{
}

rgb::rgb(rgb &&other)
    : m_value(std::move(other.m_value))
{
}

rgb &rgb::operator=(const rgb &other)
{
    m_value = other.m_value;
    return *this;
}

rgb &rgb::operator=(rgb &&other)
{
    m_value = std::move(other.m_value);
    return *this;
}

bool rgb::operator==(const rgb &other) const
{
    return m_value.value == other.m_value.value;
}

bool rgb::operator!=(const rgb &other) const
{
    return m_value.value != other.m_value.value;
}

uint32_t rgb::red(uint32_t value)
{
    m_value.r = value;
    return m_value.r;
}

uint32_t rgb::green(uint32_t value)
{
    m_value.g = value;
    return m_value.g;
}

uint32_t rgb::blue(uint32_t value)
{
    m_value.b = value;
    return m_value.b;
}
const uint32_t rgb::red(void) const
{
    return m_value.r;
}

const uint32_t rgb::green(void) const
{
    return m_value.g;
}

const uint32_t rgb::blue(void) const
{
    return m_value.b;
}

namespace std
{

string to_string(const rgb &c)
{
    // Reformat the stringstream for each uint32_t we stream to it. This
    // forces each number to be treated as a size 2 hex value.
    stringstream ss;
    ss << hex << setw(2) << setfill('0') << c.red() << setw(2) << setfill('0')
       << c.green() << setw(2) << setfill('0') << c.blue();
    return ss.str();
}

}; // namespace std

