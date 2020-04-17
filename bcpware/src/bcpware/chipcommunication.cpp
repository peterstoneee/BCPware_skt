#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <Windows.h>
#include <cstring>
#include <cctype>
#include <cstdio>
#include <ctime>
#include "chipcommunication.h"

#define RCVBUFSIZE 0x4000   /* Size of receive buffer 16M */
#define USB2HTTP_IP "127.0.0.1"
#define USB2HTTP_DEFPORT 8080

void UartSubClass::init()
{
	findUart();
	if (!uart)
	{
		checkUartTimer = new QTimer;
		connect(checkUartTimer, SIGNAL(timeout()), this, SLOT(findUart()));
		checkUartTimer->start(2000);
	}
}

UartSubClass::UartSubClass(ChipCommunication_t *p) : another(p), uart(NULL), checkUartTimer(NULL) {}

UartSubClass::~UartSubClass()
{
	delete uart;
}


void UartSubClass::findUart()
{
	auto list = QSerialPortInfo::availablePorts();
	for (QSerialPortInfo &info : list)
	{
		if (info.vendorIdentifier() == FPGA_VID && info.productIdentifier() == FPGA_PID)
		{
			uart = new QSerialPort(info);
			uart->setBaudRate(FPGA_BaudRate);
			connect(uart, SIGNAL(readyRead()), this, SLOT(msgParser()));
			uart->open(QIODevice::ReadWrite);
			if (checkUartTimer)
			{
				disconnect(checkUartTimer, SIGNAL(timeout()), this, SLOT(findUart()));
				delete checkUartTimer;
				checkUartTimer = NULL;
			}
			return;
		}
	}
}

void UartSubClass::msgParser()
{
	static const char *data;
	static int siz;
	static int i, beg;
	register char c;
	static char buf[128];

	QByteArray buffer = uart->readAll();
	data = buffer.constData();
	siz = buffer.count();
	i = 0;
	while (true)
	{
		do {
			if (i == siz) return;

			if ((c = data[i]) == '\n') fputc(c, another->fLog);
			else if (c == '\r') ;
			else break;

			++i;
		} while (true);

		for (beg = i; (c = data[i]) != '\n' && i < siz; ++i)
			fputc(c, another->fLog);


		if (data[beg] != '@')
			continue;
		std::string str(data+beg, data+i);
		const char *cstr = str.c_str();

		if (str == "@PowerOn")
		{
			//if (another->findPrinter())//@@@@@
			uart->write("@PCOn");
		}
		else if (str == "@Lock")
			;// emit lock signal
		else if (str == "@Unlock")
			;// emit unlock signal
		else if (str == "@Idle")
			;// PC can send zx file
		else if (strstr(cstr, "@XYZ_PRINTING_3D_PICASSO"))
		{
			sscanf(cstr + 24, "%s", buf);
			XYZ_PRINTING_3D_PICASSO = buf;
		}
		else if (strstr(cstr, "@EGB_VERSION"))
		{
			sscanf(cstr + 12, "%s", buf);
			EGB_VERSION = buf;
		}
		else if (strstr(cstr, "@CHIP_ID"))
		{
			sscanf(cstr + 8, "%s", buf);
			CHIP_ID = buf;
		}
		else if (strstr(cstr, "@CHIP_TIME_STAMP"))
			sscanf(cstr + 16, "%X", &CHIP_TIME_STAMP);
		else if (strstr(cstr, "@PRINT_POSITION"))
			sscanf(cstr + 15, "%d", &PRINT_POSITION);
		else if (strstr(cstr, "@HOME_POSITION"))
			sscanf(cstr + 14, "%d", &HOME_POSITION);
		else if (strstr(cstr, "@WIPE_POSITION"))
			sscanf(cstr + 14, "%d", &WIPE_POSITION);

	}
	
}

void ChipCommunication_t::init()
{
	//initialize uart connection first
	//fpga.init(); //@@@comment temporary 

	ShellExecute(NULL, L"open", L"usb2http.exe", L"", NULL, SW_HIDE);
	readInfFile();

	//*****************find the printer that has plugged into PC*******************
	findPrinter();
	//*****************************************************************************
}

