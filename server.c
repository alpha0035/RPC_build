#include <stdio.h>
#include <winsock2.h>
#include <conio.h>
#include <time.h>

#define port 8080
#define timeout 60

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

// Hàm xử lý yêu cầu RPC trong thread riêng biệt
/*
param client_socket_ptr: con trỏ đến socket của client
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

    printf("\nPerforming request from client: func_id=%d, a=%d, b=%d\n",
           req.function_id, req.params[0], req.params[1]);

    res.status_code = 0;
    switch (req.function_id) {
        case 1: // add
            res.result = sum(req.params[0], req.params[1]);
            break;
        case 2: // sub
            res.result = sub(req.params[0], req.params[1]);
            break;
        case 3: // mul
            res.result = mul(req.params[0], req.params[1]);
            break;
        case 4: // div
            if (req.params[1] == 0) {
                res.status_code = 1;
                res.result = 0;
            } else {
                res.result = div1(req.params[0], req.params[1]);
            }
            break;
        default:
            res.status_code = 2;
            res.result = 0;
    }
    send(client_socket, (char *)&res, sizeof(res), 0);
    printf("Result sent: %d (Status code: %d)\n", res.result, res.status_code);

    closesocket(client_socket);
    // printf("Closed client connection.\n\n");
    return 0;
}

int main() {
    WSADATA wsa;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server, client;
    int c;

    WSAStartup(MAKEWORD(2, 2), &wsa);

    // khởi tạo socket
    // AF_INET: IPv4
    // SOCK_STREAM: TCP
    // 0: default protocol
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        printf("Could not create socket: %d\n", WSAGetLastError());
        return 1;
    }

    // Configure server address
    // INADDR_ANY allows the server to accept connections from any IP address
    // htons() converts the port number from host byte order to network byte order
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);

    // Bind the socket to the sever address and listen for incoming connections
    bind(server_socket, (struct sockaddr *)&server, sizeof(server));
    listen(server_socket, 3);
    
    u_long mode = 1;
    /* 
    ioctlsocket() is used to set the socket to non-blocking mode
    FIONBIO is the command to set the socket to non-blocking mode
    The mode variable is set to 1 to enable non-blocking mode
    */
   ioctlsocket(server_socket, FIONBIO, &mode);
   time_t start_time = time(NULL);
   
   printf("Server is listening on port %d...\n", port);
   while (1) {
       // Check if the user pressed a key to exit
       if (_kbhit()) {
           char ch = _getch();
           break;
        }
        // Accept a client connection, if no client is connected, it will return INVALID_SOCKET
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
            // if no client is connected, wait for a while and continue
            Sleep(300);
            continue;
        }
        printf("\nClient connected from %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
        
        // Create a new thread to handle the client request
        SOCKET *new_sock = malloc(sizeof(SOCKET));
        *new_sock = client_socket;
        CreateThread(NULL, 0, handle_client, (void *)new_sock, 0, NULL);
        printf("\nRPC is listening on port %d...\n", port);
    }
    
    // Close the server socket
    closesocket(server_socket);
    printf("Server socket is closed.\n");
    // Cleanup Winsock
    WSACleanup();
    return 0;
}
