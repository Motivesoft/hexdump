#include <string>

#include "Launch.h"

Launch* Launch::CreateLaunch( int argc, char** argv )
{
   LaunchBuilder builder;

   for ( int i = 1; i < argc; )
   {
      int additionalArgs = argc - i;
      std::string arg( argv[ i++ ] );

      if ( !arg.empty() )
      {
         if ( arg[ 0 ] == '-' || arg[ 0 ] == '/' )
         {
            std::string param = arg.substr( 1 );

            // Process command line switches
            if ( param == "C" )
            {
               builder.setCanonical();
            }
            if ( param == "x" )
            {
               builder.setTwoByte();
            }
            if ( param == "be" )
            {
               builder.setBigEndian();
            }
            if ( param == "le" )
            {
               builder.setLittleEndian();
            }
         }
         else
         {
            // Treat as an input file 
            builder.SetFilename( arg );
         }
      }
   }

   return builder.Build();
}

Launch* Launch::LaunchBuilder::Build()
{
   if ( hasFilename )
   {
      return new FileLaunch( filename, isCanonical, isBigEndian );
   }

   return new StreamLaunch( isCanonical, isBigEndian );
}
