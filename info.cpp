#ifdef __linux__
#include <pwd.h>
#endif

#ifdef _WIN32
#include <Winsock2.h>
#endif

#include <string>

   
   // <editor-fold defaultstate="collapsed" desc="HostName">
#ifdef __linux__

   ::std::string HostName()
   {
      char Buffer[256];
      Buffer[0] = 0;
      int rc = gethostname( &Buffer[0], sizeof ( Buffer ) );
      return ::std::string( Buffer );
   }
#endif
#ifdef _WIN32

::std::string HostName()
{
   ::std::string result;

   /* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
   WORD wVersionRequested = MAKEWORD( 2, 0 );

   WSADATA wsaData;
   int err = WSAStartup( wVersionRequested, &wsaData );
   if ( err != 0 )
      return result;

   /* Confirm that the WinSock DLL supports 2.0.*/
   if ( LOBYTE( wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 0 )
   {
      WSACleanup();
      return result;
   }

   char Buffer[256];
   Buffer[0] = 0;
   int rc = gethostname( &Buffer[0], sizeof ( Buffer ) );
   if ( rc == 0 )
      result = Buffer;

   WSACleanup();
   return result;
}
#endif
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="Get User Name">
#ifdef _WIN32

::std::string UserName()
{
   char Buffer[256];
   DWORD lpnSize = sizeof( Buffer );
   // documentation says windows 2000 and up have this function
   bool rc = GetUserName( &Buffer[0], &lpnSize );
   ::std::string result;
   if ( rc )
      result = Buffer;
   return result;
}
#endif
#ifdef __linux__

::std::string UserName()
{
    ::std::string result;
    char* p = getlogin();
    if ( p )
        result = p;
   return result;
}
#endif
// </editor-fold>

