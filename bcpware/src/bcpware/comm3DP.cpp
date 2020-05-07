/****************************************************************************
* BCPware
*
*  Copyright 2018 by
*
* This program is free software: you can redistribute
* it and/or modify it under the terms of the GNU General Public
* License as published by the Free Software Foundation, either
* version 3 of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will
* be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
* @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
****************************************************************************/
#include "comm3DP.h"
#include <ShellAPI.h>
#include <cstdio>
#include <cstring>
#include <memory>
#include <algorithm>
#include <ctime>
#include <tchar.h>

#include "skt_function.h"

#ifndef UNICODE
#define UNICODE
#endif

#ifndef _UNICODE
#define _UNICODE
#endif


#ifdef _DEBUG
#define DEBUG_COMM3DP
#endif

#ifdef _DEBUG
#define DEBUG_COMM3DP
#endif
///////////////////////////////////////////////////////////////////////////
#ifdef DEBUG_COMM3DP
//Some customize setting
#define LOG_STDOUT 1
#define LOGNAME "comm3dp.log"
#endif
///////////////////////////////////////////////////////////////////////////
#ifdef DEBUG_COMM3DP
static FILE *_log_fptr = fopen(LOGNAME, "wb");
static int _log_funclv = 0;
#endif
///////////////////////////////////////////////////////////////////////////
#ifdef DEBUG_COMM3DP

#if LOG_STDOUT

#define ENTERFUNC_STDOUT \
	fprintf(stdout, ">>>% 2d>>> Enter: %s  Clock: %d\n", _log_funclv, __FUNCTION__, clock()); \
	fflush(stdout);
#define LEAVEFUNC_STDOUT \
	fprintf(stdout, "<<<% 2d<<< Leave: %s  Clock: %d\n", _log_funclv, __FUNCTION__, clock()); \
	fflush(stdout);
#define PRINTLN_STDOUT(printstr, va_args) \
	fprintf(stdout, printstr, _log_funclv); \
	fprintf(stdout, va_args); \
	fprintf(stdout, " (%s:%d)\n", __FILE__, __LINE__); \
	fflush(stdout);
#else
#define ENTERFUNC_STDOUT
#define LEAVEFUNC_STDOUT
#define PRINTLN_STDOUT(printstr, va_args)
#endif

#endif
////////////////////////////////////////////////////////////////////////////
#ifdef DEBUG_COMM3DP
#define ENTERFUNC { \
	++_log_funclv; \
	fprintf(_log_fptr, ">>>% 2d>>> Enter: %s  Clock: %d\n", _log_funclv, __FUNCTION__, clock()); \
	fflush(_log_fptr); \
	ENTERFUNC_STDOUT \
}
#define LEAVEFUNC { \
	fprintf(_log_fptr, "<<<% 2d<<< Leave: %s  Clock: %d  (%s:%d)\n", _log_funclv, __FUNCTION__, clock(), __FILE__, __LINE__); \
	fflush(_log_fptr); \
	LEAVEFUNC_STDOUT \
	--_log_funclv; \
	return; \
}
#define LEAVEFUNC2(retval) { \
	fprintf(_log_fptr, "<<<% 2d<<< Leave: %s  Clock: %d  (%s:%d)\n", _log_funclv, __FUNCTION__, clock(), __FILE__, __LINE__); \
	fflush(_log_fptr); \
	LEAVEFUNC_STDOUT \
	--_log_funclv; \
	return (retval); \
}
#define log_println(...) { \
	fprintf(_log_fptr, "   % 2d  | ", _log_funclv); \
	fprintf(_log_fptr, __VA_ARGS__); \
	fprintf(_log_fptr, " (%s:%d)\n", __FILE__, __LINE__); \
	fflush(_log_fptr); \
	PRINTLN_STDOUT("   % 2d  | ", __VA_ARGS__) \
}
#define log_warnln(...) { \
	fprintf(_log_fptr, "!  % 2d  | ", _log_funclv); \
	fprintf(_log_fptr, __VA_ARGS__); \
	fprintf(_log_fptr, " (%s:%d)\n", __FILE__, __LINE__); \
	fflush(_log_fptr); \
	PRINTLN_STDOUT("!  % 2d  | ", __VA_ARGS__) \
}
#define log_criticalln(...) { \
	fprintf(_log_fptr, "x  % 2d  | ", _log_funclv); \
	fprintf(_log_fptr, __VA_ARGS__); \
	fprintf(_log_fptr, " (%s:%d)\n", __FILE__, __LINE__); \
	fflush(_log_fptr); \
	PRINTLN_STDOUT("x  % 2d  | ", __VA_ARGS__) \
}
#else
#define ENTERFUNC
#define LEAVEFUNC  return;
#define LEAVEFUNC2(retval) { return (retval); }
#define log_println(...) {}
#define log_warnln(...) {}
#define log_criticalln(...) {}
#endif
//////////////////////////////////////////////////////////////////////////////////////////

static char cmdbuffer[1024];

std::atomic<int> Comm3DP::nObjCalled = Comm3DP::initNObjCalled();
int Comm3DP::initNObjCalled() { return 0; }
std::atomic<bool> Comm3DP::wsastartupCalled = Comm3DP::initWsastartupCalled();
bool Comm3DP::initWsastartupCalled() { return false; }

bool Comm3DP::initFail() { return _initFail; }

Comm3DP::Comm3DP() : _initFail(false), sockfd(INVALID_SOCKET)
{
	ENTERFUNC

		//check the number of objects created
		if (nObjCalled >= CONNLIMIT)
		{
			_initFail = true;
			LEAVEFUNC;
		}

	//init WSA
	if (!wsastartupCalled)
	{
		WSADATA wsaData;
		int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult != 0) {
			_initFail = true;
			LEAVEFUNC;
		}
		else wsastartupCalled = true;
	}

	//create socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == INVALID_SOCKET) {
		_initFail = true;
		LEAVEFUNC;
	}

	// Set the socket I/O mode: In this case FIONBIO
	// enables or disables the blocking mode for the 
	// socket based on the numerical value of iMode.
	// If iMode = 0, blocking is enabled; 
	// If iMode != 0, non-blocking mode is enabled.
	unsigned long iMode = 0;
	if (ioctlsocket(sockfd, FIONBIO, &iMode) != NO_ERROR)
	{
		_initFail = true;
		LEAVEFUNC;
	}

	//connect socket
	SOCKADDR_IN serv;
	serv.sin_family = AF_INET;
	serv.sin_port = htons(PORT);
	serv.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	if (connect(sockfd, (SOCKADDR*)&serv, sizeof(serv)) != 0)
	{
		log_warnln("connect socket with error: %d.", WSAGetLastError());
		_initFail = true;
		LEAVEFUNC;
	}

	//all success
	++nObjCalled;

	LEAVEFUNC;
}
Comm3DP::~Comm3DP()
{
	ENTERFUNC

		if (nObjCalled == 0 && wsastartupCalled) WSACleanup();
	if (!_initFail)
	{
		closesocket(sockfd);
		--nObjCalled;
	}
	sockfd = INVALID_SOCKET;

	LEAVEFUNC
}

char* Comm3DP::tokenJSON(char *cstr)
{//like strtok()

	static char *sbeg = NULL;
	static char *ster = NULL; //terminate character of c-string

	if (!cstr && (!sbeg && !ster)) return NULL;

	if (cstr)
	{
		sbeg = cstr;
		ster = sbeg + strlen(cstr);
	}

	//ignore unwanted characters
	bool loop = true;
	while (loop)
	{
		if (sbeg == ster)
		{
			sbeg = NULL;
			ster = NULL;
			return NULL;
		}

		switch (*sbeg)
		{
		case '{': case '}': case '[': case ']': case ':': case ',': case ' ':
		case '\t': case '\r': case '\n': case '\0':
			++sbeg;
			break;
		default:
			loop = false;
			break;
		}
	}


	//get the token
	bool isquote = (*sbeg == '"');
	char *preturn = sbeg;
	++sbeg;
	if (isquote)
	{
		while (true)
		{
			if (sbeg == ster)
			{//This is a failed token (Can't find second quote)
				sbeg = NULL;
				ster = NULL;
				return NULL;
			}

			if (*sbeg == '"')
			{
				++sbeg;
				//set the chracter behind second quote to terminate-character without checking
				//because I know in JSON format, there must exist some chracter not needed behind the second quote
				*sbeg = '\0';
				return preturn;
			}
			else
				++sbeg;
		}
	}
	else // !isquote
	{
		while (true)
		{
			if (sbeg == ster)
			{
				sbeg = NULL;
				ster = NULL;
				return NULL;
			}

			switch (*sbeg)
			{
			case '{': case '}': case '[': case ']': case ':': case ',': case ' ':
			case '\t': case '\r': case '\n': case '\0':
				*sbeg = '\0';
				return preturn;
			default:
				++sbeg;
				break;
			}
		}
	}
}

