#include "Ticket.h"

std::expected<bool, TicketError> Ticket::reserveTicket(std::string userId)
{
    std::lock_guard<std::mutex> lock(ticketMutex);
    if (status == TicketStatus::AVAILABLE)
    {
        status = TicketStatus::RESERVED;
        this->userId = userId;
        return true;
    }
    return std::unexpected(TicketError::ALREADY_RESERVED);
}
