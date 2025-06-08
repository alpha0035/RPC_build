#include <stdio.h>
#include <winsock2.h>
#include <stdlib.h>

#define port 8080
#define sever_address "127.0.0.1"

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
    /*
     Tạo socket
     pram 1: AF_INET - giao thức IPv4
     pram 2: SOCK_STREAM - giao thức TCP
     pram 3: 0 - tự động chọn giao thức
    */
   sock = socket(AF_INET, SOCK_STREAM, 0);
   
    /*
     Cấu hình địa chỉ server
     sever_address là địa chỉ IP của server
     AF_INET là giao thức IPv4
     htons() chuyển đổi số nguyên từ host byte order sang network byte order
    */
    server.sin_addr.s_addr = inet_addr(sever_address);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

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

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Caution! Wrong input syntax! <function_id> <a> <b>\n");
        printf("function_id: 1:add, 2:sub, 3:mul, 4:div\n");
        return 1;
    }
    
    // Convert command line arguments to integers
    int function_id = atoi(argv[1]);
    int a = atoi(argv[2]);
    int b = atoi(argv[3]);
    int result = rpc_call(function_id, a, b);

    // Check for errors, if not, print the result
    if (result == -9999) {
        printf("Error! Can't connect to server.\n");
    } else if (result == -10000) {
        printf("Error! Division by zero.\n");
    } else if (result == -10001) {
        printf("Error! Sever can't perform the request.\n");
    } else {
        printf("Result: %d\n", result);
    }

    return 0;
}
