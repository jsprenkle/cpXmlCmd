/* 
 * File:   cpXmlCmdOutput.cpp
 * Author: Jay Sprenkle
 * 
 * Created on April 28, 2015
 */
#include <stdexcept>

#include "cpXmlCmdOutput.h"

cpXmlCmdOutput::cpXmlCmdOutput( const char* ResultDocumentFileName )
   : encoding( (const xmlChar*) "ISO-8859-1" )
   , cpXmlCmdNamespace( (const xmlChar*) "http://www.XmlCommandLine.org/cpXmlCmd/1.0" )
{
   /* Create a new XmlWriter for uri, with no compression. */
   writer = xmlNewTextWriterFilename( ResultDocumentFileName, 0 );
   if ( writer == NULL )
      throw ::std::runtime_error( "Error creating the xml writer" );

   /* Start the document with the xml default for the version,
    * encoding ISO 8859-1 and the default for the standalone
    * declaration. */
   if ( xmlTextWriterStartDocument( writer, NULL, (const char*)encoding, NULL ) < 0 )
      throw ::std::runtime_error( "Error starting the output document" );

   // Start root element
   if ( xmlTextWriterStartElementNS( writer, NULL, (xmlChar *) "cpXmlCmdLog", cpXmlCmdNamespace ) < 0 )
      throw ::std::runtime_error( "Error writing to the output document" );
}

cpXmlCmdOutput::~cpXmlCmdOutput()
{
    /* Here we could close open elements but since we do not want to
     * write any other elements, we simply call xmlTextWriterEndDocument,
     * which will do all the work */
    if ( xmlTextWriterEndDocument( writer ) < 0 )
       throw ::std::runtime_error( "Error ending to the output document" );

    xmlFreeTextWriter( writer );
}

void cpXmlCmdOutput::Context( const ::std::string& args, const ::std::string& User, const ::std::string& Host )
{
   // Start Context element
   if ( xmlTextWriterStartElement( writer, (xmlChar *) "Context" ) < 0 )
      throw ::std::runtime_error( "Error writing to the output document" );

   if ( xmlTextWriterStartElement( writer, (xmlChar *) "Environment" ) < 0 )
      throw ::std::runtime_error( "Error writing to the output document" );
   
   xmlChar* tmp;
   tmp = ConvertInput( (xmlChar*)args.c_str(), encoding );
   if ( xmlTextWriterWriteElement( writer, (xmlChar *) "args", tmp ) < 0 )
      throw ::std::runtime_error( "Error writing to the output document" );
   if ( tmp != NULL )
      xmlFree( tmp );
   
   tmp = ConvertInput( (xmlChar*)User.c_str(), encoding );
   if ( xmlTextWriterWriteElement( writer, (xmlChar *) "User", tmp ) < 0 )
      throw ::std::runtime_error( "Error writing to the output document" );
   if ( tmp != NULL )
      xmlFree( tmp );
   
   tmp = ConvertInput( (xmlChar*)Host.c_str(), encoding );
   if ( xmlTextWriterWriteElement( writer, (xmlChar *) "Host", tmp ) < 0 )
      throw ::std::runtime_error( "Error writing to the output document" );
   if ( tmp != NULL )
      xmlFree( tmp );

   // </Environment>
   xmlTextWriterEndElement( writer );

   // </Context>
   xmlTextWriterEndElement( writer );
}

void cpXmlCmdOutput::StartContent()
{
   // Start Content element
   if ( xmlTextWriterStartElement( writer, (xmlChar *) "Content" ) < 0 )
      throw ::std::runtime_error( "Error writing to the output document" );
}

void cpXmlCmdOutput::FinishContent()
{
   // </Content>
   xmlTextWriterEndElement( writer );
}

void cpXmlCmdOutput::Notes( const char* msg )
{
   xmlChar* tmp = ConvertInput( (xmlChar*)msg, encoding );
   if ( xmlTextWriterWriteElement( writer, (xmlChar *) "Notes", tmp ) < 0 )
      throw ::std::runtime_error( "Error writing to the output document" );
   if ( tmp != NULL )
      xmlFree( tmp );
}

