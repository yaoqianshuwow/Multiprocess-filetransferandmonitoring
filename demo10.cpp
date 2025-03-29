#include <iostream>
#include <cstdio>
#include <cstring>

//?????????
#include <cstdlib>
#include <unistd.h>

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include<signal.h>
using namespace std;


 class ServerSocket{
private:
int listenfd;
int clientfd;
public:
  bool connect(const unsigned short port){
  // 第1步：创建服务端的socket。 
  listenfd = socket(AF_INET,SOCK_STREAM,0);
  if (listenfd==-1) 
  { 
    perror("socket"); 
    return false; 
  }
  
  // 第2步：把服务端用于通信的IP和端口绑定到socket上。 
  struct sockaddr_in servaddr;          // 用于存放服务端IP和端口的数据结构。
  memset(&servaddr,0,sizeof(servaddr));
  servaddr.sin_family = AF_INET;        // 指定协议。
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // 服务端任意网卡的IP都可以用于通讯。
  servaddr.sin_port = htons(port);     // 指定通信端口，普通用户只能用1024以上的端口。


  // 绑定服务端的IP和端口。
  if (bind(listenfd,(struct sockaddr *)&servaddr,sizeof(servaddr)) != 0 )
  { 
    perror("bind"); 
    ::close(listenfd); return false; 
  }
 

  // 第3步：把socket设置为可连接（监听）的状态。
  if (listen(listenfd,5) != 0 ) 
  { 
    perror("listen"); 
    ::close(listenfd);
     return false; 
  }
 

  
  return true;
  }

  bool accept(){
 // 第4步：受理客户端的连接请求，如果没有客户端连上来，accept()函数将阻塞等待。
  clientfd=::accept(listenfd,0,0);

  if (clientfd==-1)
  {
    perror("accept"); 
    ::close(listenfd); return false; 
  }
  return true;
  }
  
  bool recv(string &data,size_t maxsize){
     data.clear();
     data.resize(maxsize);
     int iret;
     iret=::recv(clientfd,&data[0],data.size(),0);
     if (iret<=0) {
        //cout<<data<<endl;
        return false;
     }
      return true;
  }
  bool send(const string &data){
    int iret=::send(clientfd,&data[0],data.size(),0);
    if(iret<=0){
      return false;
    }
    return true;
  }

  void close(){
     if(clientfd==-1||listenfd==-1)return ;
     ::close(listenfd);
     ::close(clientfd);
     listenfd=-1;
     clientfd=-1;
  }
  ~ServerSocket(){
    close();
  }
};

void FathEXIT(int sig);  // 父进程的信号处理函数。
void ChldEXIT(int sig);  // 子进程的信号处理函数。
ServerSocket ss;
int main(int argc,char *argv[])
{
  if (argc!=2)
  {
    cout << "Using:./demo2 通讯端口\nExample:./demo2 5005\n\n";   // 端口大于1024，不与其它的重复。
    cout << "注意：运行服务端程序的Linux系统的防火墙必须要开通5005端口。\n";
    cout << "      如果是云服务器，还要开通云平台的访问策略。\n\n";
    return -1;
  }
  if(!ss.connect(atoi(argv[1]))){
    cout<<"连接失败"<<endl;
    return -1;
  }
  
  for (int ii=1;ii<=64;ii++)signal(ii,SIG_IGN);
  // 设置信号,在shell状态下可用 "kill 进程号" 或 "Ctrl+c" 正常终止些进程
  // 但请不要用 "kill -9 +进程号" 强行终止
  signal(SIGTERM,FathEXIT); 
  signal(SIGINT,FathEXIT);  // SIGTERM 15 SIGINT 2
  int j=1;

  while(1)
  {

  if(!ss.accept()){
    cout<<"连接失败"<<endl;
    return -1;
  }

  int pid=fork();
  if (pid==-1) { perror("fork()"); return -1; }  // 系统资源不足。
  if (pid>0){
   printf("第%d个连接\n",j);
   j++;
   continue;
  } 
  else
  {
    signal(SIGTERM,ChldEXIT);   // 子进程的退出函数与父进程不一样。
    signal(SIGINT ,SIG_IGN);    // 子进程不需要捕获SIGINT信号。
  for(int k=1;k<=10;k++)
  {
   string str;
  if(!ss.recv(str,1024)){
    
    return 0;
    
  }
  cout<<str<<endl;
  str.clear();
  str="OK";
  
  if(!ss.send(str)){
    cout<<"发送失败"<<endl;
    return -1;
  }
  cout<<str<<endl;
  sleep(1);
  }
  cout<<"退出\n";
  return 0;
  }



  }


  cout<<"主进程退出\n";
  return 0;
  }



  // 父进程的信号处理函数。
void FathEXIT(int sig)
{
  // 以下代码是为了防止信号处理函数在执行的过程中再次被信号中断。
  signal(SIGINT,SIG_IGN); signal(SIGTERM,SIG_IGN);
  
  cout << "父进程退出，sig=" << sig << endl;

  kill(0,SIGTERM);     // 向全部的子进程发送15的信号，通知它们退出。

  // 在这里增加释放资源的代码（全局的资源）。
  ss.close();// 父进程关闭监听的socket。

  exit(0);
}

// 子进程的信号处理函数。
void ChldEXIT(int sig)
{
  // 以下代码是为了防止信号处理函数在执行的过程中再次被信号中断。
  signal(SIGINT,SIG_IGN); signal(SIGTERM,SIG_IGN);

  cout << "子进程" << getpid() << "退出，sig=" << sig << endl;

  // 在这里增加释放资源的代码（只释放子进程的资源）。
 
  ss.close();// 子进程关闭客户端连上来的socket。

  exit(0);
}