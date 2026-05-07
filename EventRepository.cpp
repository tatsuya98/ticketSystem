#include "EventRepository.h"
#include "Helper.h"
EventRepository::EventRepository(PGconn *conn) : connection(conn) {}

std::expected<EventDetailDTO, DatabaseError> EventRepository::getEventById(std::string eventId)
{
    const char *query = "SELECT e.event_id, e.event_name, v.venue_name, v.venue_city, v.venue_address, v.venue_id, e.event_date, e.event_price, e.event_capacity FROM events e"
                        "  JOIN venues v ON e.venue_id = v.venue_id"
                        "  WHERE event_id = $1";
    const char *params[1] = {eventId.c_str()};
    PGResultRAII result(PQexecParams(connection, query, 1, NULL, params, NULL, NULL, 0));
    if (PQresultStatus(result.result) != PGRES_TUPLES_OK)
        return std::unexpected(DatabaseError::QUERY_FAILED);
    if (PQntuples(result.result) == 0)
        return std::unexpected(DatabaseError::NOT_FOUND);
    int eventIdCol = PQfnumber(result.result, "event_id");
    int eventNameCol = PQfnumber(result.result, "event_name");
    int venueAddressCol = PQfnumber(result.result, "venue_address");
    int venueNameCol = PQfnumber(result.result, "venue_name");
    int venueCityCol = PQfnumber(result.result, "venue_city");
    int venueIdCol = PQfnumber(result.result, "venue_id");
    int eventDateCol = PQfnumber(result.result, "event_date");
    int eventPriceCol = PQfnumber(result.result, "event_price");
    int eventCapacityCol = PQfnumber(result.result, "event_capacity");
    if (eventIdCol == -1 || eventNameCol == -1 || venueAddressCol == -1 || venueNameCol == -1 || venueCityCol == -1 || venueIdCol == -1 || eventDateCol == -1 || eventPriceCol == -1 || eventCapacityCol == -1)
        return std::unexpected(DatabaseError::QUERY_FAILED);
    std::string eventIdValue = PQgetvalue(result.result, 0, eventIdCol);
    std::string eventNameValue = PQgetvalue(result.result, 0, eventNameCol);
    std::string venueAddressValue = PQgetvalue(result.result, 0, venueAddressCol);
    std::string venueNameValue = PQgetvalue(result.result, 0, venueNameCol);
    std::string venueCityValue = PQgetvalue(result.result, 0, venueCityCol);
    std::string venueIdValue = PQgetvalue(result.result, 0, venueIdCol);
    std::string eventDateValue = PQgetvalue(result.result, 0, eventDateCol);
    std::string eventPriceValue = PQgetvalue(result.result, 0, eventPriceCol);
    std::string eventCapacityValue = PQgetvalue(result.result, 0, eventCapacityCol);
    EventDetailDTO foundEvent;
    foundEvent.eventId = eventIdValue;
    foundEvent.eventName = eventNameValue;
    foundEvent.venueName = venueNameValue;
    foundEvent.venueCity = venueCityValue;
    foundEvent.venueAddress = venueAddressValue;
    foundEvent.venueId = venueIdValue;
    foundEvent.eventDate = parseTimestamp(eventDateValue.c_str());
    foundEvent.eventPrice = std::stoi(eventPriceValue);
    foundEvent.eventCapacity = std::stoi(eventCapacityValue);
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
};

std::expected<bool, DatabaseError> EventRepository::cancelTicketTransaction(std::string ticketId, std::string eventId)
{
    PGResultRAII beginResult(PQexec(connection, "BEGIN"));
    if (PQresultStatus(beginResult.result) != PGRES_COMMAND_OK)
    {
        return std::unexpected(DatabaseError::QUERY_FAILED);
    }
    const char *updateTicket = "UPDATE tickets SET status = 'CANCELLED', user_id = NULL WHERE ticket_id = $1";
    const char *paramsTicket[1] = {ticketId.c_str()};
    PGResultRAII updateTicketResult(PQexecParams(connection, updateTicket, 1, NULL, paramsTicket, NULL, NULL, 0));
    if (PQresultStatus(updateTicketResult.result) != PGRES_COMMAND_OK)
    {
        PQexec(connection, "ROLLBACK");
        return std::unexpected(DatabaseError::QUERY_FAILED);
    }

    const char *updateEvent = "UPDATE events SET event_capacity = event_capacity + 1 WHERE event_id = $1";
    const char *paramsEvent[1] = {eventId.c_str()};
    PGResultRAII updateEventResult(PQexecParams(connection, updateEvent, 1, NULL, paramsEvent, NULL, NULL, 0));
    if (PQresultStatus(updateEventResult.result) != PGRES_COMMAND_OK)
    {
        PQexec(connection, "ROLLBACK");
        return std::unexpected(DatabaseError::QUERY_FAILED);
    }
    PQexec(connection, "COMMIT");
    return true;
};

