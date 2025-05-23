#include <stdio.h>
#include <winsock2.h>
#include <conio.h>
#include <time.h>

#define port 8080
#define timeout 30

struct RPCRequest {
    int function_id;
    int params[2];
};

struct RPCResponse {
    int result;
    int status_code;
};

int sum(int a, int b) {
    return a + b;
}
int sub(int a, int b) {
    return a - b;
}
int mul(int a, int b) {
    return a * b;
}
int div1(int a, int b) {
    return a / b;
}

void handle_rpc(int function_id, int a, int b, struct RPCResponse *res) {
    res->status_code = 0;
    switch (function_id) {
        case 1: // add
            res->result = sum(a, b);
            break;
        case 2: // sub
            res->result = sub(a, b);
            break;
        case 3: // mul
            res->result = mul(a, b);
            break;
        case 4: // div
            if (b == 0) {
                res->status_code = 1;
                res->result = 0;
            } else {
                res->result = div1(a, b);
            }
            break;
        default:
            res->status_code = 2;
            res->result = 0;
            break;
    }
}

/*
 Hàm xử lý yêu cầu RPC trong thread riêng biệt
 param: client_socket_ptr - con trỏ đến socket của client
 return: 0 nếu thành công, 1 nếu thất bại
*/
DWORD WINAPI handle_client(LPVOID client_socket_ptr) {
    SOCKET client_socket = *(SOCKET *)client_socket_ptr;
    free(client_socket_ptr); // giải phóng con trỏ được cấp phát trong main()

    struct RPCRequest req;
    struct RPCResponse res;
    int recv_size;

    recv_size = recv(client_socket, (char *)&req, sizeof(req), 0);
    if (recv_size == SOCKET_ERROR || recv_size == 0) {
        printf("Receive failed or client disconnected.\n");
        closesocket(client_socket);
        return 1;
    }

    handle_rpc(req.function_id, req.params[0], req.params[1], &res);
    Sleep(5000);
    send(client_socket, (char *)&res, sizeof(res), 0);
    printf("\nPerforming request from client: func_id=%d, a=%d, b=%d\nResult sent: %d (Status code: %d)\n",
           req.function_id, req.params[0], req.params[1], res.result, res.status_code);
    closesocket(client_socket);
    return 0;
}

int main() {
    WSADATA wsa;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server, client;
    int c;
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
                printf("\nNo client connected for %d seconds. Shutting down...\n", timeout);
                break;
            }
            // if no client is connected, wait for a while and continue
            Sleep(5000);
            printf("...\n");
            continue;
        }
        printf("\nClient connected from %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
        // Reset timeout
        start_time = time(NULL);
        
        // Create a new thread to handle the client request
        SOCKET *new_sock = malloc(sizeof(SOCKET));
        *new_sock = client_socket;
        CreateThread(NULL, 0, handle_client, (void *)new_sock, 0, NULL);
        printf("\nRPC is listening on port %d...\n", port);
        }
        
        // Close the server socket
        closesocket(server_socket);
        WSACleanup();
        printf("---Server socket is closed---\n");

    return 0;
}
