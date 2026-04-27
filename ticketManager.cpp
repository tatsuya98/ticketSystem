#include "TicketManager.h"

TicketManager::TicketManager(EventRepository &eventRepo, TicketRepository &ticketRepo) : eventRepository(eventRepo), ticketRepository(ticketRepo) {}

std::expected<bool, ServiceError> TicketManager::handlePurchase(std::string ticketId, std::string userId)
{
    auto ticket = ticketRepository.findTicketById(ticketId);
    if (!ticket)
    {
        return std::unexpected(ServiceError::NOT_FOUND);
    }
    auto result = ticket.value()->purchaseTicket(userId);

    if (!result)
    {
        switch (result.error())
        {
        case TicketError::WRONG_STATE:
            return std::unexpected(ServiceError::WRONG_STATE);
        case TicketError::INVALID_USER:
            return std::unexpected(ServiceError::INVALID_USER);
        default:
            return std::unexpected(ServiceError::NOT_FOUND);
        }
    }
    auto transactionResult = eventRepository.purchaseTicketTransaction(ticketId, ticket.value()->getEventId());
    if (!transactionResult)
    {
        return std::unexpected(ServiceError::DATABASE_ERROR);
    }
    return true;
};

std::expected<bool, ServiceError> TicketManager::handleReserve(std::string ticketId, std::string userId)
{
    auto ticket = ticketRepository.findTicketById(ticketId);
    if (!ticket)
    {
        return std::unexpected(ServiceError::NOT_FOUND);
    }
    auto result = ticket.value()->reserveTicket(userId);

    if (!result)
    {
        switch (result.error())
        {
        case TicketError::WRONG_STATE:
            return std::unexpected(ServiceError::WRONG_STATE);
        case TicketError::INVALID_USER:
            return std::unexpected(ServiceError::INVALID_USER);
        default:
            return std::unexpected(ServiceError::NOT_FOUND);
        }
    }
    TicketUpdate fieldsToUpdate;
    fieldsToUpdate.userId = userId;
    fieldsToUpdate.status = TicketStatus::RESERVED;
    auto transactionResult = ticketRepository.updateTicket(ticketId, fieldsToUpdate);
    if (!transactionResult)
    {
        return std::unexpected(ServiceError::DATABASE_ERROR);
    }
    return true;
};

std::expected<bool, ServiceError> TicketManager::handleCancel(std::string ticketId, std::string userId)
{
    auto ticket = ticketRepository.findTicketById(ticketId);
    if (!ticket)
    {
        return std::unexpected(ServiceError::NOT_FOUND);
    }
    auto result = ticket.value()->cancelTicket(userId);

    if (!result)
    {
        switch (result.error())
        {
        case TicketError::WRONG_STATE:
            return std::unexpected(ServiceError::WRONG_STATE);
        case TicketError::INVALID_USER:
            return std::unexpected(ServiceError::INVALID_USER);
        case TicketError::CANCELLATION_WINDOW_EXPIRED:
            return std::unexpected(ServiceError::CANCELLATION_WINDOW_EXPIRED);
        default:
            return std::unexpected(ServiceError::NOT_FOUND);
        }
    }
    auto cancelTransactionResult = eventRepository.cancelTicketTransaction(ticketId, ticket.value()->getEventId());
    if (!cancelTransactionResult)
    {
        return std::unexpected(ServiceError::DATABASE_ERROR);
    }
    return true;
};