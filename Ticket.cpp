#include "Ticket.h"

std::expected<bool, TicketError> Ticket::reserveTicket(std::string userId)
{
    std::lock_guard<std::mutex> lock(ticketMutex);
    if (status != TicketStatus::AVAILABLE)
    {
        return std::unexpected(TicketError::WRONG_STATE);
    }
    if (userId != this->userId)
    {
        return std::unexpected(TicketError::INVALID_USER);
    }
    this->userId = userId;
    status = TicketStatus::RESERVED;
    return true;
}

std::expected<bool, TicketError> Ticket::purchaseTicket(std::string userId)
{
    std::lock_guard<std::mutex> lock(ticketMutex);
    if (status != TicketStatus::RESERVED)
    {
        return std::unexpected(TicketError::WRONG_STATE);
    }
    if (userId != this->userId)
    {
        return std::unexpected(TicketError::INVALID_USER);
    }
    status = TicketStatus::PURCHASED;
    return true;
}

std::expected<bool, TicketError> Ticket::cancelTicket(std::string userId)
{
    std::lock_guard<std::mutex> lock(ticketMutex);
    if (status != TicketStatus::RESERVED)
    {
        return std::unexpected(TicketError::WRONG_STATE);
    }
    if (userId != this->userId)
    {
        return std::unexpected(TicketError::INVALID_USER);
    }
    if (status == TicketStatus::RESERVED || status == TicketStatus::PURCHASED)
    {
        status = TicketStatus::AVAILABLE;
    }
    return true;
}