std::string Comm3DP::jsonItemVal(std::string json, const char* itemName)
{
	if (json.empty() || itemName == NULL)
		return std::string();

	char *buffer = new char[json.length() + 1];
	std::unique_ptr<char[]> uptr(buffer);
	strcpy(buffer, json.c_str());
	char *p;
	if ((p = strstr(buffer, itemName)) != NULL)
	{
		p = tokenJSON(p);
		p = tokenJSON(NULL);
		if (p)
			return std::string(p);
	}

	return std::string();
}


bool Comm3DP::catchResponse()
{
	ENTERFUNC

		static int iResult;
	static char *pc;
	recvstr.clear();
	while (true)
	{
		iResult = recv(sockfd, recvbuf, COMM3DP_RECVLEN - 1, 0);
		if (iResult > 0)
		{
			log_println("Bytes received: %d", iResult);
			recvbuf[iResult] = '\0';
			if (pc = strstr(recvbuf, ENDCHAR))
			{
				log_println("a complete response is collected");
				recvstr.append(recvbuf, pc);
				log_println(
					"vvvvvvvvvv Response is catched completely vvvvvvvvvvv\n"
					"%s"
					"\n         ^^^^^^^^^^^^^^^^^^^(DATA END)^^^^^^^^^^^^^^^^^^^^",
					recvstr.c_str()
					);
				LEAVEFUNC2(true);
			}
			else recvstr.append(recvbuf);
		}
		else if (iResult == 0)
		{
			log_criticalln("Connection closed. Should not happen");
			LEAVEFUNC2(false);
		}
		else
		{
			log_criticalln("recv failed with error: %d. Should not happen", WSAGetLastError());
			LEAVEFUNC2(false);
		}
	}
}

void Comm3DP::openProc()
{
	ENTERFUNC

		ShellExecuteW(NULL, L"open", WPROCNAME, L"", NULL, SW_HIDE);

	LEAVEFUNC
}

bool Comm3DP::closeProc()
{
	ENTERFUNC

		if (_initFail){
			LEAVEFUNC2(false)
		}


	static int iResult;
	static char sendbuf[] = "@CloseProc" ENDCHAR;
	iResult = send(sockfd, sendbuf, sizeof(sendbuf) - 1, 0);
	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_OK){
			LEAVEFUNC2(true);
		}
		else if (recvstr == FEEDBACK_ERR) {
			LEAVEFUNC2(false);
		}
		else
		{
			log_println("catch an unknown response in closeProc()");
			LEAVEFUNC2(false);
		}
	}
	else LEAVEFUNC2(false);
}

bool Comm3DP::printerStatus(std::string &status, std::string &error)
{
	ENTERFUNC

		if (_initFail) {
			LEAVEFUNC2(false);
		}

	static int pos;
	static int iResult;
	static char sendbuf[] = "@PrinterStatus" ENDCHAR;
	iResult = send(sockfd, sendbuf, sizeof(sendbuf) - 1, 0);
	if (iResult == SOCKET_ERROR) {
		log_println("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		pos = recvstr.find('/');
		status = recvstr.substr(0, pos);
		error = recvstr.substr(pos + 1);
		LEAVEFUNC2(true);
	}
	else
	{
		LEAVEFUNC2(false);
	}
}

bool Comm3DP::siriusError(std::string &s)
{
	ENTERFUNC

		if (_initFail) {
			LEAVEFUNC2(false);
		}

	static int iResult;
	static char sendbuf[] = "@SiriusError" ENDCHAR;
	iResult = send(sockfd, sendbuf, sizeof(sendbuf) - 1, 0);
	if (iResult == SOCKET_ERROR) {
		log_println("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		s = recvstr;
		LEAVEFUNC2(true);
	}
	else {
		LEAVEFUNC2(false);
	}
}



bool Comm3DP::zAxis(float &mmBuilder, float &mmFeeder)
{
	ENTERFUNC

		if (_initFail) {
			LEAVEFUNC2(false);
		}

	static int pos;
	static int iResult;
	static char sendbuf[] = "@ZAxis" ENDCHAR;
	iResult = send(sockfd, sendbuf, sizeof(sendbuf) - 1, 0);
	if (iResult == SOCKET_ERROR) {
		log_println("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			if (2 == sscanf(recvstr.c_str(), "builder%f_feeder%f", &mmBuilder, &mmFeeder))
			{
				LEAVEFUNC2(true);
			}
			LEAVEFUNC2(false);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}
}

//2017.7.13-Timmy: add Door Sensor cmd API for Dashboard
bool Comm3DP::DoorSt(int &i_door)
{
	ENTERFUNC
		if (_initFail) {
			LEAVEFUNC2(false);
		}
	static int iResult;
	static char sendbuf[] = "@DoorSt" ENDCHAR;
	iResult = send(sockfd, sendbuf, sizeof(sendbuf) - 1, 0);
	if (iResult == SOCKET_ERROR) {
		log_println("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			if (recvstr == "Off")
			{
				i_door = 0; //close
				LEAVEFUNC2(true);
			}
			else if (recvstr == "On")
			{
				i_door = 1; //open
				LEAVEFUNC2(true);
			}
			i_door = -1; //unknow
			//.........
			LEAVEFUNC2(false);
		}
	}
	else
	{
		i_door = -1;
		LEAVEFUNC2(false);
	}
}
/*Kenneth_2018_04_25*/
bool Comm3DP::CheckCancel(bool &iscancel)
{
	ENTERFUNC
		if (_initFail) {
			LEAVEFUNC2(false);
		}
	static int iResult;
	static char sendbuf[] = "@CancelCheck" ENDCHAR;
	iResult = send(sockfd, sendbuf, sizeof(sendbuf) - 1, 0);
	if (iResult == SOCKET_ERROR) {
		log_println("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			if (recvstr == "true")
			{
				iscancel = true;
				LEAVEFUNC2(true);
			}
			else if (recvstr == "false")
			{
				iscancel = false;
				LEAVEFUNC2(true);
			}
			iscancel = false;
			LEAVEFUNC2(false);
		}
	}
	else
	{
		iscancel = false;
		LEAVEFUNC2(false);
	}
}

std::string Comm3DP::appVersion()
{
	ENTERFUNC

		std::string emptystr;
	if (_initFail){
		LEAVEFUNC2(emptystr);
	}


	static int iResult;
	static char sendbuf[] = "@AppVersion" ENDCHAR;
	iResult = send(sockfd, sendbuf, sizeof(sendbuf) - 1, 0);
	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(emptystr);
	}

	if (catchResponse())
	{
		LEAVEFUNC2(recvstr);
	}
	else {
		LEAVEFUNC2(emptystr);
	}
}

std::string Comm3DP::firmwareVersion()
{
	ENTERFUNC

		std::string emptystr;
	if (_initFail) {
		LEAVEFUNC2(emptystr);
	}


	static int iResult;
	static char sendbuf[] = "@FirmwareVersion" ENDCHAR;
	iResult = send(sockfd, sendbuf, sizeof(sendbuf) - 1, 0);
	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(emptystr);
	}

	if (catchResponse())
	{
		LEAVEFUNC2(recvstr);
	}
	else
	{
		LEAVEFUNC2(emptystr);
	}
}

bool Comm3DP::wakeUp()
{
	ENTERFUNC

		if (_initFail) {
			LEAVEFUNC2(false);
		}

	static int iResult;
	sprintf(cmdbuffer, "@WakeUp" ENDCHAR);
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);
	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_OK) {
			LEAVEFUNC2(true);
		}
		else if (recvstr == FEEDBACK_ERR) {
			LEAVEFUNC2(false);
		}
		else
		{
			log_println("catch an unknown response.");
			LEAVEFUNC2(false);
		}
	}
	else LEAVEFUNC2(false);
}

bool Comm3DP::sleep()
{
	ENTERFUNC

		if (_initFail) {
			LEAVEFUNC2(false);
		}

	static int iResult;
	sprintf(cmdbuffer, "@Sleep" ENDCHAR);
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);
	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_OK) {
			LEAVEFUNC2(true);
		}
		else if (recvstr == FEEDBACK_ERR) {
			LEAVEFUNC2(false);
		}
		else
		{
			log_println("catch an unknown response.");
			LEAVEFUNC2(false);
		}
	}
	else LEAVEFUNC2(false);
}
bool Comm3DP::poweroff()
{
	ENTERFUNC

		if (_initFail) {
			LEAVEFUNC2(false);
		}

	static int iResult;
	sprintf(cmdbuffer, "@Poweroff" ENDCHAR);
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);
	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_OK) {
			LEAVEFUNC2(true);
		}
		else if (recvstr == FEEDBACK_ERR) {
			LEAVEFUNC2(false);
		}
		else
		{
			log_println("catch an unknown response.");
			LEAVEFUNC2(false);
		}
	}
	else LEAVEFUNC2(false);
}
bool Comm3DP::clearSiriusError()
{
	ENTERFUNC

		if (_initFail) {
			LEAVEFUNC2(false);
		}


	static int iResult;
	//static char sendbuf[128];
	sprintf(cmdbuffer, "@ClearSiriusErr" ENDCHAR);
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);
	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_OK)
		{
			LEAVEFUNC2(true);
		}
		else if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			log_criticalln("catch an unknown response in setPagePerLayerHeight()");
			LEAVEFUNC2(false);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}
}
//Kenneth add2018_04_10 
bool Comm3DP::clearISSError()
{
	ENTERFUNC

		if (_initFail) {
			LEAVEFUNC2(false);
		}


	static int iResult;
	//static char sendbuf[128];
	sprintf(cmdbuffer, "@ClearISSErr" ENDCHAR);
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);
	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_OK)
		{
			LEAVEFUNC2(true);
		}
		else if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			log_criticalln("catch an unknown response in setPagePerLayerHeight()");
			LEAVEFUNC2(false);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}
}

