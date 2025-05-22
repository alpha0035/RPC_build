#include <stdio.h>
#include <winsock2.h>
#include <conio.h>
#include <time.h>

struct RPCRequest {
    int function_id;
    int params[2];
};

struct RPCResponse {
    int result;
    int status_code;
};

#define port 8080
#define timeout 20

int main() {
    WSADATA wsa;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server, client;
    int c, recv_size;
    struct RPCRequest req;
    struct RPCResponse res;

    WSAStartup(MAKEWORD(2, 2), &wsa);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // Configure server address
    // INADDR_ANY allows the server to accept connections from any IP address
    // htons() converts the port number from host byte order to network byte order
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);

    // Bind the socket to the sever address and listen for incoming connections
    bind(server_socket, (struct sockaddr *)&server, sizeof(server));
    listen(server_socket, 3);
    printf("RPC is listening on port %d...\n", port);
    
    u_long mode = 1;
    /* 
    ioctlsocket() is used to set the socket to non-blocking mode
    FIONBIO is the command to set the socket to non-blocking mode
    The mode variable is set to 1 to enable non-blocking mode
    */
    ioctlsocket(server_socket, FIONBIO, &mode);
    time_t start_time = time(NULL);

    while (1) {
        c = sizeof(struct sockaddr_in);
        client_socket = accept(server_socket, (struct sockaddr *)&client, &c);
        if (client_socket == INVALID_SOCKET) {
            int err = WSAGetLastError();
            if (err != WSAEWOULDBLOCK) {
                printf("Connection failed with error: %d\n", err);
                break;
            }
            
            // If no client is connected, check if timeout
            if (difftime(time(NULL), start_time) >= timeout) {
                printf("No client connected for %d seconds. Shutting down...\n", timeout);
                break;
            }
            
            // Không có client => chờ 100ms rồi lặp lại
            Sleep(100);
            continue;
        }
        printf("Client connected from %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
        
        recv_size = recv(client_socket, (char *)&req, sizeof(req), 0);
        if (recv_size == SOCKET_ERROR) {
            printf("Receive failed!\n");
            closesocket(client_socket);
            continue;
        }
        printf("Request from client: func_id=%d, a=%d, b=%d\n", req.function_id, req.params[0], req.params[1]);
        
        // Reset timeout
        start_time = time(NULL);
        res.status_code = 0;
        
        switch (req.function_id) {
            case 1: // add
            res.result = req.params[0] + req.params[1];
            break;
            case 2: // sub
            res.result = req.params[0] - req.params[1];
            break;
            case 3: // mul
            res.result = req.params[0] * req.params[1];
            break;
            case 4: // div
            if (req.params[1] == 0) {
                res.status_code = 1;
                res.result = 0;
            } else {
                res.result = req.params[0] / req.params[1];
            }
            break;
            default:
            res.status_code = 2;
            res.result = 0;
        }
        
        send(client_socket, (char *)&res, sizeof(res), 0);
        printf("Result is sent to client: %d (Status code: %d)\n", res.result, res.status_code);
        
        closesocket(client_socket);
        printf("Close connection.\n\n\n");
        printf("RPC is listening on port %d...\n", port);
    }
    
    // Close the server socket
    closesocket(server_socket);
    printf("Server socket is closed.\n");
    // Cleanup Winsock
    WSACleanup();
    return 0;
}
