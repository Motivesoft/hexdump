#include <string>

#include "Launch.h"

Launch* Launch::CreateLaunch( int argc, char** argv )
{
   LaunchBuilder builder;

   for ( int i = 1; i < argc; )
   {
      std::string arg( argv[ i++ ] );

      if ( !arg.empty() )
      {
         if ( arg[ 0 ] == '-' || arg[ 0 ] == '/' )
         {
            // Process command line switches
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
      return new FileLaunch( filename );
   }

   return new StreamLaunch();
}
