/* Serial port implementations for cross-platform support */
#include "serial.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
    #include <windows.h>
    static HANDLE hSerial = INVALID_HANDLE_VALUE;
#else
    #include <unistd.h>
    #include <fcntl.h>
    #include <termios.h>
    #include <errno.h>
    static int serial_fd = -1;
    static struct termios old_tio, new_tio;
#endif

/* Global variables */
int sibuff = 0, eibuff = 0, ilen = 0;
unsigned char far *inbuff = NULL;
int rx_flow = 0, rx_rts = 0, rx_dtr = 0, rx_xon = 0;
int tx_rts = 0, tx_dtr = 0, tx_xon = 0, tx_xonoff = 0;
int fon = 0, foff = 0;
int base = 0x3F8;  /* Default COM1 base address */
int ansi_dsr_flag = 0;
int (*ansi_dsr)(unsigned char n) = NULL;

/* Configuration variables */
static long current_baud = 9600;
static int current_bits = 8;
static int current_parity = NO_PARITY;
static int current_stopbits = 1;

/* Serial port functions */
void ansiout(int c) { 
    putchar(c); 
    fflush(stdout);
}

int ansiprintf(char *f, ...) { 
    (void)f; 
    return 0; 
}

void ansistring(char *s) { 
    if(s) printf("%s", s); 
}

int carrier(void) { 
    return 1; 
}

void close_port(void) { 
#ifdef _WIN32
    if(hSerial != INVALID_HANDLE_VALUE) {
        CloseHandle(hSerial);
        hSerial = INVALID_HANDLE_VALUE;
    }
#else
    if(serial_fd != -1) {
        tcsetattr(serial_fd, TCSANOW, &old_tio);
        close(serial_fd);
        serial_fd = -1;
    }
#endif
}

void delay(clock_t n) { 
    clock_t start = clock();
    while(clock() - start < n) ;
}

void fifo(int n) { 
    (void)n; 
}

long get_baud(void) { 
    return current_baud; 
}

int get_bits(void) { 
    return current_bits; 
}

int get_parity(void) { 
    return current_parity; 
}

int get_rx_dtr(void) { 
    return rx_dtr; 
}

int get_rx_rts(void) { 
    return rx_rts; 
}

int get_rx_xon(void) { 
    return rx_xon; 
}

int get_serial(void) { 
#ifdef _WIN32
    unsigned char byte;
    DWORD bytes_read;
    if(hSerial == INVALID_HANDLE_VALUE) return -1;
    if(ReadFile(hSerial, &byte, 1, &bytes_read, NULL) && bytes_read > 0) {
        return (int)byte;
    }
    return -1;
#else
    unsigned char byte;
    if(serial_fd == -1) return -1;
    if(read(serial_fd, &byte, 1) > 0) {
        return (int)byte;
    }
    return -1;
#endif
}

int get_stopbits(void) { 
    return current_stopbits; 
}

int get_tx_dtr(void) { 
    return tx_dtr; 
}

int get_tx_rts(void) { 
    return tx_rts; 
}

int get_tx_xon(void) { 
    return tx_xon; 
}

#ifdef __TURBOC__
void interrupt handler(void) { }
#else
void _interrupt _far handler(void) { }
#endif

char *ibmtoansi(int att, char *s) { 
    (void)att;
    return s; 
}

int in_ready(void) { 
#ifdef _WIN32
    DWORD errors;
    COMSTAT stat;
    if(hSerial == INVALID_HANDLE_VALUE) return 0;
    if(!ClearCommError(hSerial, &errors, &stat)) return 0;
    return (stat.cbInQue > 0) ? 1 : 0;
#else
    if(serial_fd == -1) return 0;
    /* Non-blocking check - would need more sophisticated implementation */
    return 0;
#endif
}

unsigned mpeek(unsigned seg, unsigned off) { 
    (void)seg; 
    (void)off; 
    return 0; 
}