std::expected<std::vector<EventSummaryDTO>, DatabaseError> EventRepository::getAllEvents(int limit, int offset)
{
    const char *query = "SELECT e.event_id, e.event_name, v.venue_name, v.venue_city, e.event_date, e.event_price, e.event_capacity"
                        " FROM events e JOIN venues v ON e.venue_id = v.venue_id LIMIT $1 OFFSET $2";
    const char *params[2] = {std::to_string(limit).c_str(), std::to_string(offset).c_str()};
    PGResultRAII result(PQexecParams(connection, query, 2, NULL, params, NULL, NULL, 0));
    if (PQresultStatus(result.result) != PGRES_TUPLES_OK)
    {
        return std::unexpected(DatabaseError::QUERY_FAILED);
    }
    if (PQntuples(result.result) == 0)
    {
        return std::unexpected(DatabaseError::NOT_FOUND);
    }
    std::vector<EventSummaryDTO> events;
    int eventIdCol = PQfnumber(result.result, "event_id");
    int eventNameCol = PQfnumber(result.result, "event_name");
    int venueCityCol = PQfnumber(result.result, "venue_city");
    int venueAddressCol = PQfnumber(result.result, "venue_address");
    int eventDateCol = PQfnumber(result.result, "event_date");
    int eventPriceCol = PQfnumber(result.result, "event_price");
    int eventCapacityCol = PQfnumber(result.result, "event_capacity");
    if (eventIdCol == -1 || eventNameCol == -1 || venueCityCol == -1 || venueAddressCol == -1 || eventDateCol == -1 || eventPriceCol == -1 || eventCapacityCol == -1)
        return std::unexpected(DatabaseError::QUERY_FAILED);
    for (int i = 0; i < PQntuples(result.result); i++)
    {
        std::string eventIdValue = PQgetvalue(result.result, i, eventIdCol);
        std::string eventNameValue = PQgetvalue(result.result, i, eventNameCol);
        std::string venueCityValue = PQgetvalue(result.result, i, venueCityCol);
        std::string venueAddressValue = PQgetvalue(result.result, i, venueAddressCol);
        std::string eventDateValue = PQgetvalue(result.result, i, eventDateCol);
        std::string eventPriceValue = PQgetvalue(result.result, i, eventPriceCol);
        std::string eventCapacityValue = PQgetvalue(result.result, i, eventCapacityCol);
        EventSummaryDTO foundEvent;
        foundEvent.eventId = eventIdValue;
        foundEvent.eventName = eventNameValue;
        foundEvent.venueCity = venueCityValue;
        foundEvent.venueAddress = venueAddressValue;
        foundEvent.eventDate = parseTimestamp(eventDateValue.c_str());
        foundEvent.eventPrice = std::stoi(eventPriceValue);
        foundEvent.eventCapacity = std::stoi(eventCapacityValue);
        events.push_back(std::move(foundEvent));
    }
    return events;
};

