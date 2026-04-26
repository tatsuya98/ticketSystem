#pragma once
#include "Enums.h"
#include "Structs.h"
#include <memory>
#include <libpq-fe.h>
#include <vector>
#include <expected>
class EventRepository
{
private:
    PGconn *connection;

public:
    EventRepository(PGconn *conn);
    std::expected<std::vector<std::unique_ptr<Event>>, DatabaseError> getAllEvents();
    std::expected<std::vector<std::unique_ptr<Event>>, DatabaseError> getEventsByLocation(std::string eventLocation);
    std::expected<std::unique_ptr<Event>, DatabaseError> getEventById(std::string eventId);
    std::expected<std::unique_ptr<Event>, DatabaseError> getEventByName(std::string eventName);
    std::expected<bool, DatabaseError> updateEvent(std::string eventId, EventUpdate eventUpdate);
    std::expected<bool, DatabaseError> purchaseTicketTransaction(
        std::string ticketId,
        std::string eventId);
};