void open_port(int port, int inlen) { 
#ifdef _WIN32
    char port_name[10];
    DCB dcb;
    COMMTIMEOUTS timeouts;
    
    snprintf(port_name, sizeof(port_name), "COM%d", port);
    hSerial = CreateFileA(port_name, GENERIC_READ | GENERIC_WRITE, 0, NULL, 
                          OPEN_EXISTING, 0, NULL);
    if(hSerial == INVALID_HANDLE_VALUE) return;
    
    /* Set default parameters */
    GetCommState(hSerial, &dcb);
    dcb.BaudRate = CBR_9600;
    dcb.ByteSize = 8;
    dcb.StopBits = ONESTOPBIT;
    dcb.Parity = NOPARITY;
    SetCommState(hSerial, &dcb);
    
    /* Set timeouts */
    GetCommTimeouts(hSerial, &timeouts);
    timeouts.ReadIntervalTimeout = 100;
    timeouts.ReadTotalTimeoutConstant = 100;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    SetCommTimeouts(hSerial, &timeouts);
#else
    char port_name[32];
    struct termios tio;
    
    (void)inlen;
    
    snprintf(port_name, sizeof(port_name), "/dev/ttyS%d", port - 1);
    serial_fd = open(port_name, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if(serial_fd == -1) {
        snprintf(port_name, sizeof(port_name), "/dev/ttyUSB%d", port - 1);
        serial_fd = open(port_name, O_RDWR | O_NOCTTY | O_NONBLOCK);
    }
    
    if(serial_fd != -1) {
        tcgetattr(serial_fd, &old_tio);
        memset(&new_tio, 0, sizeof(new_tio));
        new_tio = old_tio;
        
        new_tio.c_cflag = CS8 | CLOCAL | CREAD;
        new_tio.c_iflag = 0;
        new_tio.c_oflag = 0;
        new_tio.c_lflag = 0;
        new_tio.c_cc[VMIN] = 0;
        new_tio.c_cc[VTIME] = 1;
        
        cfsetispeed(&new_tio, B9600);
        cfsetospeed(&new_tio, B9600);
        tcsetattr(serial_fd, TCSANOW, &new_tio);
    }
#endif
}

int port_exist(int port) { 
#ifdef _WIN32
    char port_name[10];
    HANDLE h;
    snprintf(port_name, sizeof(port_name), "COM%d", port);
    h = CreateFileA(port_name, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
    if(h != INVALID_HANDLE_VALUE) {
        CloseHandle(h);
        return 1;
    }
    return 0;
#else
    (void)port;
    return 1;  /* Assume port exists on Unix */
#endif
}

void purge_in(void) { 
#ifdef _WIN32
    if(hSerial != INVALID_HANDLE_VALUE) {
        PurgeComm(hSerial, PURGE_RXCLEAR);
    }
#else
    if(serial_fd != -1) {
        tcflush(serial_fd, TCIFLUSH);
    }
#endif
}

int put_serial(unsigned char n) { 
#ifdef _WIN32
    DWORD bytes_written;
    if(hSerial == INVALID_HANDLE_VALUE) return -1;
    if(WriteFile(hSerial, &n, 1, &bytes_written, NULL)) {
        return bytes_written > 0 ? 0 : -1;
    }
    return -1;
#else
    if(serial_fd == -1) return -1;
    if(write(serial_fd, &n, 1) > 0) {
        return 0;
    }
    return -1;
#endif
}

int recv_file(int xtype, int(*error_handler)(int c, long p, char *s), char *path) {
    (void)xtype; 
    (void)error_handler; 
    (void)path;
    return -1;
}

void set_baud(long baud) { 
    current_baud = baud;
#ifdef _WIN32
    DCB dcb;
    if(hSerial == INVALID_HANDLE_VALUE) return;
    GetCommState(hSerial, &dcb);
    dcb.BaudRate = baud;
    SetCommState(hSerial, &dcb);
#else
    if(serial_fd == -1) return;
    speed_t speed = B9600;
    switch(baud) {
        case 300: speed = B300; break;
        case 600: speed = B600; break;
        case 1200: speed = B1200; break;
        case 2400: speed = B2400; break;
        case 4800: speed = B4800; break;
        case 9600: speed = B9600; break;
        case 19200: speed = B19200; break;
        case 38400: speed = B38400; break;
    }
    cfsetispeed(&new_tio, speed);
    cfsetospeed(&new_tio, speed);
    tcsetattr(serial_fd, TCSANOW, &new_tio);
#endif
}

void set_data_format(int bits, int parity, int stopbit) {
    current_bits = bits;
    current_parity = parity;
    current_stopbits = stopbit;

#ifdef _WIN32
    DCB dcb;
    if(hSerial == INVALID_HANDLE_VALUE) return;
    GetCommState(hSerial, &dcb);
    dcb.ByteSize = bits;
    dcb.StopBits = (stopbit == 2) ? TWOSTOPBITS : ONESTOPBIT;
    dcb.Parity = (parity == EVEN_PARITY) ? EVENPARITY : 
                 (parity == ODD_PARITY) ? ODDPARITY : NOPARITY;
    SetCommState(hSerial, &dcb);
#else
    if(serial_fd == -1) return;
    
    new_tio.c_cflag &= ~CSIZE;
    switch(bits) {
        case 5: new_tio.c_cflag |= CS5; break;
        case 6: new_tio.c_cflag |= CS6; break;
        case 7: new_tio.c_cflag |= CS7; break;
        case 8: new_tio.c_cflag |= CS8; break;
    }
    
    if(stopbit == 2) {
        new_tio.c_cflag |= CSTOPB;
    } else {
        new_tio.c_cflag &= ~CSTOPB;
    }
    
    new_tio.c_cflag &= ~(PARENB | PARODD);
    if(parity != NO_PARITY) {
        new_tio.c_cflag |= PARENB;
        if(parity == ODD_PARITY) {
            new_tio.c_cflag |= PARODD;
        }
    }
    
    tcsetattr(serial_fd, TCSANOW, &new_tio);
#endif
}

void set_dtr(int n) { 
    (void)n; 
}

void set_port(long baud, int bits, int parity, int stopbit) {
    set_baud(baud);
    set_data_format(bits, parity, stopbit);
}

void set_rx_dtr(int n) { 
    rx_dtr = n; 
}

void set_rx_rts(int n) { 
    rx_rts = n; 
}

void set_rx_xon(int n) { 
    rx_xon = n; 
}

void set_tx_dtr(int n) { 
    tx_dtr = n; 
}

void set_tx_rts(int n) { 
    tx_rts = n; 
}

void set_tx_xon(int n) { 
    tx_xon = n; 
}

int xmit_file(int xtype, int (*error_handler)(int c, long p, char *s), char *files[]) {
    (void)xtype; 
    (void)error_handler; 
    (void)files;
    return -1;
}