ChipCommunication_t::ChipCommunication_t() : portUsb2http(USB2HTTP_DEFPORT), tcp(true), active_printer(-1), printerName(NULL), fpga(this)
{
	//==========open a FILE* for writing log===============
	time_t t = time(NULL);
	char* str = ctime(&t);  //eg "Wed Feb 13 16:06:10 2013\n"
	str[3] = '_'; str[7] = '_'; str[10] = '_'; str[13] = 'h'; str[16] = 'm'; str[19] = '_'; str[24] = '\0';

	CreateDirectory(WLOG_FOLDER, NULL);

	std::string filestr(LOG_FOLDER);
	filestr += str;
	filestr += ".log";
	fLog = fopen(filestr.c_str(), "w");
	//===================================================
}

ChipCommunication_t::~ChipCommunication_t()
{
	fflush(fLog);
	ShellExecute(NULL, NULL, L"taskkill", L"/IM usb2http.exe /F", NULL, SW_HIDE);
}

void ChipCommunication_t::parseParm(char * str, int len)
{
	char *p;
	p = strstr(str, "=");

	if (p != NULL)
	{
		if (!strncmp(str, "Port", 4))
			portUsb2http = atoi(&p[1]);
	}
}

void ChipCommunication_t::readInfFile(void)
{
	FILE *hFile = NULL;

	// Open the inf file.
	fopen_s(&hFile, "usb2http.inf", "r");

	if (!hFile)
	{
		printf("Error opening setup file");
		return;
	}

	{
		char buffer[100];
		int buflen, count;
		unsigned dwFileSize;

		buflen = count = 0;
		fseek(hFile, 0, SEEK_END);
		dwFileSize = ftell(hFile);
		fseek(hFile, 0, SEEK_SET);

		// process the file
		while (count < (int)dwFileSize)
		{
			buffer[buflen] = fgetc(hFile);
			// check for end of line
			if (buffer[buflen] == '\r' || buffer[buflen] == '\n')
			{
				if (buflen>0)
				{
					buffer[buflen] = '\0';
					if (buffer[0] != '#') parseParm(buffer, buflen);
				}
				buflen = 0;
			}
			else if (buflen > 98)
				buflen = 0;
			else
				buflen++;
			count++;
		}
		if (buflen > 0)
			parseParm(buffer, buflen);
	}

	fclose(hFile);
}

// get the parsed json response for a given url request using current connection
bool ChipCommunication_t::getJsonUrl(char *request, Json::Value &root)
{
	BOOL result = false;

	// if an active printer exists
	if (active_printer >= 0)
	{
		char query[RCVBUFSIZE + 1];
		if (directConnection)
			sprintf_s(query, RCVBUFSIZE + 1, "GET /%s HTTP/1.0\r\n\r\n", request);
		else
			sprintf_s(query, RCVBUFSIZE + 1, "GET /%s/%s HTTP/1.0\r\n\r\n", printer_list[active_printer].port.c_str(), request);
		tcp.send_tcpip_request(query, strlen(query), getPrinterIP(), getPrinterPort());
		if (tcp.bodySize() > 0)
		{
			char *body = tcp.getDecodedBody();

			if ((body != NULL) && strlen(body) > 5)
			{
				Json::Features features;
				Json::Reader reader(features);

				bool parsingSuccessful = reader.parse(body, root);
				if (!parsingSuccessful)
					throw CommunicateUsb2httpException::msg(fLog, "parse data from usb2http fail ", __FILE__, __LINE__);
				else
					result = true;
			}
			else
				throw CommunicateUsb2httpException::msg(fLog, "no content in the data send by usb2http", __FILE__, __LINE__);
		}
		else
			throw CommunicateUsb2httpException::msg(fLog, "no data body from usb2http", __FILE__, __LINE__);

	}

	return result;
}

int ChipCommunication_t::getPrinterPort(void)
{
	return (directConnection) ? 80 : portUsb2http;
}

char* ChipCommunication_t::getPrinterIP(void)
{
	return (directConnection) ? directIP.c_str() : USB2HTTP_IP;
}


