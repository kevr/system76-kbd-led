#include "fs.hpp"
#include <unistd.h>

bool fs::exists(const std::string &path)
{
    return access(path.c_str(), F_OK) != -1;
}

std::fstream fs::open(const std::string &path, std::ios::openmode modes)
{
    return std::fstream(path.c_str(), modes);
}
