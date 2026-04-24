#pragma once
#include <mutex>
#include <string>
#include <expected>
#include "Enums.h"
class Ticket
{
private:
    std::mutex ticketMutex;
    std::string ticketId;
    std::string eventId;
    std::string userId;
    TicketStatus status;
    std::string seatId;

public:
    std::expected<bool, TicketError> reserveTicket(std::string userId);
    std::expected<bool, TicketError> purchaseTicket(std::string userId);
    std::expected<bool, TicketError> cancelTicket(std::string userId);
};
