/*
 * 程序名：demo1.cpp，此程序用于演示socket的客户端
*/
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstring>
#include<string.h>
#include <cstdlib>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
using namespace std;
class ClientSocket{
private:

int sockfd;


public:

bool connects(const string& host ,const unsigned short port )
{
//1、创建socket
 sockfd=socket(AF_INET,SOCK_STREAM,0);
if (sockfd==-1)
  {
    perror("socket"); return false;
  }



//2、发起连接
struct hostent* h;    // 用于存放服务端IP的结构体。
  if ( (h = gethostbyname(host.c_str()))== 0 )  // 把字符串格式的IP转换成结构体。
  { 
    cout << "gethostbyname failed.\n" << endl;  return false;
  }



struct sockaddr_in servaddr;              // 用于存放服务端IP和端口的结构体。
memset(&servaddr,0,sizeof(servaddr));
servaddr.sin_family = AF_INET;    //协议
memcpy(&servaddr.sin_addr,h->h_addr,h->h_length); // IP地址。
servaddr.sin_port = htons(port);         // 端口。



if (::connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr))!=0)  // 向服务端发起连接清求。
{ 
    perror("connect"); ::close(sockfd); return false; 
}
//cout<<"wow连接成功"<<endl;
return true;
}



bool send(const string& data){

    int iret;
    if((iret=::send(sockfd,data.data(),data.size(),0))<=0){
      //cout<<"shibai"<<endl;
      perror("send");
      return false;
      }
    //cout<<"发送成功"<<endl;
    return true;

}
bool sendfile(const string&filename){
      // 以二进制的方式打开文件。
    ifstream fin(filename,ios::binary);


    if (!fin.is_open() ) { cout << "打开文件" << filename << "失败。\n";  return false; }

    send(filename);
    sleep(1);

    // 移动到文件末尾 , 获取文件大小
    fin.seekg(0, ios::end);
    const size_t filesize = static_cast<size_t>(fin.tellg());
    fin.seekg(0,ios::beg);
    

    //cout<<"正在传输文件:"<<filename<<" 大小："<<filesize<<endl;
   
   char buff[4096];
   size_t totalsize=0;//总步长
   size_t onread=0;//步长
   while(true){
    if(totalsize+4096<=filesize){
      onread=4096;
    }
    else{
      onread=filesize-totalsize;
    }
    fin.read(buff,onread);
    if(!send(buff))return false;
    sleep(10);
    totalsize+=onread;
    if(totalsize==filesize)break;
   }
    // 关闭文件
    fin.close();
    
    return true;


}
bool recv( string &data,const size_t maxlen){
  data.clear();         // 清空容器。
  data.resize(maxlen);  // 设置容器的大小为maxlen。
  int iret;
  if((iret=::recv(sockfd,&data[0],sizeof(data),1024)<=0))return false;
  
  return true;
}

void close()
{
sockfd=-1;
::close(sockfd);
}

~ClientSocket(){
  close();
}
};

 
int main(int argc,char *argv[])
{  
   

 
  if (argc!=4)
  {
    cout << "Using:./demo1 服务端的IP 服务端的端口\nExample:./demo11 192.168.240.139 5005 文件名\n\n"; 
    return -1;
  }


  ClientSocket cs;

  if(!cs.connects(argv[1],(unsigned short)atoi(argv[2]))){
  //cout<<"拜拜"<<endl;
  return 0;
  }
  char buff[1024];

 if( !cs.sendfile(argv[3]))cout<<"文件传输失败"<<endl;
 //else cout<<"文件传输完成"<<endl;
  
  // for(int i=1;i<=10;i++){

  // memset(buff,0,sizeof(buff));
  // sprintf(buff,"这是第%d个超级女生，编号%03d。",i,i);
  // //cout<<"kaisi"<<endl;
  // if(!cs.send(buff)){cout<<"发送失败"<<endl;
  // return 0;}
  // cout<<"客户端:"<<buff<<endl;


  // string str;
  // if(!cs.recv(str,1024)){
  //   cout<<"接收失败"<<endl;
  // return 0;
  // }
  // cout<<"服务端："<<str<<endl;
   
  // }
  cs.close();
  
   return 0;
}