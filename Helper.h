
#pragma once
#include <chrono>
#include <sstream>
#include <iomanip>

inline std::chrono::system_clock::time_point parseTimestamp(const char *str)
{
    if (!str || str[0] == '\0')
    {
        return std::chrono::system_clock::time_point{};
    }
    std::tm tm = {};
    std::istringstream ss(str);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    return std::chrono::system_clock::from_time_t(std::mktime(&tm));
}