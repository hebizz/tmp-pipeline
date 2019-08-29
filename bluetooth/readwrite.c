#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    struct sockaddr_rc addr = { 0 };
    int s, status, len=0;
    char dest[18] = "34:43:01:04:13:52";
    char buf;
    // allocate a socket
    s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

    // set the connection parameters (who to connect to)
    addr.rc_family = AF_BLUETOOTH;
    addr.rc_channel = (uint8_t) 1;
    str2ba( dest, &addr.rc_bdaddr );

    // connect to server
    status = connect(s, (struct sockaddr *)&addr, sizeof(addr));

    
    if(status){
        printf(" failed to connect the device!\n");
        return -1;
    }

    
    do{
    // len = read(s, buf, sizeof buf);
    
    // if( len>0 ) {
         buf=0x41;
         printf("%c\n",buf);
         write(s, &buf, 1);
     usleep(1000);
    // }
    }while(len==0);

    close(s);
    return 0;
}
