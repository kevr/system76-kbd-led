#ifndef RGB_HPP
#define RGB_HPP

#include "../cache.hpp"
#include "../logging.hpp"
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

template <std::size_t N>
string to_string(const std::array<color::rgb, N> &arr)
{
    string tmp;
    for (auto &e : arr)
        tmp.append(to_string(e));
    return tmp;
}

}; // namespace std

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

    std::optional<std::array<color::rgb, 4>> data(void) const
    {
        if (!cache<T>::exists())
            return std::nullopt;

        const auto s = cache<T>::data().value();
        // This will throw std::out_of_range on an invalid cache value.
        return std::array<color::rgb, 4>{
            color::rgb(s.substr(0, 6)), color::rgb(s.substr(6, 6)),
            color::rgb(s.substr(12, 6)), color::rgb(s.substr(18, 6))};
    }

    void set_data(const std::array<color::rgb, 4> &regions)
    {
        cache<T>::set_data(std::to_string(regions));
    }
};

}; // namespace fs

#endif /* COLOR_HPP */

