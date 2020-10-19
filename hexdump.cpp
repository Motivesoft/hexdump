// hexdump.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "Launch.h"

int process( Launch* launch );

int main( int argc, char** argv )
{
   int result;

   try
   {
      Launch* launch = Launch::CreateLaunch( argc, argv );
      result = process( launch );
   }
   catch ( std::exception ex )
   {
      std::cerr << ex.what() << std::endl;
      result = 1;
   }

   return result;
}

int process( Launch* launch )
{
   std::istream& cs = launch->GetStream();

   // Some counters and sizes
   bool done = false;
   std::streamoff current = 0;
   std::streamoff width = 16;
   std::streamoff count = launch->IsCanonical() ? 1 : 2;

   // The read/display loop
   std::ios_base::fmtflags f( std::cout.flags() );  // save flags state

   std::string previousLine;
   bool matchesPreviousLine = false;
   while ( cs.good() )
   {
      // Write out the current address at the start of the line
      std::ostringstream location;
      location << std::hex << std::setw( 8 ) << std::setfill( '0' ) << std::nouppercase << current;

      // Line at a time
      std::ostringstream hexdata;
      std::ostringstream chardata;
      hexdata << std::hex << std::setfill( '0' ) << std::nouppercase;
      for ( std::streamoff loop = 0; loop < width; )
      {
         unsigned long value = 0;
         std::streamoff i;
         for ( i = 0; i < count; i++, loop++, current++ )
         {
            // Try and get the next value
            int next = cs.get();
            if ( !cs.good() )
            {
               // End of stream (expected or not)
               done = true;
               break;
            }

            // Process the value read
            if ( i == 0 )
            {
               value = next;
            }
            else
            {
               if ( launch->IsBigEndian() )
               {
                  value <<= 8;
                  value += next;
               }
               else
               {
                  value += (next << 8);
               }
            }

            chardata << (isascii( next ) && next >= 0x20 ? (char) next : '.');
         }

         // Write the value - single or multibyte as configured and depending on bytes read
         if ( i )
         {
            hexdata << " " << std::setw( ( std::streamsize ) pow( 2, (int) std::min( i, count ) ) ) << value;
         }

         if ( done )
         {
            break;
         }
      }

      // Write the line
      if ( !hexdata.str().empty() )
      {
         if ( previousLine == hexdata.str() )
         {
            if ( !matchesPreviousLine )
            {
               matchesPreviousLine = true;

               std::cout << "*" << std::endl;
            }
         }
         else
         {
            matchesPreviousLine = false;
            std::cout << location.str() << hexdata.str();
            if ( launch->IsCanonical() )
            {
               // Output the chars - but put some padding first if required
               std::cout << std::string( 3 * width, ' ' ).substr( hexdata.str().length() );
               std::cout << "  |" << chardata.str().c_str() << "|";
            }
            std::cout << std::endl;
            previousLine = hexdata.str();
         }
      }

      // Step out if we've hit the end of the file
      if ( done )
      {
         // hexdump on Linux dumps out the end address
         std::cout << std::hex << std::setw( 8 ) << std::setfill( '0' ) << std::nouppercase << current;
         break;
      }
   }

   // Restore flags state
   std::cout.flags( f );  

   // Signal success
   return 0;
}
