/* 
 * File:   mvXmlCmdOutput.h
 * Author: Jay Sprenkle
 *
 * Created on May 26, 2015
 */

#ifndef CPXMLCMDOUTPUT_H
#define	CPXMLCMDOUTPUT_H

#include <time.h>

#include "libxml.h"
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

#include <errno.h>

#include <string>

class cpXmlCmdOutput
{
public:
   cpXmlCmdOutput( const char* ResultDocumentFileName );
   virtual ~cpXmlCmdOutput();
   
   void Context( const ::std::string& args, const ::std::string& User, const ::std::string& Host );
   
   /**
    * create content area of output document
    */
   void StartContent();
   
   void FinishContent();
   
   void Notes( const char* msg );
   
   void Status( bool success, time_t start, time_t finish );
   
   void cp( const char* source, const char* destination, time_t start );

private:
   xmlChar* ConvertInput( const xmlChar* in, const xmlChar* encoding );
   
   const xmlChar* encoding;
   const xmlChar* cpXmlCmdNamespace;

   xmlTextWriterPtr writer;
};

#endif
