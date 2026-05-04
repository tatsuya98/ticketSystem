#pragma once
#include <mutex>
#include <string>
#include <expected>
#include <chrono>
#include "Enums.h"
class Ticket
{
    friend class TicketRepository;

private:
    std::mutex ticketMutex;
    std::string ticketId;
    std::string eventId;
    std::string userId;
    TicketStatus status;
    std::string seatId;
    std::chrono::system_clock::time_point purchasedAt;

public:
    Ticket(std::string id, std::string eId, std::string uId,
           TicketStatus s, std::string seat, std::optional<std::chrono::system_clock::time_point> pAt)
        : ticketId(id), eventId(eId), userId(uId),
          status(s), seatId(seat), purchasedAt(pAt.value_or(std::chrono::system_clock::time_point{})) {}
    std::expected<bool, TicketError> reserveTicket(std::string userId);
    std::expected<bool, TicketError> purchaseTicket(std::string userId);
    std::expected<bool, TicketError> cancelTicket(std::string userId);
    std::string getEventId() const;
    std::string getUserId() const;
    std::string getTicketId() const;
    TicketStatus getStatus() const;
    std::string getSeatId() const;
    std::chrono::system_clock::time_point getPurchasedAt() const;
};
