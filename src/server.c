#include "../include/server.h"
 
#pragma comment(lib, "ws2_32.lib")

char rev_buffer[1024];	//������Ϣ������
char filePath[128];		//��ҳ�ļ�·��
int port = 8765;
 
 
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
	char filePath[128];	
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

	//����socket
 
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
	serverAddr.sin_port = htons(port);//���ö˿� �����ֽ���
 
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
	printf("HServir Now on loaded");
	//============================================================================
	//��������
 
	listen(serverSocket, 10);
	printf("HServir listening on port:%d\r\nAddress http://127.0.0.1:%d\r\n",port,port);
 
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
		printf("[MSG]The return of recv is:%d\r\n", ret);
 
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
			printf("[MSG] Successfuly Receive data from cilent\r\n");
			// printf("%s\r\n", rev_buffer);
		}
 
		// strcpy(filePath, "../html/index.html");//�����ô�ʵ����ҳ�ض���
		char *reqAddress = combinePath(rev_buffer);
		char *suffix = getSuffix(findPath(rev_buffer));
		// printf("%s\n",suffix);
		strcpy(filePath,reqAddress);
		printf("[MSG] Requests address: %s\n\r",filePath);
		free(reqAddress);
 
		ret = access(filePath, 0);//0 �����ж��ļ��Ƿ����  ������ڷ���0 ���򷵻�-1
		if (ret != 0)
		{
			//δ�ҵ��ļ�
			char sendBuf[10240] = { 0 };
 
			sprintf(sendBuf, "HTTP/1.1 404 NOT FOUND\r\n");
			send(clientSocket, sendBuf, strlen(sendBuf), 0);

			sprintf(sendBuf, "Content-type:text/html\r\n");
			send(clientSocket, sendBuf, strlen(sendBuf), 0);
 
			sprintf(sendBuf, "\r\n");
			send(clientSocket, sendBuf, strlen(sendBuf), 0);
 
			sendText(clientSocket,"../html/404.html","html");
			// sprintf(sendBuf, "[WARN] no such file found\r\n");
			// send(clientSocket, sendBuf, strlen(sendBuf), 0);
		}
		else
		{
			// sendImg(clientSocket,filePath,suffix);
			if(strcmp(suffix,"jpg")==0 || strcmp(suffix,"jepg")==0){
				sendImg(clientSocket,filePath,"jpeg");
			}else if(strcmp(suffix,"html")==0){
				sendText(clientSocket,filePath,"html");
			}else if(strcmp(suffix,"css")==0){
				sendText(clientSocket,filePath,"css");
			}else if(strcmp(suffix,"html")==0){
				sendImg(clientSocket,filePath,"png");
			}
		}
		closesocket(clientSocket);//������ֱ�ӹر�  ��ΪHTTPЭ���������ӵ�
	}
 
	//�ر����ӣ����������
 
	closesocket(serverSocket);
	WSACleanup();
 
	return 0;
}

char *findPath(char *rev_buffer){
	int flag = -1;
	int low = 0;
	int high = 0;
	while (*(rev_buffer+high)!='\0')
	{
        // printf("%c\n",*(rev_buffer+high));
		if(*(rev_buffer+high)=='/'&&flag==-1){
			flag = 1;
		}
		if(flag == 1)
        {
            low = high;
            flag = 2;
        };
		if(flag == 2 && *(rev_buffer+high)==' ') break;
        high++;
	}
	int size = high-low;
    // printf("%d %d",low,high);
	char *tmp = (rev_buffer+low);
	char *buf = (char *)malloc(size+1);
	memcpy(buf,tmp,size);
    buf[(size)] = '\0';
    // printf("buf:%s\n",buf);
	return buf;
}

char *combinePath(char *rev_buffer){
	char *base = "../html";
	char *addpath = findPath(rev_buffer);
	// printf("addpath->%s\n",addpath);
	int basesize = strlen(base);
	// printf("%d\n",basesize);
	int addpathsize = strlen(addpath);
	// printf("addpathsize:%d\n",addpathsize);
	char *resPath = (char *)malloc((basesize+addpathsize+1));
	char *tmp = resPath;
	memcpy(tmp,base,basesize);
	// printf("respath:->%s\n",resPath);
	tmp+=basesize;
	memcpy(tmp,addpath,addpathsize);
	// printf("respath:->%s\n",resPath);
	resPath[(basesize+addpathsize)] = '\0';
	// printf("respath:->%s\n",resPath);
	return resPath;
}

char *getSuffix(char *buf){
	int low = 0;
	int i=0;
	int flag = -1;
	while(*(buf+i)!='\0'){
		if(*(buf+i)=='.')
		{
			low = i;
		}
		i++;
	}
	low+=1;
	int size = i-low;
	buf+=low;
	char *res = (char *)malloc(size);
	memcpy(res,buf,size);
	// printf("%s\n",res);
	return res;	
}

void sendText(SOCKET clientSocket,char *filePath,char *Type){
	FILE* fs = fopen(filePath, "r");
	if (fs == NULL)
	{
		printf("[WARN]Open File Failed\r\n");
		exit(-1);
	}else{
		char dataBuf[1024] = { 0 };
	
		sprintf(dataBuf, "http/1.1 200 success\r\n");
		send(clientSocket, dataBuf, strlen(dataBuf), 0);
		sprintf(dataBuf, "content-type:text/%s\r\n",Type);
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
void sendImg(SOCKET clientSocket,char *filePath,char *Type){
	FILE* fs = fopen(filePath, "rb");
	if (fs == NULL)
	{
		printf("[WARN]Open File Failed\r\n");
		exit(-1);
	}else{
		byte dataBuf[1024] = { 0 };
		fseek(fs,0L,SEEK_END);
		long imgsize = ftell(fs);
	
		sprintf(dataBuf, "http/1.1 200 success\r\n");
		send(clientSocket, dataBuf, strlen(dataBuf), 0);
		sprintf(dataBuf, "content-type:image/%s\r\n",Type);
		send(clientSocket, dataBuf, strlen(dataBuf), 0);
		sprintf(dataBuf, "content-length:%d\r\n",imgsize);
		send(clientSocket, dataBuf, strlen(dataBuf), 0);
		sprintf(dataBuf, "\r\n");
		send(clientSocket, dataBuf, strlen(dataBuf), 0);

		byte *imgBuf = (byte *)malloc(imgsize);
		fseek(fs,0L,SEEK_SET);
		int readsize = fread(imgBuf,sizeof(byte),imgsize,fs);
		printf("size is %d\n",readsize);
		send(clientSocket, imgBuf,imgsize,0);
		fclose(fs);
		free(imgBuf);
	}
}
