// hexdump.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <iomanip>
#include <fstream>

int main( int argc, char** argv )
{
   int arg = argc > 0 ? 1 : 0;

   std::cout << argv[ arg ] << std::endl << std::endl;

   std::ifstream is;
   is.open( argv[ arg ], std::ios::binary );

   // Get the file length
   is.seekg( 0, std::ios::end );
   std::streamoff length = is.tellg();
   is.seekg( 0, std::ios::beg );

   std::streamoff current = 0;
   std::streamoff width = 16;
   std::streamoff count = 2;

   std::ios_base::fmtflags f( std::cout.flags() );  // save flags state
   while ( current < length )
   {
      std::cout << std::hex << std::setw( 8 ) << std::setfill( '0' ) << std::nouppercase << current;

      for ( std::streamoff loop = 0; loop < width; )
      {
         unsigned long value = 0;
         for ( std::streamoff i = 0; i < count; i++, loop++, current++ )
         {
            if ( current == length )
            {
               break;
            }
            value <<= 8;
            value += is.get();
         }

         std::cout << " " << std::setw( (std::streamsize) pow( 2, (int) count ) ) << value;
         if ( current == length )
         {
            break;
         }
      }

      std::cout << std::endl;
      if ( current == length )
      {
         break;
      }
   }
   std::cout.flags( f );  // restore flags state

   is.close();
}
