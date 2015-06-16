/* 
 * File:   mvXmlCmdApplication.cpp
 * Author: Jay Sprenkle   jsprenkle at gmail.com
 * 
 * Created on April 28, 2015
 */
#include <stdexcept>
#include <string>
#include <iostream>
#include <fcntl.h>   // open
#include <unistd.h>  // read, write, close

#include <sys/stat.h> // stat
#include <errno.h>    // errno, ENOENT, EEXIST
#if defined(_WIN32)
#include <direct.h>   // _mkdir
#endif

#include "conversion.h"
//#include "XmlCmd.info.h" // why can't I put these into a static library?
extern ::std::string HostName();
extern ::std::string UserName();

#include "cpXmlCmdApplication.h"

cpXmlCmdApplication::cpXmlCmdApplication( const char* ResultDocumentFileName, int argc, char** argv )
   : cpXmlCmdNamespace( "http://www.XmlCommandLine.org/cpXmlCmd/1.0" )
   , output( 0 )
   , status( true )
   , argc( argc )
   , argv( argv )
{
   if ( ResultDocumentFileName )
   {
      start = time( 0 );

      output = new cpXmlCmdOutput( ResultDocumentFileName );

      // run time environment
      ::std::string separator;
      ::std::string args;
      for ( int i = 0; i < argc; ++i )
      {
         args.append( separator );
         args.append( argv[i] );
         separator = " ";
      }
      
      // create context area
      output->Context( args, UserName(), HostName() );
   }
}

cpXmlCmdApplication::~cpXmlCmdApplication()
{
   if ( output )
   {
      finish = time( 0 );
      
      // write results of processing

      // notes
      ::std::string msg( "cpXmlCmd: " );
      msg.append( "copied: " );
      msg.append( toString<int>( copied ) );
      msg.append( ", failed: " );
      msg.append( toString<int>( failed ) );
      output->Notes( msg.c_str() );

      output->Status( status, start, finish );
      
      output->FinishContent();
      delete output;
   }
}

void cpXmlCmdApplication::initialize()
{
   copied = 0;
   failed = 0;
   
   // begin content node
   output->StartContent();
}

bool cpXmlCmdApplication::process( const char* filename )
{
   bool result = true;
   try
   {
      xmlTextReaderPtr readerptr = xmlNewTextReaderFilename( filename );
      if ( readerptr != NULL )
      {
         int ret = xmlTextReaderRead( readerptr );
         while ( ret == 1 )
         {
            processNode( readerptr );
            ret = xmlTextReaderRead( readerptr );
         }
         xmlFreeTextReader( readerptr );
         if ( ret != 0 )
         {
            ::std::string msg( filename );
            msg.append( " failed to parse " );
            msg.append( strerror( errno ) );
            throw ::std::runtime_error( msg );
         }
      }
      else
      {
         ::std::string msg( filename );
         msg.append( " failed to open: " );
         msg.append( strerror( errno ) );
         throw ::std::runtime_error( msg );
      }
   }
   catch ( ::std::exception ex )
   {
      ::std::cerr << ex.what() << "\n";
      result = false;
      status = false;
   }
   catch ( ... )
   {
      ::std::cerr << "Unknown error\n";
      result = false;
      status = false;
   }

   return result;
}

bool isDir( const ::std::string& path )
{
#if defined(_WIN32)
   struct _stat info;
   if ( _stat( path.c_str(), &info ) != 0 )
      return false;
   return (info.st_mode & _S_IFDIR ) != 0;
#else 
   struct stat info;
   if ( stat( path.c_str(), &info ) != 0 )
      return false;
   return (info.st_mode & S_IFDIR ) != 0;
#endif
}

bool makePath( const ::std::string& path )
{
   mode_t mode = 0755;
   int ret = mkdir( path.c_str(), mode );
   if ( ret == 0 )
      return true;

   switch ( errno )
   {
      // parent didn't exist, try to create it
      case ENOENT:
         {
            int pos = path.find_last_of( "/\\" );
            if ( pos == ::std::string::npos )
               return false;
            if ( ! makePath( path.substr( 0, pos ) ) )
               return false;
         }
         // now, try to create again
         return mkdir( path.c_str(), mode ) == 0;

      case EEXIST:
         // done!
         return isDir( path );

      default:
         return false;
   }
}

void cpXmlCmdApplication::processNode( xmlTextReaderPtr readerptr )
{
   if ( compareNode( readerptr, "cp" ) )
   {
      xmlChar* ns = xmlTextReaderNamespaceUri( readerptr );
      if ( ns )
      {
         if ( strcmp( (const char*) ns, cpXmlCmdNamespace ) == 0 )
         {
            xmlChar* source = xmlTextReaderGetAttribute( readerptr, (const unsigned char*) "Source" );
            if ( source )
            {
               xmlChar* destination = xmlTextReaderGetAttribute( readerptr, (const unsigned char*) "Destination" );
               if ( destination )
               {
                  validate( (const char*) destination );

                  // rename/move the file
                  copy( (const char*) source, (const char*) destination );

                  xmlFree( destination );
               }
               xmlFree( source );
            }
         }
         xmlFree( ns );
      }
   }
}

void cpXmlCmdApplication::validate( const char* destination )
{
   ::std::string PathAndFileName( (const char*) destination );
   // if no path
   size_t off = PathAndFileName.find_last_of( "/\\" );
   if ( off != ::std::string::npos )
      if ( ! makePath( PathAndFileName.substr( 0, off ) ) )
      {
         ::std::string msg( "failed to create path to output file: '" );
         msg.append( (const char*) destination );
         msg.append( "'. " );
         msg.append( strerror( errno ) );
         throw ::std::runtime_error( msg );
      }
}

bool cpXmlCmdApplication::copy( const char* source, const char* destination )
{
   time_t start;
   if ( output )
      start = time( 0 );
   
   bool result;

   char buf[BUFSIZ];
   size_t size;

   int srcfd = open( source, O_RDONLY | O_BINARY, 0 );
   if ( srcfd != -1 )
   {
      int destfd = open( destination, O_WRONLY | O_CREAT | O_BINARY /*| O_TRUNC/**/, 0644 );
      if ( destfd != -1 )
      {
         while ( ( size = read( srcfd, buf, BUFSIZ ) ) > 0 )
            write( destfd, buf, size );
         close( destfd );
   
         ++copied;
         result = true;
      }
      else
      {
         ::std::cerr << "copy failed: "
             << "'" << strerror( errno ) << "'"
            << " copy from:'" << (const char*) source << "'"
            << " to:'" << (const char*) destination << "'"
            << "\n"
            ;
         ++failed;
         result = false;
      }
      close( srcfd );
   }
   else
   {
      ::std::cerr << "copy failed: "
          << "'" << strerror( errno ) << "'"
         << " copy from:'" << (const char*) source << "'"
         << " to:'" << (const char*) destination << "'"
         << "\n"
         ;
      ++failed;
      result = false;
   }
   
   if ( output )
      output->cp( source, destination, start );
   
   return result;
}

bool cpXmlCmdApplication::compareNode( xmlTextReaderPtr readerptr, const char* NodeName )
{
   bool result = false;
   xmlChar* name = xmlTextReaderLocalName( readerptr );
   if ( name )
   {
      result = strcmp( (const char*) name, NodeName ) == 0;
      xmlFree( name );
   }
   return result;
}

