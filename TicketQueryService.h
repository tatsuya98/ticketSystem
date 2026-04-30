#pragma once
#include "Structs.h"
#include <vector>
#include <expected>
#include "Enums.h"
#include "DTOs.h"
#include <memory>
#include "Ticket.h"
#include "TicketRepository.h"
#include "EventRepository.h"
class TicketQueryService
{
private:
    TicketRepository &ticketRepository;
    EventRepository &eventRepository;

public:
    TicketQueryService(TicketRepository &ticketRepo, EventRepository &eventRepo);
    std::expected<std::vector<UserTicketDTO>, ServiceError> findTicketsByUserId(std::string id);
    std::expected<std::vector<Ticket>, ServiceError> findTicketsByEventId(std::string id);
    std::expected<std::vector<EventDetailDTO>, ServiceError> findAllEvents();
    std::expected<std::vector<EventDetailDTO>, ServiceError> findEventsByLocation(std::string location);
    std::expected<CheckInDTO, ServiceError> findTicketForCheckIn(std::string ticketId);
    std::expected<EventDetailDTO, ServiceError> findEventDetailsById(std::string eventId);
};