#include <iostream> 
#include <iostream>  
#include <Winsock2.h>  

#pragma comment( lib, "ws2_32.lib" )   
using namespace std;

void main()
{
	WSADATA wsaData;
	//要添加链接库函数，就是为了向操作系统说明，我们要用哪个库文件，让该库文件与当前的应用程序绑定，从而就可以调用该版本的socket的各种函数了。
	//对winsock DLL（动态链接库文件）进行初始化，协商Winsock的版本支持，并分配必要的资源。  
	WSAStartup(MAKEWORD(1, 1), &wsaData);

	/*创建socket;
	AF_INET表示在Internet中通信；
	SOCK_STREAM表示socket是流套接字，对应tcp；
	0指定网络协议为TCP/IP */
	SOCKET sockClient = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN addrClt;//创建一个套接字存放地址和端口;


						/*addrClt这个套接字是存放目标（服务器）的IP地址和端口的*/
						//addr：地址    port：端口
	addrClt.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");// inet_addr将IP地址从点数格式转换成网络字节格式整型。
	addrClt.sin_family = AF_INET;//AF_INET是互联网络:UDP TCP等。
	addrClt.sin_port = htons(4000);//htons用来将主机字节顺序转换为网络字节顺序(to network short) 

								   //客户机向服务器发出连接请求  该函数的功能为客户端主动连接服务器;

	connect(sockClient, (SOCKADDR*)&addrClt, sizeof(SOCKADDR));
	while (true)
	{
		char recvBuf[50];
		/*接收端发送来的数据:
		sockfd  :  接收端套接字描述符;
		buff    :  用来存放recv函数接收到的数据的缓冲区;
		nbytes  :  指明buff的长度;
		flags   :  一般置为0;
		recv(int sockfd, void *buff, size_t nbytes, int flags);*/
		recv(sockClient, recvBuf, 50, 0);

		cout << "服务端回应 : " << recvBuf << endl; //printf("服务端回应 : %s\n", recvBuf);

		char sendBuf[50];
		//sprintf(sendBuf, "%3d,", index);
		strcpy(sendBuf, "客户端发送来的数据：hello world!");
		/*发送数据端:
		sockfd :  指定发送端套接字描述符;
		buff   :  存放要发送数据的缓冲区;
		nbytes :  实际要改善的数据的字节数;
		flags  :  一般置为0;
		send(int sockfd, const void *buff, size_t nbytes, int flags);*/
		send(sockClient, sendBuf, strlen(sendBuf) + 1, 0);

		Sleep(2000);
	}
	closesocket(sockClient);//关闭连接;
	WSACleanup();
	/*WSAStartup的功能是初始化Winsock DLL，
	WSACleanup是来解除与Socket库的绑定并且释放Socket库所占用的系统资源。*/
}