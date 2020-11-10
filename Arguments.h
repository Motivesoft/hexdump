#pragma once

#include <map>
#include <stdexcept>
#include <string>
#include <vector>

class Arguments
{
private:
   std::map<std::string,std::vector<std::string>*> args;
   std::vector<std::string> orphanedArgs;

   Arguments()
   {
   }

   std::map<std::string, std::vector<std::string>*>::iterator FindOne( std::vector<std::string>& params )
   {
      for ( std::vector<std::string>::iterator it = params.begin(); it != params.end(); it++ )
      {
         std::map<std::string, std::vector<std::string>*>::iterator arg = args.find( *it );
         if ( arg != args.end() )
         {
            return arg;
         }
      }
      return args.end();
   }

public:
   static Arguments* ParseArguments( int argc, const char** argv, bool orphansAnywhere = true )
   {
      Arguments* arguments = new Arguments();

      std::vector<std::string>* current = nullptr;
      for ( int loop = 1; loop < argc; loop++ )
      {
         std::string arg( argv[ loop ] );

         // Guard against ""
         if ( arg.empty() )
         {
            continue;
         }

         if ( arg[ 0 ] == '-' || arg[ 0 ] == '/' )
         {
            // This flag determines where an unrecognised argument is always treated like an orphan
            // or is added to the list for the most recently discovered '-' or '/' parameter
            if ( !orphansAnywhere )
            {
               current = new std::vector<std::string>();
            }
            arguments->args.insert( std::pair<std::string, std::vector<std::string>*>( arg, current ) );
         }
         else
         {
            if ( current == nullptr )
            {
               arguments->orphanedArgs.push_back( arg );
            }
            else
            {
               current->push_back( arg );
            }
         }
      }

      return arguments;
   }

   std::vector<std::string> FindAnyUnrecognised( std::vector<std::string> params )
   {
      std::vector<std::string> unrecognised;
      for ( std::map<std::string, std::vector<std::string>*>::const_iterator it = args.cbegin(); it != args.cend(); it++ )
      {
         bool recognised = false;
         std::string arg = ( *it ).first;
         for ( std::vector<std::string>::const_iterator paramIt = params.cbegin(); paramIt != params.cend(); paramIt++ )
         {
            if ( arg == *paramIt )
            {
               recognised = true;
               break;
            }
         }
         if ( !recognised )
         {
            unrecognised.push_back( arg );
         }
      }
      return unrecognised;
   }

   bool HasOneOf( std::vector<std::string> params )
   {
      std::map<std::string, std::vector<std::string>*>::iterator it = FindOne( params );
      return it != args.end();
   }

   std::vector<std::string> Get( std::vector<std::string> params )
   {
      std::map<std::string, std::vector<std::string>*>::iterator it = FindOne( params );
      if ( it != args.end() )
      {
         return std::vector<std::string>( *( *it ).second );
      }
      throw std::invalid_argument( "No arguments provided for parameter" );
   }

   bool HasOrphaned()
   {
      return !orphanedArgs.empty();
   }

   std::vector<std::string> GetOrphaned()
   {
      return std::vector<std::string>( orphanedArgs );
   }
};