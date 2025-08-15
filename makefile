all : moon_cal moon_cal.cgi

moon_cal : moon_cal.c
	$(CC) -Wall -Wextra -pedantic -Werror -o moon_cal moon_cal.c -lm

moon_cal.cgi : moon_cal.c
	$(CC) -Wall -Wextra -pedantic -Werror -o moon_cal.cgi -I../include moon_cal.c -DCGI_VERSION -L../lib -lm -llunar

clean :
	rm moon_cal moon_cal.cgi
