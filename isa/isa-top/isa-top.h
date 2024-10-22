/**
 * Author: Michal Pavlíček xpavlim00
 * Date: 21. 10. 2024
 *
 * isa-top: displays network traffic on an interface by host.
 * Created for course ISA @ FIT VUT. Copyright Michal Pavlíček
 * <xpavlim00@stud.fit.vutbr.cz, michaelg.pavlicek@gmail.com>, 2024.
 */

#ifndef ISA_TOP_H
#define ISA_TOP_H

#include <pthread.h>
#include <stdio.h>

// threading imports
#include "error.h"
#include "global.c"
#include "linked_list.h"
#include "packet_handler.h"
#include "stats.h"

int main(int argc, char *argv[]);

#endif // ISA_TOP_H
