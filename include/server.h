#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include <io.h>


char *findPath(char *rev_buffer);
char *combinePath(char *rev_buffer);
char *getSuffix(char *buf);
void sendText(SOCKET clientSocket,char *filePath,char *Type);
void sendImg(SOCKET clientSocket,char *filePath,char *Type);
typedef struct{
    int len;
    int free;
    char buf[];
}Hsds;

typedef unsigned char byte;