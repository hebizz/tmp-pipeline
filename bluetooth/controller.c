#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

#include <fcntl.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define PORT	4869
#define MSG_MAX	5

const char deviceId[18] = "34:43:01:04:13:52";

int UdpSockFd, BleSockFd;
struct sockaddr_in serverAddr, clientAddr;

int listenForUdp() {
  if ((UdpSockFd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("failed to open AF_INET socket!\n");
    exit(EXIT_FAILURE);
  }

  memset(&serverAddr, 0, sizeof(serverAddr));

  serverAddr.sin_family		= AF_INET;  // IPv4
  serverAddr.sin_port		= PORT;
  serverAddr.sin_addr.s_addr	= INADDR_ANY;

  if (bind(UdpSockFd, (const struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
    perror("failed to bind sockaddr struct!\n");
    exit(EXIT_FAILURE);
  }

  // close(UdpSockFd);
  return 0;
}

int connectToBle() {
  struct sockaddr_rc deviceAddr;

  if ((BleSockFd = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM)) < 0) {
    perror("failed to open AF_BLUETOOTH socket!\n");
    exit(EXIT_FAILURE);
  }

  memset(&deviceAddr, 0, sizeof(deviceAddr));

  deviceAddr.rc_family		= AF_BLUETOOTH;
  deviceAddr.rc_channel		= (uint8_t) 1;
  str2ba(deviceId, &deviceAddr.rc_bdaddr);

  if (connect(BleSockFd, (struct sockaddr *)&deviceAddr, sizeof(deviceAddr)) < 0) {
    perror("failed to connect bluetooth device!\n");
    exit(EXIT_FAILURE);
  }
  //close(BleSockFd);
  return 0;
} 

int mainLoop() {
  char inst[MSG_MAX];
  memset(&clientAddr, 0, sizeof(clientAddr));

  while(1) {
    printf("[INFO] mainloop cycle...\n");
    // receive from udp socket
    int clientLen = sizeof(clientAddr);
    int n = recvfrom(UdpSockFd,
        (char *)inst,
        sizeof(inst),
        0,
        (struct sockaddr *)&clientAddr,
        &clientLen);

    printf("[RCVD] << ------>>%d\n", n);
    
    // filter invalid instructions 
    char v = inst[0];
    printf("!!! '%c'\n", v);
//    if (v != 'A' && v != 'C' && v != 'E' && v != 'G'&&v != 'Z') {
//      printf("invalid inst\n");
//      continue ;
//    }

    // send inst to ble device
    write(BleSockFd, &inst[0], 1);
    printf("[SEND] BLE >> %c\n", inst[0]);
    usleep(1000);

    // send ack message to udp socket
    int ret = sendto(UdpSockFd,(const char *)&inst,strlen(inst)+1,0,(struct sockaddr *)&clientAddr,sizeof(clientAddr));
    printf("---------%d\n",ret);
    if (ret < 0) {
      perror("failed to send instruction!\n");
      exit(EXIT_FAILURE);
    } else {
      printf("reply udp\n");
    }
  }
    printf("[SEND] ACK(%c)\n", inst[0]);
  return 0;
}

int main(int argc, char **argv) {
  connectToBle();
  printf("[SUCC] connected to ble\n");
  listenForUdp();
  printf("[SUCC] connected to udp\n");

  mainLoop();
  return 0;
}
