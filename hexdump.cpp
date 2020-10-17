// hexdump.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <fstream>

int main( int argc, char** argv )
{
   // Some counters and sizes
   bool done = false;
   std::streamoff current = 0;
   std::streamoff width = 16;
   std::streamoff count = 2;
   std::streamoff length = LONG_MAX;

   std::istream* cs;
   std::ifstream is;
   if ( argc <= 1 )
   { 
      cs = &std::cin;
   }
   else
   {
      // Filename argument
      int arg = 1;

      is.open( argv[ arg ], std::ios::binary );

      if ( !is.is_open() )
      {
         std::cerr << "Failed to open file: " << argv[ arg ] << std::endl;
         return 2;
      }

      // Get the file length - we will use this to avoid reading past the end of the file
      is.seekg( 0, std::ios::end );
      length = is.tellg();
      is.seekg( 0, std::ios::beg );

      cs = &is;
   }

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
               done = true;
               break;
            }

            // Try and get the next value
            int next = cs->get();
            if ( !cs->good() )
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
