#ifndef RGB_HPP
#define RGB_HPP

#include "../cache.hpp"
#include <cstdint>
#include <string>

#define COLOR_CACHE JOIN(CACHE_PREFIX, "colors")

namespace color
{

class rgb
{
private:
    union rgb_value {
        uint32_t value = 0;
        struct {
            uint32_t b : 8, g : 8, r : 8, : 8;
        };
    };

    union rgb_value m_value;

public:
    rgb(void) = default;
    rgb(const std::string &rgb_s);

    rgb(const rgb &other);
    rgb(rgb &&other);

    rgb &operator=(const rgb &other);
    rgb &operator=(rgb &&other);

    bool operator==(const rgb &other) const;
    bool operator!=(const rgb &other) const;

    uint32_t red(uint32_t value);
    uint32_t green(uint32_t value);
    uint32_t blue(uint32_t value);

    const uint32_t red(void) const;
    const uint32_t green(void) const;
    const uint32_t blue(void) const;
};

}; // namespace color

namespace std
{
string to_string(const color::rgb &c);
};

namespace fs
{

template <typename T = std::string>
class color_cache : public cache<T>
{
public:
    color_cache(void)
        : cache<T>(COLOR_CACHE)
    {
    }

    std::array<color::rgb, 3> data(void) const
    {
        const auto s = cache<T>::data().value();
        std::array<color::rgb, 3> regions;
        regions[0] = color::rgb(s.substr(0, 6));
        regions[1] = color::rgb(s.substr(6, 6));
        regions[2] = color::rgb(s.substr(12, 6));
        return regions;
    }

    void set_data(const color::rgb &left, const color::rgb &center,
                  const color::rgb &right)
    {
        auto s = std::to_string(left) + std::to_string(center) +
                 std::to_string(right);
        cache<T>::set_data(s);
    }
};

}; // namespace fs

#endif /* COLOR_HPP */

