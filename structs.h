#pragma once
#include <optional>
#include <string>
#include <chrono>
#include "Enums.h"
struct EventUpdate
{
    std::optional<std::string> eventLocation;
    std::optional<int> eventCapacity;
    std::optional<std::chrono::system_clock::time_point> eventDate;
    std::optional<int> eventPrice;
};

struct TicketUpdate
{
    std::optional<std::string> userId;
    std::optional<std::string> seatId;
    std::optional<TicketStatus> status;
};