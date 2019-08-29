#include <sys/types.h>
#include <sys/socket.h>


#include <arpa/inet.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <hiredis/hiredis.h>
#include <math.h>

#define PORT	4869
#define MSG_MAX	4

#define SERVER "192.168.0.219"
#define SERVER_REDIS "192.168.0.123"

int UdpSockFd;
struct sockaddr_in serverAddr;

void action(char *inst);

int connectToUdp() {
  if ((UdpSockFd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("failed to open AF_INET socket!\n");
    exit(EXIT_FAILURE);
  }
  memset(&serverAddr, 0, sizeof(serverAddr));
  serverAddr.sin_family		= AF_INET;  // IPv4
  serverAddr.sin_port		= PORT;
  serverAddr.sin_addr.s_addr	= inet_addr(SERVER);
  // close(UdpSockFd);
  return 0;
}

int mainLoop(redisContext* conn,char *inX,char*inY) {
  char inst[MSG_MAX];
  redisReply* reply=NULL;
  int number=0;
  char *value;
  long longX=strtol(inX,&value,10);
  long longY=strtol(inY,&value,10);

  while(1){
    char strNum[255];
    sprintf(strNum,"%d",number);
    //char tableName="XY";
    //strcpy(tableName, "hashinfo_");
    //strcat(tableName, strNum);
    // send instruction to udp socket
    reply = redisCommand(conn,  "GET %s", "XY");
    if(!reply->str){ 
       continue;
    }
    char result[10];
    strcpy(result,reply->str);
    freeReplyObject(reply);
    //printf("redis result is %s",result);
    if (result == NULL) {
        continue;// it is empty string
    }
    char *re =strtok(result,",");
    char pointX[8]={""};
    char pointY[8]={""};
    strncpy(pointX,re+1,strlen(re));
    char *rey = strtok(NULL,",");
    strncpy(pointY,rey,strlen(rey)-1);
    //printf("------%s====%s",rex,rey); 
    long reX=strtol(pointX,&value,10);
    long reY=strtol(pointY,&value,10);
    //printf("rex =%ld= rey==%ld",reX,reY);
    long offsetX=abs(reX-longX);
    long offsetY=abs(reY-longY);
      
    //printf("%ld--->>>%ld--->>>>%ld----->>>>%ld\n",reX,reY,offsetX,offsetY);
    if(offsetX>30){
        long moveX=reX-longX;
        // move to right
        if((number % 16) == 0){
            if (moveX<0){
                action("C");
            }else{
                // move to left
                action("G");
            }
        }
    }else if(offsetX<=30 && offsetY>30){
        long moveY=reY-longY;
        // move to back
        if((number % 16) == 0){
            if (moveY<0){
                action("E");
            }else{
                // move to front
                action("A");
            }
        }
    }else if(offsetX<=30 && offsetY<=30){
        action("Z");
        exit(0);
    }
    number++;
  }
  return 0;
}

void action(char *inst) {
  int ret = sendto(UdpSockFd,(const char *)inst,strlen(inst)+1,0,(struct sockaddr *)&serverAddr,sizeof(serverAddr));
  if (ret < 0) {
    perror("failed to send instruction!\n");
    exit(EXIT_FAILURE);
  } else {
    printf("Sent udp\n");
  }
}

redisContext* connectRedis(){
  redisContext* conn = redisConnect(SERVER_REDIS, 6379);
  if(conn->err)   printf("connection error:%s\n", conn->errstr);
  redisReply* reply = redisCommand(conn,"AUTH %s","jiangxing");
  freeReplyObject(reply);
  return conn;
}

int main(int argc, char **argv) {
  connectToUdp();
  redisContext *conn=connectRedis();
  char inputStr[10];
  scanf("%s",inputStr);
  printf("input string is %s",inputStr);
 // split input string
  char *inX=strtok(inputStr,",");
  char *inY = strtok(NULL,",");
  mainLoop(conn,inX,inY);
  return 0;
}
