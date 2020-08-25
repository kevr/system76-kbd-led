#ifndef LOGGING_HPP
#define LOGGING_HPP

#include <iomanip>
#include <iostream>
#include <sstream>
#include <type_traits>

namespace logging
{

struct state {
    static bool debug;
};

template <typename T>
void _log_to(std::stringstream &ss, T &&arg)
{
    ss << arg << std::endl;
}

template <typename T, typename... Args>
void _log_to(std::stringstream &ss, T &&arg, Args &&... args)
{
    ss << arg;

    // If our given T is not an openmode or fmtflag, add a space
    // to the stringstream after it.
    if constexpr (std::is_same_v<std::decay_t<T>, std::string> ||
                  std::is_same_v<std::decay_t<T>, const char *>)
        ss << " ";

    _log_to(ss, std::forward<Args>(args)...);
}

template <typename... Args>
void log_to(std::ostream &os, Args &&... args)
{
    std::stringstream ss;
    _log_to(ss, std::forward<Args>(args)...);
    os << ss.str();
}

template <typename... Args>
void info(Args &&... args)
{
    log_to(std::cout, "[INFO]", std::forward<Args>(args)...);
}

template <typename... Args>
void debug(Args &&... args)
{
    if (state::debug)
        log_to(std::cout, "[ DBG]", std::forward<Args>(args)...);
}

template <typename... Args>
void error(Args &&... args)
{
    log_to(std::cerr, "[ ERR]", std::forward<Args>(args)...);
}

template <typename... Args>
void warn(Args &&... args)
{
    log_to(std::cout, "[WARN]", std::forward<Args>(args)...);
}

void set_debug(bool enabled);

}; // namespace logging

#endif /* LOGGING_HPP */
