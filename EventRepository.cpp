#include "EventRepository.h"
#include "Helper.h"
EventRepository::EventRepository(PGconn *conn) : connection(conn) {}

std::expected<std::unique_ptr<Event>, DatabaseError> EventRepository::getEventById(std::string eventId)
{
    const char *query = "SELECT event_id, event_name, event_location, event_date, event_price, event_capacity FROM events WHERE event_id = $1";
    const char *params[1] = {eventId.c_str()};
    PGResultRAII result(PQexecParams(connection, query, 1, NULL, params, NULL, NULL, 0));
    if (PQresultStatus(result.result) != PGRES_TUPLES_OK)
        return std::unexpected(DatabaseError::QUERY_FAILED);
    if (PQntuples(result.result) == 0)
        return std::unexpected(DatabaseError::NOT_FOUND);
    int eventIdCol = PQfnumber(result.result, "event_id");
    int eventNameCol = PQfnumber(result.result, "event_name");
    int eventLocationCol = PQfnumber(result.result, "event_location");
    int eventDateCol = PQfnumber(result.result, "event_date");
    int eventPriceCol = PQfnumber(result.result, "event_price");
    int eventCapacityCol = PQfnumber(result.result, "event_capacity");
    if (eventIdCol == -1 || eventNameCol == -1 || eventLocationCol == -1 || eventDateCol == -1 || eventPriceCol == -1 || eventCapacityCol == -1)
        return std::unexpected(DatabaseError::QUERY_FAILED);
    std::string eventIdValue = PQgetvalue(result.result, 0, eventIdCol);
    std::string eventNameValue = PQgetvalue(result.result, 0, eventNameCol);
    std::string eventLocationValue = PQgetvalue(result.result, 0, eventLocationCol);
    std::string eventDateValue = PQgetvalue(result.result, 0, eventDateCol);
    std::string eventPriceValue = PQgetvalue(result.result, 0, eventPriceCol);
    std::string eventCapacityValue = PQgetvalue(result.result, 0, eventCapacityCol);
    auto foundEvent = std::make_unique<Event>();
    foundEvent->eventId = eventIdValue;
    foundEvent->eventName = eventNameValue;
    foundEvent->eventLocation = eventLocationValue;
    foundEvent->eventDate = parseTimestamp(eventDateValue.c_str());
    foundEvent->eventPrice = std::stoi(eventPriceValue);
    foundEvent->eventCapacity = std::stoi(eventCapacityValue);
    return foundEvent;
}

std::expected<bool, DatabaseError> EventRepository::purchaseTicketTransaction(std::string ticketId, std::string eventId)
{
    PGResultRAII beginResult(PQexec(connection, "BEGIN"));
    if (PQresultStatus(beginResult.result) != PGRES_COMMAND_OK)
    {
        return std::unexpected(DatabaseError::QUERY_FAILED);
    }
    const char *updateTicket = "UPDATE tickets SET status = 'PURCHASED' WHERE ticket_id = $1";
    const char *paramsTicket[1] = {ticketId.c_str()};
    PGResultRAII updateTicketResult(PQexecParams(connection, updateTicket, 1, NULL, paramsTicket, NULL, NULL, 0));
    if (PQresultStatus(updateTicketResult.result) != PGRES_COMMAND_OK)
    {
        PQexec(connection, "ROLLBACK");
        return std::unexpected(DatabaseError::QUERY_FAILED);
    }

    const char *updateEvent = "UPDATE events SET event_capacity = event_capacity - 1 WHERE event_id = $1";
    const char *paramsEvent[1] = {eventId.c_str()};
    PGResultRAII updateEventResult(PQexecParams(connection, updateEvent, 1, NULL, paramsEvent, NULL, NULL, 0));
    if (PQresultStatus(updateEventResult.result) != PGRES_COMMAND_OK)
    {
        PQexec(connection, "ROLLBACK");
        return std::unexpected(DatabaseError::QUERY_FAILED);
    }
    PQexec(connection, "COMMIT");
    return true;
}