#include "../include/server.h"
 
#pragma comment(lib, "ws2_32.lib")

char rev_buffer[1024];	//接收信息缓冲区
char filePath[128];		//网页文件路径
int port = 8765;
 
 
/**
* 主要步骤：
* 1.初始化网络库
* 2.创建socket
* 3.绑定IP地址和端口号
* 4.监听连接
* 5.接收连接
* 6.处理连接请求
* 7.关闭连接，关闭网络库
**/
 
 
int main()
{
 
	int ret;
	char filePath[128];	
	WSADATA wsadata;
 
	ret = WSAStartup(MAKEWORD(2, 2), &wsadata);
	if (ret != 0)
	{
		printf("WSADATA启动失败\r\n");
		exit(-1);
	}
	else
	{
		printf("WSADATA启动成功\r\n");
	}

	//创建socket
 
	SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverSocket == INVALID_SOCKET)
	{
		printf("创建serverSocket失败\r\n");
		exit(-1);
	}
	else
	{
		printf("创建serverSocket成功\r\n");
	}
 
	//绑定IP地址和端口号(对于服务器是绑定，对于客户端是连接)
	//para1:  指定socket
	//para2:  IP地址和端口号
	//para3:  para2的长度
 
	SOCKADDR_IN serverAddr;
	serverAddr.sin_family = AF_INET;//必须和创建socket时一样
	serverAddr.sin_addr.S_un.S_addr = INADDR_ANY;
	serverAddr.sin_port = htons(port);//设置端口 网络字节码
 
	ret = bind(serverSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (ret == SOCKET_ERROR)
	{
		printf("套接字绑定失败\r\n");
		exit(-1);
	}
	else
	{
		printf("套接字绑定成功\r\n");
	}
	printf("HServir Now on loaded");
	//============================================================================
	//监听连接
 
	listen(serverSocket, 10);
	printf("HServir listening on port:%d\r\nAddress http://127.0.0.1:%d\r\n",port,port);
 
	//============================================================================
	//接收连接
 
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
 
		//6.处理连接请求
		//从clientSocket接受数据
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
 
		// strcpy(filePath, "../html/index.html");//可以用此实现网页重定向
		char *reqAddress = combinePath(rev_buffer);
		char *suffix = getSuffix(findPath(rev_buffer));
		// printf("%s\n",suffix);
		strcpy(filePath,reqAddress);
		printf("[MSG] Requests address: %s\n\r",filePath);
		free(reqAddress);
 
		ret = access(filePath, 0);//0 代表判断文件是否存在  如果存在返回0 否则返回-1
		if (ret != 0)
		{
			//未找到文件
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
		closesocket(clientSocket);//发送完直接关闭  因为HTTP协议是无连接的
	}
 
	//关闭连接，清理网络库
 
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
		} while (!feof(fs));//检查文件流是否结束
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
