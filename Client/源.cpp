// TcpClient.cpp : �������̨Ӧ�ó������ڵ㡣  
 

#include <winsock2.h>  
#include <string>  
#include <iostream>  

#pragma comment(lib,"ws2_32.lib")  

#define BUF_SIZE 64  
using namespace std;


/*
				����cmd  ��
netstat -nao        :			�鿴����Щ�˿��ڱ�ʹ��;
netstat -nao | findstr 63790  :  ����63790����˿�;
*/


int main()
{
	WSADATA wsd;
	SOCKET sHost;
	SOCKADDR_IN servAddr;
	char buf[BUF_SIZE];
	int retVal;
	//��ʼ��Socket  
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)//WSAStartup�Ĺ����ǳ�ʼ��Winsock DLL��
	{
		cout << "WSAStartup failed!" << endl;
		return -1;
	}
	//�������ڼ�����Socket  
	sHost = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == sHost)//INVALID_SOCKET ����Ч�׽���
	{
		cout << "socket failed!" << endl;
		WSACleanup();/*WSACleanup���������Socket��İ󶨲����ͷ�Socket����ռ�õ�ϵͳ��Դ��*/
		return -1;
	}
	//����SocketΪ������ģʽ  
	int iMode = 1;
	/*int ioctlsocket( SOCKET s, long cmd, u_long FAR *argp );
	s��һ����ʶ�׽ӿڵ�������;		cmd�����׽ӿ�s�Ĳ�������;		argp��ָ��cmd��������������ָ��;*/
	retVal = ioctlsocket(sHost, FIONBIO, (u_long FAR*)&iMode);//FIONBIO����socket����Ϊ������
																//u_long �޷��ų����� 4���ֽ�
	//��ioctlsocket�����ɹ���ioctlsocket()����0������Ļ�������SOCKET_ERROR����
	if (retVal == SOCKET_ERROR)		//SOCKET_ERROR 0 �ӿڴ���
	{
		cout << "ioctlsocket failed!" << endl;
		WSACleanup();/*WSACleanup���������Socket��İ󶨲����ͷ�Socket����ռ�õ�ϵͳ��Դ��*/
		return -1;
	}
	//���÷�����Socket��ַ  
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(63790);/*htons�����������˿�ת��Ϊ�����ֽ�˳��(to network short);
							htons ���޷��Ŷ�����ת��Ϊ�����ֽ�˳��;*/
	servAddr.sin_addr.S_un.S_addr = htonl(INADDR_LOOPBACK);//inet_addr("127.0.0.1");//htonl(INADDR_ANY); 255.255.255.255 => ffffffff, 127.0.0.1 => 7f000001
				/*���أ����ַ�����Ч���ַ���ת��Ϊ32λ�����������ֽ����IPV4��ַ������ΪINADDR_NONE
							htonl������������ַת��Ϊ�����ֽ�˳��(to network long);
								 htonl ���޷��ų�����ת��Ϊ�����ֽ�˳��;
							 INADDR_ANY����ָ����ַΪ0.0.0.0�ĵ�ַ,��ʾ��ȷ����ַ,�������ַ������  */

	int sServerAddlen = sizeof(servAddr);

	//���ӵ�������  
	while (true)
	{
		/*����ԭ��: int connect(int s, const struct sockaddr * name, int namelen);
				s����ʶһ��δ����socket;
				name��ָ��Ҫ�����׽��ֵ�sockaddr�ṹ���ָ��;
				namelen��sockaddr�ṹ����ֽڳ���;
				�ɹ��򷵻�0, ʧ�ܷ���-1, */
		// �ͻ����������������������  �ú����Ĺ���Ϊ�ͻ����������ӷ�����;
		retVal = connect(sHost, (LPSOCKADDR)&servAddr, sizeof(servAddr));
		if (SOCKET_ERROR == retVal)//SOCKET_ERROR 0 �ӿڴ���
		{
			int err = WSAGetLastError(); //ָ�ú��������ϴη������������,��������кܶ��ַ���ֵ ���ٶ�...
			
			//WSAEWOULDBLOCK : 10035 ��Դ��ʱ������ ;  WSAEINVAL  10022 - ������Ч
			if (err == WSAEWOULDBLOCK || err == WSAEINVAL)
			{
				Sleep(500);
				continue;
			}
			else if (err == WSAEISCONN)//WSAEISCONN    10056 - �׽����Ѿ�����
				break;
			else
			{
				cout << "connection failed!" << endl;
				closesocket(sHost);	//�ر������ӵ�sHost����������IP���˿ڵ�����
				WSACleanup();/*WSACleanup���������Socket��İ󶨲����ͷ�Socket����ռ�õ�ϵͳ��Դ��*/
				return -1;
			}
		}
	}

	//�շ�����  
	while (true)
	{
		cout << "������һ���ַ�������:";
		string str;
		getline(cin, str);//��ȡ������ַ���

		/*void ZeroMemory( PVOID Destination,SIZE_T Length );
			  Destination :ָ��һ��׼����0�������ڴ�����Ŀ�ʼ��ַ��
	    	  Length :׼����0�������ڴ�����Ĵ�С�����ֽ������㡣
			  ZeroMemoryֻ�ǽ�ָ�����ڴ�����㡣ʹ�ýṹǰ���㣬�����ýṹ�ĳ�Ա��ֵ���в�ȷ���ԣ�
		���ǽ�buf�����е�64��Ԫ��ȫ����ֵΪ0*/
		ZeroMemory(buf, BUF_SIZE);
		strcpy_s(buf, str.c_str());//c_str���ص�ǰ�ַ��������ַ���ַ������Ҫ��һ�����û��Լ������ļ������ļ�ʱ����������д��ifstream in(st.c_str())��
		while (true)
		{
			/*�������ݶ�:	sockfd :  ָ�����Ͷ��׽���������;
			buff   :  ���Ҫ�������ݵĻ�����;
			nbytes :  ʵ��Ҫ���Ƶ����ݵ��ֽ���;
			flags  :  һ����Ϊ0;
			send(int sockfd, const void *buff, size_t nbytes, int flags);
			���޴�������send()�������������ݵ���������ע��������ֿ���С��len�����涨�Ĵ�С����
			����Ļ�������//SOCKET_ERROR 0 �ӿڴ���Ӧ�ó����ͨ��WSAGetLastError()��ȡ��Ӧ������롣*/
			retVal = send(sHost, buf, strlen(buf), 0);
			if (SOCKET_ERROR == retVal)//SOCKET_ERROR 0 �ӿڴ���
			{
				int err = WSAGetLastError();//ָ�ú��������ϴη������������,��������кܶ��ַ���ֵ ���ٶ�...
				if (err == WSAEWOULDBLOCK)//WSAEWOULDBLOCK : 10035 ��Դ��ʱ������ ;
				{
					Sleep(500);
					continue;
				}
				else
				{
					cout << "send failed!" << endl;
					closesocket(sHost);//�ر������ӵ�sHost����������IP���˿ڵ�����
					WSACleanup();/*WSACleanup���������Socket��İ󶨲����ͷ�Socket����ռ�õ�ϵͳ��Դ��*/
				}
			}
			break;

		}

		while (true)
		{
			ZeroMemory(buf, BUF_SIZE);//����
			/*���ն˷�����������:
			sockfd  :  ���ն��׽���������;
			buff    :  �������recv�������յ������ݵĻ�����;
			nbytes  :  ָ��buff�ĳ���;
			flags   :  һ����Ϊ0;
			recv(int sockfd, void *buff, size_t nbytes, int flags);*/
			retVal = recv(sHost, buf, sizeof(buf) + 1, 0);
			if (SOCKET_ERROR == retVal)
			{
				int err = WSAGetLastError();//ָ�ú��������ϴη������������,��������кܶ��ַ���ֵ ���ٶ�...
				if (err == WSAEWOULDBLOCK)//WSAEWOULDBLOCK : 10035 ��Դ��ʱ������
				{
					Sleep(100);
					printf("�ȴ��ظ���\n");
					continue;
				}
				else if (err == WSAETIMEDOUT || err == WSAENETDOWN)
				{
					printf("recv failed!\n");
					closesocket(sHost);//�ر������ӵ�sHost����������IP���˿ڵ�����
					WSACleanup();/*WSACleanup���������Socket��İ󶨲����ͷ�Socket����ռ�õ�ϵͳ��Դ��*/
					return -1;
				}
				break;
			}
			break;

		}
		printf("Recv�ӷ�������%s\n", buf);
		if (strcmp(buf, "quit") == 0)
		{
			printf("quit!\n");
			break;
		}
	}

	closesocket(sHost);//�ر������ӵ�sHost����������IP���˿ڵ�����
	WSACleanup();/*WSACleanup���������Socket��İ󶨲����ͷ�Socket����ռ�õ�ϵͳ��Դ��*/
	system("pause");

	return 0;
}