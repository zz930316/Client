#include <iostream> 
#include <iostream>  
#include <Winsock2.h>  

#pragma comment( lib, "ws2_32.lib" )   
using namespace std;

void main()
{
	WSADATA wsaData;
	//Ҫ������ӿ⺯��������Ϊ�������ϵͳ˵��������Ҫ���ĸ����ļ����øÿ��ļ��뵱ǰ��Ӧ�ó���󶨣��Ӷ��Ϳ��Ե��øð汾��socket�ĸ��ֺ����ˡ�
	//��winsock DLL����̬���ӿ��ļ������г�ʼ����Э��Winsock�İ汾֧�֣��������Ҫ����Դ��  
	WSAStartup(MAKEWORD(1, 1), &wsaData);

	/*����socket;
	AF_INET��ʾ��Internet��ͨ�ţ�
	SOCK_STREAM��ʾsocket�����׽��֣���Ӧtcp��
	0ָ������Э��ΪTCP/IP */
	SOCKET sockClient = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN addrClt;//����һ���׽��ִ�ŵ�ַ�Ͷ˿�;


						/*addrClt����׽����Ǵ��Ŀ�꣨����������IP��ַ�Ͷ˿ڵ�*/
						//addr����ַ    port���˿�
	addrClt.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");// inet_addr��IP��ַ�ӵ�����ʽת���������ֽڸ�ʽ���͡�
	addrClt.sin_family = AF_INET;//AF_INET�ǻ�������:UDP TCP�ȡ�
	addrClt.sin_port = htons(4000);//htons�����������ֽ�˳��ת��Ϊ�����ֽ�˳��(to network short) 

								   //�ͻ����������������������  �ú����Ĺ���Ϊ�ͻ����������ӷ�����;

	connect(sockClient, (SOCKADDR*)&addrClt, sizeof(SOCKADDR));
	while (true)
	{
		char recvBuf[50];
		/*���ն˷�����������:
		sockfd  :  ���ն��׽���������;
		buff    :  �������recv�������յ������ݵĻ�����;
		nbytes  :  ָ��buff�ĳ���;
		flags   :  һ����Ϊ0;
		recv(int sockfd, void *buff, size_t nbytes, int flags);*/
		recv(sockClient, recvBuf, 50, 0);

		cout << "����˻�Ӧ : " << recvBuf << endl; //printf("����˻�Ӧ : %s\n", recvBuf);

		char sendBuf[50];
		//sprintf(sendBuf, "%3d,", index);
		strcpy(sendBuf, "�ͻ��˷����������ݣ�hello world!");
		/*�������ݶ�:
		sockfd :  ָ�����Ͷ��׽���������;
		buff   :  ���Ҫ�������ݵĻ�����;
		nbytes :  ʵ��Ҫ���Ƶ����ݵ��ֽ���;
		flags  :  һ����Ϊ0;
		send(int sockfd, const void *buff, size_t nbytes, int flags);*/
		send(sockClient, sendBuf, strlen(sendBuf) + 1, 0);

		Sleep(2000);
	}
	closesocket(sockClient);//�ر�����;
	WSACleanup();
	/*WSAStartup�Ĺ����ǳ�ʼ��Winsock DLL��
	WSACleanup���������Socket��İ󶨲����ͷ�Socket����ռ�õ�ϵͳ��Դ��*/
}