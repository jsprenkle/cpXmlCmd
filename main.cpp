/* 
 * File:   main.cpp
 * Author: Jay Sprenkle
 *
 * Created on May 26, 2015
 * 
 * A command line program to rename/move files.
 * 
 * A source file name and destination file name may be provided on the command line to
 * copy a single file.
 * 
 * 
 * expected command line inputs:
 *    cpXmlCmd [-l outputdocument] ExistingFileName NewFileName
 * 
 * Or
 * 
 * An XML document containing many copy operations may be provided.
 * The program does not load the entire XML document into memory so it may be
 * used in minimal resource systems with huge lists.
 * 
 * expected command line inputs:
 *    cpXmlCmd [-l outputdocument] XMLDocumentWithOneOrMoreFilesToCopy
 * 
 * The option switch(es) may be placed anywhere on the command line.
 * 
 * The program returns a non zero exit code in case of a failure.
 * Optionally produces an XML document that lists the status of the operation(s).
 * By default the status output document is not produced.
 * If the output document is requested but no file name is provided then
 * the document will be named "cpXmlCmd.result.xml"
 * 
 */
#include <iostream>
#include <stdexcept>

#include "cpXmlCmdApplication.h"

/*
 * program entry point
 */
int main( int argc, char** argv )
{
   int rc = 1;
   ::std::string InputFileName;
   ::std::string OutputFileName;
   ::std::string ResultDocumentFileName;
   bool ProduceResultDocument = false;

   try
   {
      // <editor-fold defaultstate="collapsed" desc="command line argument parsing">
      for ( int i = 1; i < argc; ++i )
      {
         ::std::string arg( argv[i] );

         // todo: allow "-lfilename" as an option
         
         // if '-l' then Produce Result Document
         if ( arg == "-l" )
         {
            ProduceResultDocument = true;
            ResultDocumentFileName.clear();
            continue;
         }
         if ( ProduceResultDocument && ResultDocumentFileName.empty() )
         {
            ResultDocumentFileName = arg;
            continue;
         }
         
         // input file name first
         if ( ! arg.empty() && InputFileName.empty() )
         {
            InputFileName = arg;
            continue;
         }

         // output file name next
         if ( ! arg.empty() && OutputFileName.empty() )
         {
            OutputFileName = arg;
            continue;
         }

         // something unrecognized
         throw ::std::runtime_error( ::std::string( "Invalid command line parameter: " ) + arg );
      }

      if ( InputFileName.empty() )
         throw ::std::runtime_error( ::std::string( "No command line parameters. Expected a single command list file name or old and new file names" ) );
      
      // if no output document name given then use default
      if ( ProduceResultDocument && ResultDocumentFileName.empty() )
         ResultDocumentFileName = "cpXmlCmd.result.xml";
      // </editor-fold>

      // pass in a file name if an output document is required
      cpXmlCmdApplication app( ProduceResultDocument ? ResultDocumentFileName.c_str() : 0, argc, argv );
      
      if ( ! InputFileName.empty() && ! OutputFileName.empty() )
      {
         const char* destination = OutputFileName.c_str();

         app.initialize();
        
         app.validate( destination );

         if ( app.copy( InputFileName.c_str(), destination) )
            rc = 0;
      }
      else
      {
         app.initialize();
         if ( app.process( InputFileName.c_str() ) )
            rc = 0;
      }
   }
   // <editor-fold defaultstate="collapsed" desc="Error handling">
   catch ( ::std::runtime_error ex )
   {
      ::std::cerr << "Run time exception thrown.\n" << ex.what() << "\n";
      rc = 2;
   }
   catch ( ::std::exception ex )
   {
      ::std::cerr << "Exception thrown.\n" << ex.what() << "\n";
      rc = 2;
   }
   catch ( ... )
   {
      ::std::cerr << "Unknown error\n";
      rc = 3;
   }
   // </editor-fold>

   return rc;
}