bool Comm3DP::lightPHRecover()
{
	ENTERFUNC

		if (_initFail) {
			LEAVEFUNC2(false);
		}


	static int iResult;
	//static char sendbuf[128];
	sprintf(cmdbuffer, "@LightPHRecovery" ENDCHAR);
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);
	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_OK)
		{
			LEAVEFUNC2(true);
		}
		else if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			log_criticalln("catch an unknown response in setPagePerLayerHeight()");
			LEAVEFUNC2(false);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}
}

bool Comm3DP::heavyPHRecover()
{
	ENTERFUNC

		if (_initFail) {
			LEAVEFUNC2(false);
		}


	static int iResult;
	//static char sendbuf[128];
	sprintf(cmdbuffer, "@HeavyPHRecovery" ENDCHAR);
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);
	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_OK)
		{
			LEAVEFUNC2(true);
		}
		else if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			log_criticalln("catch an unknown response in setPagePerLayerHeight()");
			LEAVEFUNC2(false);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}
}

bool Comm3DP::removePH()
{
	ENTERFUNC

		if (_initFail) {
			LEAVEFUNC2(false);
		}


	static int iResult;
	//static char sendbuf[128];
	sprintf(cmdbuffer, "@RemovePH" ENDCHAR);
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);
	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_OK)
		{
			LEAVEFUNC2(true);
		}
		else if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			log_criticalln("catch an unknown response in setPagePerLayerHeight()");
			LEAVEFUNC2(false);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}
}

bool Comm3DP::installPH()
{
	ENTERFUNC

		if (_initFail) {
			LEAVEFUNC2(false);
		}


	static int iResult;
	//static char sendbuf[128];
	sprintf(cmdbuffer, "@InstallPH" ENDCHAR);
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);
	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_OK)
		{
			LEAVEFUNC2(true);
		}
		else if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			log_criticalln("catch an unknown response in setPagePerLayerHeight()");
			LEAVEFUNC2(false);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}
}

bool Comm3DP::installPHApproval()
{
	ENTERFUNC

		if (_initFail) {
			LEAVEFUNC2(false);
		}


	static int iResult;
	//static char sendbuf[128];
	sprintf(cmdbuffer, "@InstallPHApproval" ENDCHAR);
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);
	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_OK)
		{
			LEAVEFUNC2(true);
		}
		else if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			log_criticalln("catch an unknown response in setPagePerLayerHeight()");
			LEAVEFUNC2(false);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}
}

bool Comm3DP::installPHRegret()
{
	ENTERFUNC

		if (_initFail) {
			LEAVEFUNC2(false);
		}


	static int iResult;
	//static char sendbuf[128];
	sprintf(cmdbuffer, "@InstallPHRegret" ENDCHAR);
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);
	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_OK)
		{
			LEAVEFUNC2(true);
		}
		else if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			log_criticalln("catch an unknown response in setPagePerLayerHeight()");
			LEAVEFUNC2(false);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}
}

bool Comm3DP::printTestPage()
{
	ENTERFUNC

		if (_initFail)
		{
			LEAVEFUNC2(false);
		}


	static int iResult;
	//static char sendbuf[128];
	sprintf(cmdbuffer, "@PrintTestPage" ENDCHAR);
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);
	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_OK)
		{
			LEAVEFUNC2(true);
		}
		else if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			log_criticalln("catch an unknown response in setPagePerLayerHeight()");
			LEAVEFUNC2(false);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}
}

bool Comm3DP::printIPQCPage()
{
	ENTERFUNC

		if (_initFail)
		{
			LEAVEFUNC2(false);
		}


	static int iResult;
	//static char sendbuf[128];
	sprintf(cmdbuffer, "@PrintIPQCPage" ENDCHAR);
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);
	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_OK)
		{
			LEAVEFUNC2(true);
		}
		else if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			log_criticalln("catch an unknown response in setPagePerLayerHeight()");
			LEAVEFUNC2(false);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}
}

bool Comm3DP::sendPrintjobHeader(unsigned nPage, unsigned hLayer, char lors, int Fan)
{
	ENTERFUNC

		//#define DEF_FAN_SPEED 20u  //2017.8.18-Timmy: increase from 40 to 2

		if (_initFail)
		{
			LEAVEFUNC2(false);
		}
	switch (lors)
	{
	case 'S': case 'L': break;
	default: LEAVEFUNC2(false)
	}

	static int iResult;
	//static char sendbuf[128];
	sprintf(cmdbuffer, "@SendHeader %u %u %c %u" ENDCHAR, nPage, hLayer, lors, Fan);
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);
	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_OK)
		{
			LEAVEFUNC2(true);
		}
		else if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			log_criticalln("catch an unknown response in setPagePerLayerHeight()");
			LEAVEFUNC2(false);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}

	//#undef DEF_FAN_SPEED
}

bool Comm3DP::cancelPrintJob()
{
	ENTERFUNC

		if (_initFail) {
			LEAVEFUNC2(false);
		}


	static int iResult;
	//static char sendbuf[128];
	sprintf(cmdbuffer, "@PrintCancel" ENDCHAR);
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);
	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_OK)
		{
			LEAVEFUNC2(true);
		}
		else if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			log_criticalln("catch an unknown response in setPagePerLayerHeight()");
			LEAVEFUNC2(false);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}
}

/*Kenneth Add & Sub*************************/
//bool Comm3DP::sendSiriusOntoFPGA()
//{
//	ENTERFUNC
//
//		if (_initFail) {
//			LEAVEFUNC2(false);
//		}
//
//
//	static int iResult;
//	//static char sendbuf[128];
//	sprintf(cmdbuffer, "@SendSiriusOntoFPGA" ENDCHAR);
//	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);
//	if (iResult == SOCKET_ERROR) {
//		log_warnln("send failed with error: %d", WSAGetLastError());
//		LEAVEFUNC2(false);
//	}
//
//	if (catchResponse())
//	{
//		if (recvstr == FEEDBACK_OK)
//		{
//			LEAVEFUNC2(true);
//		}
//		else if (recvstr == FEEDBACK_ERR)
//		{
//			LEAVEFUNC2(false);
//		}
//		else
//		{
//			log_criticalln("catch an unknown response in setPagePerLayerHeight()");
//			LEAVEFUNC2(false);
//		}
//	}
//	else
//	{
//		LEAVEFUNC2(false);
//	}
//}
/*2017_7_26**********************************/


bool Comm3DP::pausePrintJob()
{
	ENTERFUNC

		if (_initFail) {
			LEAVEFUNC2(false);
		}


	static int iResult;
	//static char sendbuf[128];
	sprintf(cmdbuffer, "@PrintPause" ENDCHAR);
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);
	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_OK)
		{
			LEAVEFUNC2(true);
		}
		else if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			log_criticalln("catch an unknown response in setPagePerLayerHeight()");
			LEAVEFUNC2(false);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}
}

bool Comm3DP::resumePrintJob()
{

	ENTERFUNC

		if (_initFail) {
			LEAVEFUNC2(false);
		}


	static int iResult;
	//static char sendbuf[128];
	sprintf(cmdbuffer, "@PrintResume" ENDCHAR);
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);
	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_OK)
		{
			LEAVEFUNC2(true);
		}
		else if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			log_criticalln("catch an unknown response in setPagePerLayerHeight()");
			LEAVEFUNC2(false);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}
}

/////////////////////////////////////////Kenneth 2018_07_10 OK to INK error************/////////////////////////////////////////
bool Comm3DP::OKtoINKerror()
{
	ENTERFUNC

		if (_initFail) {
			LEAVEFUNC2(false);
		}


	static int iResult;
	//static char sendbuf[128];
	sprintf(cmdbuffer, "@OKtoINKerror" ENDCHAR);
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);
	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_OK)
		{
			LEAVEFUNC2(true);
		}
		else if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			log_criticalln("catch an unknown response in setPagePerLayerHeight()");
			LEAVEFUNC2(false);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}
}

