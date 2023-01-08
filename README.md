# moon_cal

Code to make lunar phase calendars
==================================
![snippet](https://user-images.githubusercontent.com/16679342/211208815-8e810991-ae76-4cda-9f8e-af890bde88f2.png)


(The above shows the overall idea.  [Click here for the calendar as a PDF.](https://github.com/Bill-Gray/moon_cal/blob/master/2023.pdf)  You can just grab that and print it out;  you need only read the following if you'd like to tweak the calendar,  generate calendars for other years,  or read about why I went to the trouble of doing this.)

Building/running the code
-------------------------

Run
```c
cc -Wall -Wextra -pedantic -o moon_cal moon_cal.c -lm
```
I assume you can run something like `cl -W4 -Ox moon_cal.c` on Microsoft® Windows™ and it'll work,  but I've not tried it on anything but GNU/Linux.  The code is very generic,  has no dependencies except for the math library,  and would presumably compile/run on *BSD,  OS/X,  etc.

To generate a chart for a given year,  run it as,  e.g.,  `./moon_cal 2023`.  The result will be written to `z.ps`.  You can either view/print that,  or run `ps2pdf z.ps 2023.pdf` to make a PDF.

Once printed on two US letter or A4 pages,  a bit of scissors work and transparent tape will assemble the calendar.

Rationale
---------

Some years back,  my sister gave me a printed calendar resembling this one for Christmas.  It was useful,  and had better artwork than what I'm providing here.  But it wasn't particularly oriented to what a working astronomer might actually need (it was a bit "New Age"-ish),  and it made me think about what I'd actually want to have.

The column of days of the month was on the left edge,  which made it a little tricky to find the day of the month you wanted a little tricky.  Putting it in the middle helped.

I added the 'Su Mo Tu..." to make it still easier to find the desired day.

The lunar month names are the traditional ones from the _Maine Farmer's Almanac_ (I'm from Maine).  The rationale is described in `fullmoon.txt`.

Possible improvements
---------------------

It might be nice if,  for the dates of the equinoxes and solstices,  the day-of-week was changed to E or S.

I'd sort of like to have lunar eclipses noted.  As it stands,  I just have solar eclipses noted,  and it's gracelessly hacked in for a few years only.

I could imagine indicating lunar and solar eclipses with some modification of the new moon/full moon symbol.  Possibly even taking a chunk out of the symbol to indicate the maximum extent of the eclipse.

I made no use of color,  since my printer is a black-and-white laser.  I could imagine different colors indicating... well.. something or other.
