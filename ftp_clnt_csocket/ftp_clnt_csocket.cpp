// ftp_clnt_csocket.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Client.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;

using namespace std;

char* sendCommand(char str[100])
{
	//sprintf(buf,"USER %s\r\n",info);
	return NULL;
}

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule != NULL)
	{
		// initialize MFC and print and error on failure
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
		{
			// TODO: change error code to suit your needs
			_tprintf(_T("Fatal Error: MFC initialization failed\n"));
			nRetCode = 1;
		}
		else
		{
			// TODO: code your application's behavior here.
			// Khoi tao thu vien Socket
			if (AfxSocketInit() == FALSE)
			{
				cout << "Can't initialize Socket Libraray\n";
				return FALSE;
			}

			/*string path, cmd, input;
			getline(cin, input);
			cmd = input.substr(0, input.find(" "));
			path = input.substr(input.find(" ") + 1);
			cout << endl << cmd << endl << path << endl;

			cout << getFileName(path) << endl;*/
			/*Client H;
			H.play();*/
			//H.login();
			//H.lsdir("ls","");
			//H.get("Tam9/neuanhdi.mp3");
			//H.put("D:/by.mp3");
			string in;
			getline(cin, in);
			istringstream iss(in);
			vector<string>paths((istream_iterator<string>(iss)), istream_iterator<string>());
			for (int i = 0; i < paths.size(); i++)
			{
				cout << paths[i] << "|";
			}
		}
	}
	else
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: GetModuleHandle failed\n"));
		nRetCode = 1;
	}

	return nRetCode;
}
