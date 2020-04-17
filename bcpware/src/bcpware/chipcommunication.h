//$$ver 0.1
#ifndef CHIPCOMMUNICATION_H
#define CHIPCOMMUNICATION_H

#include <exception>
#include <string>
#include <vector>
#include <utility>
#include <mutex>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTimer>
#include "TcpClient.h"
#include "json/json.h"

/*Exception of ChipCommunication_t
*/
class CommunicateUsb2httpException : public std::exception
{
public:
	static CommunicateUsb2httpException msg(FILE* fLog, const char* s, const char* fileName, const unsigned int &line)
	{
		char linestr[24];
		sprintf_s(linestr, 24, "%u", line);

		std::string msg(s);
		msg.push_back(' ');
		msg.append(fileName).push_back(':');
		msg.append(linestr);

		fprintf(fLog, "#CommunicateUsb2httpException# %s\n", msg.c_str());
		fflush(fLog);

		return CommunicateUsb2httpException(msg.c_str());
	}
	CommunicateUsb2httpException(const char *s) : std::exception(s) {}
};

/*the name of folder contains log
*/
#define LOG_FOLDER "log\\"
#define WLOG_FOLDER L"log\\"

/*Maximun number of printers can be saved
*/
#define MAX_PRINTER_INFO 20

/*PID, VID and BaudRate of serial port of FPGA
*/
#define FPGA_PID 0x6001 // 0x7523
#define FPGA_VID 0x403 // 0x1a86
#define FPGA_BaudRate 115200 // 19200

enum UartMessage
{
	/*from FPGA to PC*/
	_PowerOn, _Lock, _Unlock,
	/*from PC to FPGA*/
	_PCOn
};

class ChipCommunication_t;
class UartSubClass : public QObject
{
	Q_OBJECT

	friend class ChipCommunication_t;

private:
	ChipCommunication_t *another;

public:
	void init();
	UartSubClass(ChipCommunication_t*);
	~UartSubClass();


	/*record info of FPGA*/
private:
	std::string XYZ_PRINTING_3D_PICASSO;
	std::string EGB_VERSION;
	std::string CHIP_ID;
	unsigned CHIP_TIME_STAMP;
	int PRINT_POSITION;
	int HOME_POSITION;
	int WIPE_POSITION;


private slots:
	void findUart();
private:
	QTimer *checkUartTimer;
	QSerialPort *uart;


private slots:
	void msgParser();
	/*parameters of msgGet() and msgSend() need be enum UartMessage
	*/
signals:
	void msgGet(int);
public slots:
	//void msgSend(int);

};

/*class responsible for communicating 
*/
class ChipCommunication_t
{
	friend class UartSubClass;
private:
	FILE *fLog;


public:
	void init();
	ChipCommunication_t();
	~ChipCommunication_t();


	/*Part 1 of 2 : Communication with Sirius chip*/
	/*##############################################################################################*/
private:
	/*Record printers Usb2http detect
	*/
	struct printer_info_s{
		std::string port;
		std::string name;
	};
	printer_info_s printer_list[MAX_PRINTER_INFO];

private:
	void parseParm(char * str, int len);
	void readInfFile();
	bool getJsonUrl(char *request, Json::Value &root);
	int getPrinterPort(void);
	char* getPrinterIP(void);

	
private:
	/*active_printer: indicate the target printer, -1 if no printer is chosen
	  nPrinters: total printers detect
	*/
	int active_printer;
	int nPrinters;
	std::mutex activePrinterMutex;
public:
	const std::string *printerName;


private:
	/*If a printer not connect computer using usb, it connect using ethernet.
	  In this condition, directConnection will be true and directIP will contain an IP that represent the printer.
	*/
	std::string directIP;
	bool directConnection;


private:
	/*portUsb2http: the port number Usb2http.exe use */
	int portUsb2http;


private:
	/*member that send and receive data via socket */
	TcpClient tcp;

public:
	typedef std::pair<const char*, const char*> ParamPair;
	typedef std::vector<ParamPair> GETParamPairSet;
	bool sendGET(const char *command, GETParamPairSet& parameterSet, std::string &outJsonData);
	bool sendPOST(const char *command, const char *script, std::string &outJsonData);

private:
	void updatePrinterList();
	void setActivePrinter(int index);
public:
	bool findPrinter();
	//int getActivePrinter();
	/*##### Part 1 of 2 end #############################################################################*/




	/*Part 2 of 2 : Communication with FPGA using UART*/
	/*##############################################################################################*/
private:
	UartSubClass fpga;
	/*##### Part 2 of 2 end ########################################################################*/
};


/*Do communication job in the background. Global variable. Remember to define it somewhere
  ccbg: 'C'hip 'C'ommunication 'B'ack'G'round
*/
extern ChipCommunication_t ccbg;


#endif