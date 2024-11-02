#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

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

#define MAX_DATES 408

int main( const int argc, const char **argv)
{
   FILE *ofile, *ifile;
   char buff[100], *dates[MAX_DATES];
   const char *events_shown = "nfeh";  /* by default,  show new/full moons, */
   int month, jd, year;                /* equinoxes,  and holidays */
   size_t i;
   bool use_color = false;

   if( argc < 2 || (year = atoi( argv[1])) < 1900)
      {
      printf( "'moon_cal' needs a year as a command-line argument.  The lunar chart\n"
              "for that year will be written to 'z.ps'.\n");
      return( -1);
      }
   for( i = 2; i < (size_t)argc; i++)
      if( argv[i][0] == '-')
         switch( argv[i][1])
            {
            case 'e':
               events_shown = argv[i] + 2;
               break;
            case 'c':
               use_color = true;
               break;
            default:
               fprintf( stderr, "Argument '%s' not interpreted\n", argv[i]);
               return( -1);
            }

   jd = jd_of_year( year) - 1;
   snprintf( buff, sizeof( buff), "date%d.txt", year);
   ifile = fopen( buff, "rb");
   assert( ifile);
   memset( dates, 0, sizeof( dates));
   while( fgets( buff, sizeof( buff), ifile))
      if( !memcmp( buff, argv[1], 4) && strlen( buff) > 13)
         {
         const int pseudo_jd = atoi( buff + 5) * 31 + atoi( buff + 8);

         assert( pseudo_jd > 31 && pseudo_jd <= 31 * 13);
         if( dates[pseudo_jd])
            {
            fprintf( stderr, "You have two date texts defined for this date\n");
            fprintf( stderr, "%s%s\nPlease remove one of them.\n",
                        buff + 11, dates[pseudo_jd]);
            return( -1);
            }
         dates[pseudo_jd] = (char *)malloc( strlen( buff + 10));
         buff[strlen( buff) - 1] = '\0';     /* remove trailing lf */
         strcpy( dates[pseudo_jd], buff + 11);
         }
   fclose( ifile);

   ifile = fopen( "moon_cal.ps", "rb");
   assert( ifile);
   ofile = fopen( "z.ps", "wb");
   assert( ofile);
   while( fgets( buff, sizeof( buff), ifile) && memcmp( buff, "BREAK", 5))
      {
      char *tptr = strstr( buff, "YEAR");

      if( tptr)
         memcpy( tptr, argv[1], 4);
      fputs( buff, ofile);
      }

   fprintf( ofile, " (%s) yearshow\n", argv[1]);
   for( month = 1; month <= 12; month++)
      {
      const int end_day = month_length( month, year);
      const int xloc = 30 + (month + month / 7) * 40;
      int day;

      for( day = 1; day <= end_day; day++)
         {
         int yloc = 670 - day * 40;
         const char *month_name = " JFMAMJJASOND";
         const char *text, *weekdays[7] = { "Su", "Mo", "Tu",
                     "We", "Th", "Fr", "Sa" };
         const char *date_text = dates[month * 31 + day];
         const char *split_ptr;
         const int day_of_week = (jd + day + 2) % 7;
         int is_new_moon;

         is_new_moon = (date_text && *date_text == 'n');
         text = weekdays[day_of_week];
         if( date_text && date_text[2] != '-' && strchr( events_shown, *date_text))
            text = date_text + 2;
         split_ptr = strchr( text, '$');
         if( strstr( text, "\\p"))
            split_ptr = text;
         if( month == 7)
            fprintf( ofile, "(%d) %d %d dayshow\n",
                    day, xloc - 40, yloc);
         if( use_color)
            {
            if( date_text && *date_text == 'h')
               fprintf( ofile, "holiday ");
            else if( !day_of_week)
               fprintf( ofile, "sunday ");
            }
         if( is_new_moon)
            fprintf( ofile, "(%s) %d %d newmoon\n",
                     (split_ptr ? " " : text), xloc, yloc);
         else if( date_text && *date_text == 'f')
            fprintf( ofile, "(%s) %d %d fullmoon\n",
                     (split_ptr ? " " : text), xloc, yloc);
         else
            {
            double phase = ((double)(jd + day) + 1.0 - 2451550.09766) / 29.530588861;

            phase = fmod( phase, 1);
            if( phase < 0.)
               phase += 1.;
            fprintf( ofile, " (%s) %.3f %d %d %smoon\n",
                     (split_ptr ? " " : text),
                     cos( phase * 2. * PI), xloc, yloc,
                     (phase < 0.5 ? "right" : "left"));
            }
         if( split_ptr)
            {
            if( *split_ptr == '$')     /* text on two lines */
               fprintf( ofile, "(%.*s) (%s) %d two_strings\n",
                     (int)( split_ptr - text) - 3,
                     text + 3, split_ptr + 1, atoi( text));
            else        /* pi (3/14) or ~pi (22/7)   */
               fprintf( ofile, "(%s) show_pi\n", (*split_ptr == '~' ? "~p" : "p"));
            }
         if( use_color)
            if( !day_of_week || (date_text && *date_text == 'h'))
               fprintf( ofile, "def_day\n");
         if( day == 1)
            fprintf( ofile, "(%c) %d %d monthshow\n",
                     month_name[month], xloc, yloc + 40);
         }
      jd += month_length( month, year);
      }
   for( i = 0; i < sizeof( dates) / sizeof( dates[0]); i++)
      if( dates[i])
         free( dates[i]);
   while( fgets( buff, sizeof( buff), ifile))
      fputs( buff, ofile);
   fclose( ifile);
   fclose( ofile);
   return( 0);
}