/////////////////////////////////////////Kenneth 2017.1.17******ink chage******/////////////////////////////////////////
bool Comm3DP::inkStatus(unsigned &c, unsigned &m, unsigned &y, unsigned &binder)
{
	ENTERFUNC

		if (_initFail){
			LEAVEFUNC2(false);
		}

	static int iResult;
	sprintf(cmdbuffer, "@InkStatus" ENDCHAR);
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);
	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}


	if (catchResponse())
	{
		if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			char *buffer = new char[recvstr.size() + 1];
			std::unique_ptr<char[]> uptr(buffer);
			strcpy(buffer, recvstr.c_str());

			static const char* names[8] = {
				"\"CyanLevel\"", "\"MagentaLevel\"", "\"YellowLevel\"", "\"BlackLevel\"",
				"\"CyanTankSize\"", "\"MagentaTankSize\"", "\"YellowTankSize\"", "\"BlackTankSize\""
			};
			static int val[8]; //cmyb-level cmyb-total
			std::fill_n(val, 8, -1);
			int i;
			for (char *p = tokenJSON(buffer);
				p && std::any_of(val, val + 8, [](int a){return a < 0; });
				p = tokenJSON(NULL))
			{
				i = 0;
				for (; i < 8; ++i)
				{
					if (0 == strcmp(p, names[i])) break;
				}
				if (i == 8) continue;

				p = tokenJSON(NULL);
				if (!p)
				{
					LEAVEFUNC2(false)
				}
				if (0 == strcmp(p, "\"NA\""))
				{
					LEAVEFUNC2(false)
				}
				val[i] = 0;
				while (*p != '\0')
				{
					if (!isdigit(*p))
					{
						LEAVEFUNC2(false);
					}
					val[i] = val[i] * 10 + (*p - '0');
					++p;
				}

			}

			c = (float(val[0]) / val[4]) * 100;
			m = (float(val[1]) / val[5]) * 100;
			y = (float(val[2]) / val[6]) * 100;
			binder = (float(val[3]) / val[7]) * 100;

			LEAVEFUNC2(true);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}
}


bool Comm3DP::inkStatus2(unsigned &c, unsigned &m, unsigned &y, unsigned &binder)
{
	ENTERFUNC

		if (_initFail){
			LEAVEFUNC2(false);
		}


	static int iResult;
	static char sendbuf[] = "GET /status/ink HTTP/1.1\r\n\r\n" ENDCHAR;
	iResult = send(sockfd, sendbuf, sizeof(sendbuf) - 1, 0);
	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}


	if (catchResponse())
	{
		if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			char *buffer = new char[recvstr.size() + 1];
			std::unique_ptr<char[]> uptr(buffer);
			strcpy(buffer, recvstr.c_str());

			static const char* names[8] = {
				"\"CyanLevel\"", "\"MagentaLevel\"", "\"YellowLevel\"", "\"BlackLevel\"",
				"\"CyanTankSize\"", "\"MagentaTankSize\"", "\"YellowTankSize\"", "\"BlackTankSize\""
			};
			static int val[8]; //cmyb-level cmyb-total
			std::fill_n(val, 8, -1);
			int i;
			for (char *p = tokenJSON(buffer);
				p && std::any_of(val, val + 8, [](int a){return a < 0; });
				p = tokenJSON(NULL))
			{
				i = 0;
				for (; i < 8; ++i)
				{
					if (0 == strcmp(p, names[i])) break;
				}
				if (i == 8) continue;

				p = tokenJSON(NULL);
				if (!p)
				{
					LEAVEFUNC2(false)
				}
				if (0 == strcmp(p, "\"NA\""))
				{
					LEAVEFUNC2(false)
				}
				val[i] = 0;
				while (*p != '\0')
				{
					if (!isdigit(*p))
					{
						LEAVEFUNC2(false);
					}
					val[i] = val[i] * 10 + (*p - '0');
					++p;
				}

			}

			c = (float(val[0]) / val[4]) * 100;
			m = (float(val[1]) / val[5]) * 100;
			y = (float(val[2]) / val[6]) * 100;
			binder = (float(val[3]) / val[7]) * 100;

			LEAVEFUNC2(true);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}
}

bool Comm3DP::inkLevel(unsigned &c, unsigned &m, unsigned &y, unsigned &binder)
{
	ENTERFUNC

		if (_initFail){
			LEAVEFUNC2(false);
		}

	static int iResult;
	sprintf(cmdbuffer, "@InkLevel" ENDCHAR);
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);
	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			char *buffer = new char[recvstr.size() + 1];
			std::unique_ptr<char[]> uptr(buffer);
			strcpy(buffer, recvstr.c_str());

			static const char* names[8] = {
				"\"CyanLevel\"", "\"MagentaLevel\"", "\"YellowLevel\"", "\"BlackLevel\"",
				"\"CyanTankSize\"", "\"MagentaTankSize\"", "\"YellowTankSize\"", "\"BlackTankSize\""
			};
			static int val[8]; //cmyb-level cmyb-total
			std::fill_n(val, 8, -1);
			int i;
			for (char *p = tokenJSON(buffer);
				p && std::any_of(val, val + 8, [](int a){return a < 0; });
				p = tokenJSON(NULL))
			{
				i = 0;
				for (; i < 8; ++i)
				{
					if (0 == strcmp(p, names[i])) break;
				}
				if (i == 8) continue;

				p = tokenJSON(NULL);
				if (!p)
				{
					LEAVEFUNC2(false)
				}
				if (0 == strcmp(p, "\"NA\""))
				{
					LEAVEFUNC2(false)
				}
				val[i] = 0;
				while (*p != '\0')
				{
					if (!isdigit(*p))
					{
						LEAVEFUNC2(false);
					}
					val[i] = val[i] * 10 + (*p - '0');
					++p;
				}

			}

			c = val[0];
			m = val[1];
			y = val[2];
			binder = val[3];

			LEAVEFUNC2(true);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}
}


/////////////////////////////////////////Kenneth 2017.1.17******CompletedPages chage******/////////////////////////////////////////
int Comm3DP::jobCompletedPages()
{
	ENTERFUNC

		if (_initFail)
		{
			LEAVEFUNC2(-1);
		}

	static int iResult;
	sprintf(cmdbuffer, "@JobCompletedPages" ENDCHAR);
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);

	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(-1);
	}

	static int tmpi;
	static std::string tmpstr;
	if (catchResponse())
	{
		if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(-1);
		}
		else
		{
			tmpstr = jsonItemVal(recvstr, "\"Printed\"");
			try{ tmpi = std::stoi(tmpstr); }
			catch (std::invalid_argument &e) {
				log_criticalln("catch an unknown response in jobCompletedPages()");
				LEAVEFUNC2(-1);
			}
			catch (std::out_of_range &e) {
				log_criticalln("catch a too large number in jobCompletedPages()");
				LEAVEFUNC2(-1);
			}
			LEAVEFUNC2(tmpi);
		}
	}
	else
	{
		LEAVEFUNC2(-1);
	}

}

int Comm3DP::jobCompletedPages2()
{
	ENTERFUNC

		if (_initFail)
		{
			LEAVEFUNC2(-1);
		}

	static int iResult;
	static char sendbuf[] = "GET /job/pages HTTP/1.1\r\n\r\n" ENDCHAR;
	iResult = send(sockfd, sendbuf, sizeof(sendbuf) - 1, 0);
	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(-1);
	}

	static int tmpi;
	static std::string tmpstr;
	if (catchResponse())
	{
		if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(-1);
		}
		else
		{
			tmpstr = jsonItemVal(recvstr, "\"Printed\"");
			try{ tmpi = std::stoi(tmpstr); }
			catch (std::invalid_argument &e) {
				log_criticalln("catch an unknown response in jobCompletedPages()");
				LEAVEFUNC2(-1);
			}
			catch (std::out_of_range &e) {
				log_criticalln("catch a too large number in jobCompletedPages()");
				LEAVEFUNC2(-1);
			}
			LEAVEFUNC2(tmpi);
		}
	}
	else
	{
		LEAVEFUNC2(-1);
	}

}

int Comm3DP::getNowPrintedpage()
{
	ENTERFUNC

		if (_initFail)
		{
			LEAVEFUNC2(-1);
		}
	static int iResult;
	sprintf(cmdbuffer, "@getNowPrintedpage" ENDCHAR);
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);

	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(-1);
	}

	static int tmpi;
	static std::string tmpstr;
	if (catchResponse())
	{
		if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(-1);
		}
		else
		{
			tmpstr = recvstr;
			try{ tmpi = std::stoi(tmpstr); }
			catch (std::invalid_argument &e) {
				log_criticalln("catch an unknown response in jobCompletedPages()");
				LEAVEFUNC2(-1);
			}
			catch (std::out_of_range &e) {
				log_criticalln("catch a too large number in jobCompletedPages()");
				LEAVEFUNC2(-1);
			}
			LEAVEFUNC2(tmpi);
		}
	}
	else
	{
		LEAVEFUNC2(-1);
	}
}
/////////////////////////////////////////Kenneth 2017.1.17******sirius status system******/////////////////////////////////////////
bool Comm3DP::systemStatus(std::string &result)
{
	if (_initFail)
	{
		LEAVEFUNC2(false);
	}


	static int iResult;
	sprintf(cmdbuffer, "@SiriusSystemStatas" ENDCHAR);
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);

	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	static int tmpi;
	if (catchResponse())
	{
		if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			result = recvstr;
			LEAVEFUNC2(true);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}

}

