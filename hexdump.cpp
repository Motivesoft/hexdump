// hexdump.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <fstream>

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
   while ( cs.good() )
   {
      // Write out the current address at the start of the line
      std::cout << std::hex << std::setw( 8 ) << std::setfill( '0' ) << std::nouppercase << current;

      // Line at a time
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
            value <<= 8;
            value += next;
         }

         // Write the value - single or multibyte as configured and depending on bytes read
         if ( i )
         {
            std::cout << " " << std::setw( ( std::streamsize ) pow( 2, (int) std::min( i, count ) ) ) << value;
         }

         if ( done )
         {
            break;
         }
      }

      // Finish the line
      std::cout << std::endl;

      // Step out if we've hit the end of the file
      if ( done )
      {
         if ( current % 16 ) // TODO we may take this out once we've changed the way the output is constructed
         {
            // hexdump on Linux dumps out the end address
            std::cout << std::hex << std::setw( 8 ) << std::setfill( '0' ) << std::nouppercase << current;
         }
         break;
      }
   }

   // Restore flags state
   std::cout.flags( f );  

   // Signal success
   return 0;
}
