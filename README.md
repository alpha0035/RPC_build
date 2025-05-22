# RPC_build
## Cách chạy biên dịch
```
gcc sever.c -o bin\sever.exe -lws2_32
```
```
gcc client.c -o bin\client.exe -lws2_32
```

## Thực thi
```
.\bin\sever
```
```
.\bin\client
```
## Đặt chế độ tự dừng
*Tổng quan cách hoạt động:*
- **Bình thường:** `accept()` sẽ chờ mãi cho đến khi có client kết nối.

- **Non-blocking:** `accept()` **không chờ** — nếu không có kết nối, nó trả về lỗi `WSAEWOULDBLOCK`, cho phép ta chủ động đếm thời gian.

Thực hiện:
```
#define timeout 10
```