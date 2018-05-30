#include "stdafx.h"
#include "Client.h"


Client::Client()
{
	ClientSocket.Create();
	srand(time(NULL));
	clientDataPort = rand() % 40000 + 1024;
	mode = ACTIVE;
}


Client::~Client()
{
	ClientSocket.Close();
}


void Client::play()
{
	string input, cmd, path;

	while (true)
	{
		if (mode == ACTIVE)
			cout << "ftp> ";
		else
			cout << "ftp-pasv> ";
		rewind(stdin);
		getline(cin, input);

		cmd = input.substr(0, input.find(" "));
		path = input.substr(input.find(" ") + 1);
		if (cmd.length() == input.length())
			path = "";

		if (command(cmd, path) == false)
			break;

	}
}


bool Client::command(const string cmd, const string path)
{
	if (cmd == "open")
		login(path); //host
	else if (cmd == "quote")
		changeMode(path);
	else if (cmd == "ls")
		lsdir("ls", path);
	else if (cmd == "dir")
		lsdir("dir", path);
	else if (cmd == "get")
		get(path);
	else if (cmd == "put")
		put(path);
	else if (cmd == "mget")
		mget(path);
	else if (cmd == "mput")
		mput(path);
	else if (cmd == "delete")
		del(path);
	else if (cmd == "mdelete")
		mdel(path);
	else if (cmd == "pwd")
		pwd();
	else if (cmd == "cd")
		cd(path);
	else if (cmd == "lcd")
		lcd(path);
	else if (cmd == "mkdir")
		mkdir(path);
	else if (cmd == "rmdir")
		rmdir(path);
	else if (cmd == "quit" || cmd == "exit" || cmd == "close")
		return quitexit();
	else if (cmd == "?" || cmd == "help")
		help();
	else if (cmd == "")
		return true;
	else
		cout << "Invalid command.\n";

	return true;
}


bool Client::transferCMD(const string cmd, const string infor)
{
	int tmpres;
	int codeftp;
	char buf[BUFSIZ + 1];

	memset(buf, 0, sizeof buf);
	if (infor == "")
		sprintf(buf, "%s\r\n", cmd.c_str());
	else
		sprintf(buf, "%s %s\r\n", cmd.c_str(), infor.c_str());

	tmpres = ClientSocket.Send(buf, strlen(buf), 0);

	memset(buf, 0, tmpres);
	tmpres = ClientSocket.Receive(buf, BUFSIZ, 0);
	sscanf(buf, "%d", &codeftp);

	if (codeftp == 200 || codeftp == 230 || codeftp == 331 || codeftp == 150 || codeftp == 227 ||
		codeftp == 257 || codeftp == 250 || codeftp == 221)
	{
		printf("%s", buf);
		return true;
	}
	else if (codeftp == 550)
		printf("%s", buf);
	else if (codeftp == 530)
		printf("%sLogin failed.\n", buf);
	else
		replylogcode(codeftp);

	return false;
}


bool Client::connect(wstring host, unsigned int port)
{
	LPCTSTR temp = host.c_str();

	if (ClientSocket.Connect(temp, port) != 0)
	{
		cout << "Connected to ";
		wcout << temp << ".\n";
		return true;
	}
	else
	{
		cout << "> ftp: connect :ICMP network unreachable\n";
		return false;
	}
}


bool Client::login(string host)
{
	wstring w_host(host.begin(), host.end());
	hostIPaddr = w_host;
	if (connect(hostIPaddr, 21) == false)
		return false;

	char buf[BUFSIZ + 1];
	int tmpres, size, status;
	char * str;
	int codeftp;
	//How to know the end of welcome message:
	//http://stackoverflow.com/questions/13082538/how-to-know-the-end-of-ftp-welcome-message
	memset(buf, 0, sizeof buf);
	while ((tmpres = ClientSocket.Receive(buf, BUFSIZ, 0)) > 0) {
		sscanf(buf, "%d", &codeftp);
		printf("%s", buf);
		if (codeftp != 220) //120, 240, 421: something wrong
		{
			replylogcode(codeftp);
			exit(1);
		}

		str = strstr(buf, "220");//Why ???
		if (str != NULL) {
			break;
		}
		memset(buf, 0, tmpres);
	}

	//Send Username
	char info[50];
	cout << "User (";
	wcout << hostIPaddr;
	cout << ":(none)): ";
	memset(buf, 0, sizeof buf);
	scanf("%s", info);

	transferCMD("USER", info);

	//Send Password
	memset(info, 0, sizeof info);
	printf("Password: ");
	memset(buf, 0, sizeof buf);
	scanf("%s", info);

	transferCMD("PASS", info);

	return true;
}


