#ifndef NETWORK_H
#define NETWORK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define NET_PORT 5555  // Must be > 5000 [cite: 223]
#define BUFF_SIZE 256

// Coordinate Conversion
// Converts ncurses (Top-Left 0,0) to Virtual (Bottom-Left 0,0) and back
int to_virtual_y(int y, int height);
int to_ncurses_y(int y, int height);

// Socket Setup
int setup_server_socket(int port);
int connect_to_server(const char *ip, int port);

// Strict Protocol Helpers (Blocking Handshakes) 
void net_send_string(int fd, const char *str);
void net_recv_string(int fd, char *buffer);
void net_send_ack(int fd, const char *ack_msg);
void net_expect_ack(int fd, const char *expected_ack);

#endif
