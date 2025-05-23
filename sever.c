#include <stdio.h>
#include <winsock2.h>
#include <conio.h>

#define port 8080

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
    
    while (1) {
        if (_kbhit()) {
            char ch = _getch();
            if (ch == 'q' || ch == 'Q') {
                break;
            }
        }
        printf("RPC is listening on port %d...\n", port);

        c = sizeof(struct sockaddr_in);
        client_socket = accept(server_socket, (struct sockaddr *)&client, &c);
        if (client_socket == INVALID_SOCKET) {
            printf("Refuse connection!.\n");
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
        printf("Result is sent to client: %d (Status code: %d)\n", res.result, res.status_code);
        closesocket(client_socket);
        printf("Close connection.\n\n\n");
    }

    // Close the server socket
    closesocket(server_socket);
    printf("Server socket is closed\n");
    // Cleanup Winsock
    WSACleanup();
    return 0;
}
