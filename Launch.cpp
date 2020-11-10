#include <string>

#include "Arguments.h"
#include "Launch.h"

Launch* Launch::CreateLaunch( Arguments* arguments )
{
   LaunchBuilder builder;

   // Process command line switches
   if ( arguments->HasOneOf( { "-C", "/C" } ) )
   {
      builder.setCanonical();
   }
   if ( arguments->HasOneOf( { "-x", "/x" } ) )
   {
      builder.setTwoByte();
   }
   if ( arguments->HasOneOf( { "-be", "/be" } ) )
   {
      builder.setBigEndian();
   }
   if ( arguments->HasOneOf( { "-le", "/le" } ) )
   {
      builder.setLittleEndian();
   }

   // Treat as an input file 
   if ( arguments->HasOrphaned() )
   {
      builder.SetFilename( arguments->GetOrphaned()[ 0 ] );
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
