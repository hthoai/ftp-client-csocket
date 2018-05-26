// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include <stdio.h>
#include <tchar.h>
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include <afx.h>
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <iostream>
#include <corecrt_wio.h>
#include <afxsock.h>
using namespace std;
#include <string>
#include <stdio.h>
#include <corecrt_wstring.h>
#include <fstream>
#include <vector>
#include <ctime>
#include <sstream>
#include <istream>
#include <iterator>
#include <map>

// TODO: reference additional headers your program requires here

map<string, string>serverCMD{
{"ls","LIST"},
{"dir","NLST"},
{"get","RETR"},
{"put","STOR"}};