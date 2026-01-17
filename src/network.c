#include "../include/handlers.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define NET_PORT_DEFAULT 5555

float to_virtual_y(float y) { return (LINES_WIN - 1) - y; }
float to_local_y(float y) { return (LINES_WIN - 1) - y; }

// Helper to send line
int send_msg(int sock, const char *fmt, ...) {
    char buf[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    strcat(buf, "\n"); 
    return write(sock, buf, strlen(buf));
}

int recv_line(int sock, char *buf, int size) {
    int i = 0; char c;
    while (i < size - 1) {
        if (read(sock, &c, 1) <= 0) return -1;
        if (c == '\n') break;
        buf[i++] = c;
    }
    buf[i] = '\0';
    return i;
}

int main(int argc, char *argv[]) {
    log_process_pid("Network"); 

    if (argc < 4) return 1;
    int mode = atoi(argv[1]); 
    int pipe_rx = atoi(argv[2]); // Reads Drone Pos from Server
    int pipe_tx = atoi(argv[3]); // Writes Remote Pos to Server
    int port = (argc > 4) ? atoi(argv[4]) : NET_PORT_DEFAULT;
    char *ip = (argc > 5) ? argv[5] : "127.0.0.1";

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    // --- CONNECTION PHASE ---
    if (mode == 2) { // SERVER
        addr.sin_addr.s_addr = INADDR_ANY;
        int opt = 1;
        setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        bind(sock, (struct sockaddr*)&addr, sizeof(addr));
        listen(sock, 1);
        int client_sock = accept(sock, NULL, NULL);
        close(sock);
        sock = client_sock;
        
        // Handshake
        send_msg(sock, "ok");
        char buf[100]; recv_line(sock, buf, 100);
        send_msg(sock, "size %d %d", COLS_WIN, LINES_WIN);
        recv_line(sock, buf, 100);

    } else { // CLIENT
        struct hostent *he = gethostbyname(ip);
        if (he) memcpy(&addr.sin_addr, he->h_addr_list[0], he->h_length);
        while(connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) sleep(1);
        
        // Handshake
        char buf[100]; recv_line(sock, buf, 100);
        send_msg(sock, "ook");
        recv_line(sock, buf, 100);
        send_msg(sock, "sok");
    }

    // --- MAIN LOOP ---
    Drone my_drone = {0};
    Obstacle remote_obs = {0}; // Uses common.h struct
    char buf[256];
    
    // Non-blocking read from server pipe
    fcntl(pipe_rx, F_SETFL, O_NONBLOCK);

    while(1) {
        // 1. Get Local Drone Position
        while(read(pipe_rx, &my_drone, sizeof(Drone)) > 0);
        
        if (mode == 2) { // SERVER PROTOCOL
            send_msg(sock, "drone");
            send_msg(sock, "%.2f %.2f", my_drone.x, to_virtual_y(my_drone.y));
            recv_line(sock, buf, 256); 

            send_msg(sock, "obst");
            recv_line(sock, buf, 256);
            
            float rx, ry; sscanf(buf, "%f %f", &rx, &ry);
            send_msg(sock, "pok");

            // [EDIT]: Adapt single remote coordinate to Obstacle Array
            memset(&remote_obs, 0, sizeof(remote_obs)); 
            remote_obs.x[0] = (int)rx; 
            remote_obs.y[0] = (int)to_local_y(ry);
            write(pipe_tx, &remote_obs, sizeof(Obstacle));

        } else { // CLIENT PROTOCOL
            recv_line(sock, buf, 256); // "drone"
            recv_line(sock, buf, 256); // Coords
            
            float rx, ry; sscanf(buf, "%f %f", &rx, &ry);
            send_msg(sock, "dok");

            memset(&remote_obs, 0, sizeof(remote_obs));
            remote_obs.x[0] = (int)rx;
            remote_obs.y[0] = (int)to_local_y(ry);
            write(pipe_tx, &remote_obs, sizeof(Obstacle));

            recv_line(sock, buf, 256); // "obst"
            send_msg(sock, "%.2f %.2f", my_drone.x, to_virtual_y(my_drone.y));
            recv_line(sock, buf, 256); // "pok"
        }
        usleep(30000); // 30ms sync rate
    }
    close(sock);
    return 0;
}
