/*
 * hexdump
 *
 * A simple executable to show the contents of a file, or the input stream as hex. This is
 * somewhat similar to the Linux hexdump utility, but is not intended to be a full reproduction,
 * start with the basics and build on it from there as I find reason to.
 *
 * MIT License
 *
 * Copyright (c) 2020 Motivesoft
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "Arguments.h"
#include "Launch.h"

int process( Launch* launch );
std::string getAppName( const char* arg0 );
void showHelp( std::string app );

int main( int argc, const char** argv )
{
   int result = 0;

   try
   {
      Arguments* args = Arguments::ParseArguments( argc, argv );

      if( args->HasOneOf( { "-?", "-h", "/?", "/h", "--help" } ) )
      {
         showHelp( getAppName( argv[ 0 ] ) );
      }
      else
      {
         std::vector<std::string> unrecognised( args->FindAnyUnrecognised( { "-C", "/C", "-x", "/x", "-be", "/be", "-le", "/le" } ) );
         if ( !unrecognised.empty() )
         {
            std::cerr << "Unrecognised argument: " << unrecognised[ 0 ].c_str() << std::endl;
            showHelp( getAppName( argv[ 0 ] ) );
         }
         else
         {
            Launch* launch = Launch::CreateLaunch( args );
            result = process( launch );
         }
      }
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
               std::cout << std::string( (size_t)( width * 3 ), ' ' ).substr( hexdata.str().length() );
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

std::string getAppName( const char* arg0 )
{
   char appName[ _MAX_FNAME ];
   _splitpath_s( arg0, NULL, 0, NULL, 0, appName, _MAX_FNAME, NULL, 0 );
   _strlwr_s( appName, _MAX_FNAME );
   return appName;
}

void showHelp( std::string app )
{
   std::cout << std::endl;
   std::cout << "Usage:" << std::endl;
   std::cout << "  " << app.c_str() << " <options> <filename>" << std::endl;
   std::cout << "" << std::endl;
   std::cout << "Options:" << std::endl;
   std::cout << "  -C   canonical form, single byte hex output and printable characters" << std::endl;
   std::cout << "  -x   two byte hex output (default)" << std::endl;
   std::cout << "  -be  treat the data as big-endian when using -x" << std::endl;
   std::cout << "  -le  treat the data as little-endian when using -x" << std::endl;
   std::cout << "  -h   this help information" << std::endl;
   std::cout << "" << std::endl;
   std::cout << "If filename is excluded, the input stream shall be used instead. For example:" << std::endl;
   std::cout << "  dir | " << app.c_str() << std::endl;
   std::cout << std::endl;
}
