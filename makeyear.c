#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

/* Utility to generate the list of new and full moons and equinoxes
and solstices for a year specified on the command line.  Hand editing
to insert solar eclipses and any desired holidays is still needed.
Compile with

gcc -Wall -Wextra -pedantic -o makeyear makeyear.c */

int main( const int argc, const char **argv)
{
   FILE *ifile = fopen( "phases.txt", "rb");
   char buff[90];
   size_t i, pass;

   assert( ifile);
   assert( argc == 2);
   printf( "   Dates for %s.  See 'date2023.txt' for explanation.\n", argv[1]);
   printf( "YYYY MM DD M Pt Text\n");
   while( fgets( buff, sizeof( buff), ifile))
      if( !memcmp( buff, "New moon:", 8))
         for( i = 15; i < 55; i += 19)
            if( !memcmp( buff + i, argv[1], 4))
               {
               if( buff[i + 5] == ' ')
                  buff[i + 5] = '0';
               if( buff[i + 8] == ' ')
                  buff[i + 8] = '0';
               buff[i + 10] = '\0';
               printf( "%s n -\n", buff + i);
               }
   fclose( ifile);
   ifile = fopen( "fullmoon.txt", "rb");
   assert( ifile);
   for( pass = 0; pass < 2; pass++)
      {
      fseek( ifile, 0L, SEEK_SET);
      while( fgets( buff, sizeof( buff), ifile))
         if( !memcmp( buff + 18, argv[1], 4))
            if( (!pass && *buff == 'F') || (pass && buff[7] == ':'))
               {
               const char *months = "Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec ";

               i = 0;
               while( i < 12 && memcmp( months + i * 4, buff + 14, 4))
                  i++;
               assert( i < 12);
               printf( "%s %02d %02d ", argv[1], (int)i + 1, atoi( buff + 11));
               if( !pass)
                  {
                  if( !memcmp( buff + 47, "After Yule", 10))
                     printf( "f 12 After$Yule\n");
                  else if( !memcmp( buff + 47, "Before Yul", 10))
                     printf( "f 12 Before$Yule\n");
                  else
                     printf( "f %s", buff + 47);
                  }
               else switch( i)
                  {
                  case 2: case 8:
                     printf( "e Equi\n");
                     break;
                  case 5: case 11:
                     printf( "e Sols\n");
                     break;
                  default:
                     assert( 0);
                     break;
                  }
               }
      }
   fclose( ifile);
   return( 0);
}
