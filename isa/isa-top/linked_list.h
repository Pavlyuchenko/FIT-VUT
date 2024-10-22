/**
 * Author: Michal Pavlíček xpavlim00
 * Date: 21. 10. 2024
 *
 * Description: Defines functions for an automatically sorted
 * LinkedList for CommunicationInfo struct. It is sorted either
 * based on packets sent or bytes sent.
 */

// This file
#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "global.h"

#define MAX_IP_LEN 40
#define MAX_PROTO_LEN 100

// ncurses
#define MAX_DISPLAY_ROWS 10
#define MAX_IP_DISPLAY_LEN 46

// struct for holding information about a communication between two ip addresses
typedef struct CommunicationInfo {
    // info about hosts
    char src_ip[MAX_IP_LEN];
    char dst_ip[MAX_IP_LEN];
    uint16_t src_port;
    uint16_t dst_port;
    char protocol[MAX_PROTO_LEN];

    // info about their communication (statistics)
    long int Rx;              // received data in bytes
    long int Tx;              // transmitted data in bytes
    long int packets_sent_Rx; // packets sent between these two IPs
    long int packets_sent_Tx; // packets sent between these two IPs
} CommunicationInfo;

// Node of doubly LinkedList
typedef struct Node {
    CommunicationInfo *data;
    struct Node *next;
    struct Node *prev;
} Node;

typedef struct LinkedList {
    Node *head;
} LinkedList;

// global linked list variable
extern LinkedList *llist;

// initializes linked list structure
void init_llist();

// initializes empty node object
Node *init_node();

// returns node whose CommunicationInfo matches that in param
Node *get_node(CommunicationInfo *c_info);

// sorts node to its rightful place based on sorting chosen
void update_node_position(Node *node);

void print_node_data(Node *node);
void print_llist(int count);
void display_network_data(int count);

#endif // LINKED_LIST_H
