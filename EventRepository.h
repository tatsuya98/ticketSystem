#pragma once
#include "Enums.h"
#include "Structs.h"
#include "DTOs.h"
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
    std::expected<std::vector<std::unique_ptr<EventSummaryDTO>>, DatabaseError> getEventsByCity(std::string city);
    std::expected<std::unique_ptr<Event>, DatabaseError> getEventById(std::string eventId);
    std::expected<std::unique_ptr<Event>, DatabaseError> getEventByName(std::string eventName);
    std::expected<bool, DatabaseError> updateEvent(std::string eventId, EventUpdate eventUpdate);
    std::expected<bool, DatabaseError> purchaseTicketTransaction(
        std::string ticketId,
        std::string eventId);
    std::expected<bool, DatabaseError> cancelTicketTransaction(std::string ticketId, std::string eventId);
    std::expected<std::vector<EventSummaryDTO>, DatabaseError>
    getAllEventsWithVenue(int limit, int offset);

    std::expected<std::vector<EventSummaryDTO>, DatabaseError>
    getEventsByCity(std::string city, int limit, int offset);

    std::expected<EventDetailDTO, DatabaseError>
    getEventDetailsById(std::string eventId);
};
