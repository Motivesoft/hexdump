// hexdump.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <fstream>

int main( int argc, char** argv )
{
   if ( argc <= 1 )
   {
      std::cerr << "Filename not specified" << std::endl;
      return 2;
   }

   // Filename argument
   int arg = 1;

   std::ifstream is;
   is.open( argv[ arg ], std::ios::binary );

   if ( !is.is_open() )
   {
      std::cerr << "Failed to open file: " << argv[ arg ] << std::endl;
      return 2;
   }

   // Get the file length - we will use this to avoid reading past the end of the file
   is.seekg( 0, std::ios::end );
   std::streamoff length = is.tellg();
   is.seekg( 0, std::ios::beg );

   // Some counters and sizes
   std::streamoff current = 0;
   std::streamoff width = 16;
   std::streamoff count = 2;

   // The read/display loop
   std::ios_base::fmtflags f( std::cout.flags() );  // save flags state
   while ( current < length )
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
            // If we hit the end of the file, step out
            if ( current == length )
            {
               break;
            }

            value <<= 8;
            value += is.get();
         }

         // Write the value - single or multibyte as configured and depending on bytes read
         std::cout << " " << std::setw( (std::streamsize) pow( 2, (int) std::min( i, count ) ) ) << value;
         if ( current == length )
         {
            break;
         }
      }

      // Finish the line
      std::cout << std::endl;

      // Step out if we've hit the end of the file
      if ( current == length )
      {
         // hexdump on Linux dumps out the end address
         std::cout << std::hex << std::setw( 8 ) << std::setfill( '0' ) << std::nouppercase << current;
         break;
      }
   }

   // Restore flags state
   std::cout.flags( f );  

   // Release resources
   is.close();

   // Signal success
   return 0;
}
