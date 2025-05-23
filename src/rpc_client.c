// src/rpc_client.c
#include <stdio.h>
#include <winsock2.h>
#include "rpc.h"

#define PORT 8080
#define SERVER_ADDRESS "127.0.0.1"

struct RPCRequest {
    int function_id;
    int params[2];
};

struct RPCResponse {
    int result;
    int status_code;
};

int rpc_call(int function_id, int a, int b) {
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server;
    struct RPCRequest req;
    struct RPCResponse res;

    req.function_id = function_id;
    req.params[0] = a;
    req.params[1] = b;

    WSAStartup(MAKEWORD(2, 2), &wsa);
    sock = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    int check = connect(sock, (struct sockaddr *)&server, sizeof(server));
    if (check == SOCKET_ERROR) {
        closesocket(sock);
        WSACleanup();
        return -9999; // Error code for connection failure
    }

    send(sock, (char *)&req, sizeof(req), 0);
    recv(sock, (char *)&res, sizeof(res), 0);

    closesocket(sock);
    WSACleanup();

    if (res.status_code != 0) {
        return -9999 - res.status_code;  // error mapping
    }

    return res.result;
}