bool Comm3DP::sendSCICommand(std::string cmd, std::string &result)
{
	ENTERFUNC

		if (_initFail)
		{
			LEAVEFUNC2(false);
		}
	if (cmd.empty())
	{
		LEAVEFUNC2(false);
	}

	static int iResult;
	cmd += ENDCHAR;
	iResult = send(sockfd, cmd.c_str(), cmd.length(), 0);
	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	static int tmpi;
	if (catchResponse())
	{
		if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			result = recvstr;
			LEAVEFUNC2(true);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}
}

bool Comm3DP::getHeader(
	int &year, int &month, int &mday, int &hr, int &min, int &sec,
	int &page, int &layer, char &format, int &fan)
{
	ENTERFUNC

		if (_initFail)
		{
			LEAVEFUNC2(false);
		}

	static int iResult;
	//static char sendbuf[128];
	sprintf(cmdbuffer, "@RegisteredHeader" ENDCHAR);
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);
	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			if (10 != sscanf(recvstr.c_str(),
				"year%d_month%d_mday%d_hr%d_min%d_sec%d_page%d_layer%d_format%c_fan%d",
				&year, &month, &mday, &hr, &min, &sec,
				&page, &layer, &format, &fan))
			{
				log_criticalln("parse header info failed");
				LEAVEFUNC2(false);
			}
			LEAVEFUNC2(true);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}

}

#ifdef COMM3DP_FORTESTING
bool Comm3DP::rawSend(std::string s)
{
	ENTERFUNC

		static int iResult;
	s += ENDCHAR;
	iResult = send(sockfd, s.c_str(), s.length(), 0);
	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	LEAVEFUNC2(true);
}
#endif

//2017.8.2-Timmy: send "PC On" to FPGA for speical case
bool Comm3DP::SendSiriusOntoFPGA()
{
	ENTERFUNC

		if (_initFail) {
			LEAVEFUNC2(false);
		}

	static int iResult;
	sprintf(cmdbuffer, "@SendSiriusOntoFPGA" ENDCHAR); //new
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);

	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_OK)
		{
			LEAVEFUNC2(true);
		}
		else if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			log_criticalln("catch an unknown response...");
			LEAVEFUNC2(false);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}

}

//2017.7.31-Timmy: send Sirius Shutdown for speical case
bool Comm3DP::SiriusShutDown()
{
	ENTERFUNC

		if (_initFail) {
			LEAVEFUNC2(false);
		}

	static int iResult;
	sprintf(cmdbuffer, "@SiriusShutDown" ENDCHAR); //new
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);

	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_OK)
		{
			LEAVEFUNC2(true);
		}
		else if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			log_criticalln("catch an unknown response...");
			LEAVEFUNC2(false);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}
}

//2017.8.4-Timmy: add for fpga error
bool Comm3DP::fpgaError(std::string &s)
{
	ENTERFUNC

		if (_initFail) {
			LEAVEFUNC2(false);
		}

	static int iResult;
	static char sendbuf[] = "@fpgaError" ENDCHAR;
	iResult = send(sockfd, sendbuf, sizeof(sendbuf) - 1, 0);
	if (iResult == SOCKET_ERROR) {
		log_println("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		s = recvstr;
		LEAVEFUNC2(true);
	}
	else {
		LEAVEFUNC2(false);
	}
}

//2017.8.4-Timmy:
bool Comm3DP::getServiceProgress(std::string &s)
{
	ENTERFUNC

		if (_initFail) {
			LEAVEFUNC2(false);
		}

	static int iResult;
	static char sendbuf[] = "@ServiceProgress" ENDCHAR;
	iResult = send(sockfd, sendbuf, sizeof(sendbuf) - 1, 0);
	if (iResult == SOCKET_ERROR) {
		log_println("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		s = recvstr;
		LEAVEFUNC2(true);
	}
	else {
		LEAVEFUNC2(false);
	}
}


bool Comm3DP::ReleaseLatch() //2017.8.24-Timmy add for error handle
{
	ENTERFUNC

		if (_initFail) {
			LEAVEFUNC2(false);
		}

	static int iResult;
	sprintf(cmdbuffer, "@ReleaseLatch" ENDCHAR); //new
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);

	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_OK)
		{
			LEAVEFUNC2(true);
		}
		else if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			log_criticalln("catch an unknown response...");
			LEAVEFUNC2(false);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}
}

bool Comm3DP::SetPrimeStatusOnly() //2017.8.24-Timmy add for error handle
{
	ENTERFUNC

		if (_initFail) {
			LEAVEFUNC2(false);
		}

	static int iResult;
	sprintf(cmdbuffer, "@SetPrimeStatusOnly" ENDCHAR); //new
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);

	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_OK)
		{
			LEAVEFUNC2(true);
		}
		else if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			log_criticalln("catch an unknown response...");
			LEAVEFUNC2(false);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}
}

//2017.9.7: set mid job page # in array
bool Comm3DP::setDynamicMidjob_Page(int ipage, int i_PageInterval)
{
	ENTERFUNC

		if (_initFail) {
			LEAVEFUNC2(false);
		}

	static int iResult;
	sprintf(cmdbuffer, "@setDynamicMidjobPage %u %u" ENDCHAR, ipage, i_PageInterval);
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);

	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_OK)
		{
			LEAVEFUNC2(true);
		}
		else if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			log_criticalln("catch an unknown response...");
			LEAVEFUNC2(false);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}
}

//2017.9.13-Timmy: set page interval
bool Comm3DP::setDynamicMidjob_PageInterval(int i_PageInterval)
{
	ENTERFUNC

		if (_initFail) {
			LEAVEFUNC2(false);
		}

	static int iResult;
	sprintf(cmdbuffer, "@setDynamicMidjobPageInterval %u" ENDCHAR, i_PageInterval);
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);

	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_OK)
		{
			LEAVEFUNC2(true);
		}
		else if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			log_criticalln("catch an unknown response...");
			LEAVEFUNC2(false);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}
}
//2017.9.14-Timmy
bool Comm3DP::setDynamicMidjob_Mode(int i_mode)
{
	/*
	mode 0: fixed mid job, interval = 4 or 8; refer to midjob_middle.txt
	mode 1: dynamic mid job, set interval from bcpware in printing
	mode 2: dynamic mid job, set page # from bcpware, store in Array before printing
	*/

	ENTERFUNC

		if (_initFail) {
			LEAVEFUNC2(false);
		}

	static int iResult;
	sprintf(cmdbuffer, "@setDynamicMidjobMode %u" ENDCHAR, i_mode);
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);

	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_OK)
		{
			LEAVEFUNC2(true);
		}
		else if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			log_criticalln("catch an unknown response...");
			LEAVEFUNC2(false);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}
}

//2019.9.22-Timmy: get page interval from Sirius, provide for BCPware
bool Comm3DP::getDynamicMidjob_PageInterval(int &i_PageInterval)
{
	ENTERFUNC
		if (_initFail) {
			LEAVEFUNC2(false);
		}
	static int iResult;
	static char sendbuf[] = "@getDynamicMidjobPageInterval" ENDCHAR;
	iResult = send(sockfd, sendbuf, sizeof(sendbuf) - 1, 0);
	if (iResult == SOCKET_ERROR) {
		log_println("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}
	i_PageInterval = -1;
	if (catchResponse())
	{
		if (recvstr == FEEDBACK_ERR)
		{
			i_PageInterval = -1;
			LEAVEFUNC2(false);
		}
		else
		{
			//if string content is digit
			//if (isdigit(*recvstr.c_str())) //use C
			if (recvstr.find_first_not_of("0123456789") == std::string::npos) //use C++
			{
				//convert recvstr to value
				i_PageInterval = std::stoi(recvstr);
				//i_PageInterval = 10;
				LEAVEFUNC2(true);
			}
			else
			{
				i_PageInterval = -1; //error
				LEAVEFUNC2(false);
			}
		}
	}
	else
	{
		i_PageInterval = -1;
		LEAVEFUNC2(false);
	}
}

//2017.9.26 Kenneth~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool Comm3DP::DoorEN()
{
	ENTERFUNC

		if (_initFail) {
			LEAVEFUNC2(false);
		}

	static int iResult;
	sprintf(cmdbuffer, "@DoorEN" ENDCHAR); //new
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);

	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_OK)
		{
			LEAVEFUNC2(true);
		}
		else if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			log_criticalln("catch an unknown response...");
			LEAVEFUNC2(false);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}
}

bool Comm3DP::DoorDIS()
{
	ENTERFUNC

		if (_initFail) {
			LEAVEFUNC2(false);
		}

	static int iResult;
	sprintf(cmdbuffer, "@DoorDIS" ENDCHAR); //new
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);

	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_OK)
		{
			LEAVEFUNC2(true);
		}
		else if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			log_criticalln("catch an unknown response...");
			LEAVEFUNC2(false);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}
}

