#pragma once
#include "stdafx.h"

#define ACTIVE 0
#define PASSIVE 1

class Client
{
	CSocket ClientSocket;
	wstring hostIPaddr;
	unsigned int clientPort;
	unsigned int clientDataPort;
	bool mode;
public:
	Client();
	~Client();

	void play();
	bool command(const string cmd, const string path);
	bool transferCMD(const string cmd, const string infor);
	bool connect(wstring host, unsigned int port);
	bool login(string host);
	void changeMode(const string path);
	void transferInit(CSocket & dsocket, CSocket & connector);
	bool afterTransfer(const int size, const double time_used);
	bool lsdir(const string cmd, const string path);
	bool get(const string path);
	bool put(const string path);
	bool mget(const string pathRAW);
	bool mput(const string pathRAW);
	bool del(const string path);
	bool mdel(const string pathRAW);
	bool pwd();
	bool cd(const string path);
	bool lcd(const string path);
	bool mkdir(const string path);
	bool rmdir(const string path);
	bool disconnect();
	bool quit();
};
