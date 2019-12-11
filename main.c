   #include <stdio.h>
   #include <stdlib.h>
   #include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

    int main(int argc, char ** argv) {
        int fd = open("/dev/foobar", O_RDWR);
	//printf("error open is %s\n", strerror(errno));
	char x = *argv[1];
	printf("X is %c\n", x);
	int err = write(fd, &x, 1);
	//printf("char read %d\n", err);
	printf("write error is %s\n", strerror(errno));
	char y;
	read(fd, &y, 1);
	printf("character is %c\n", y);
	return 0;
    }
