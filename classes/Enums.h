#pragma once
enum class TicketError
{
    ALREADY_RESERVED,
    ALREADY_PURCHASED,
    CANCELLED,
    NOT_FOUND,
    INVALID_USER
};

enum class TicketStatus
{
    AVAILABLE,
    RESERVED,
    PURCHASED,
    CANCELLED
};

enum class ServiceError
{
    NOT_FOUND,
    WRONG_STATE,
    INVALID_USER,
    DATABASE_ERROR
};

enum class DatabaseError
{
    NOT_FOUND,
    QUERY_FAILED,
    CONNECTION_FAILED,
    DUPLICATE_ENTRY
};