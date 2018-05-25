#include "stdafx.h"
#include "Client.h"


Client::Client()
{
	ClientSocket.Create();
	srand(time(NULL));
	clientDataPort = rand() % 40000 + 1024;
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
		cout << "ftp> ";
		getline(cin, input);

		if (input == "quit")
			break;
		else
		{
			cmd = input.substr(0, input.find(" "));
			path = input.substr(input.find(" ") + 1);
			if (cmd.length() == input.length())
				path = "";
			command(cmd, path);
		}
	}
}


void Client::command(const string cmd, const string path)
{
	if (cmd == "open")
		login(path); //host
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
	else
		cout << "Invalid command.\n";
}


bool Client::connect(wstring host, unsigned int port)
{
	LPCTSTR temp = host.c_str();

	if (ClientSocket.Connect(temp, port) != 0)
	{
		cout << "Connected to ";
		wcout << temp << endl;
		return true;
	}

	return false;
}


bool Client::login(string host)
{
	wstring w_host(host.begin(), host.end());
	hostIPaddr = w_host;
	connect(hostIPaddr, 21);

	char buf[BUFSIZ + 1];
	int tmpres, size, status;
	/*
	Connection Establishment
	120
	220
	220
	421
	Login
	USER
	230
	530
	500, 501, 421
	331, 332
	PASS
	230
	202
	530
	500, 501, 503, 421
	332
	*/
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

	sprintf(buf, "USER %s\r\n", info);
	tmpres = ClientSocket.Send(buf, strlen(buf), 0);

	memset(buf, 0, sizeof buf);
	tmpres = ClientSocket.Receive(buf, BUFSIZ, 0);

	sscanf(buf, "%d", &codeftp);
	if (codeftp != 331)
	{
		replylogcode(codeftp);
		exit(1);
	}
	printf("%s", buf);

	//Send Password
	memset(info, 0, sizeof info);
	printf("Password: ");
	memset(buf, 0, sizeof buf);
	scanf("%s", info);

	sprintf(buf, "PASS %s\r\n", info);
	tmpres = ClientSocket.Send(buf, strlen(buf), 0);

	memset(buf, 0, sizeof buf);
	tmpres = ClientSocket.Receive(buf, BUFSIZ, 0);

	sscanf(buf, "%d", &codeftp);
	if (codeftp != 230)
	{
		replylogcode(codeftp);
		exit(1);
	}
	printf("%s", buf);

	return true;
}


void Client::transferInit(CSocket &dsocket, const string path)
{
	int tmpres;
	int codeftp;
	char buf[BUFSIZ + 1];

	CString sockIP; //Type of host IP is CString
	//unsigned int clientDataPort;

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
	//sscanf(buf, "%d", &codeftp);
	printf("%s", buf);
	//memset(buf, 0, sizeof buf);

	/*if (cmd == "ls")
	{
		if (path == "")
			sprintf(buf, "NLST \r\n");
		else
			sprintf(buf, "NLST %s \r\n", path.c_str());
	}
	else if (cmd == "dir")
	{
		if (path == "")
			sprintf(buf, "LIST \r\n");
		else
			sprintf(buf, "LIST %s \r\n", path.c_str());
	}
	else if (cmd == "get")
		sprintf(buf, "RETR %s \r\n", path.c_str());
	else if (cmd == "put")
	{
		string filename = getFileName(path);
		sprintf(buf, "STOR %s \r\n", filename.c_str());
	}

	tmpres = ClientSocket.Send(buf, strlen(buf), 0);

	memset(buf, 0, sizeof buf);
	tmpres = ClientSocket.Receive(buf, BUFSIZ, 0);
	sscanf(buf, "%d", &codeftp);
	printf("%s", buf);

	dsocket.Accept(connector);*/
}