void Client::changeMode(const string path)
{
	if (path == "pasv")
		mode = PASSIVE;
	else if (path == "acti")
		mode = ACTIVE;
	else
		printf("Syntax error.\n");
}


void Client::transferInit(CSocket & dsocket, CSocket & connector)
{
	int tmpres;
	int codeftp;
	char buf[BUFSIZ + 1];

	if (mode == ACTIVE)
	{
		CString sockIP; //Type of host IP is CString

		//Get IP and port 
		ClientSocket.GetSockName(sockIP, clientPort);
		const size_t newsize = sockIP.GetLength() + 1;
		char* IPstr = new char[newsize];
		CStringA IP_Cstr = (CStringA)sockIP;
		strcpy_s(IPstr, newsize, IP_Cstr);

		//Port command's arguments
		clientDataPort++;
		int p1 = clientDataPort / 256;
		int p2 = clientDataPort % 256;
		int a, b, c, d;

		sscanf(IPstr, "%d.%d.%d.%d", &a, &b, &c, &d);
		sprintf(buf, "PORT %d,%d,%d,%d,%d,%d\r\n", a, b, c, d, p1, p2);
		LPCTSTR hostIP = hostIPaddr.c_str();
		dsocket.Create(clientDataPort, SOCK_STREAM, hostIP);
		dsocket.Listen();

		tmpres = ClientSocket.Send(buf, strlen(buf), 0);
		memset(buf, 0, sizeof buf);
		tmpres = ClientSocket.Receive(buf, BUFSIZ, 0);

		sscanf(buf, "%d", &codeftp);
		if (codeftp == 150 || codeftp == 200)
			printf("%s", buf);
		else
			replylogcode(codeftp);
	}
	else //PASSIVE
	{
		memset(buf, 0, sizeof buf);
		sprintf(buf, "PASV\r\n");

		tmpres = ClientSocket.Send(buf, strlen(buf), 0);

		memset(buf, 0, tmpres);
		tmpres = ClientSocket.Receive(buf, BUFSIZ, 0);
		sscanf(buf, "%d", &codeftp);

		if (codeftp != 227)
			replylogcode(codeftp);
		else
		{
			int a, b, c, d, p1, p2;
			int serverDataPort;

			sscanf(buf, "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)\r\n", &a, &b, &c, &d, &p1, &p2);
			serverDataPort = p1 * 256 + p2;

			LPCTSTR hostIP = hostIPaddr.c_str();
			connector.Create();
			connector.Connect(hostIP, serverDataPort);
		}
	}
}


bool Client::afterTransfer()
{
	int tmpres;
	int codeftp;
	char buf[BUFSIZ + 1];

	memset(buf, 0, sizeof buf);
	tmpres = ClientSocket.Receive(buf, BUFSIZ, 0);
	sscanf(buf, "%d", &codeftp);
	if (codeftp == 226)
	{
		printf("%s", buf);
		return true;
	}
	else
	{
		replylogcode(codeftp);
		return false;
	}
}


bool Client::lsdir(const string cmd, const string path)
{
	int tmpres;
	int codeftp;
	CSocket dsocket, connector;
	char buf[BUFSIZ + 1];

	transferInit(dsocket, connector);

	if (cmd == "ls")
		transferCMD("NLST", path);
	else
		transferCMD("LIST", path);

	if (mode == ACTIVE)
		dsocket.Accept(connector);

	memset(buf, 0, sizeof buf);
	while ((tmpres = connector.Receive(buf, BUFSIZ, 0)) > 0)
	{
		printf("%s", buf);
		memset(buf, 0, tmpres);
	}

	connector.Close();
	dsocket.Close();

	return afterTransfer();
}


bool Client::get(const string path)
{
	int tmpres;
	int codeftp;
	CSocket dsocket, connector;
	char buf[BUFSIZ];

	transferInit(dsocket, connector);

	if (transferCMD("RETR", path) == false)
		return false;

	//Create file to write into
	string filename = getFileName(path);
	ofstream downfile(filename, ios::binary);

	if (mode == ACTIVE)
		dsocket.Accept(connector);

	memset(buf, 0, sizeof buf);
	while ((tmpres = connector.Receive(buf, BUFSIZ, 0)) > 0)
	{
		downfile.write(buf, tmpres);
		memset(buf, 0, tmpres);
	}

	downfile.close();
	connector.Close();
	dsocket.Close();

	return afterTransfer();
}


