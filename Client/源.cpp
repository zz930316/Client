// TcpClient.cpp : 定义控制台应用程序的入口点。  
 

#include <winsock2.h>  
#include <string>  
#include <iostream>  

#pragma comment(lib,"ws2_32.lib")  

#define BUF_SIZE 64  
using namespace std;


/*
				运行cmd  ：
netstat -nao        :			查看有哪些端口在被使用;
netstat -nao | findstr 63790  :  查找63790这个端口;
*/


int main()
{
	WSADATA wsd;
	SOCKET sHost;
	SOCKADDR_IN servAddr;
	char buf[BUF_SIZE];
	int retVal;
	//初始化Socket  
	//MAKEWORD(1, 1)和MAKEWORD(2, 2)的区别在于，前者只能一次接收一次，不能马上发送，而后者能
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)//WSAStartup的功能是初始化Winsock DLL，
	{
		cout << "WSAStartup failed!" << endl;
		return -1;
	}
	//创建用于监听的Socket  
	sHost = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == sHost)//INVALID_SOCKET 是无效套接字
	{
		cout << "socket failed!" << endl;
		WSACleanup();/*WSACleanup是来解除与Socket库的绑定并且释放Socket库所占用的系统资源。*/
		return -1;
	}
	//设置Socket为非阻塞模式  
	int iMode = 1;
	/*int ioctlsocket( SOCKET s, long cmd, u_long FAR *argp );
	s：一个标识套接口的描述字;		cmd：对套接口s的操作命令;		argp：指向cmd命令所带参数的指针;*/
	retVal = ioctlsocket(sHost, FIONBIO, (u_long FAR*)&iMode);//FIONBIO：将socket设置为非阻塞
																//u_long 无符号长整型 4个字节
	//当ioctlsocket函数成功后，ioctlsocket()返回0。否则的话，返回SOCKET_ERROR错误，
	if (retVal == SOCKET_ERROR)		//SOCKET_ERROR 0 接口错误
	{
		cout << "ioctlsocket failed!" << endl;
		WSACleanup();/*WSACleanup是来解除与Socket库的绑定并且释放Socket库所占用的系统资源。*/
		return -1;
	}
	//设置服务器Socket地址  
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(63790);/*htons用来将主机端口转换为网络字节顺序(to network short);
							htons 将无符号短整型转换为网络字节顺序;*/
	servAddr.sin_addr.S_un.S_addr = htonl(INADDR_LOOPBACK);//inet_addr("127.0.0.1");//htonl(INADDR_ANY); 255.255.255.255 => ffffffff, 127.0.0.1 => 7f000001
				/*返回：若字符串有效则将字符串转换为32位二进制网络字节序的IPV4地址，否则为INADDR_NONE
							htonl用来将主机地址转换为网络字节顺序(to network long);
								 htonl 将无符号长整形转换为网络字节顺序;
							 INADDR_ANY就是指定地址为0.0.0.0的地址,表示不确定地址,或“任意地址”。”  */

	int sServerAddlen = sizeof(servAddr);

	//连接到服务器  
	while (true)
	{
		/*函数原型: int connect(int s, const struct sockaddr * name, int namelen);
				s：标识一个未连接socket;
				name：指向要连接套接字的sockaddr结构体的指针;
				namelen：sockaddr结构体的字节长度;
				成功则返回0, 失败返回-1, */
		// 客户机向服务器发出连接请求  该函数的功能为客户端主动连接服务器;
		retVal = connect(sHost, (LPSOCKADDR)&servAddr, sizeof(servAddr));
		if (SOCKET_ERROR == retVal)//SOCKET_ERROR 0 接口错误
		{
			int err = WSAGetLastError(); //指该函数返回上次发生的网络错误,这个函数有很多种返回值 ，百度...
			
			//WSAEWOULDBLOCK : 10035 资源暂时不可用 ;  WSAEINVAL  10022 - 参数无效
			if (err == WSAEWOULDBLOCK || err == WSAEINVAL)
			{
				Sleep(500);
				continue;
			}
			else if (err == WSAEISCONN)//WSAEISCONN    10056 - 套接字已经连接
				break;
			else
			{
				cout << "connection failed!" << endl;
				closesocket(sHost);	//关闭与连接的sHost（服务器）IP，端口的连接
				WSACleanup();/*WSACleanup是来解除与Socket库的绑定并且释放Socket库所占用的系统资源。*/
				return -1;
			}
		}
	}

	//收发数据  
	while (true)
	{
		cout << "请输入一个字符串发送:";
		string str;
		getline(cin, str);//获取输入的字符串

		/*void ZeroMemory( PVOID Destination,SIZE_T Length );
			  Destination :指向一块准备用0来填充的内存区域的开始地址。
	    	  Length :准备用0来填充的内存区域的大小，按字节来计算。
			  ZeroMemory只是将指定的内存块清零。使用结构前清零，而不让结构的成员数值具有不确定性，
		就是讲buf数组中的64个元素全部赋值为0*/
		ZeroMemory(buf, BUF_SIZE);
		strcpy_s(buf, str.c_str());//c_str返回当前字符串的首字符地址。当需要打开一个由用户自己输入文件名的文件时，可以这样写：ifstream in(st.c_str())。
		while (true)
		{
			/*发送数据端:	sockfd :  指定发送端套接字描述符;
			buff   :  存放要发送数据的缓冲区;
			nbytes :  实际要改善的数据的字节数;
			flags  :  一般置为0;
			send(int sockfd, const void *buff, size_t nbytes, int flags);
			若无错误发生，send()返回所发送数据的总数（请注意这个数字可能小于len中所规定的大小）。
			否则的话，返回//SOCKET_ERROR 0 接口错误，应用程序可通过WSAGetLastError()获取相应错误代码。*/
			retVal = send(sHost, buf, strlen(buf), 0);
			if (SOCKET_ERROR == retVal)//SOCKET_ERROR 0 接口错误
			{
				int err = WSAGetLastError();//指该函数返回上次发生的网络错误,这个函数有很多种返回值 ，百度...
				if (err == WSAEWOULDBLOCK)//WSAEWOULDBLOCK : 10035 资源暂时不可用 ;
				{
					Sleep(500);
					continue;
				}
				else
				{
					cout << "send failed!" << endl;
					closesocket(sHost);//关闭与连接的sHost（服务器）IP，端口的连接
					WSACleanup();/*WSACleanup是来解除与Socket库的绑定并且释放Socket库所占用的系统资源。*/
				}
			}
			break;

		}

		while (true)
		{
			ZeroMemory(buf, BUF_SIZE);//清零
			/*接收端发送来的数据:
			sockfd  :  接收端套接字描述符;
			buff    :  用来存放recv函数接收到的数据的缓冲区;
			nbytes  :  指明buff的长度;
			flags   :  一般置为0;
			recv(int sockfd, void *buff, size_t nbytes, int flags);*/
			retVal = recv(sHost, buf, sizeof(buf) + 1, 0);
			if (SOCKET_ERROR == retVal)
			{
				int err = WSAGetLastError();//指该函数返回上次发生的网络错误,这个函数有很多种返回值 ，百度...
				if (err == WSAEWOULDBLOCK)//WSAEWOULDBLOCK : 10035 资源暂时不可用
				{
					Sleep(100);
					printf("等待回复！\n");
					continue;
				}
				else if (err == WSAETIMEDOUT || err == WSAENETDOWN)
				{
					printf("recv failed!\n");
					closesocket(sHost);//关闭与连接的sHost（服务器）IP，端口的连接
					WSACleanup();/*WSACleanup是来解除与Socket库的绑定并且释放Socket库所占用的系统资源。*/
					return -1;
				}
				break;
			}
			break;

		}
		printf("Recv从服务器：%s\n", buf);
		if (strcmp(buf, "quit") == 0)
		{
			printf("quit!\n");
			break;
		}
	}

	closesocket(sHost);//关闭与连接的sHost（服务器）IP，端口的连接
	WSACleanup();/*WSACleanup是来解除与Socket库的绑定并且释放Socket库所占用的系统资源。*/
	system("pause");

	return 0;
}
