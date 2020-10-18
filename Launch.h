#pragma once

#include <algorithm>
#include <iomanip>
#include <fstream>
#include <istream>
#include <iostream>
#include <sstream>

class Launch
{
private:
   bool isCanonical;

public:
   static Launch* CreateLaunch( int argc, char** argv );

   virtual std::istream& GetStream() = 0;

   bool IsCanonical()
   {
      return isCanonical;
   }

protected:
   Launch( bool isCanonical )
   {
      this->isCanonical = isCanonical;
   }

   virtual ~Launch()
   {
      // Nothing to do
   }

private:
   class LaunchBuilder
   {
   private:
      bool isCanonical;
      bool hasFilename;
      std::string filename;

   public:
      LaunchBuilder()
      {
         isCanonical = false;
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

      void setCanonical()
      {
         isCanonical = true;
      }
   };
};

class StreamLaunch : public Launch
{
public:
   StreamLaunch( bool isCanonical ) : Launch( isCanonical )
   {
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
   FileLaunch( std::string& filename, bool isCanonical ) : Launch( isCanonical )
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
