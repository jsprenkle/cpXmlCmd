/* 
 * File:   mvXmlCmdApplication.h
 * Author: Jay Sprenkle
 *
 * Created on May 26, 2015
 */

#ifndef CPXMLCMDAPPLICATION_H
#define	CPXMLCMDAPPLICATION_H

#include <errno.h>

#include "libxml.h"
#include "libxml/xmlreader.h"

#include "cpXmlCmdOutput.h"

class cpXmlCmdApplication
{
public:
   /**
    * Constructor
    * @param ResultDocumentFileName: optional. If null then no output document is produced
    */
   cpXmlCmdApplication( const char* ResultDocumentFileName, int argc, char** argv );
   virtual ~cpXmlCmdApplication();

   bool process( const char* filename );
   bool copy( const char* source, const char* destination );
   void validate( const char* destination );
   
   bool status;

private:
   void processNode( xmlTextReaderPtr readerptr );
   bool compareNode( xmlTextReaderPtr readerptr, const char* NodeName );
   
   const char* cpXmlCmdNamespace;
   cpXmlCmdOutput* output;
   int copied, failed;
   time_t start, finish;
   int argc;
   char** argv;
};

#endif
