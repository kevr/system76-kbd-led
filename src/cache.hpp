#ifndef CACHE_HPP
#define CACHE_HPP

#include "fs.hpp"
#include <fstream>
#include <optional>
#include <tuple>

#define CACHE_PREFIX "/var/cache/system76-kbd-led/"

namespace fs
{

template <typename T>
class cache
{
private:
    // Path to this particular cache, given on construction.
    const std::string m_path;

    // Internal data.
    std::optional<T> m_data;

public:
    cache(const std::string &path)
        : m_path(path)
    {
        init();
    }

    bool exists(void) const
    {
        return m_data.has_value();
    }

    std::optional<T> data(void) const
    {
        return m_data;
    }

    void set_data(T data)
    {
        m_data = std::move(data);
        auto ofs = fs::open(m_path, std::ios::out);
        ofs << m_data.value();
        ofs.close();
    }

private:
    void init(void)
    {
        if (fs::exists(m_path)) {
            T data;
            auto ifs = fs::open(m_path, std::ios::in);
            ifs >> data;
            ifs.close();
            m_data = std::move(data);
        }
    }
};

}; // namespace fs

#endif /* CACHE_HPP */
