#pragma once
#include "EventRepository.h"
#include "TicketRepository.h"
#include <expected>
#include <memory>
#include <string>
#include "Enums.h"
#include "Structs.h"

class TicketManager
{
private:
    EventRepository &eventRepository;
    TicketRepository &ticketRepository;

public:
    TicketManager(EventRepository &eventRepo, TicketRepository &ticketRepo);
    std::expected<bool, ServiceError> handleReserve(std::string ticketId, std::string userId);
    std::expected<bool, ServiceError> handlePurchase(std::string ticketId, std::string userId);
    std::expected<bool, ServiceError> handleCancel(std::string ticketId, std::string userId);
};