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
    std::string ticketIdValue = PQgetvalue(result.result, 0, ticketIdCol);
    std::string eventIdValue = PQgetvalue(result.result, 0, ticketEventIdCol);
    std::string userIdValue = PQgetvalue(result.result, 0, ticketUserIdCol);
    std::string seatIdValue = PQgetvalue(result.result, 0, ticketSeatIdCol);
    std::string purchasedAtValue = PQgetvalue(result.result, 0, ticketPurchasedAtCol);
    std::string statusStr = PQgetvalue(result.result, 0, ticketStatusCol);
    TicketStatus status;
    if (statusStr == "PURCHASED")
        status = TicketStatus::PURCHASED;
    else if (statusStr == "RESERVED")
        status = TicketStatus::RESERVED;
    else if (statusStr == "CANCELLED")
        status = TicketStatus::CANCELLED;
    else
        status = TicketStatus::AVAILABLE;
    auto foundTicket = std::make_unique<Ticket>(
        ticketIdValue,
        eventIdValue,
        userIdValue,
        status,
        seatIdValue);
    foundTicket->purchasedAt = parseTimestamp(purchasedAtValue.c_str());
    return foundTicket;
};
