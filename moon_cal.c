#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

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

void utf8_to_win1252( char *text)
{
   char *optr = text;

   while( *text)
      if( (unsigned char)*text < 0x80)
         *optr++ = *text++;
      else
         {
         const unsigned char t0 = (unsigned char)text[0];
         const unsigned char t1 = (unsigned char)text[1];

         *optr++ = (char)( (t0 << 6) | (t1 & 0x3f));
         text += 2;
         }
   *optr = '\0';
}

static void fix_for_postscript( char *buff)
{
   unsigned char *tptr;

   utf8_to_win1252( buff);
   for( tptr = (unsigned char *)buff; *tptr; tptr++)
      if( *tptr == '(' || *tptr == ')' || (*tptr & 0x80))
         {
         char tbuff[8];

         memmove( tptr + 3, tptr, strlen( (char *)tptr) + 1);
         sprintf( tbuff, "%03o", *tptr);
         memcpy( tptr + 1, tbuff, 3);
         tptr[0] = '\\';
         }
}

static char *get_text( char *buff, const char language, int idx)
{
   static char *xlate = NULL;
   char search[3], *rval = buff, *tptr;

   if( !buff)
      {
      free( xlate);
      xlate = NULL;
      return( NULL);
      }
   if( !xlate)
      {
      FILE *ifile = fopen( "xlate.txt", "rb");
      size_t len;

      assert( ifile);
      fseek( ifile, 0, SEEK_END);
      len = ftell( ifile);
      fseek( ifile, 0, SEEK_SET);
      xlate = (char *)malloc( len + 1);
      fread( xlate, len, 1, ifile);
      fclose( ifile);
      xlate[len] = '\0';
      }
   search[0] = '!';
   search[1] = language;
   search[2] = '\0';
   tptr = strstr( xlate, search);
   assert( tptr);
   tptr += 3;
   while( idx >= 0)
      {
      while( isspace( *tptr))
         tptr++;
      while( !isspace( *tptr))
         if( !idx)
            *buff++ = *tptr++;
         else
            tptr++;
      idx--;
      }
   *buff = '\0';
   fix_for_postscript( rval);
   return( rval);
}

#define MAX_DATES 408

int dummy_main( const int argc, const char **argv)
{
   FILE *ofile, *ifile;
   char buff[100], *dates[MAX_DATES];
   const char *events_shown = "nfehu";  /* by default,  show new/full moons, */
   int month, jd, year;            /* equinoxes,  holidays,  and US holidays */
   size_t i;
   bool use_color = false;
   char language = 'e';

   if( argc < 2 || (year = atoi( argv[1])) < 1900 || year > 2100)
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
            case 'l':
               language = argv[i][2];
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
   memset( dates, 0, sizeof( dates));
   ifile = fopen( buff, "rb");
   if( ifile)
      {
      while( fgets( buff, sizeof( buff), ifile))
         if( !memcmp( buff, argv[1], 4) && strlen( buff) > 13)
            {
            int show_it = (NULL != strchr( events_shown, buff[11]));

            if( !show_it && (buff[11] == 'n' || buff[11] == 'f'))
               {
               show_it = 1;
               strcpy( buff + 12, " -\n");
               }
            if( show_it)
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
         }
      fclose( ifile);
      }

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
      char tbuff[30];
      int day;

      fprintf( ofile, "%d set_month\n", month);
      get_text( tbuff, language, month + 6);
      fprintf( ofile, "(%c) monthshow\n", *tbuff);
      for( day = 1; day <= end_day; day++)
         {
         int yloc = 670 - day * 40;
         const char *override_default_day = NULL;
         const char *date_text = dates[month * 31 + day];
         const char *split_ptr, *text;
         const int day_of_week = (jd + day + 2) % 7;
         int is_new_moon;

         is_new_moon = (date_text && *date_text == 'n');
         text = get_text( tbuff, language, day_of_week);
         if( date_text && date_text[2] != '-')
            text = date_text + 2;
         split_ptr = strchr( text, '$');
         if( strstr( text, "\\p"))
            split_ptr = text;
         if( use_color)
            {
            if( date_text && *date_text == 'h')
               override_default_day = "holiday";
            else if( date_text && *date_text == 'b')
               override_default_day = "birthday";
            else if( !day_of_week)
               override_default_day = "sunday";
            if( override_default_day)
               fprintf( ofile, "%s ", override_default_day);
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
         if( override_default_day)
            fprintf( ofile, "def_day\n");
         }
      jd += month_length( month, year);
      }
   get_text( NULL, '\0', 0);
   for( i = 0; i < sizeof( dates) / sizeof( dates[0]); i++)
      if( dates[i])
         free( dates[i]);
   while( fgets( buff, sizeof( buff), ifile))
      fputs( buff, ofile);
   fclose( ifile);
   fclose( ofile);
   return( 0);
}

