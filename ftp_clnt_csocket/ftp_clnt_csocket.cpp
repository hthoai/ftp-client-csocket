#include "stdafx.h"
#include "Client.h"


int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule != NULL)
	{
		//Initialize MFC and print and error on failure
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
		{
			_tprintf(_T("Fatal Error: MFC initialization failed\n"));
			nRetCode = 1;
		}
		else
		{
			//Initialize Socket library
			if (AfxSocketInit() == FALSE)
			{
				cout << "Can't initialize Socket Libraray\n";
				return FALSE;
			}

			string input, cmd, path;
			while (true)
			{
				Client H;
				while (true)
				{
					cout << "ftp> ";
					rewind(stdin);
					getline(cin, input);

					cmd = input.substr(0, input.find(" "));
					path = input.substr(input.find(" ") + 1);
					if (cmd.length() == input.length())
						path = "";

					if (H.command(cmd, path) == false)
						break;
				}

				if (H.getDisconnectStt() == false)
					break;
			}
		}
	}
	else
	{
		_tprintf(_T("Fatal Error: GetModuleHandle failed\n"));
		nRetCode = 1;
	}

	return nRetCode;
}
