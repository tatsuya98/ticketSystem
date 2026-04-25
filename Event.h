#pragma once
#include <string>
#include <chrono>
#include "Ticket.h"
#include <vector>
#include "Structs.h"
class Event
{
private:
    std::string eventId;
    std::string eventName;
    std::string eventLocation;
    std::chrono::system_clock::time_point eventDate;
    int eventPrice;
    int eventCapacity;
    std::vector<std::unique_ptr<Ticket>> tickets;

public:
    std::vector<std::unique_ptr<Ticket>> getAvailableEventTickets();
    bool updateEvent(EventUpdate eventUpdate);
};