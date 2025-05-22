#include <stdio.h>
#include <winsock2.h>
#include <stdlib.h>

#define port 10498 // Port ngrok cung cấp
#define host_addr "0.tcp.ap.ngrok.io" // Địa chỉ ngrok

struct RPCRequest {
    int function_id;
    int params[2];
};

struct RPCResponse {
    int result;
    int status_code;
};

int main(int argc, char *argv[]) {
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server;
    struct RPCRequest req;
    struct RPCResponse res;
    struct hostent *host;

    if (argc != 4) {
        printf("Caution! Wrong input syntax! <function_id> <a> <b>\n");
        printf("function_id: 1:add, 2:sub, 3:mul, 4:div\n");
        return 1;
    }

    // Chuyển đổi các tham số đầu vào từ chuỗi sang số nguyên
    req.function_id = atoi(argv[1]);
    req.params[0] = atoi(argv[2]);
    req.params[1] = atoi(argv[3]);

    WSAStartup(MAKEWORD(2, 2), &wsa);
    host = gethostbyname(host_addr);
    if (host == NULL) {
        printf("Error: Can't resolve domain name\n");
        return 0;
    }

    /*
    Tạo socket
    pram 1: AF_INET - giao thức IPv4
    pram 2: SOCK_STREAM - giao thức TCP
    pram 3: 0 - tự động chọn giao thức
    */
    sock = socket(AF_INET, SOCK_STREAM, 0);

    // Lấy địa chỉ IP từ hostname ngrok
    // htons() chuyển đổi số nguyên từ host byte order sang network byte order
    memcpy(&server.sin_addr, host->h_addr_list[0], host->h_length); 
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    // Kết nối đến server
    connect(sock, (struct sockaddr *)&server, sizeof(server));

    // Gửi yêu cầu tới server và nhận phản hồi
    send(sock, (char *)&req, sizeof(req), 0);
    printf("Request is sent to server: func_id=%d, a=%d, b=%d\n", req.function_id, req.params[0], req.params[1]);
    recv(sock, (char *)&res, sizeof(res), 0);

    if (res.status_code == 0) {
        printf("Result: %d\n", res.result);
    } else {
        printf("Error! Bad connection.\n");
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}