bool Client::lsdir(const string cmd, const string path)
{
	int tmpres;
	int codeftp;
	CSocket dsocket, connector;
	char buf[BUFSIZ + 1];

	transferInit(dsocket, path);

	memset(buf, 0, sizeof buf);
	if (cmd == "ls")
	{
		if (path == "")
			sprintf(buf, "NLST \r\n");
		else
			sprintf(buf, "NLST %s \r\n", path.c_str());
	}
	else if (cmd == "dir")
	{
		if (path == "")
			sprintf(buf, "LIST \r\n");
		else
			sprintf(buf, "LIST %s \r\n", path.c_str());
	}

	tmpres = ClientSocket.Send(buf, strlen(buf), 0);

	memset(buf, 0, tmpres);
	tmpres = ClientSocket.Receive(buf, BUFSIZ, 0);
	sscanf(buf, "%d", &codeftp);
	printf("%s", buf);

	dsocket.Accept(connector);

	memset(buf, 0, tmpres);
	while ((tmpres = connector.Receive(buf, BUFSIZ, 0)) > 0)
	{
		sscanf(buf, "%d", &codeftp);
		printf("%s", buf);
		memset(buf, 0, tmpres);
	}

	connector.Close();
	dsocket.Close();

	return true;
}


bool Client::get(const string path)
{
	int tmpres;
	int codeftp;
	CSocket dsocket, connector;
	char buf[BUFSIZ];

	transferInit(dsocket, path);

	memset(buf, 0, sizeof buf);
	sprintf(buf, "RETR %s \r\n", path.c_str());
	tmpres = ClientSocket.Send(buf, strlen(buf), 0);

	memset(buf, 0, sizeof buf);
	tmpres = ClientSocket.Receive(buf, BUFSIZ, 0);
	sscanf(buf, "%d", &codeftp);
	printf("%s", buf);

	dsocket.Accept(connector);

	//Create file to write into
	string filename = getFileName(path);
	filename = "D:/" + filename;
	ofstream downfile(filename, ios::binary);

	memset(buf, 0, sizeof buf);
	while ((tmpres = connector.Receive(buf, BUFSIZ, 0)) > 0)
	{
		//sscanf(buf, "%d", &codeftp);
		downfile.write(buf, sizeof buf);
		memset(buf, 0, tmpres);
	}

	downfile.close();
	connector.Close();
	dsocket.Close();

	memset(buf, 0, sizeof buf);
	tmpres = ClientSocket.Receive(buf, BUFSIZ, 0);
	//sscanf(buf, "%d", &codeftp);
	printf("%s", buf);

	return true;
}


bool Client::put(const string path)
{
	int tmpres;
	int codeftp;
	int count;
	CSocket dsocket, connector;
	char buf[BUFSIZ];

	transferInit(dsocket, path);

	string filename = getFileName(path);
	cout << filename << endl;
	memset(buf, 0, sizeof buf);
	sprintf(buf, "STOR %s \r\n", filename.c_str());
	tmpres = ClientSocket.Send(buf, strlen(buf), 0);

	memset(buf, 0, sizeof buf);
	tmpres = ClientSocket.Receive(buf, BUFSIZ, 0);
	sscanf(buf, "%d", &codeftp);
	printf("%s", buf);

	dsocket.Accept(connector);

	//Create file to read
	ifstream upfile(path, ios::binary);

	upfile.seekg(ios::beg);
	do
	{
		memset(buf, 0, sizeof buf);
		upfile.read(buf, BUFSIZ);
		count = upfile.gcount();
		tmpres = connector.Send(buf, count, 0);
		//sscanf(buf, "%d", &codeftp);
	} while (!upfile.eof());

	upfile.close();
	connector.Close();
	dsocket.Close();

	memset(buf, 0, sizeof buf);
	tmpres = ClientSocket.Receive(buf, BUFSIZ, 0);
	//sscanf(buf, "%d", &codeftp);
	printf("%s", buf);

	return true;
}


bool Client::mget(const string pathRAW)
{
	istringstream iss(pathRAW);
	vector<string>paths((istream_iterator<string>(iss)), istream_iterator<string>());
	//REF: https://www.fluentcpp.com/2017/04/21/how-to-split-a-string-in-c/

	for (int i = 0; i < paths.size(); i++)
	{
		cout << paths[i];
	}

	for (int i = 0; i < paths.size(); i++)
	{
		get(paths[i]);
	}

	return true;
}


void replylogcode(int code)
{
	switch (code) {
	case 200:
		printf("Command okay.");
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
	case 530:
		printf("Permission denied\nLogin failed.");
		break;
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
