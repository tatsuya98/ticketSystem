#pragma once
#include <string>
#include <vector>
#include <expected>
#include "Enums.h"
#include "Structs.h"
#include "Ticket.h"
#include <memory>
#include <libpq-fe.h>
class TicketRepository
{
private:
    PGconn *connection;
    std::unique_ptr<Ticket> mapRowToTicket(PGresult *result, int row);

public:
    TicketRepository(PGconn *conn);
    std::expected<std::vector<std::unique_ptr<Ticket>>, DatabaseError> getTicketsByEventId(std::string id);
    std::expected<std::vector<std::unique_ptr<Ticket>>, DatabaseError> getTicketsByUserId(std::string id);
    std::expected<std::unique_ptr<Ticket>, DatabaseError> getTicketById(std::string ticketId);
    std::expected<bool, DatabaseError> updateTicket(std::string ticketId, TicketUpdate ticketUpdate);
    std::expected<bool, DatabaseError> cancelReserve(std::string ticketId);
    std::expected<std::vector<UserTicketDTO>, DatabaseError>
    getTicketHistoryByUserId(std::string userId, int limit, int offset);
    std::expected<std::vector<SeatMapDTO>, DatabaseError>
    getSeatMapByEvent(std::string eventId, std::string venueId);
};