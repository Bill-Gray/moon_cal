moon_cal : moon_cal.c
	gcc -Wall -Wextra -pedantic -Werror -o moon_cal moon_cal.c -lm

clean :
	rm moon_cal
