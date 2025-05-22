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
## Tạo giao diện đồ họa (GUI)
File RPCGui.java tạo một GUI bằng java và liên kết tới client.exe để thực hiện.
Chạy lệnh biên dịch và thực thi:
```
javac RPCGui.java
```
```
java RPCGui
```