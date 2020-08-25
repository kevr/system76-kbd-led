#ifndef COLOR_REGION_HPP
#define COLOR_REGION_HPP

#include "../fs.hpp"
#include "rgb.hpp"
#include <string>

namespace color
{

struct left {
    static constexpr const char *const value = "left";
    static constexpr const char *const path = JOIN(SYSFS_PREFIX, "color_left");
};

struct center {
    static constexpr const char *const value = "center";
    static constexpr const char *const path =
        JOIN(SYSFS_PREFIX, "color_center");
};

struct right {
    static constexpr const char *const value = "right";
    static constexpr const char *const path =
        JOIN(SYSFS_PREFIX, "color_right");
};

struct extra {
    static constexpr const char *const value = "extra";
    static constexpr const char *const path =
        JOIN(SYSFS_PREFIX, "color_extra");
};

template <typename Region, typename Color>
class region
{
private:
    Color m_color;

public:
    region(void)
    {
        // No color was given on construction; read it from sysfs.
        read_color();
    }

    region(Color &&color)
        : m_color(std::move(color))
    {
    }

    region(const region &other)
        : m_color(other.m_color)
    {
    }

    region(region &&other)
        : m_color(std::move(other.m_color))
    {
    }

    region &operator=(const region &other)
    {
        m_color = other.m_color;
        return *this;
    }

    region &operator=(region &&other)
    {
        m_color = std::move(other.m_color);
        return *this;
    }
    void read_color(void)
    {
        auto path = std::string(Region::path);
        auto stream = fs::open(path, std::ios::in);
        if (!stream) {
            throw std::runtime_error("Unable to open " + path + " for input.");
        }

        std::string s(6, '\0');
        stream.read(&s[0], 6);

        // Update m_color with a new Color given string s.
        m_color = Color(s);

        // Close off the descriptor.
        stream.close();
    }

    void set_color(const Color &color, bool commit = false)
    {
        m_color = color;
        if (commit)
            this->commit();
    }

    void commit(void)
    {
        auto path = std::string(Region::path);
        auto stream = fs::open(path, std::ios::out);
        if (!stream) {
            throw std::runtime_error("Unable to open " + path +
                                     " for output.");
        }

        // Write m_color's string out to file.
        stream << std::to_string(m_color);

        // Close off the descriptor.
        stream.close();
    }

    const Color &color(void) const
    {
        return m_color;
    }
};

}; // namespace color

#endif /* COLOR_REGION_HPP */
