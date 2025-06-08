# RPC_build
## Cách chạy biên dịch
```
gcc server.c -o bin\server.exe -lws2_32
```
```
gcc client.c -o bin\client.exe -lws2_32
```

## Thực thi
```
.\bin\server
```
Điền `func_id`, `a`, `b` là các số nguyên
```
.\bin\client func_id a b
```
## Tạo giao diện đồ họa (GUI)
File RPCGui.java tạo một GUI bằng java và liên kết tới client.exe để thực hiện.
Chạy lệnh biên dịch và thực thi:
```
javac RPCGui.java
```
```
java RPCGui
```
## Đặt chế độ tự dừng
*Tổng quan cách hoạt động:*
- **Bình thường:** `accept()` sẽ chờ mãi cho đến khi có client kết nối.

- **Non-blocking:** `accept()` **không chờ** — nếu không có kết nối, nó trả về lỗi `WSAEWOULDBLOCK`, cho phép ta chủ động đếm thời gian.

Thực hiện:
```
#define timeout 60
```