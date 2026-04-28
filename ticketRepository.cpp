#include <sstream>
#include <iomanip>
#include "ticketRepository.h"
#include "Helper.h"
TicketRepository::TicketRepository(PGconn *conn) : connection(conn) {}

std::expected<std::unique_ptr<Ticket>, DatabaseError> TicketRepository::findTicketById(std::string ticketId)
{
    const char *query = "SELECT ticket_id, event_id, user_id, status, seat_id, purchased_at FROM tickets WHERE ticket_id = $1";
    const char *params[1] = {ticketId.c_str()};
    PGResultRAII result(PQexecParams(connection, query, 1, NULL, params, NULL, NULL, 0));
    if (PQresultStatus(result.result) != PGRES_TUPLES_OK)
        return std::unexpected(DatabaseError::QUERY_FAILED);

    if (PQntuples(result.result) == 0)
        return std::unexpected(DatabaseError::NOT_FOUND);
    int ticketIdCol = PQfnumber(result.result, "ticket_id");
    int ticketEventIdCol = PQfnumber(result.result, "event_id");
    int ticketUserIdCol = PQfnumber(result.result, "user_id");
    int ticketStatusCol = PQfnumber(result.result, "status");
    int ticketSeatIdCol = PQfnumber(result.result, "seat_id");
    int ticketPurchasedAtCol = PQfnumber(result.result, "purchased_at");
    if (ticketEventIdCol == -1 || ticketIdCol == -1 || ticketUserIdCol == -1 || ticketStatusCol == -1 || ticketSeatIdCol == -1 || ticketPurchasedAtCol == -1)
        return std::unexpected(DatabaseError::QUERY_FAILED);
    return mapRowToTicket(result.result, 0);
};

std::expected<bool, DatabaseError> TicketRepository::updateTicket(std::string ticketId, TicketUpdate ticketUpdate)
{

    if (!ticketUpdate.status.has_value())
    {
        return true; // no need to update
    }
    char const *status = nullptr;
    if (ticketUpdate.status.has_value())
    {
        switch (ticketUpdate.status.value())
        {
        case TicketStatus::AVAILABLE:
            status = "AVAILABLE";
            break;
        case TicketStatus::RESERVED:
            status = "RESERVED";
            break;
        case TicketStatus::PURCHASED:
            status = "PURCHASED";
            break;
        case TicketStatus::CANCELLED:
            status = "CANCELLED";
            break;
        }
    }

    const char *query = "UPDATE tickets SET status = $1 WHERE ticket_id = $2";
    const char *params[2] = {status, ticketId.c_str()};
    PGResultRAII result(PQexecParams(connection, query, 2, NULL, params, NULL, NULL, 0));
    if (PQresultStatus(result.result) != PGRES_COMMAND_OK)
        return std::unexpected(DatabaseError::QUERY_FAILED);
    return true;
}

std::expected<bool, DatabaseError> TicketRepository::cancelReserve(std::string ticketId)
{
    const char *query = "UPDATE tickets SET status = 'CANCELLED', user_id = NULL WHERE ticket_id = $1";
    const char *params[1] = {ticketId.c_str()};
    PGResultRAII result(PQexecParams(connection, query, 1, NULL, params, NULL, NULL, 0));
    if (PQresultStatus(result.result) != PGRES_COMMAND_OK)
        return std::unexpected(DatabaseError::QUERY_FAILED);
    return true;
};

