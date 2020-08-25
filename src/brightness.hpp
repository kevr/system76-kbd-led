#ifndef BRIGHTNESS_HPP
#define BRIGHTNESS_HPP

#include "cache.hpp"
#include "fs.hpp"
#include <cstdint>

#define BRIGHTNESS_PATH JOIN(SYSFS_PREFIX, "brightness")
#define MAX_BRIGHTNESS_PATH JOIN(SYSFS_PREFIX, "max_brightness")
#define HW_BRIGHTNESS_PATH JOIN(SYSFS_PREFIX, "brightness_hw_changed")

#define BRIGHTNESS_CACHE JOIN(CACHE_PREFIX, "brightness")
#define HW_BRIGHTNESS_CACHE JOIN(CACHE_PREFIX, "hw_brightness")

namespace led
{

template <typename T>
class brightness
{
private:
    inline static T m_level = 0;
    inline static T m_hw_level = 0;
    inline static T m_max_level = 255;

public:
    brightness(void)
    {
        init();
    }

    void increment(int value)
    {
        int tmp = m_level;
        tmp += value;
        if (tmp > m_max_level)
            tmp = m_max_level;
        else if (tmp < 0)
            tmp = 0;
        m_level = tmp;

        auto stream = fs::open(BRIGHTNESS_PATH, std::ios::out);
        stream << m_level;
        stream.close();
    }

    void set_value(int value)
    {
        if (value > m_max_level)
            value = m_max_level;
        else if (value < 0)
            value = 0;
        m_level = value;

        auto stream = fs::open(BRIGHTNESS_PATH, std::ios::out);
        stream << m_level;
        stream.close();
    }

    const T &level(void) const
    {
        return m_level;
    }

    const T &hw_level(void) const
    {
        return m_hw_level;
    }

    const T &max_level(void) const
    {
        return m_max_level;
    }

private:
    void init(void)
    {
        auto stream = fs::open(HW_BRIGHTNESS_PATH, std::ios::in);
        T b;
        if ((stream >> b))
            m_hw_level = b;
        stream.close();

        stream = fs::open(MAX_BRIGHTNESS_PATH, std::ios::in);
        stream >> m_max_level;
        stream.close();

        stream = fs::open(BRIGHTNESS_PATH, std::ios::in);
        stream >> m_level;
        stream.close();
    }
};

}; // namespace led

namespace fs
{

template <typename T>
class brightness_cache : public cache<T>
{
public:
    brightness_cache(void)
        : cache<T>(BRIGHTNESS_CACHE)
    {
    }
};

template <typename T>
class hw_brightness_cache : public fs::cache<T>
{
public:
    hw_brightness_cache(void)
        : cache<T>(HW_BRIGHTNESS_CACHE)
    {
    }
};

}; // namespace fs

#endif /* BRIGHTNESS_HPP */