//2017.10.30 Kenneth~send FPGA command
bool Comm3DP::sendFPGAcommand(std::string cmd)
{
	ENTERFUNC

		if (_initFail) {
			LEAVEFUNC2(false);
		}

	static int iResult;
	//cmd += ENDCHAR;  //Notice: it caused in double end char in next cmd!
	sprintf(cmdbuffer, "@SendFPGAcommand %s" ENDCHAR, cmd.c_str());
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);

	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_OK)
		{
			LEAVEFUNC2(true);
		}
		else if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			log_criticalln("command is not send...");
			LEAVEFUNC2(false);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}
}

bool Comm3DP::getWiperSN(std::string &sn)
{
	ENTERFUNC
		if (_initFail) {
			LEAVEFUNC2(false);
		}
	static int iResult;
	static char sendbuf[] = "@getWiperSN" ENDCHAR;
	sn = ""; //default
	iResult = send(sockfd, sendbuf, sizeof(sendbuf) - 1, 0);
	if (iResult == SOCKET_ERROR) {
		log_println("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_ERR)
		{
			sn = "????";
			LEAVEFUNC2(false);
		}
		else
		{
			sn = recvstr;
			LEAVEFUNC2(true);
		}
	}
	else
	{
		sn = "????";
		LEAVEFUNC2(false);
	}
}

//2017.11.22-Timmy
bool Comm3DP::getWiperCount(int &iWiper)
{
	ENTERFUNC
		if (_initFail) {
			LEAVEFUNC2(false);
		}
	static int iResult;
	static char sendbuf[] = "@getWiperCount" ENDCHAR;
	iWiper = -1; //2017.11.27-Timmy: set -1 for fail case!
	iResult = send(sockfd, sendbuf, sizeof(sendbuf) - 1, 0);
	if (iResult == SOCKET_ERROR) {
		log_println("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_ERR)
		{
			//iWiper = -1;
			LEAVEFUNC2(false);
		}
		else
		{
			//if string content is digit
			//if (isdigit(*recvstr.c_str())) //use C
			if (recvstr.find_first_not_of("-0123456789") == std::string::npos) //use C++
			{
				//convert recvstr to value
				iWiper = std::stoi(recvstr);
				LEAVEFUNC2(true);
			}
			else
			{
				//iWiper = -1; //error
				LEAVEFUNC2(false);
			}
		}
	}
	else
	{
		//iWiper = -1;
		LEAVEFUNC2(false);
	}
}

//2017.11.22-Timmy
bool Comm3DP::setWiperCount(int iWiper)
{
	ENTERFUNC

		if (_initFail) {
			LEAVEFUNC2(false);
		}

	static int iResult;
	sprintf(cmdbuffer, "@setWiperCount %u" ENDCHAR, iWiper);
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);

	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_OK)
		{
			LEAVEFUNC2(true);
		}
		else if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			log_criticalln("catch an unknown response...");
			LEAVEFUNC2(false);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}

}


bool Comm3DP::setWiperOK()
{
	ENTERFUNC

		if (_initFail) {
			LEAVEFUNC2(false);
		}

	static int iResult;
	sprintf(cmdbuffer, "@WiperSetOk" ENDCHAR);
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);

	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_OK)
		{
			LEAVEFUNC2(true);
		}
		else if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			log_criticalln("catch an unknown response...");
			LEAVEFUNC2(false);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}
}


//2017.11.22-Timmy
bool Comm3DP::setPrintLeftTime(int time_min)
{
	ENTERFUNC

		if (_initFail) {
			LEAVEFUNC2(false);
		}

	static int iResult;
	sprintf(cmdbuffer, "@setPrintLeftTime %u" ENDCHAR, time_min);
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);

	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_OK)
		{
			LEAVEFUNC2(true);
		}
		else if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			log_criticalln("catch an unknown response...");
			LEAVEFUNC2(false);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}

}

//2017.11.27
bool Comm3DP::setWiperSN(std::string sn)
{
	ENTERFUNC

		if (_initFail) {
			LEAVEFUNC2(false);
		}

	static int iResult;
	sprintf(cmdbuffer, "@setWiperSN %s" ENDCHAR, sn.c_str());
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);

	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_OK)
		{
			LEAVEFUNC2(true);
		}
		else if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			log_criticalln("command is not send...");
			LEAVEFUNC2(false);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}
}



//2017.11.22-Timmy
bool Comm3DP::setPrintfilename(std::string filename)
{
	ENTERFUNC

		if (_initFail) {
			LEAVEFUNC2(false);
		}

	static int iResult;
	sprintf(cmdbuffer, "@setPrintfilename %s" ENDCHAR, filename.c_str());
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);

	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_OK)
		{
			LEAVEFUNC2(true);
		}
		else if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			log_criticalln("command is not send...");
			LEAVEFUNC2(false);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}
}



//2017.11.22-Timmy
bool Comm3DP::CheckShutdown()
{
	int iflag;
	ENTERFUNC
		if (_initFail) {
			LEAVEFUNC2(false);
		}
	static int iResult;
	static char sendbuf[] = "@CheckShutdown" ENDCHAR;
	iflag = -1;
	iResult = send(sockfd, sendbuf, sizeof(sendbuf) - 1, 0);
	if (iResult == SOCKET_ERROR) {
		log_println("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			//if string content is digit
			//if (isdigit(*recvstr.c_str())) //use C
			if (recvstr.find_first_not_of("-0123456789") == std::string::npos) //use C++
			{
				//convert recvstr to value
				iflag = std::stoi(recvstr);
				if (iflag == 1) //show Warning
				{
					LEAVEFUNC2(true);
				}
				LEAVEFUNC2(false);
			}
			else
			{
				LEAVEFUNC2(false);
			}
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}
}

/////////////////////////////////////////Kenneth_2018_6_21******Binder used level******/////////////////////////////////////////@getBinderLevel
bool Comm3DP::getBinderINKLevel(std::string &result)
{
	ENTERFUNC
		if (_initFail)
		{
			LEAVEFUNC2(false);
		}


	static int iResult;
	sprintf(cmdbuffer, "@getBinderLevel" ENDCHAR);
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);

	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	static int tmpi;
	if (catchResponse())
	{
		if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			result = recvstr;
			LEAVEFUNC2(true);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}

}

/////////////////////////////////////////Kenneth 2017.1.31******Estimated Time to FPGA******/////////////////////////////////////////
bool Comm3DP::ETtofpga(std::string ETtime)
{
	ENTERFUNC

		if (_initFail) {
			LEAVEFUNC2(false);
		}

	static int iResult;
	//cmd += ENDCHAR;  //Notice: it caused in double end char in next cmd!
	sprintf(cmdbuffer, "@ETtoFPGA %s" ENDCHAR, ETtime.c_str());
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);

	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_OK)
		{
			LEAVEFUNC2(true);
		}
		else if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			log_criticalln("command is not send...");
			LEAVEFUNC2(false);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}


}

/////////////////////////////////////////Kenneth 2018_07_12******Color Mixed Binder******/////////////////////////////////////////
bool Comm3DP::BinderRecovery()
{
	ENTERFUNC

		if (_initFail) {
			LEAVEFUNC2(false);
		}


	static int iResult;
	//static char sendbuf[128];
	sprintf(cmdbuffer, "@BinderRecovery" ENDCHAR);
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);
	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_OK)
		{
			LEAVEFUNC2(true);
		}
		else if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			log_criticalln("catch an unknown response in setPagePerLayerHeight()");
			LEAVEFUNC2(false);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}
}

/////////////////////////////////////////Kenneth 2018_08_13******Heated******/////////////////////////////////////////
bool Comm3DP::PreHeatedTimes(std::string PreheatedTimes)
{
	ENTERFUNC

		if (_initFail) {
			LEAVEFUNC2(false);
		}

	static int iResult;
	//cmd += ENDCHAR;  //Notice: it caused in double end char in next cmd!
	sprintf(cmdbuffer, "@PreHeated %s" ENDCHAR, PreheatedTimes.c_str());
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);

	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_OK)
		{
			LEAVEFUNC2(true);
		}
		else if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			log_criticalln("command is not send...");
			LEAVEFUNC2(false);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}


}

bool Comm3DP::PreAndPrintingHeated(unsigned prenumTimes, unsigned preTemp, unsigned printHTemp, unsigned printLTemp)
{
	ENTERFUNC

		if (_initFail)
		{
			LEAVEFUNC2(false);
		}

	static int iResult;
	//static char sendbuf[128];
	sprintf(cmdbuffer, "@PreAndPrintingHeated %u %u %u %u" ENDCHAR, prenumTimes, preTemp, printHTemp, printLTemp);
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);
	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_OK)
		{
			LEAVEFUNC2(true);
		}
		else if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			log_criticalln("catch an unknown response in setPagePerLayerHeight()");
			LEAVEFUNC2(false);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}

	//#undef DEF_FAN_SPEED
}