bool Client::put(const string path)
{
	string t_path = path;
	//Delete " at begin and end position if exists
	if (t_path[0] == 34) //"
	{
		t_path.erase(0, 1);
		t_path.pop_back();
	}

	//Create file to read
	ifstream upfile(t_path, ios::binary);
	//Check if file exists
	if (upfile.good() == false)
	{
		cout << path << ": File not found\n";
		return false;
	}

	int tmpres;
	int codeftp;
	int count;
	CSocket dsocket, connector;
	char buf[BUFSIZ];

	transferInit(dsocket, connector);

	string filename = getFileName(path);
	transferCMD("STOR", filename);

	if (mode == ACTIVE)
		dsocket.Accept(connector);

	upfile.seekg(ios::beg);
	do
	{
		memset(buf, 0, sizeof buf);
		upfile.read(buf, BUFSIZ);
		count = upfile.gcount();
		tmpres = connector.Send(buf, count, 0);
	} while (!upfile.eof());

	upfile.close();
	connector.Close();
	dsocket.Close();

	return afterTransfer();
}


bool Client::mget(const string pathRAW)
{
	string ucmd;
	vector<string>paths;
	splitPaths(pathRAW, paths);

	//transferCMD("TYPE", "I");

	for (int i = 0; i < paths.size(); i++)
	{
		printf("mget %s? ", paths[i].c_str());
		rewind(stdin);
		getline(cin, ucmd);

		if (ucmd == "" || ucmd == "Y" || ucmd == "y")
			get(paths[i]);
	}

	return true;
}


bool Client::mput(const string pathRAW)
{
	string ucmd;
	vector<string>paths;
	splitPaths(pathRAW, paths);

	//transferCMD("TYPE", "I");

	for (int i = 0; i < paths.size(); i++)
	{
		printf("mput %s? ", paths[i].c_str());
		rewind(stdin);
		getline(cin, ucmd);

		if (ucmd == "" || ucmd == "Y" || ucmd == "y")
			put(paths[i]);
	}

	return true;
}


bool Client::del(const string path)
{
	transferCMD("DELE", path);

	return true;
}

bool Client::mdel(const string pathRAW)
{
	string ucmd;
	vector<string>paths;
	splitPaths(pathRAW, paths);

	//transferCMD("TYPE", "I");

	for (int i = 0; i < paths.size(); i++)
	{
		printf("mdelete %s? ", paths[i].c_str());
		rewind(stdin);
		getline(cin, ucmd);

		if (ucmd == "" || ucmd == "Y" || ucmd == "y")
			del(paths[i]);
	}

	return true;
}


bool Client::pwd()
{
	transferCMD("PWD", "");

	return true;
}


bool Client::cd(const string path)
{
	transferCMD("CWD", path);

	return true;
}


//REF: https://msdn.microsoft.com/en-us/library/windows/desktop/aa363806%28v=vs.85%29.aspx
bool Client::lcd(const string path)
{
	DWORD dwRet;
	TCHAR buf[1024];
	int error;

	if (path != "")
	{
		TCHAR *param = new TCHAR[path.size() + 1];
		param[path.size()] = 0;
		copy(path.begin(), path.end(), param);

		if (!SetCurrentDirectory(param))
		{
			error = GetLastError();

			if (error == 2)
				cout << path.substr(path.find(":") + 1) << ": File not found\n";
			else if (error == 3)
				cout << "> " << path.substr(0, 2) << "No Such file or directory\n";
			//printf("SetCurrentDirectory failed (%d)\n", GetLastError());
			return false;
		}
	}

	dwRet = GetCurrentDirectory(BUFSIZ, buf);

	if (dwRet == 0)
	{
		printf("GetCurrentDirectory failed (%d)\n", GetLastError());
		return false;
	}
	if (dwRet > BUFSIZ)
	{
		printf("Buffer too small; need %d characters\n", dwRet);
		return false;
	}

	_tprintf(TEXT("Local directory now %s.\n"), buf);

	return true;
}


bool Client::mkdir(const string path)
{
	transferCMD("MKD", path);

	return true;
}


bool Client::rmdir(const string path)
{
	transferCMD("RMD", path);

	return true;
}


bool Client::quitexit()
{
	transferCMD("QUIT", "");

	return false;
}


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
	cout << "ls\t\tdir\n";
	cout << "put\t\tget\t\tmput\t\tmget\n";
	cout << "delete\t\tmdelete\t\tmkdir\t\trmdir\n";
	cout << "pwd\t\tcd\t\tlcd\n";
	cout << "quit\t\tclose\t\t\exit\n";
	cout << "?\t\thelp\t\tquote\n";
}
