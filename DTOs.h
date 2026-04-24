#pragma once
#include <string>
#include "Enums.h"
#include <chrono>
#include <vector>

struct SeatMapDTO
{
    std::string seatId;
    std::string rowName;
    std::string sectionName;
    std::string seatNumber;
    float xPosition;
    float yPosition;
    bool isAvailable;
};

struct UserTicketDTO
{
    std::string ticketId;
    std::string seatId;
    TicketStatus status;
    std::string eventName;
    std::string eventLocation;
    std::chrono::system_clock::time_point eventDate;
};

struct CheckInDTO
{
    std::string ticketId;
    TicketStatus status;
    std::string eventId;
};

struct EventSummaryDTO
{
    std::string eventId;
    std::string eventName;
    std::string eventLocation;
    std::chrono::system_clock::time_point eventDate;
    int price;
    int capacity;
};

struct EventDetailDTO
{
    std::string eventId;
    std::string eventName;
    std::string eventLocation;
    std::chrono::system_clock::time_point eventDate;
    int price;
    int capacity;
    std::vector<SeatMapDTO> availableSeats;
};