bool Comm3DP::PostHeated(unsigned postTimes, unsigned postHTemp, unsigned postLTemp)
{
	ENTERFUNC


		if (_initFail)
		{
			LEAVEFUNC2(false);
		}


	static int iResult;
	//static char sendbuf[128];
	sprintf(cmdbuffer, "@PostHeated %u %u %u" ENDCHAR, postTimes, postHTemp, postLTemp);
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);
	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_OK)
		{
			LEAVEFUNC2(true);
		}
		else if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			log_criticalln("catch an unknown response in setPagePerLayerHeight()");
			LEAVEFUNC2(false);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}

	//#undef DEF_FAN_SPEED
}

bool Comm3DP::PrePHin()
{
	ENTERFUNC
		if (_initFail)
		{
			LEAVEFUNC2(false);
		}
	static int iResult;
	//static char sendbuf[128];
	sprintf(cmdbuffer, "@PrePHin" ENDCHAR);
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);
	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_OK)
		{
			LEAVEFUNC2(true);
		}
		else if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			log_criticalln("catch an unknown response in setPagePerLayerHeight()");
			LEAVEFUNC2(false);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}

}

int Comm3DP::GetDryConut()
{
	ENTERFUNC

		if (_initFail)
		{
			LEAVEFUNC2(-1);
		}
	static int iResult;
	sprintf(cmdbuffer, "@getDryConut" ENDCHAR);
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);

	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(-1);
	}

	static int tmpi;
	static std::string tmpstr;
	if (catchResponse())
	{
		if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(-1);
		}
		else
		{
			tmpstr = recvstr;
			try{ tmpi = std::stoi(tmpstr); }
			catch (std::invalid_argument &e) {
				log_criticalln("catch an unknown response in jobCompletedPages()");
				LEAVEFUNC2(-1);
			}
			catch (std::out_of_range &e) {
				log_criticalln("catch a too large number in jobCompletedPages()");
				LEAVEFUNC2(-1);
			}
			LEAVEFUNC2(tmpi);
		}
	}
	else
	{
		LEAVEFUNC2(-1);
	}
}

int Comm3DP::GetTotalPage()
{
	ENTERFUNC

		if (_initFail)
		{
			LEAVEFUNC2(-1);
		}
	static int iResult;
	sprintf(cmdbuffer, "@getTotalPage" ENDCHAR);
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);

	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(-1);
	}

	static int tmpi;
	static std::string tmpstr;
	if (catchResponse())
	{
		if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(-1);
		}
		else
		{
			tmpstr = recvstr;
			try{ tmpi = std::stoi(tmpstr); }
			catch (std::invalid_argument &e) {
				log_criticalln("catch an unknown response in jobCompletedPages()");
				LEAVEFUNC2(-1);
			}
			catch (std::out_of_range &e) {
				log_criticalln("catch a too large number in jobCompletedPages()");
				LEAVEFUNC2(-1);
			}
			LEAVEFUNC2(tmpi);
		}
	}
	else
	{
		LEAVEFUNC2(-1);
	}
}

/////////////////////////////////////////Kenneth 2019_1_14******printmode ->normal mode: 1(1 layer=1 page), best mode: 2(1 layer=2 page)******/////////////////////////////////////////
bool Comm3DP::PrintMode(unsigned printmode)
{
	ENTERFUNC


		if (_initFail)
		{
			LEAVEFUNC2(false);
		}


	static int iResult;
	//static char sendbuf[128];
	sprintf(cmdbuffer, "@printMode %u" ENDCHAR, printmode);
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);
	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_OK)
		{
			LEAVEFUNC2(true);
		}
		else if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			log_criticalln("catch an unknown response in setPagePerLayerHeight()");
			LEAVEFUNC2(false);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}

	//#undef DEF_FAN_SPEED
}

int Comm3DP::GetPrintMode()
{
	ENTERFUNC

		if (_initFail)
		{
			LEAVEFUNC2(-1);
		}
	static int iResult;
	sprintf(cmdbuffer, "@getPrintMode" ENDCHAR);
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);

	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(-1);
	}

	static int tmpi;
	static std::string tmpstr;
	if (catchResponse())
	{
		if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(-1);
		}
		else
		{
			tmpstr = recvstr;
			try{ tmpi = std::stoi(tmpstr); }
			catch (std::invalid_argument &e) {
				log_criticalln("catch an unknown response in jobCompletedPages()");
				LEAVEFUNC2(-1);
			}
			catch (std::out_of_range &e) {
				log_criticalln("catch a too large number in jobCompletedPages()");
				LEAVEFUNC2(-1);
			}
			LEAVEFUNC2(tmpi);
		}
	}
	else
	{
		LEAVEFUNC2(-1);
	}
}

/////////////////////////////////////////Kenneth 2019_1_28******wiper index and wipe click*/ ******/////////////////////////////////////////
bool Comm3DP::WiperIndex(unsigned unmWInex)
{
	ENTERFUNC


		if (_initFail)
		{
			LEAVEFUNC2(false);
		}


	static int iResult;
	//static char sendbuf[128];
	sprintf(cmdbuffer, "@wiperIndex %u" ENDCHAR, unmWInex);
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);
	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_OK)
		{
			LEAVEFUNC2(true);
		}
		else if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			log_criticalln("catch an unknown response in setPagePerLayerHeight()");
			LEAVEFUNC2(false);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}

	//#undef DEF_FAN_SPEED
}


bool Comm3DP::WiperClick(unsigned unmWClick)
{
	ENTERFUNC


		if (_initFail)
		{
			LEAVEFUNC2(false);
		}


	static int iResult;
	//static char sendbuf[128];
	sprintf(cmdbuffer, "@wiperClick %u" ENDCHAR, unmWClick);
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);
	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_OK)
		{
			LEAVEFUNC2(true);
		}
		else if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			log_criticalln("catch an unknown response in setPagePerLayerHeight()");
			LEAVEFUNC2(false);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}

	//#undef DEF_FAN_SPEED
}

/////////////////////////////////////////Kenneth 2019_2_15******midjob and active error*/ ******/////////////////////////////////////////
bool Comm3DP::Getactive_ids(std::string &result)
{
	if (_initFail)
	{
		LEAVEFUNC2(false);
	}


	static int iResult;
	sprintf(cmdbuffer, "@getactive_ids" ENDCHAR);
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);

	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	static int tmpi;
	if (catchResponse())
	{
		if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			result = recvstr;
			LEAVEFUNC2(true);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}

}

int Comm3DP::Getnowmidjob()
{
	ENTERFUNC

		if (_initFail)
		{
			LEAVEFUNC2(-1);
		}
	static int iResult;
	sprintf(cmdbuffer, "@getnowmidjob" ENDCHAR);
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);

	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(-1);
	}

	static int tmpi;
	static std::string tmpstr;
	if (catchResponse())
	{
		if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(-1);
		}
		else
		{
			tmpstr = recvstr;
			try{ tmpi = std::stoi(tmpstr); }
			catch (std::invalid_argument &e) {
				log_criticalln("catch an unknown response in jobCompletedPages()");
				LEAVEFUNC2(-1);
			}
			catch (std::out_of_range &e) {
				log_criticalln("catch a too large number in jobCompletedPages()");
				LEAVEFUNC2(-1);
			}
			LEAVEFUNC2(tmpi);
		}
	}
	else
	{
		LEAVEFUNC2(-1);
	}
}


