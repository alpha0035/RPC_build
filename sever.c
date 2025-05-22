#include <stdio.h>
#include <winsock2.h>
#include <conio.h>

struct RPCRequest {
    int function_id;
    int params[2];
};

struct RPCResponse {
    int result;
    int status_code;
};

#define port 8080

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
    Sleep(3000); // Giả lập thời gian xử lý yêu cầu
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
    
    // catch SIGINT signal to allow graceful shutdown
    // signal(SIGINT, intHandler);
    while (1) {
        if (_kbhit()) {
            char ch = _getch();
            break;
        }
        printf("\nRPC is listening on port %d...\n", port);

        c = sizeof(struct sockaddr_in);
        client_socket = accept(server_socket, (struct sockaddr *)&client, &c);
        if (client_socket == INVALID_SOCKET) {
            printf("Refuse connection!.\n");
            continue;
        }
        printf("\nClient connected from %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));

        // Create a new thread to handle the client request
        SOCKET *new_sock = malloc(sizeof(SOCKET));
        *new_sock = client_socket;
        CreateThread(NULL, 0, handle_client, (void *)new_sock, 0, NULL);
    }

    // Close the server socket
    closesocket(server_socket);
    printf("Server socket is closed\n");
    // Cleanup Winsock
    WSACleanup();
    return 0;
}