#ifndef CGI_VERSION     /* "standard" command-line test version */
int main( const int argc, const char **argv)
{
   return( dummy_main( argc, argv));
}
#else

#include <time.h>
#ifdef __has_include
   #if __has_include(<cgi_func.h>)
       #include "cgi_func.h"
   #else
       #error   \
         'cgi_func.h' not found.  This project depends on the 'lunar'\
         library.  See www.github.com/Bill-Gray/lunar .\
         Clone that repository,  'make'  and 'make install' it.
       #ifdef __GNUC__
         #include <stop_compiling_here>
            /* Above line suppresses cascading errors. */
       #endif
   #endif
#else
   #include "cgi_func.h"
#endif

int main( void)
{
   char field[30], year[5], buff[1000], events_shown[20];
   char language[5];
   FILE *lock_file = fopen( "lock.txt", "a"), *ifile;
   int rval, i;
   const time_t t0 = time( NULL);
   int n_args = 3;
   char *args[20];

   if( !lock_file)
      {
      printf( "Content-type: text/html\n\n");
      printf( "<pre>");
      printf( "<h1> Server is busy.  Try again in a minute or two. </h1>");
      printf( "<p> Your lunar calendar is very important to us! </p>");
      printf( "<p> (I don't really expect this service to get a lot of "
              "use.  If you see this error several times in a row, "
              "something else must be wrong; please contact me.)</p>");
      return( 0);
      }
   setvbuf( lock_file, NULL, _IONBF, 0);
   fprintf( lock_file, "Current time %.24s UTC\n", asctime( gmtime( &t0)));
   fprintf( lock_file, "moon_cal ver: %s %s\n", __DATE__, __TIME__);
   avoid_runaway_process( 300);
   fprintf( lock_file, "300-second limit set\n");
   args[0] = NULL;
   args[1] = year;
   args[2] = events_shown;
   args[3] = NULL;
   strcpy( year, "2021");
   strcpy( events_shown, "-e");
   rval = initialize_cgi_reading( );
   if( rval <= 0)
      {
      printf( "<p> <b> CGI data reading failed : error %d </b>", rval);
      printf( "This isn't supposed to happen.</p>\n");
      return( 0);
      }
   while( !get_cgi_data( field, buff, NULL, sizeof( buff)))
      {
      fprintf( lock_file, "Field '%s': '%s'\n", field, buff);
      if( !strcmp( field, "year") && strlen( buff) == 4)
         strcpy( year, buff);
      else if( !strcmp( field, "us_hol"))
         strcat( events_shown, "u");
      else if( !strcmp( field, "full"))
         strcat( events_shown, "f");
      else if( !strcmp( field, "new"))
         strcat( events_shown, "n");
      else if( !strcmp( field, "holi"))
         strcat( events_shown, "h");
      else if( !strcmp( field, "equi"))
         strcat( events_shown, "e");
      else if( !strcmp( field, "birt"))
         strcat( events_shown, "b");
      else if( !strcmp( field, "color"))
         {
         args[n_args++] = "-c";
         args[n_args] = NULL;
         }
      else if( !strcmp( field, "lang"))
         {
         args[n_args++] = language;
         args[n_args] = NULL;
         snprintf( language, sizeof( language), "-l%c", buff[0]);
         }
      }
   fprintf( lock_file, "Options read and parsed;  year '%s'\n", year);
   for( i = 0; i < n_args; i++)
      fprintf( lock_file, "%d: '%s'\n", i, args[i]);
   rval = dummy_main( n_args, (const char **)args);
   fprintf( lock_file, "Done: rval %d\n", rval);
   ifile = fopen( "z.ps", "rb");
   if( !ifile)
      fprintf( lock_file, "z.ps not opened!\n");
   else
      {
      fclose( ifile);
      system( "ps2pdf z.ps z.pdf");
      ifile = fopen( "z.pdf", "rb");
      if( !ifile)
         fprintf( lock_file, "z.pdf not opened!\n");
      else
         {
         size_t bytes_read;

         printf( "Content-type: application/pdf\n");
         fseek( ifile, 0L, SEEK_END);
         printf( "Content-length: %ld\n\n", ftell( ifile));
         fseek( ifile, 0L, SEEK_SET);
         while( 0 != (bytes_read = fread( buff, 1, sizeof( buff), ifile)))
            fwrite( buff, bytes_read, 1, stdout);
         fclose( ifile);
         fprintf( lock_file, "Copied 'z.pdf'\n");
         }
      }
// for( i = 3; i < n_args; i++)
//    free( args[i]);
   fclose( lock_file);
}
#endif