std::expected<std::vector<std::unique_ptr<Ticket>>, DatabaseError> TicketRepository::findTicketsByEventId(std::string id)
{
    const char *query = "SELECT ticket_id, event_id, user_id, status, seat_id, purchased_at FROM tickets WHERE event_id = $1 LIMIT 10";
    const char *params[1] = {id.c_str()};
    PGResultRAII result(PQexecParams(connection, query, 1, NULL, params, NULL, NULL, 0));
    if (PQresultStatus(result.result) != PGRES_TUPLES_OK)
    {
        return std::unexpected(DatabaseError::QUERY_FAILED);
    }
    if (PQntuples(result.result) == 0)
    {
        return std::unexpected(DatabaseError::NOT_FOUND);
    }
    int ticketIdCol = PQfnumber(result.result, "ticket_id");
    int ticketEventIdCol = PQfnumber(result.result, "event_id");
    int ticketUserIdCol = PQfnumber(result.result, "user_id");
    int ticketStatusCol = PQfnumber(result.result, "status");
    int ticketSeatIdCol = PQfnumber(result.result, "seat_id");
    int ticketPurchasedAtCol = PQfnumber(result.result, "purchased_at");
    if (ticketIdCol == -1 || ticketEventIdCol == -1 || ticketUserIdCol == -1 || ticketStatusCol == -1 || ticketSeatIdCol == -1 || ticketPurchasedAtCol == -1)
        return std::unexpected(DatabaseError::QUERY_FAILED);
    std::vector<std::unique_ptr<Ticket>> tickets;
    for (int i = 0; i < PQntuples(result.result); i++)
    {

        tickets.push_back(mapRowToTicket(result.result, i));
    }
    return tickets;
}

std::expected<std::vector<std::unique_ptr<Ticket>>, DatabaseError> TicketRepository::findTicketsByUserId(std::string id)
{
    const char *query = "SELECT ticket_id, event_id, user_id, status, seat_id, purchased_at FROM tickets WHERE user_id = $1 LIMIT 10";
    const char *params[1] = {id.c_str()};
    PGResultRAII result(PQexecParams(connection, query, 1, NULL, params, NULL, NULL, 0));
    if (PQresultStatus(result.result) != PGRES_TUPLES_OK)
    {
        return std::unexpected(DatabaseError::QUERY_FAILED);
    }
    if (PQntuples(result.result) == 0)
    {
        return std::unexpected(DatabaseError::NOT_FOUND);
    }
    int ticketIdCol = PQfnumber(result.result, "ticket_id");
    int ticketEventIdCol = PQfnumber(result.result, "event_id");
    int ticketUserIdCol = PQfnumber(result.result, "user_id");
    int ticketStatusCol = PQfnumber(result.result, "status");
    int ticketSeatIdCol = PQfnumber(result.result, "seat_id");
    int ticketPurchasedAtCol = PQfnumber(result.result, "purchased_at");
    if (ticketIdCol == -1 || ticketEventIdCol == -1 || ticketUserIdCol == -1 || ticketStatusCol == -1 || ticketSeatIdCol == -1 || ticketPurchasedAtCol == -1)
        return std::unexpected(DatabaseError::QUERY_FAILED);
    std::vector<std::unique_ptr<Ticket>> tickets;
    for (int i = 0; i < PQntuples(result.result); i++)
    {

        tickets.push_back(mapRowToTicket(result.result, i));
    }
    return tickets;
}

std::unique_ptr<Ticket> TicketRepository::mapRowToTicket(PGresult *result, int row)
{
    int ticketIdCol = PQfnumber(result, "ticket_id");
    int ticketEventIdCol = PQfnumber(result, "event_id");
    int ticketUserIdCol = PQfnumber(result, "user_id");
    int ticketStatusCol = PQfnumber(result, "status");
    int ticketSeatIdCol = PQfnumber(result, "seat_id");
    int ticketPurchasedAtCol = PQfnumber(result, "purchased_at");

    std::string ticketIdValue = PQgetvalue(result, row, ticketIdCol);
    std::string eventIdValue = PQgetvalue(result, row, ticketEventIdCol);
    std::string userIdValue = PQgetvalue(result, row, ticketUserIdCol);
    std::string seatIdValue = PQgetvalue(result, row, ticketSeatIdCol);
    std::string purchasedAtValue = PQgetvalue(result, row, ticketPurchasedAtCol);
    std::string statusStr = PQgetvalue(result, row, ticketStatusCol);

    TicketStatus status;
    if (statusStr == "PURCHASED")
        status = TicketStatus::PURCHASED;
    else if (statusStr == "RESERVED")
        status = TicketStatus::RESERVED;
    else if (statusStr == "CANCELLED")
        status = TicketStatus::CANCELLED;
    else
        status = TicketStatus::AVAILABLE;

    return std::make_unique<Ticket>(
        ticketIdValue, eventIdValue, userIdValue,
        status, seatIdValue,
        parseTimestamp(purchasedAtValue.c_str()));
}