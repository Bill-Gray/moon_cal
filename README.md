# moon_cal

Code to make lunar phase calendars (and ready-to-print calendars)
=================================================================
![snippet](https://github.com/user-attachments/assets/fcfb4702-c1fd-4ef8-ae71-b2c32300b172)

The above image shows the overall idea.  Please note that the simplest thing to do may be to [go to this page to generate your own customized lunar calendar as a PDF.](https://www.projectpluto.com/moon_cal.htm)  That'll give you some control over what events are shown on the calendar,  layout,  color vs. black & white,  and language.  And,  of course,  you won't have to download and build this code.

So you can just grab a desired calendar at the above link and print it out on two pages;  a bit of scissors work and tape gets you a two-page-high calendar for the year.  You need only read the following if you'd like to tweak the calendar and/or read about why I went to the trouble of doing this.

Building/running the code
-------------------------

On most non-Microsoft systems,  build by running
```c
make
```
I assume you can run something like `cl -W4 -Ox moon_cal.c` on Microsoft® Windows™ and it'll work,  but I've not tried it on anything but GNU/Linux.  The code is very generic,  has no dependencies except for the math library,  and would presumably compile/run on *BSD,  OS/X,  MS-DOS,  Haiku,  etc.

To generate a chart for a given year,  run it as,  e.g.,  `./moon_cal 2026`.  The result will be written to `z.ps`.  Add the command line option `-c` for color.  You can either view/print that,  or run `ps2pdf z.ps 2026.pdf` to make the [PDF linked to above](https://github.com/Bill-Gray/moon_cal/blob/master/2026.pdf).

By default,  new and full moons,  solstices and equinoxes,  and a variety of holidays are shown.  These are listed in [`date2023.txt`](https://github.com/Bill-Gray/moon_cal/blob/master/date2023.txt),  [`date2024.txt`](https://github.com/Bill-Gray/moon_cal/blob/master/date2024.txt),  etc.  You can edit those files to change what date(s) are shown.

[`moon_cal.ps`](https://github.com/Bill-Gray/moon_cal/blob/master/moon_cal.ps) contains the PostScript routines used to build the calendar.  Editing this,  you can edit the `sunday` and `holiday` sections to change the colors used for those events.  (I really just wanted Sundays and holidays to stand out a little to make navigation easier.)

Rationale
---------
I do a fair bit of work writing software for asteroid observers ("planetary defense",  the folks looking for rocks that might hit the earth).  There's usually about a week or so around each full moon where we can't observe much,  and a few days before/after that where moonlight interferes to an annoying extent.

Some Christmases back,  my sister gave me a printed calendar vaguely resembling the ones shown here.  It was useful,  and had better artwork than what I'm providing here.  It wasn't particularly oriented to what a working astronomer might actually need (it was a bit "New Age"-ish),  but it made me think about what I'd actually want to have.

The column of days of the month was on the left edge,  which made it a little tricky to find the day of the month you wanted by the time you got to the end of the year.  Putting it in the middle helped.

I added the 'Su Mo Tu..." to make it still easier to find the desired day,  and made it possible to note other days.

The lunar month names are the traditional ones from the _Maine Farmer's Almanac_ (I'm from Maine).  The rationale is described in `fullmoon.txt`.

Possible improvements
---------------------
I could imagine indicating lunar and solar eclipses with some modification of the new moon/full moon symbol.  Possibly even taking a chunk out of the symbol to indicate the maximum extent of the eclipse.

Some printers can take larger pieces of paper,  and it might be nice to print this out on such.

Further European languages could be added without much trouble.  (As best I can tell,  PostScript has no concept of Unicode.)
