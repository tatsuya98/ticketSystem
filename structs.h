#pragma once
#include <optional>
#include <string>
#include <chrono>
#include "Enums.h"
#include <libpq-fe.h>

struct PGConnRAII
{
    PGconn *conn;
    PGConnRAII(const std::string &connStr)
    {
        conn = PQconnectdb(connStr.c_str());
    }
    ~PGConnRAII() // destructor
    {
        PQfinish(conn);
    }
};
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
struct Event
{
    std::string eventId;
    std::string eventName;
    std::string eventLocation;
    std::chrono::system_clock::time_point eventDate;
    int eventPrice;
    int eventCapacity;
};

struct Venue
{
    std::string venueId;
    std::string venueName;
    std::string venueLocation;
    int capacity;
};

struct Seat
{
    std::string seatId;
    std::string rowId;
    std::string seatNumber;
    float xPos;
    float yPos;
};
struct Section
{
    std::string sectionId;
    std::string sectionName;
    std::string venueId;
};

struct Row
{
    std::string rowId;
    std::string rowName;
    std::string sectionId;
};

struct PGResultRAII
{
    PGresult *result;

    PGResultRAII(PGresult *r) : result(r) {}

    ~PGResultRAII()
    {
        if (result)
        {
            PQclear(result);
        }
    }
};