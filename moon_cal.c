#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

/* Code to create a PostScript lunar strip chart for a given year.
Run with the year as a command-line argument.  Output is to 'z.ps'.
Build using 'make',  or compile with

gcc -Wall -Wextra -pedantic -o moon_cal moon_cal.c -lm      */

static int month_length( const int month, const int year)
{
   if( month == 2 && (year % 4 == 0) && (year % 100 || (year % 400 == 0)))
      return( 29);
   else
      {
      const char lengths[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

      assert( month >= 1 && month <= 12);
      return( lengths[month - 1]);
      }
}

#define PI 3.1415926535897932384626433832795028841971693993751058209749445923

static int jd_of_year( const int year)
{
   return( 1721060 + year * 365 + (year - 1) / 4 - (year - 1) / 100 + (year - 1) / 400);
}

static int get_moon_name( FILE *ifile, char *buff, const int jd)
{
   int ijd;

   while( memcmp( buff, "Full moon:", 10) || (ijd = (int)( atof( buff + 32) - .5)) < jd)
      if( !fgets( buff, 90, ifile))
         fprintf( stderr, "Failure reading from 'fullmoon.txt'\n");
      else
         {
         int i = 0;

         while( buff[i] >= ' ')
            i++;
         buff[i] = '\0';
         }
   return( ijd == jd);
}

static void get_new_moons( const char *year, int *dates)
{
   FILE *ifile = fopen( "phases.txt", "rb");
   char buff[90];
   int i;

   assert( ifile);
   while( fgets( buff, sizeof( buff), ifile))
      if( !memcmp( buff, "New moon:", 8))
         for( i = 15; i < 60; i += 20)
            if( !memcmp( buff + i, year, 4))
               {
               const char *months = "Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec ";
               const char *tptr;

               buff[i + 8] = '\0';
               tptr = strstr( months, buff + i + 5);
               assert( tptr);
               *dates++ = ((tptr - months) / 4 + 1) * 100 + atoi( buff + i + 9);
               }
   fclose( ifile);
   *dates = 0;
}

int main( const int argc, const char **argv)
{
   FILE *ifile = fopen( "moon_cal.ps", "rb");
   FILE *ofile = fopen( "z.ps", "wb");
   FILE *moons = fopen( "fullmoon.txt", "rb");
   char buff[100];
   int page, year, new_moons[20];

   assert( ifile);
   assert( ofile);
   assert( moons);
   if( argc < 2 || (year = atoi( argv[1])) < 1900)
      {
      printf( "'moon_cal' needs a year as a command-line argument.  The lunar chart\n"
              "for that year will be written to 'z.ps'.\n");
      return( -1);
      }
   while( fgets( buff, sizeof( buff), ifile))
      {
      char *tptr = strstr( buff, "YEAR");

      if( tptr)
         memcpy( tptr, argv[1], 4);
      fputs( buff, ofile);
      }
   fclose( ifile);
   get_new_moons( argv[1], new_moons);
   for( page = 1; page <= 2; page++)
      {
      int month, day, jd = jd_of_year( year);
      char next_moon[90];

      fseek( moons, 0L, SEEK_SET);
      *next_moon = '\0';
      fprintf( ofile, "%%%%Page: %d %d\n", page, page);
      fprintf( ofile, "%%%%PageOrientation: Portrait\n");
      if( page == 1)
         fprintf( ofile, " (%s) yearshow\n", argv[1]);
      for( month = 1; month <= 12; month++)
         for( day = 1; day <= month_length( month, year); day++, jd++)
            if( (page == 1 && day < 16) || (page == 2 && day > 15))
               {
               int xloc = 30 + (month + month / 7) * 40;
               int yloc = 720 - (day - page * 15 + 15) * 40;
               const char *month_name = " JFMAMJJASOND";
               const char *text, *weekdays[7] = { "Su", "Mo", "Tu",
                           "We", "Th", "Fr", "Sa" };

               int i = 0;

               text = weekdays[(jd + 2) % 7];
               if( page == 1)
                  yloc -= 50;
               while( new_moons[i] && new_moons[i] != month * 100 + day)
                  i++;
               if( new_moons[i])
                  {
                  if( year == 2021 && month == 6)   /* 2021 Jun 10 eclipse */
                     text = "Annular";
                  if( year == 2021 && month == 12)   /* 2021 Dec  4 eclipse */
                     text = "Total";
                  if( year == 2023 && month == 4)   /* 2023 Apr 20 eclipse */
                     text = "Hybrid";
                  if( year == 2023 && month == 10)   /* 2023 Oct 14 eclipse */
                     text = "Annular";
                  fprintf( ofile, "(%s) %d %d newmoon", text, xloc, yloc);
                  }
               else if( get_moon_name( moons, next_moon, jd))
                  {
                  char *moon_name = next_moon + 47;

                  if( !strcmp( next_moon + 47, "After Yule"))
                     fprintf( ofile, "(After) (Yule) %d %d fullmoon2\n", xloc, yloc);
                  else if( !strcmp( next_moon + 47, "Before Yule"))
                     fprintf( ofile, "(Before) (Yule) %d %d fullmoon2\n", xloc, yloc);
                  else
                     fprintf( ofile, "(%s) %d %d fullmoon\n", moon_name, xloc, yloc);
                  }
               else
                  {
                  double phase = ((double)jd + 1.0 - 2451550.09766) / 29.530588861;

                  phase = fmod( phase, 1);
                  if( phase < 0.)
                     phase += 1.;
                  fprintf( ofile, " (%s) %.3f %d %d %smoon\n",
                           text, cos( phase * 2. * PI), xloc, yloc,
                           (phase < 0.5 ? "right" : "left"));
                  }
               if( month == 7)
                  fprintf( ofile, "(%d) %d %d dayshow\n",
                          day, xloc - 40, yloc);
               if( day == 1)
                  fprintf( ofile, "(%c) %d %d monthshow\n",
                           month_name[month], xloc, yloc + 40);
               }
      fprintf( ofile, "showpage\n");
      }
   fclose( ofile);
   return( 0);
}
