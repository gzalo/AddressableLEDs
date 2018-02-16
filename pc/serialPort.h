#ifdef __linux

#include <errno.h>
#include <fcntl.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

int set_interface_attribs(int fd, int speed)
{
    struct termios tty;

    if (tcgetattr(fd, &tty) < 0) {
        printf("Error from tcgetattr: %s\n", strerror(errno));
        return -1;
    }

    cfsetospeed(&tty, (speed_t)speed);
    cfsetispeed(&tty, (speed_t)speed);

    tty.c_cflag |= (CLOCAL | CREAD);    /* ignore modem controls */
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;         /* 8-bit characters */
    tty.c_cflag &= ~PARENB;     /* no parity bit */
    tty.c_cflag &= ~CSTOPB;     /* only need 1 stop bit */
    tty.c_cflag &= ~CRTSCTS;    /* no hardware flowcontrol */

    /* setup for non-canonical mode */
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tty.c_oflag &= ~OPOST;

    /* fetch bytes as they become available */
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 1;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        printf("Error from tcsetattr: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

  int serialFd;

int serialInit(){
  const char *portname = "/dev/ttyUSB0";
  const char *portname2 = "/dev/ttyUSB1";

    serialFd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
    if (serialFd < 0) {
        printf("Error opening %s: %s\n", portname, strerror(errno));
        
       
       serialFd = open(portname2, O_RDWR | O_NOCTTY | O_SYNC);
      if (serialFd < 0) {
        printf("Error opening %s: %s\n", portname2, strerror(errno));
        return -1;
       }
        
        
    }
#define B256000 0x1005
#define B128000 0x1003

  //  set_interface_attribs(serialFd, B256000);
    return 0;
}

int serialWrite(uint8_t *bff, int len){
  return write(serialFd, bff, len);
}

void serialClose(){
  close(serialFd);
}

#else

#include <windows.h>

//Puerto Serie
HANDLE hSerial;

int serialInit(){
	int i;
	for(i=1;i<16;i++){
		
		if(i == 1 || i == 3) continue;
		
		char filename[32];
		sprintf(filename, "COM%d",i);
		hSerial = CreateFile(filename,GENERIC_READ | GENERIC_WRITE,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
		
		if(hSerial!=INVALID_HANDLE_VALUE){
			break;
		}
	}
	if(i==15) return -1;
	
	DCB dcbSerialParams = {0};
	dcbSerialParams.DCBlength=sizeof(dcbSerialParams);
	if (!GetCommState(hSerial, &dcbSerialParams)) {
		cerr << "Error obteniendo parametros en COM" << i << endl;
		return -1;
	}
	dcbSerialParams.BaudRate=CBR_128000;//CBR_256000;
	dcbSerialParams.ByteSize=8;
	dcbSerialParams.StopBits=ONESTOPBIT;
	dcbSerialParams.Parity=NOPARITY;
	if(!SetCommState(hSerial, &dcbSerialParams)){
		cerr << "Error estableciendo parametros en COM" << i << endl;
		return -1;
	}
	
	COMMTIMEOUTS timeouts={0};
	timeouts.ReadIntervalTimeout=1000;
	timeouts.ReadTotalTimeoutConstant=1000;
	timeouts.ReadTotalTimeoutMultiplier=1000;
	if(!SetCommTimeouts(hSerial, &timeouts)){
		cerr << "Error estableciendo timeout." << endl;
		return -1;
	}
	return i;
}

int serialWrite(uint8_t *bff, int len){
  
  DWORD dwBytesSent;
	WriteFile(hSerial, bff, len, &dwBytesSent, NULL);
  
  return dwBytesSent;
}
void serialClose(){
CloseHandle(hSerial);
}

#endif
