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

public:
    TicketRepository(PGconn *conn);
    std::expected<std::vector<std::unique_ptr<Ticket>>, DatabaseError> getTicketsByEventId(std::string id);
    std::expected<std::vector<std::unique_ptr<Ticket>>, DatabaseError> getTicketsByUserId(std::string id);
    std::expected<std::unique_ptr<Ticket>, DatabaseError> getTicketById(std::string ticketId);
    std::expected<bool, DatabaseError> updateTicket(std::string ticketId, TicketUpdate ticketUpdate);
    std::expected<bool, DatabaseError> cancelReserve(std::string ticketId);
    std::unique_ptr<Ticket> mapRowToTicket(PGresult *result, int row);
};