std::expected<std::vector<EventSummaryDTO>, DatabaseError> EventRepository::getEventsByCity(std::string city, int limit, int offset)
{
    const char *query = "SELECT event_id, event_name, venue_city, venue_address, event_date, event_price, event_capacity"
                        " FROM events JOIN venues ON events.venue_id = venues.venue_id"
                        " WHERE venue_city = $1 LIMIT $2 OFFSET $3";
    const char *params[3] = {city.c_str(), std::to_string(limit).c_str(), std::to_string(offset).c_str()};
    PGResultRAII result(PQexecParams(connection, query, 3, NULL, params, NULL, NULL, 0));
    if (PQresultStatus(result.result) != PGRES_TUPLES_OK)
    {
        return std::unexpected(DatabaseError::QUERY_FAILED);
    }
    if (PQntuples(result.result) == 0)
    {
        return std::unexpected(DatabaseError::NOT_FOUND);
    }
    std::vector<EventSummaryDTO> events;
    int eventIdCol = PQfnumber(result.result, "event_id");
    int eventNameCol = PQfnumber(result.result, "event_name");
    int venueCityCol = PQfnumber(result.result, "venue_city");
    int venueAddressCol = PQfnumber(result.result, "venue_address");
    int eventDateCol = PQfnumber(result.result, "event_date");
    int eventPriceCol = PQfnumber(result.result, "event_price");
    int eventCapacityCol = PQfnumber(result.result, "event_capacity");
    if (eventIdCol == -1 || eventNameCol == -1 || venueCityCol == -1 || eventDateCol == -1 || eventPriceCol == -1 || eventCapacityCol == -1)
        return std::unexpected(DatabaseError::QUERY_FAILED);
    for (int i = 0; i < PQntuples(result.result); i++)
    {
        std::string eventIdValue = PQgetvalue(result.result, i, eventIdCol);
        std::string eventNameValue = PQgetvalue(result.result, i, eventNameCol);
        std::string venueCityValue = PQgetvalue(result.result, i, venueCityCol);
        std::string venueAddressValue = PQgetvalue(result.result, i, venueAddressCol);
        std::string eventDateValue = PQgetvalue(result.result, i, eventDateCol);
        std::string eventPriceValue = PQgetvalue(result.result, i, eventPriceCol);
        std::string eventCapacityValue = PQgetvalue(result.result, i, eventCapacityCol);
        EventSummaryDTO foundEvent;
        foundEvent.eventId = eventIdValue;
        foundEvent.eventName = eventNameValue;
        foundEvent.venueCity = venueCityValue;
        foundEvent.venueAddress = venueAddressValue;
        foundEvent.eventDate = parseTimestamp(eventDateValue.c_str());
        foundEvent.eventPrice = std::stoi(eventPriceValue);
        foundEvent.eventCapacity = std::stoi(eventCapacityValue);
        events.push_back(std::move(foundEvent));
    }
    return events;
}

std::expected<bool, DatabaseError> EventRepository::updateEvent(
    std::string eventId, EventUpdate eventUpdate)
{
    std::string query = "UPDATE events SET ";
    std::vector<std::string> setClauses;
    std::vector<std::string> paramValues; // keep strings alive
    std::vector<const char *> params;

    // build SET clauses dynamically
    if (eventUpdate.eventCapacity.has_value())
    {
        paramValues.push_back(std::to_string(eventUpdate.eventCapacity.value()));
        setClauses.push_back("event_capacity = $" + std::to_string(paramValues.size()));
    }
    if (eventUpdate.eventPrice.has_value())
    {
        paramValues.push_back(std::to_string(eventUpdate.eventPrice.value()));
        setClauses.push_back("event_price = $" + std::to_string(paramValues.size()));
    }
    if (eventUpdate.eventDate.has_value())
    {
        // convert time_point to string
        auto time = std::chrono::system_clock::to_time_t(eventUpdate.eventDate.value());
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        paramValues.push_back(ss.str());
        setClauses.push_back("event_date = $" + std::to_string(paramValues.size()));
    }

    // nothing to update
    if (setClauses.empty())
    {
        return true;
    }

    // join SET clauses with commas
    for (int i = 0; i < setClauses.size(); i++)
    {
        query += setClauses[i];
        if (i < setClauses.size() - 1)
            query += ", ";
    }

    // add WHERE clause — eventId is always the last param
    paramValues.push_back(eventId);
    query += " WHERE event_id = $" + std::to_string(paramValues.size());

    // build params array from stored strings
    for (auto &val : paramValues)
    {
        params.push_back(val.c_str());
    }

    PGResultRAII result(PQexecParams(
        connection, query.c_str(),
        params.size(), NULL,
        params.data(), NULL, NULL, 0));

    if (PQresultStatus(result.result) != PGRES_COMMAND_OK)
        return std::unexpected(DatabaseError::QUERY_FAILED);

    return true;
}
std::expected<EventDetailDTO, DatabaseError> EventRepository::getEventDetailsById(std::string eventId)
{

    const char *query = "SELECT e.event_id, e.event_name, v.venue_address, v.venue_city, v.venue_name ,e.event_date, event_price, event_capacity"
                        " FROM events e JOIN venues v ON events.venue_id = venues.venue_id WHERE event_id = $1";
    const char *params[1] = {eventId.c_str()};
    PGResultRAII eventResult(PQexecParams(connection, query, 1, NULL, params, NULL, NULL, 0));
    if (PQresultStatus(eventResult.result) != PGRES_TUPLES_OK)
    {
        return std::unexpected(DatabaseError::QUERY_FAILED);
    }
    if (PQntuples(eventResult.result) == 0)
    {
        return std::unexpected(DatabaseError::NOT_FOUND);
    }
    int eventIdCol = PQfnumber(eventResult.result, "event_id");
    int eventNameCol = PQfnumber(eventResult.result, "event_name");
    int venueAddressCol = PQfnumber(eventResult.result, "venue_address");
    int venueCityCOl = PQfnumber(eventResult.result, "venue_city");
    int eventDateCol = PQfnumber(eventResult.result, "event_date");
    int eventPriceCol = PQfnumber(eventResult.result, "event_price");
    int eventCapacityCol = PQfnumber(eventResult.result, "event_capacity");
    if (eventIdCol == -1 || eventNameCol == -1 || venueAddressCol == -1 || eventDateCol == -1 || eventPriceCol == -1 || eventCapacityCol == -1)
    {
        return std::unexpected(DatabaseError::QUERY_FAILED);
    }
    std::string eventIdValue = PQgetvalue(eventResult.result, 0, eventIdCol);
    std::string eventNameValue = PQgetvalue(eventResult.result, 0, eventNameCol);
    std::string venueAddressValue = PQgetvalue(eventResult.result, 0, venueAddressCol);
    std::string venueCityValue = PQgetvalue(eventResult.result, 0, venueCityCOl);
    std::string eventDateValue = PQgetvalue(eventResult.result, 0, eventDateCol);
    std::string eventPriceValue = PQgetvalue(eventResult.result, 0, eventPriceCol);
    std::string eventCapacityValue = PQgetvalue(eventResult.result, 0, eventCapacityCol);
    EventDetailDTO foundEvent;
    foundEvent.eventId = eventIdValue;
    foundEvent.eventName = eventNameValue;
    foundEvent.venueAddress = venueAddressValue;
    foundEvent.venueCity = venueCityValue;
    foundEvent.eventDate = parseTimestamp(eventDateValue.c_str());
    foundEvent.eventPrice = std::stoi(eventPriceValue);
    foundEvent.eventCapacity = std::stoi(eventCapacityValue);
    return foundEvent;
}

