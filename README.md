# moon_cal

Code to make lunar phase calendars
==================================
![snippet](https://user-images.githubusercontent.com/16679342/211208815-8e810991-ae76-4cda-9f8e-af890bde88f2.png)


The above image shows the overall idea.  [Click here for the 2023 calendar as a PDF,](https://github.com/Bill-Gray/moon_cal/blob/master/2023.pdf) or [here for the 2024 calendar](https://github.com/Bill-Gray/moon_cal/blob/master/2024.pdf), or [here for the 2025 calendar](https://github.com/Bill-Gray/moon_cal/blob/master/2025.pdf) (haven't gotten past 2025 yet,  but will as that approaches).  You can just grab a desired calendar and print it out;  you need only read the following if you'd like to tweak the calendar,  generate calendars for other years,  or read about why I went to the trouble of doing this.

Building/running the code
-------------------------

On most non-Microsoft systems,  build by running
```c
make
```
I assume you can run something like `cl -W4 -Ox moon_cal.c` on Microsoft® Windows™ and it'll work,  but I've not tried it on anything but GNU/Linux.  The code is very generic,  has no dependencies except for the math library,  and would presumably compile/run on *BSD,  OS/X,  etc.

To generate a chart for a given year,  run it as,  e.g.,  `./moon_cal 2023`.  The result will be written to `z.ps`.  You can either view/print that,  or run `ps2pdf z.ps 2023.pdf` to make the [PDF linked to above](https://github.com/Bill-Gray/moon_cal/blob/master/2023.pdf).

By default,  new and full moons,  solstices and equinoxes,  and a variety of holidays are shown.  These are listed in `date2023.txt`,  `date2024.txt`,  etc.  You can edit those files to change what date(s) are shown.

Once printed on two US letter or A4 pages,  a bit of scissors work and transparent tape will assemble the calendar.

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

I made no use of color,  since my printer is a black-and-white laser.  I could imagine different colors indicating... well.. something or other.

Somewhere or other,  I've seen a chart resembling this sort,  but in 'landscape' mode (days of the month run horizontally,  so you have 31 columns and 12 rows instead of 12 columns and 31 rows.)  That might make a nice alternative.

I may put together an on-line service on [my Web site](https://www.projectpluto.com) where you would enter the desired year,  then be presented with an edit box containing the default `dateYYYY.txt` file for that year.  Edit as desired to remove stuff you don't want and to label your pet's birthday or other date(s) of interest,  click 'Make Calendar',  and you'd get your customized calendar without having to download and compile and run this code.
