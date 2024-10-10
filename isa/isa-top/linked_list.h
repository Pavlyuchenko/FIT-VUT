// This file defines functions for an automatically sorted
// LinkedList for CommunicationInfo struct. It is sorted either
// based on packets sent or bytes sent. 
#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "error.h"

#define MAX_IP_LEN  40
#define MAX_PROTO_LEN  100

// struct for holding information about a communication between two ip addresses
typedef struct CommunicationInfo {
    // info about hosts
    char src_ip[MAX_IP_LEN];
    char dst_ip[MAX_IP_LEN];
    uint16_t src_port;
    uint16_t dst_port;
    char protocol[MAX_PROTO_LEN];

    // info about their communication (statistics)
    long int Rx;           // received data in bytes
    long int Tx;           // transmitted data in bytes
    long int packets_sent; // packets sent between these two IPs
} CommunicationInfo;

typedef struct Node {
	CommunicationInfo *data;
	struct Node *next;
	struct Node *prev;
} Node;

typedef struct LinkedList {
	Node *head;
} LinkedList;

extern LinkedList *llist;

void init_llist();

Node *get_node(CommunicationInfo *c_info);

void update_node_position(Node *node, bool order);
Node *init_node();

void print_llist(int count);

#endif // LINKED_LIST_H