std::expected<std::vector<EventSummaryDTO>, DatabaseError> EventRepository::getAllEventsWithVenue(int limit, int offset)
{
    const char *query = "SELECT e.event_id, e.event_name, e.event_date, e.event_price"
                        ", e.event_capacity, v.venue_address, v.venue_city"
                        " FROM events e JOIN venues v ON e.venue_id = v.venue_id"
                        " ORDER BY event_date ASC"
                        " LIMIT $1 OFFSET $2";
    const char *params[2] = {std::to_string(limit).c_str(), std::to_string(offset).c_str()};
    PGResultRAII result(PQexecParams(connection, query, 2, NULL, params, NULL, NULL, 0));
    if (PQresultStatus(result.result) != PGRES_TUPLES_OK)
    {
        return std::unexpected(DatabaseError::QUERY_FAILED);
    }
    if (PQntuples(result.result) == 0)
    {
        return std::unexpected(DatabaseError::NOT_FOUND);
    }
    int eventIdCol = PQfnumber(result.result, "event_id");
    int eventNameCol = PQfnumber(result.result, "event_name");
    int eventDateCol = PQfnumber(result.result, "event_date");
    int eventPriceCol = PQfnumber(result.result, "event_price");
    int eventCapacityCol = PQfnumber(result.result, "event_capacity");
    int venueAddressCol = PQfnumber(result.result, "venue_address");
    int venueCityCol = PQfnumber(result.result, "venue_city");
    if (eventIdCol == -1 || eventNameCol == -1 || eventDateCol == -1 || eventPriceCol == -1 || eventCapacityCol == -1 || venueAddressCol == -1 || venueCityCol == -1)
    {
        return std::unexpected(DatabaseError::QUERY_FAILED);
    }
    std::vector<EventSummaryDTO> events;
    for (int i = 0; i < PQntuples(result.result); i++)
    {
        EventSummaryDTO event;
        event.eventId = PQgetvalue(result.result, i, eventIdCol);
        event.eventName = PQgetvalue(result.result, i, eventNameCol);
        event.eventDate = parseTimestamp(PQgetvalue(result.result, i, eventDateCol));
        event.eventPrice = std::stoi(PQgetvalue(result.result, i, eventPriceCol));
        event.eventCapacity = std::stoi(PQgetvalue(result.result, i, eventCapacityCol));
        event.venueAddress = PQgetvalue(result.result, i, venueAddressCol);
        event.venueCity = PQgetvalue(result.result, i, venueCityCol);
        events.push_back(event);
    }
    return events;
}