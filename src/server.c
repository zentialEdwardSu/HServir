#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include <io.h>
 
#pragma comment(lib, "ws2_32.lib")

char rev_buffer[1024];	//������Ϣ������
char filePath[128];		//��ҳ�ļ�·��
 
 
/**
* ��Ҫ���裺
* 1.��ʼ�������
* 2.����socket
* 3.��IP��ַ�Ͷ˿ں�
* 4.��������
* 5.��������
* 6.������������
* 7.�ر����ӣ��ر������
**/
 
 
int main()
{
 
	int ret;
	WSADATA wsadata;
 
	ret = WSAStartup(MAKEWORD(2, 2), &wsadata);
	if (ret != 0)
	{
		printf("WSADATA����ʧ��\r\n");
		exit(-1);
	}
	else
	{
		printf("WSADATA�����ɹ�\r\n");
	}
 
	//============================================================================
	//����socket
	//para1:ָ��IPЭ��  AF_INET -> IPV4  AF_INET6 -> IPV6
	//para2:���ݴ����ʽ  ���õ������֣���ʽ���䣨TCP��  ���ݰ����䣨UDP��
	//para3:����Э�飺
 
	SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverSocket == INVALID_SOCKET)
	{
		printf("����serverSocketʧ��\r\n");
		exit(-1);
	}
	else
	{
		printf("����serverSocket�ɹ�\r\n");
	}
 
	//��IP��ַ�Ͷ˿ں�(���ڷ������ǰ󶨣����ڿͻ���������)
	//para1:  ָ��socket
	//para2:  IP��ַ�Ͷ˿ں�
	//para3:  para2�ĳ���
 
	SOCKADDR_IN serverAddr;
	serverAddr.sin_family = AF_INET;//����ʹ���socketʱһ��
	serverAddr.sin_addr.S_un.S_addr = INADDR_ANY;
	serverAddr.sin_port = htons(8765);//���ö˿� �����ֽ���
 
	ret = bind(serverSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (ret == SOCKET_ERROR)
	{
		printf("�׽��ְ�ʧ��\r\n");
		exit(-1);
	}
	else
	{
		printf("�׽��ְ󶨳ɹ�\r\n");
	}
 
	//============================================================================
	//��������
 
	listen(serverSocket, 10);
	printf("������������......\r\n");
 
	//============================================================================
	//��������
 
	SOCKADDR clientAddr;
	int len_clientAddr = sizeof(clientAddr);
 
	while (1)
	{
		SOCKET clientSocket = accept(serverSocket, &clientAddr, &len_clientAddr);
		if (clientSocket == INVALID_SOCKET)
		{
			printf("[WARN] Connect failed\r\n");
			exit(-1);
		}
		else
		{
			printf("[MSG] Cilent connected \r\n");
		}
 
		//6.������������
		//��clientSocket��������
 
		ret = recv(clientSocket, rev_buffer, sizeof(rev_buffer), 0);
		printf("The return of recv is:%d\r\n", ret);
 
		if (ret <= 0)
		{
			printf("[WARN] Failed to receive stream from cilent\r\n");
			printf("[MSG] Waitting for next connection...\r\n");
			closesocket(clientSocket);
			continue;
			//exit(-1);
		}
		else
		{
			printf("[MSG] Receive data from cilent->\r\n");
			printf("%s\r\n", rev_buffer);
		}
 
		strcpy(filePath, "../html/index.html");//�����ô�ʵ����ҳ�ض���
 
		ret = access(filePath, 0);//0 �����ж��ļ��Ƿ����  ������ڷ���0 ���򷵻�-1
		if (ret != 0)
		{
			//δ�ҵ��ļ�
			char sendBuf[1024] = { 0 };
 
			sprintf(sendBuf, "HTTP/1.1 404 NOT FOUND\r\n");
			send(clientSocket, sendBuf, strlen(sendBuf), 0);
 
			sprintf(sendBuf, "Content-type:text/html\r\n");
			send(clientSocket, sendBuf, strlen(sendBuf), 0);
 
			sprintf(sendBuf, "\r\n");
			send(clientSocket, sendBuf, strlen(sendBuf), 0);
 
 
			sprintf(sendBuf, "[WARN] no such file found\r\n");
			send(clientSocket, sendBuf, strlen(sendBuf), 0);
		}
		else
		{
			//�����ҳ�ļ�
			FILE* fs = fopen(filePath, "r");
			if (fs == NULL)
			{
				printf("[WARN]Open HTML Failed\r\n");
				exit(-1);
			}
			else
			{
				char dataBuf[1024] = { 0 };
 
				sprintf(dataBuf, "http/1.1 200 success\r\n");
				send(clientSocket, dataBuf, strlen(dataBuf), 0);
 
				sprintf(dataBuf, "content-type:text/html\r\n");
				send(clientSocket, dataBuf, strlen(dataBuf), 0);
 
				sprintf(dataBuf, "\r\n");
				send(clientSocket, dataBuf, strlen(dataBuf), 0);
 
				do
				{
					fgets(dataBuf, 1024, fs);
					send(clientSocket, dataBuf, strlen(dataBuf), 0);
				} while (!feof(fs));//����ļ����Ƿ����
 
				fclose(fs);
			}
		}
		closesocket(clientSocket);//������ֱ�ӹر�  ��ΪHTTPЭ���������ӵ�
	}
 
	//�ر����ӣ����������
 
	closesocket(serverSocket);
	WSACleanup();
 
	return 0;
}