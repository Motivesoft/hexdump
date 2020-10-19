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
   bool isBigEndian;

public:
   static Launch* CreateLaunch( int argc, char** argv );

   virtual std::istream& GetStream() = 0;

   bool IsCanonical()
   {
      return isCanonical;
   }

   bool IsBigEndian()
   {
      return isBigEndian;
   }

protected:
   Launch( bool isCanonical, bool isBigEndian )
   {
      this->isCanonical = isCanonical;
      this->isBigEndian = isBigEndian;
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
      bool isBigEndian;
      bool hasFilename;
      std::string filename;

   public:
      LaunchBuilder()
      {
         uint16_t i = 1;
         uint8_t s = *(uint8_t*) &i;
         isCanonical = false;
         isBigEndian = s == 0;
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

      void setTwoByte()
      {
         isCanonical = false;
      }

      void setBigEndian()
      {
         isBigEndian = true;
      }

      void setLittleEndian()
      {
         isBigEndian = false;
      }
   };
};

class StreamLaunch : public Launch
{
public:
   StreamLaunch( bool isCanonical, bool isBigEndian ) : Launch( isCanonical, isBigEndian )
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
   FileLaunch( std::string& filename, bool isCanonical, bool isBigEndian ) : Launch( isCanonical, isBigEndian )
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
