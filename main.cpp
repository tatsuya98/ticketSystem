#include "Enums.h"
#include "Structs.h"
#include "Ticket.h"
#include "DTOs.h"
#include <iostream>
#include <libpq-fe.h>

int main()
{
    std::cout << "libpq version: " << PQlibVersion() << std::endl;
    return 0;
}