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

## Sử dụng ngrok
Vào phần mềm ngrok và chạy lệnh
```
ngrok tcp 8080
```
![Hình ảnh sau khi chạy lệnh](image/using%20ngrok.png)

Thay địa chỉ và port tương ứng trong file client.c
```
#define port = 11582
```
```
#define host_addr = "0.tcp.ap.ngrok.io"
```