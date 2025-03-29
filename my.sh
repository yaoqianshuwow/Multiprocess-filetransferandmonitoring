#!/bin/bash

# 运行命令10次
# for i in {1..10}
# do
#     echo "Iteration $i"
#     ./demo11 192.168.240.139 5005 demo10.cpp&
# done
#!/bin/bash

# 定义一个函数来显示系统信息
show_system_info() {
    echo "CPU Usage:"
    mpstat | tail -1 | awk '{print "CPU: " $1}'

    echo "Memory Usage:"
    free -m | awk '/^Mem:/ {print "Memory: " $3 "/" $2 "MB, " $3/$2*100 "% used"}'

    echo "Disk Usage:"
    df -h | grep -vE '^Filesystem|tmpfs|cdrom' | awk '{print $1 " " $5 " used"}'
}

# 定义一个函数来显示当前运行的进程
show_processes() {
    echo "Current Processes:"
    ps -ef | grep demo11
     
}

# 定义一个函数来结束进程
#-v grep | head -5
kill_process() {
    read -p "Enter the PID of the process to kill: " pid
    if [[ $pid =~ ^[0-9]+$ ]]; then
        kill $pid 2>/dev/null
        if [ $? -eq 0 ]; then
            echo "Process with PID $pid has been killed."
        else
            echo "Failed to kill process with PID $pid."
        fi
    else
        echo "Invalid PID. Please enter a numeric value."
    fi
}
send_file(){
    for i in {1..10}
do
    echo "Iteration $i"
    ./demo11 192.168.240.139 5005 demo10.cpp&
done
}
# 定义一个函数来定期更新监控数据
monitor_system() {
    while true; do
        clear
        show_system_info
        show_processes
        sleep 2
    done
}

# 显示菜单
show_menu() {
    echo "System Monitor"
    echo "1. Show system information"
    echo "2. Show current processes"
    echo "3. Kill a process"
    echo "4. Start monitoring system"
    echo "5. Exit"
    echo "6.sendfile"
    echo "-------------------------"
    echo -n "Enter your choice: "
}

# 主循环
while true; do
    show_menu
    read choice
    case $choice in
        1) show_system_info ;;
        2) show_processes ;;
        3) kill_process ;;
        4) monitor_system ;;
        5) echo "Exiting..."; break ;;
        6) send_file;;
        *) echo "Invalid choice. Please try again." ;;
    esac
done