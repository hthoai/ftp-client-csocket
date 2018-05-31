#include "stdafx.h"

void replylogcode(int code)
{
	switch (code) {
	case 421:
		printf("No-transfer-time exceeded. Closing control connection.");
	case 451:
		printf("Can't remove directory");
		break;
	case 500:
		printf("Syntax error, command unrecognized.\n");
		printf("This may include errors such as command line too long.");
		break;
	case 501:
		printf("Syntax error in parameters or arguments.");
		break;
	case 202:
		printf("Command not implemented, superfluous at this site.");
		break;
	case 502:
		printf("Command not implemented.");
		break;
	case 503:
		printf("Bad sequence of commands.");
		break;
	default:
		printf("It's error .__.");
	}
	printf("\n");
}


string getFileName(string path)
{
	string filename;
	int len = path.length() - 1;

	while (len != -1 && path[len] != '/')
		filename = path[len--] + filename;

	if (filename[0] == 34) //"
		filename.erase(0, 1);

	int lenFN = filename.length() - 1;
	if (filename[lenFN] == 34)
		filename.erase(lenFN, lenFN + 1);

	return filename;
}


void splitPaths(string pathRAW, vector<string>& paths)
{
	int pos = 0;
	int i = 0;

	paths.resize(0);
	while (i < pathRAW.size() && pos != -1)
	{
		if (pathRAW[i] == 34) //"
		{
			pos = pathRAW.find_first_of(34, i + 1);
			paths.push_back(pathRAW.substr(i + 1, pos - i - 1));
			i = pos + 2;
		}
		else
		{
			pos = pathRAW.find(" ", i);
			paths.push_back(pathRAW.substr(i, pos - i));
			i = pos + 1;
		}
	}
}


void help()
{
	cout << "Commands may be abbreviated. Commands are:\n";
	cout << "?\t\thelp\t\tquote\n";
	cout << "open\t\tls\t\tdir\n";
	cout << "pwd\t\tcd\t\tlcd\n";
	cout << "put\t\tget\t\tmput\t\tmget\n";
	cout << "delete\t\tmdelete\t\tmkdir\t\trmdir\n";
	cout << "disconnect\tquit\t\tclose\t\t\exit\n";
}
