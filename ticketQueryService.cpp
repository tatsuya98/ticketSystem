#include "TicketQueryService.h"

TicketQueryService::TicketQueryService(TicketRepository &ticketRepo, EventRepository &eventRepo)
    : ticketRepository(ticketRepo), eventRepository(eventRepo) {}

std::expected<std::vector<UserTicketDTO>, ServiceError> TicketQueryService::findTicketsByUserId(std::string id)
{
    auto tickets = ticketRepository.getTicketsByUserId(id);
    if (!tickets)
    {
        return std::unexpected(ServiceError::NOT_FOUND);
    }
    std::vector<UserTicketDTO> ticketsToDisplay;
    for (auto &ticket : tickets.value())
    {
        auto event = eventRepository.getEventById(ticket->getEventId());
        if (!event)
        {
            return std::unexpected(ServiceError::NOT_FOUND);
        }
        UserTicketDTO ticketInfoToDisplay;
        ticketInfoToDisplay.ticketId = ticket->getTicketId();
        ticketInfoToDisplay.eventName = event.value().eventName;
        std::string eventLocation = event.value().venueAddress + "\n" + event.value().venueCity;
        ticketInfoToDisplay.eventLocation = eventLocation;
        ticketInfoToDisplay.eventDate = event.value().eventDate;
        ticketInfoToDisplay.seatId = ticket->getSeatId();
        ticketInfoToDisplay.status = ticket->getStatus();
        ticketsToDisplay.push_back(ticketInfoToDisplay);
    }
    return ticketsToDisplay;
}
