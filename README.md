# Multiprocess-filetransferandmonitoring
本工具采用多进程架构，基于 TCP 协议实现高效文件传输。服务器端支持多客户端并发连接，通过多进程处理文件读写操作，显著提升传输效率。 客户端部分使用 Shell 脚本智能管理多进程，可同时启动 100 个客户端向服务器端发送文件。还能监控各个进行的运行情况。
运行命令
g++ -o demo10 demo10.cpp
g++ -o demo11 demo11.cpp
./demo10 运行服务端端
./mysh  运行客户端
