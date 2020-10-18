#pragma once

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <fstream>

#include <istream>
#include <fstream>
#include <sstream>

class Launch
{
public:
   static Launch* CreateLaunch( int argc, char** argv );

   virtual std::istream& GetStream() = 0;

protected:
   Launch()
   {
      // Nothing to do
   }

   virtual ~Launch()
   {
      // Nothing to do
   }

   class LaunchBuilder
   {
   private:
      bool hasFilename;
      std::string filename;

   public:
      LaunchBuilder()
      {
         hasFilename = false;
      }

      Launch* Build();

      void SetFilename( std::string& filename )
      { 
         if ( !hasFilename )
         {
            this->filename = filename;
            hasFilename = true;
         }
      }
   };
};

class StreamLaunch : public Launch
{
public:
   StreamLaunch()
   {
      // Nothing to do
   }

   virtual std::istream& GetStream()
   {
      return std::cin;
   }
};

class FileLaunch : public Launch
{
private:
   std::string filename;
   std::ifstream stream;
   bool opened;

public:
   FileLaunch( std::string& filename )
   {
      this->filename = filename;
   }

   ~FileLaunch()
   {
      CloseStream();
   }

   std::istream& GetStream()
   {
      if ( !opened )
      {
         stream.open( filename, std::ios::binary );

         if ( !stream.is_open() )
         {
            std::ostringstream message;
            message << "Failed to open file: " << filename;
            throw std::exception( message.str().c_str() );
         }

         opened = true;
      }
      return stream;
   }

   void CloseStream()
   {
      if ( opened )
      {
         stream.close();
         opened = false;
      }
   }
};