/*command (for example "/maint/activate"). Do not add any space to the head and the trail of string
*/
bool ChipCommunication_t::sendGET(const char *command, GETParamPairSet& parameterSet, std::string &outJsonData)
{
	if (active_printer < 0) return false;
	std::string outstr = "GET ";

	if (!directConnection)
	{
		outstr.push_back('/');
		outstr.append(printer_list[active_printer].port.c_str());
	}
	outstr.push_back('/');
	outstr.append(command);

	bool firstArg = true;
	int i = 0;
	for (ParamPair &p : parameterSet)
	{
		if (firstArg)
		{
			firstArg = false;
			outstr.push_back('?');
		}
		else
			outstr.push_back('&');

		outstr.append(p.first).push_back('=');
		outstr.append(p.second);

		++i;
	}

	outstr.append(" HTTP/1.1\r\n\r\n");

	if (0 != tcp.send_tcpip_request(const_cast<char*>(outstr.c_str()), outstr.length(), getPrinterIP(), getPrinterPort())) //if not success
		throw CommunicateUsb2httpException::msg(fLog, "fail send cmd data to USB2Http", __FILE__, __LINE__);

	if (tcp.isChunked)
	{
		if (!printerName)
		{
			activePrinterMutex.lock();
			printerName = &(printer_list[active_printer].name);
			activePrinterMutex.unlock();
		}
		outJsonData = tcp.getDecodedBody();
		return true;
	}
	else
	{
		activePrinterMutex.lock();
		printerName = NULL;
		activePrinterMutex.unlock();
		return false;
	}
}

bool ChipCommunication_t::sendPOST(const char *command, const char *script, std::string &outJsonData)
{
	int scriptlen = strlen(script);
	if (active_printer < 0) return false;
	if (scriptlen == 0) return false;

	std::string outstr = "POST ";

	if (!directConnection)
	{
		outstr.push_back('/');
		outstr.append(printer_list[active_printer].port.c_str());
	}
	outstr.append(command);

	char *buffer = new char[scriptlen+64];
	sprintf_s(buffer, scriptlen + 64, " HTTP/1.1\r\nContent-Length: %d\r\n\r\n%s", scriptlen, script);
	outstr.append(buffer);
	delete [] buffer;

	if (0 != tcp.send_tcpip_request(const_cast<char*>(outstr.c_str()), outstr.length(), getPrinterIP(), getPrinterPort())) //if not success
		throw CommunicateUsb2httpException::msg(fLog, "fail send cmd data to USB2Http", __FILE__, __LINE__);

	if (tcp.isChunked)
	{
		activePrinterMutex.lock();
		outJsonData = tcp.getDecodedBody();
		activePrinterMutex.unlock();
		return true;
	}
	else
	{
		activePrinterMutex.lock();
		printerName = NULL;
		activePrinterMutex.unlock();
		return false;
	}
}

void ChipCommunication_t::updatePrinterList()
{
	//get printer list
	char recvBuffer[RCVBUFSIZE];
	unsigned int recvMsgSize;
	sprintf_s(recvBuffer, RCVBUFSIZE, "GET /_$/control/list HTTP/1.0\r\n\r\n");
	recvMsgSize = strlen(recvBuffer);
	tcp.send_tcpip_request(recvBuffer, recvMsgSize, USB2HTTP_IP, portUsb2http);
	if (tcp.recvSize() > 0)
	{
		Json::Features features;
		Json::Reader reader(features);
		Json::Value root;

		bool parsingSuccessful = reader.parse(tcp.getDecodedBody(), root);
		if (!parsingSuccessful) throw CommunicateUsb2httpException::msg(fLog, "parse data from usb2http fail ", __FILE__, __LINE__);
		else
		{
			nPrinters = 0;
			//Json::Value printers = root["printers"];
			Json::Value list = root["printers"]["list"];

			Json::Value printer = list[nPrinters];
			while (!printer.isNull())
			{
				printer_list[nPrinters].name = printer["name"].asCString();
				printer_list[nPrinters].port = printer["port"].asCString();

				//printer_menu->AppendMenu(MF_STRING, ID_PRINTER_LOAD_STR_START + num_printers, printer_list[num_printers].name);

				++nPrinters;
				printer = list[nPrinters];
			}

		}
	}
	else
	{
		//no printer connect
		//throw CommunicateUsb2httpException::msg("receive no data from usb2http", __FILE__, __LINE__);
	}
}

void ChipCommunication_t::setActivePrinter(int index)
{
	activePrinterMutex.lock();
	active_printer = index;
	printerName = (index == -1) ? NULL : &(printer_list[index].name);
	activePrinterMutex.unlock();
	if (active_printer >= 0)
	{
		if (printer_list[index].port.substr(0, 3) == "IP_")
		{
			directConnection = true;
			directIP = printer_list[index].port.substr(3);
		}
		else directConnection = false;
	}
}

bool ChipCommunication_t::findPrinter()
{
	std::string nomatter;
	updatePrinterList();
	for (int i = 0; i < nPrinters; ++i)
	{
		setActivePrinter(i);
		if (sendGET("job/pages", ChipCommunication_t::GETParamPairSet(), nomatter))
			return true;
	}
	setActivePrinter(-1);
	return false;
}
