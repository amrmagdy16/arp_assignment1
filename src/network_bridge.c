#include "../include/handlers.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define NET_PORT_DEFAULT 5555

// Coordinate Conversion: ncurses (Top-Left) <-> Virtual (Bottom-Left)
float to_virtual_y(float y) { return (LINES_WIN - 1) - y; }
float to_local_y(float y) { return (LINES_WIN - 1) - y; }

// Protocol Helpers for String-based transmission
int send_msg(int sock, const char *msg) {
    char buf[256];
    snprintf(buf, sizeof(buf), "%s\n", msg);
    return write(sock, buf, strlen(buf));
}

int recv_msg(int sock, char *buf, int size) {
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
    if (argc < 4) return 1;
    
    int mode = atoi(argv[1]); 
    int pipe_rx = atoi(argv[2]); 
    int pipe_tx = atoi(argv[3]); 
    int port = (argc > 4) ? atoi(argv[4]) : NET_PORT_DEFAULT;
    char *ip = (argc > 5) ? argv[5] : "127.0.0.1";

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    log_process_pid("NetBridge");

    if (mode == 2) { // SERVER MODE
        addr.sin_addr.s_addr = INADDR_ANY;
        int opt = 1;
        setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        bind(sock, (struct sockaddr*)&addr, sizeof(addr));
        listen(sock, 1);
        int client_sock = accept(sock, NULL, NULL);
        close(sock);
        sock = client_sock;
        
        send_msg(sock, "ok");
        char buf[100]; recv_msg(sock, buf, 100);
        char size_msg[50]; sprintf(size_msg, "size %d %d", COLS_WIN, LINES_WIN);
        send_msg(sock, size_msg);
        recv_msg(sock, buf, 100);
    } else { // CLIENT MODE
        struct hostent *he = gethostbyname(ip);
        if (he) memcpy(&addr.sin_addr, he->h_addr_list[0], he->h_length);
        while(connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            sleep(1);
        }
        
        char buf[100]; recv_msg(sock, buf, 100);
        send_msg(sock, "ook");
        recv_msg(sock, buf, 100);
        send_msg(sock, "sok");
    }

    Drone my_drone = {0};
    Obstacle remote_obs = {0};
    char buf[256];
    fcntl(pipe_rx, F_SETFL, O_NONBLOCK);

    while(1) {
        while(read(pipe_rx, &my_drone, sizeof(Drone)) > 0);
        
        if (mode == 2) { 
            send_msg(sock, "drone");
            sprintf(buf, "%.2f %.2f", my_drone.x, to_virtual_y(my_drone.y));
            send_msg(sock, buf);
            recv_msg(sock, buf, 256);

            send_msg(sock, "obst");
            recv_msg(sock, buf, 256);
            float rx, ry; sscanf(buf, "%f %f", &rx, &ry);
            send_msg(sock, "pok");

            memset(&remote_obs, 0, sizeof(remote_obs));
            remote_obs.x[0] = (int)rx;
            remote_obs.y[0] = (int)to_local_y(ry);
            write(pipe_tx, &remote_obs, sizeof(Obstacle));
        } else { 
            recv_msg(sock, buf, 256);
            recv_msg(sock, buf, 256);
            float rx, ry; sscanf(buf, "%f %f", &rx, &ry);
            send_msg(sock, "dok");

            memset(&remote_obs, 0, sizeof(remote_obs));
            remote_obs.x[0] = (int)rx;
            remote_obs.y[0] = (int)to_local_y(ry);
            write(pipe_tx, &remote_obs, sizeof(Obstacle));

            recv_msg(sock, buf, 256);
            sprintf(buf, "%.2f %.2f", my_drone.x, to_virtual_y(my_drone.y));
            send_msg(sock, buf);
            recv_msg(sock, buf, 256);
        }
        usleep(20000);
    }
    return 0;
}
