#include "Ticket.h"
std::expected<bool, TicketError> Ticket::reserveTicket(std::string userId)
{
    std::lock_guard<std::mutex> lock(ticketMutex);
    if (status != TicketStatus::AVAILABLE)
    {
        return std::unexpected(TicketError::WRONG_STATE);
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
    purchasedAt = std::chrono::system_clock::now();
    return true;
}

std::expected<bool, TicketError> Ticket::cancelTicket(std::string userId)
{
    std::lock_guard<std::mutex> lock(ticketMutex);
    if (status != TicketStatus::RESERVED && status != TicketStatus::PURCHASED)
    {
        return std::unexpected(TicketError::WRONG_STATE);
    }
    if (userId != this->userId)
    {
        return std::unexpected(TicketError::INVALID_USER);
    }
    if (status == TicketStatus::PURCHASED)
    {
        auto now = std::chrono::system_clock::now();
        auto hoursSincePurchase = std::chrono::duration_cast<std::chrono::hours>(now - purchasedAt).count();
        if (hoursSincePurchase > 24)
        {
            return std::unexpected(TicketError::CANCELLATION_WINDOW_EXPIRED);
        }
    }
    status = TicketStatus::CANCELLED;
    this->userId = "";
    return true;
}

std::string Ticket::getEventId() const
{
    return eventId;
}

std::string Ticket::getUserId() const
{
    return userId;
}

std::string Ticket::getTicketId() const
{
    return ticketId;
}

TicketStatus Ticket::getStatus() const
{
    return status;
}

std::string Ticket::getSeatId() const
{
    return seatId;
}

std::chrono::system_clock::time_point Ticket::getPurchasedAt() const
{
    return purchasedAt;
}