#pragma once
#include "stdafx.h"

#define A "open"
#define B "ls"
#define C "dir"

class Client
{
	CSocket ClientSocket;
	wstring hostIPaddr;
	unsigned int clientPort;
	unsigned int clientDataPort;
public:
	Client();
	~Client();

	void play();
	void command(const string cmd, const string path);
	bool connect(wstring host, unsigned int port);
	bool login(string host);
	void transferInit(CSocket &dsocket, const string path);
	bool lsdir(const string cmd, const string path);
	bool get(const string path);
	bool put(const string path);
	bool mget(const string pathRAW);
};

void replylogcode(int code);
string getFileName(string path);