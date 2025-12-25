
#include "dos_compat.h"

#include <time.h>
#include "serial.h"
#include "sersubs.h"

int GetSerialTimed(void) {
	int i;

	timeout(TIME_OUT_RESET);
	while(!in_ready() && !timeout(TIME_OUT_LIM)) ;
	i = get_serial();
	return i;
}

int timeout(int i) {
	static long int tx;

	if(i == 0) {
		tx = time(NULL);
	} else {
		if((time(NULL)-tx) >= i) return(1);
	}
	return(0);
}