void cpXmlCmdOutput::cp( const char* source, const char* destination, time_t start )
{
   xmlChar* tmp;
   
   if ( xmlTextWriterStartElement( writer, (xmlChar*) "cp" ) < 0 )
      throw ::std::runtime_error( "Error writing to the output document" );

   tmp = ConvertInput( (xmlChar*)source, encoding );
   if ( xmlTextWriterWriteAttribute( writer, (xmlChar *) "Source", tmp ) < 0 )
      throw ::std::runtime_error( "Error writing to the output document" );

   tmp = ConvertInput( (xmlChar*)destination, encoding );
   if ( xmlTextWriterWriteAttribute( writer, (xmlChar *) "Destination", tmp ) < 0 )
      throw ::std::runtime_error( "Error writing to the output document" );

   char TimeStampBuffer[32];
   strftime( TimeStampBuffer, sizeof TimeStampBuffer, "%Y-%m-%dT%H:%M:%SZ", gmtime( &start ) );
   
   if ( xmlTextWriterWriteAttribute( writer, (xmlChar *) "Start", (xmlChar *) TimeStampBuffer ) < 0 )
      throw ::std::runtime_error( "Error writing to the output document" );

   xmlTextWriterEndElement( writer );

   if ( tmp != NULL )
      xmlFree( tmp );
}

void cpXmlCmdOutput::Status( bool success, time_t start, time_t finish )
{
   if ( xmlTextWriterStartElement( writer, (xmlChar*) "Status" ) < 0 )
      throw ::std::runtime_error( "Error writing to the output document" );

   if ( xmlTextWriterWriteAttribute( writer, (xmlChar *) "success", (xmlChar *) ( success ? "true" : "false" ) ) < 0 )
      throw ::std::runtime_error( "Error writing to the output document" );

   char TimeStampBuffer[32];

   strftime( TimeStampBuffer, sizeof(TimeStampBuffer), "%Y-%m-%dT%H:%M:%SZ", gmtime( &start ) );
   if ( xmlTextWriterWriteAttribute( writer, (xmlChar *) "Start", (xmlChar *) TimeStampBuffer ) < 0 )
      throw ::std::runtime_error( "Error writing to the output document" );

   strftime( TimeStampBuffer, sizeof(TimeStampBuffer), "%Y-%m-%dT%H:%M:%SZ", gmtime( &finish ) );
   if ( xmlTextWriterWriteAttribute( writer, (xmlChar *) "Finish", (xmlChar *) TimeStampBuffer ) < 0 )
      throw ::std::runtime_error( "Error writing to the output document" );

   xmlTextWriterEndElement( writer );
}

/**
 * ConvertInput:
 * @in: string in a given encoding
 * @encoding: the encoding used
 *
 * Converts @in into UTF-8 for processing with libxml2 APIs
 *
 * Returns the converted UTF-8 string, or NULL in case of error.
 */
xmlChar* cpXmlCmdOutput::ConvertInput( const xmlChar* in, const xmlChar* encoding )
{
   xmlChar *out;
   int ret;
   int size;
   int out_size;
   int temp;
   xmlCharEncodingHandlerPtr handler;

   if ( in == 0 )
      return 0;

   handler = xmlFindCharEncodingHandler( (const char *)encoding );

   if ( !handler )
   {
      printf( "ConvertInput: no encoding handler found for '%s'\n", encoding != 0 ? (const char *)encoding : "" );
      return 0;
   }

   size = (int) strlen( (const char*) in ) + 1;
   out_size = size * 2 - 1;
   out = (xmlChar *) xmlMalloc( (size_t) out_size );

   if ( out != 0 )
   {
      temp = size - 1;
      ret = handler->input( out, &out_size, in, &temp );
      if ( ( ret < 0 ) || ( temp - size + 1 ) )
      {
         if ( ret < 0 )
         {
            printf( "ConvertInput: conversion wasn't successful.\n" );
         }
         else
         {
            printf
               ( "ConvertInput: conversion wasn't successful. converted: %i octets.\n",
               temp );
         }

         xmlFree( out );
         out = 0;
      }
      else
      {
         out = (xmlChar *) xmlRealloc( out, out_size + 1 );
         out[out_size] = 0; //null termination
      }
   }
   else
   {
      printf( "ConvertInput: no mem\n" );
   }

   return out;
}