int Comm3DP::GetTimeleft()
{
	ENTERFUNC

		if (_initFail)
		{
			LEAVEFUNC2(-1);
		}
	static int iResult;
	sprintf(cmdbuffer, "@getTimeleft" ENDCHAR);
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);

	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(-1);
	}

	static int tmpi;
	static std::string tmpstr;
	if (catchResponse())
	{
		if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(-1);
		}
		else
		{
			tmpstr = recvstr;
			try{ tmpi = std::stoi(tmpstr); }
			catch (std::invalid_argument &e) {
				log_criticalln("catch an unknown response in jobCompletedPages()");
				LEAVEFUNC2(-1);
			}
			catch (std::out_of_range &e) {
				log_criticalln("catch a too large number in jobCompletedPages()");
				LEAVEFUNC2(-1);
			}
			LEAVEFUNC2(tmpi);
		}
	}
	else
	{
		LEAVEFUNC2(-1);
	}
}
/////////////////////////////////////////Kenneth 2019_8_26******get ALL FW version*/ ******/////////////////////////////////////////
bool Comm3DP::GetFWversion(std::string &version)
{
	ENTERFUNC
		if (_initFail) {
			LEAVEFUNC2(false);
		}
	static int iResult;
	static char sendbuf[] = "@getFWversion" ENDCHAR;
	//	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);
	iResult = send(sockfd, sendbuf, sizeof(sendbuf) - 1, 0);
	if (iResult == SOCKET_ERROR) {
		log_println("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}



	if (catchResponse())
	{
		if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			version = recvstr;
			LEAVEFUNC2(true);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}
}
bool Comm3DP::GetFanversion(std::string &version)
{
	ENTERFUNC
		if (_initFail) {
			LEAVEFUNC2(false);
		}
	static int iResult;
	static char sendbuf[] = "@getFanversion" ENDCHAR;
	//	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);
	iResult = send(sockfd, sendbuf, sizeof(sendbuf) - 1, 0);
	if (iResult == SOCKET_ERROR) {
		log_println("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			version = recvstr;
			LEAVEFUNC2(true);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}
}
bool Comm3DP::GetISSversion(std::string &version)
{
	ENTERFUNC
		if (_initFail) {
			LEAVEFUNC2(false);
		}
	static int iResult;
	static char sendbuf[] = "@getISSversion" ENDCHAR;
	//iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);
	iResult = send(sockfd, sendbuf, sizeof(sendbuf) - 1, 0);
	if (iResult == SOCKET_ERROR) {
		log_println("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			version = recvstr;
			LEAVEFUNC2(true);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}
}

/////////////////////////////////////////Kenneth 2019_11_7******Send json*/ ******/////////////////////////////////////////
bool Comm3DP::SendJSONCmd(std::string classCmd)
{
	ENTERFUNC

		if (_initFail) {
			LEAVEFUNC2(false);
		}

	static int iResult;
	//cmd += ENDCHAR;  //Notice: it caused in double end char in next cmd!
	sprintf(cmdbuffer, "@JSON_COMMAND %s" ENDCHAR, classCmd.c_str());
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);

	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_OK)
		{
			LEAVEFUNC2(true);
		}
		else if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			log_criticalln("command is not send...");
			LEAVEFUNC2(false);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}
}


bool Comm3DP::SendDMCmd(std::string classCmd, std::string actionCmd)
{
	ENTERFUNC

		if (_initFail) {
			LEAVEFUNC2(false);
		}

	static int iResult;
	//cmd += ENDCHAR;  //Notice: it caused in double end char in next cmd!
	sprintf(cmdbuffer, "@SendDMCmd %s %s" ENDCHAR, classCmd.c_str(), actionCmd.c_str());
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);

	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_OK)
		{
			LEAVEFUNC2(true);
		}
		else if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			log_criticalln("command is not send...");
			LEAVEFUNC2(false);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}
}

bool Comm3DP::SendDMCmdTest(std::string classCmd)
{
	ENTERFUNC

		if (_initFail) {
			LEAVEFUNC2(false);
		}

	static int iResult;
	//cmd += ENDCHAR;  //Notice: it caused in double end char in next cmd!
	sprintf(cmdbuffer, "@JSON_COMMAND %s" ENDCHAR, classCmd.c_str());
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);

	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_OK)
		{
			LEAVEFUNC2(true);
		}
		else if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			log_criticalln("command is not send...");
			LEAVEFUNC2(false);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}
}


bool Comm3DP::getNVM(std::string &nvm)
{

	ENTERFUNC
		if (_initFail)
		{
			LEAVEFUNC2(false);
		}


	static int iResult;
	sprintf(cmdbuffer, "@getNVM" ENDCHAR);
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);

	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	static int tmpi;
	if (catchResponse())
	{
		if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			nvm = recvstr;
			LEAVEFUNC2(true);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}


}

bool Comm3DP::sendNVM(std::string NVM)
{
	ENTERFUNC

		if (_initFail) {
			LEAVEFUNC2(false);
		}

	static int iResult;
	//cmd += ENDCHAR;  //Notice: it caused in double end char in next cmd!
	sprintf(cmdbuffer, "@Sendnvm %s" ENDCHAR, NVM.c_str());
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);

	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_OK)
		{
			LEAVEFUNC2(true);
		}
		else if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			log_criticalln("command is not send...");
			LEAVEFUNC2(false);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}


}

bool Comm3DP::GetNVMValue(std::string NVMCmd, std::string &nvm)
{
	ENTERFUNC

		if (_initFail) {
			LEAVEFUNC2(false);
		}

	static int iResult;
	//cmd += ENDCHAR;  //Notice: it caused in double end char in next cmd!
	sprintf(cmdbuffer, "@COMMAND_GetNVMValue %s" ENDCHAR, NVMCmd.c_str());
	//sprintf(cmdbuffer, "@JSON_COMMAND_GET_VALUE %s" ENDCHAR, NVMCmd.c_str());
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);

	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_OK)
		{
			LEAVEFUNC2(true);
		}
		else if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			nvm = recvstr;
			log_criticalln("command is not send...");
			LEAVEFUNC2(false);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}
}
bool Comm3DP::NVMsendcheck(int &NVMcheck)
{
	ENTERFUNC

		if (_initFail)
		{
			LEAVEFUNC2(false);
		}

	static int iResult;
	//static char sendbuf[128];
	sprintf(cmdbuffer, "@NVMsendcheck" ENDCHAR);
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);
	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			if (1 != sscanf(recvstr.c_str(), "%d", &NVMcheck))
			{
				log_criticalln("parse header info failed");
				LEAVEFUNC2(false);
			}
			LEAVEFUNC2(true);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}


}

//************************************************************************************
/*2020_04_06 Kenneth add SS up and Down*/
bool Comm3DP::LIFT_intoDEVMODE()
{

	ENTERFUNC

		if (_initFail) {
			LEAVEFUNC2(false);
		}

	static int iResult;
	sprintf(cmdbuffer, "@LIFT_intoDEVMODE" ENDCHAR);
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);

	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_OK)
		{
			LEAVEFUNC2(true);
		}
		else if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			log_criticalln("catch an unknown response...");
			LEAVEFUNC2(false);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}
}
bool Comm3DP::LIFT_up()
{
	ENTERFUNC

		if (_initFail) {
			LEAVEFUNC2(false);
		}

	static int iResult;
	sprintf(cmdbuffer, "@LIFT_up" ENDCHAR);
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);

	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_OK)
		{
			LEAVEFUNC2(true);
		}
		else if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			log_criticalln("catch an unknown response...");
			LEAVEFUNC2(false);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}

}
bool Comm3DP::LIFT_down()
{
	ENTERFUNC

		if (_initFail) {
			LEAVEFUNC2(false);
		}

	static int iResult;
	sprintf(cmdbuffer, "@LIFT_down" ENDCHAR);
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);

	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_OK)
		{
			LEAVEFUNC2(true);
		}
		else if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			log_criticalln("catch an unknown response...");
			LEAVEFUNC2(false);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}

}
bool Comm3DP::LIFT_backONLINE()
{

	ENTERFUNC

		if (_initFail) {
			LEAVEFUNC2(false);
		}

	static int iResult;
	sprintf(cmdbuffer, "@LIFT_backONLINE" ENDCHAR);
	iResult = send(sockfd, cmdbuffer, strlen(cmdbuffer), 0);

	if (iResult == SOCKET_ERROR) {
		log_warnln("send failed with error: %d", WSAGetLastError());
		LEAVEFUNC2(false);
	}

	if (catchResponse())
	{
		if (recvstr == FEEDBACK_OK)
		{
			LEAVEFUNC2(true);
		}
		else if (recvstr == FEEDBACK_ERR)
		{
			LEAVEFUNC2(false);
		}
		else
		{
			log_criticalln("catch an unknown response...");
			LEAVEFUNC2(false);
		}
	}
	else
	{
		LEAVEFUNC2(false);
	}
}
//************************************************************************************


void Comm3DP::reset()
{
	ENTERFUNC

	_initFail = false;
	sockfd = INVALID_SOCKET;

	//check the number of objects created
	if (nObjCalled >= CONNLIMIT)
	{
		_initFail = true;
		LEAVEFUNC;
	}

	//init WSA
	if (!wsastartupCalled)
	{
		WSADATA wsaData;
		int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult != 0) {
			_initFail = true;
			LEAVEFUNC;
		}
		else wsastartupCalled = true;
	}

	//create socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == INVALID_SOCKET) {
		_initFail = true;
		LEAVEFUNC;
	}

	// Set the socket I/O mode: In this case FIONBIO
	// enables or disables the blocking mode for the 
	// socket based on the numerical value of iMode.
	// If iMode = 0, blocking is enabled; 
	// If iMode != 0, non-blocking mode is enabled.
	unsigned long iMode = 0;
	if (ioctlsocket(sockfd, FIONBIO, &iMode) != NO_ERROR)
	{
		_initFail = true;
		LEAVEFUNC;
	}

	//connect socket
	SOCKADDR_IN serv;
	serv.sin_family = AF_INET;
	serv.sin_port = htons(PORT);
	serv.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	if (connect(sockfd, (SOCKADDR*)&serv, sizeof(serv)) != 0)
	{
		log_warnln("connect socket with error: %d.", WSAGetLastError());
		_initFail = true;
		LEAVEFUNC;
	}

	//all success
	++nObjCalled;

	LEAVEFUNC;

}