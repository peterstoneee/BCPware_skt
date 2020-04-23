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
#ifndef COMM3DP_H
#define COMM3DP_H

#define WIN32_LEAN_AND_MEAN


#include <windows.h>
#include <winsock2.h>
#include <atomic>
#include <string>
#include <map>
#include <utility>
#include "commProtocol.h"

/*
end with "`". Don't really read it in.

1. SCI command
#### send command ####
SCI command directly send
### return message ####
A JSON format data

__________________________________________________________________________________

2. Control machine
(1) @WakeUp
return: ERR, OK
(2) @Sleep
return: ERR, OK
(3) @RemovePH
return: ERR, OK
(4) @InstallPH
return: ERR, OK
(5) @InstallPHApproval
return: ERR, OK
(6) @InstallPHRegret
return: ERR, OK
(7) @ClearSiriusErr
return: ERR, OK
(8) @LightPHRecovery
return: ERR, OK
(9) @HeavyPHRecovery
return: ERR, OK

__________________________________________________________________________________

3. Printing
(1) @PrintTestPage
return: ERR, OK
(2) @SendHeader (#arg: nPage) (#arg hLayer) (#arg 'L'or'S') (#arg fanSpeed)
(3) @PrintPause
return: ERR, OK
(4) @PrintResume
return: ERR, OK
(5) @PrintCancel
return: ERR, OK

__________________________________________________________________________________

4.Machine status
(1) @PrinterStatus
return: status/error

2019_11_20
Ink_Service

2019_1_28
Init_wiper_Wait

status: Sleep, Init, Deinit

Init_lookingupPrinter, Init_checkPH,Init_checkwiper
Init_serviceforPrinter, Init_PrinterisOK

RemovingPH_Preparing, RemovingPH, RemovingPH_WaitUser,
InstallingPH_Preparing, InstallingPH_WaitInserting, InstallingPH_WaitApproval, InstallingPH,

PHLightRecovering, PHHeavyRecovering

Online, Service, PanelControl,

PrePrinting, Printing, PostPrinting,
Pausing, Paused, DoorOpened,
WiperSetting,
BinderRecovering,Poweroff,

Printing_INK_LOW,Printing_CARTRIDGE_MISSING,
Printing_INK_OUT, Fill_INK_To_Resume,
GettingError,
PumpError

Curing, Init_nPHin, Init_PH_user_manual, Init_opendoor
Init_user_installPH, Init_closedoor_PH, Init_PCOn
Init_wipermissing, Init_wiper_usermode, Init_closedoor_wiper


error: Error_noErr,
Error_other,
Error_no_printer, Error_too_many_UARTs,
Error_init_noPH,
Error_FPGA_missing, Error_Sirius_missing,
Error_from_Sirius


(2) @SiriusError
return: The error string get from Sirius (without double quote)
ERR (if status is not Error_from_Sirius)


(3) @RegisteredHeader
return: ERR
year%d_month%d_mday%d_hr%d_min%d_sec%d_page%d_layer%d_format%c_fan%d

(4) @ZAxis
return: ERR
builder%f_feeder%f (Unit: mm)

(5) @DoorSt
return: On, Off, ERR

__________________________________________________________________________________

5. Special
(1) @CloseProc
return: ERR, OK
(2) @AppVersion
return: string of app version
(3) @FirmwareVersion
return: string of FW version

*/

#define COMM3DP_FORTESTING 0
#define COMM3DP_RECVLEN 2048
//#define COMM3DP_FORTESTING
using namespace std;

class Comm3DP
{
private:
	static std::atomic<int> nObjCalled;
	static int initNObjCalled();
	static std::atomic<bool> wsastartupCalled;
	static bool initWsastartupCalled();

private:
	bool _initFail;
public:
	bool initFail();

#ifdef COMM3DP_FORTESTING
public:
#else
private:
#endif
	SOCKET sockfd;
	char recvbuf[COMM3DP_RECVLEN];
	std::string recvstr;
	bool catchResponse();

private:
	char* tokenJSON(char *cstr);
	std::string jsonItemVal(std::string json, const char* itemName);


#ifdef COMM3DP_FORTESTING
public:
	bool rawSend(std::string s);
#endif


public:
	bool wakeUp();
	bool sleep();
	bool clearSiriusError();
	bool clearISSError();
	bool lightPHRecover();
	bool heavyPHRecover();
	bool removePH();
	bool installPH();
	bool installPHApproval();
	bool installPHRegret();

	bool printTestPage();
	bool printIPQCPage();
	bool sendPrintjobHeader(unsigned nPage, unsigned hLayer, char lors, int Fan = 20u); //return true or false
	bool cancelPrintJob(); //return true or false
	bool pausePrintJob(); //return true or false
	bool resumePrintJob(); //return true or false
	/////////////////////////////////////////Kenneth 2018_07_10 OK to INK error************/////////////////////////////////////////
	bool OKtoINKerror(); //return true or false
	/////////////////////////////////////////Kenneth 2018.1.26******power off******/////////////////////////////////////////
	bool poweroff();
	/////////////////////////////////////////Kenneth 2017.1.31******Estimated Time to FPGA******/////////////////////////////////////////
	bool ETtofpga(std::string ETtime);
	/*Kenneth Add & Sub*************************/
	/*bool sendSiriusOntoFPGA();*/
	/*2017_7_26**********************************/
	/*Kenneth_2018_04_25*/
	bool CheckCancel(bool &);

	/////////////////////////////////////////Kenneth 2017.1.17******ink chage******/////////////////////////////////////////
	bool inkStatus(unsigned &c, unsigned &m, unsigned &y, unsigned &binder);
	bool inkStatus2(unsigned &c, unsigned &m, unsigned &y, unsigned &binder);
	/////////////////////////////////////////Kenneth 2018_07_04******ink Level******/////////////////////////////////////////
	bool inkLevel(unsigned &c, unsigned &m, unsigned &y, unsigned &binder);
	/////////////////////////////////////////Kenneth 2017.1.17******CompletedPages chage******/////////////////////////////////////////
	int jobCompletedPages(); //return -1 if fail, return a number >= 0 if success
	int jobCompletedPages2(); //return -1 if fail, return a number >= 0 if success
	int getNowPrintedpage();//2018_3_15
	/////////////////////////////////////////Kenneth 2017.1.17******sirius status system******/////////////////////////////////////////
	bool systemStatus(std::string &result);

	bool printerStatus(std::string &status, std::string &error);
	bool getHeader(
		int &year, int &month, int &mday, int &hr, int &min, int &sec,
		int &page, int &layer, char &format, int &fan
		);
	bool siriusError(std::string &s);
	bool zAxis(float &mmBuilder, float &mmFeeder);

	//2017.7.13-Timmy
	bool DoorSt(int &i_door);
	//2017.9.26 Kenneth~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	bool DoorEN();
	bool DoorDIS();
	//2017.10.30- Kenneth FPGASend
	bool sendFPGAcommand(std::string cmd);

	bool sendSCICommand(std::string cmd, std::string &result); //return true or false
	bool SendSiriusOntoFPGA(); //2017.8.2-Timmy: 
	bool SiriusShutDown();     //2017.8.2-Timmy: 
	bool fpgaError(std::string &s); //2017.8.4-Timmy: 
	bool getServiceProgress(std::string &s); //2017.8.4-Timmy: 

	bool ReleaseLatch(); //2017.8.24-Timmy add for error handle
	bool SetPrimeStatusOnly(); //2017.8.24-Timmy add for error handle

	//2017.9.7-Timmy for dynamic midjob	
	bool setDynamicMidjob_Mode(int i_mode);
	bool setDynamicMidjob_PageInterval(int i_PageInterval);
	bool setDynamicMidjob_Page(int ipage, int i_PageInterval);
	bool getDynamicMidjob_PageInterval(int &i_PageInterval);

	//2017.11.22-Timmy
	bool getWiperCount(int &iWiper); //wiper count
	bool setWiperCount(int iWiper);
	bool setWiperSN(std::string sn);
	bool getWiperSN(std::string &sn);
	bool setWiperOK();

	bool setPrintfilename(std::string filename); //filename
	bool setPrintLeftTime(int time_min);
	bool CheckShutdown();

	std::string appVersion();
	std::string firmwareVersion();
	static void openProc();
	bool closeProc();
	/////////////////////////////////////////Kenneth_2018_6_21******Binder used level******/////////////////////////////////////////
	bool getBinderINKLevel(std::string &);
	/////////////////////////////////////////Kenneth 2018_07_12******Color Mixed Binder******/////////////////////////////////////////
	bool BinderRecovery();
	/////////////////////////////////////////Kenneth 2018_07_17******Pre Heated******/////////////////////////////////////////
	bool PreHeatedTimes(std::string);
	bool PreAndPrintingHeated(unsigned, unsigned, unsigned, unsigned);
	bool PostHeated(unsigned, unsigned, unsigned);
	/////////////////////////////////////////Kenneth 2018_11_26******pre PH in******/////////////////////////////////////////
	bool PrePHin();
	/////////////////////////////////////////Kenneth 2018_12_24******GET Dry Counuting******/////////////////////////////////////////
	int GetDryConut();
	int GetTotalPage();
	/////////////////////////////////////////Kenneth 2019_1_14******printmode ->normal mode: 1(1 layer=1 page), best mode: 2(1 layer=2 page)******/////////////////////////////////////////
	bool PrintMode(unsigned);
	int GetPrintMode();
	/////////////////////////////////////////Kenneth 2019_1_28******wiper index and wipe click*/ ******/////////////////////////////////////////
	bool WiperIndex(unsigned);
	bool WiperClick(unsigned);
	/////////////////////////////////////////Kenneth 2019_2_15******midjob and active error*/ ******/////////////////////////////////////////
	bool Getactive_ids(std::string &result);
	int Getnowmidjob();
	int GetTimeleft();
	bool GetFWversion(std::string &);
	bool GetFanversion(std::string &);
	bool GetISSversion(std::string &);
	/////////////////////////////////////////Kenneth 2019_11_07******Send json*/ ******/////////////////////////////////////////
	bool SendJSONCmd(std::string);
	bool SendDMCmd(std::string classCmd, std::string actionCmd);
	bool getNVM(std::string &);
	bool sendNVM(std::string);
	bool SendDMCmdTest(std::string classCmd);
	bool GetNVMValue(std::string, std::string &);
	bool NVMsendcheck(int &);
	//************************************************************************************
	/*2020_04_06 Kenneth add SS up and Down*/
	bool LIFT_intoDEVMODE();
	bool LIFT_up();
	bool LIFT_down();
	bool LIFT_backONLINE();
	//************************************************************************************


public:
	Comm3DP();
	~Comm3DP();

public: //SKT
	void reset();
	struct printer3dpString
	{
		enum printStatus{
			Ink_Service, Init_wiper_Wait,
			Sleep, Init, Deinit,

			Init_lookingupPrinter, Init_checkPH, Init_checkwiper,
			Init_serviceforPrinter, Init_PrinterisOK,

			RemovingPH_Preparing, RemovingPH, RemovingPH_WaitUser,
			InstallingPH_Preparing,
			InstallingPH_WaitInserting, InstallingPH_InsertRetry, InstallingPH_WaitApproval, InstallingPH,
			PHLightRecovering, PHHeavyRecovering,
			Online, Service, PanelControl,
			PrePrinting, Printing, PostPrinting,
			Pausing, Paused, DoorOpened,
			WiperSetting,
			BinderRecovering, Poweroff, /*None,*/

			Printing_INK_LOW, Printing_CARTRIDGE_MISSING, //2018_4_18_In printing error
			Printing_INK_OUT,Fill_INK_To_Resume, //2018_4_18_In printing error


			GettingError,

			Curing, Init_nPHin, Init_PH_user_manual, Init_opendoor,
			Init_user_installPH, Init_closedoor_PH, Init_PCOn,
			Init_wipermissing, Init_wiper_usermode, Init_closedoor_wiper


		};
		enum errorStatus
		{
			Error_noErr,
			Error_other,
			Error_no_printer, Error_too_many_UARTs,
			Error_init_noPH,
			Error_FPGA_missing, Error_Sirius_missing,
			Error_from_Sirius
		};

		std::map<std::string, int> printerStatusmap;
		std::map<std::string, int> errorStatusMap;
		printer3dpString()
		{
			printerStatusmap.insert(make_pair("Ink_Service", (int)Ink_Service));
			printerStatusmap.insert(make_pair("Init_wiper_Wait", (int)Init_wiper_Wait));

			printerStatusmap.insert(make_pair("Sleep", (int)Sleep));
			printerStatusmap.insert(make_pair("Init", (int)Init));
			printerStatusmap.insert(make_pair("Deinit", (int)Deinit));

			
			printerStatusmap.insert(make_pair("Init_lookingupPrinter", (int)Init_lookingupPrinter));
			printerStatusmap.insert(make_pair("Init_checkPH", (int)Init_checkPH));
			printerStatusmap.insert(make_pair("Init_checkwiper", (int)Init_checkwiper));		
			printerStatusmap.insert(make_pair("Init_serviceforPrinter", (int)Init_serviceforPrinter));
			printerStatusmap.insert(make_pair("Init_PrinterisOK", (int)Init_PrinterisOK));

			printerStatusmap.insert(make_pair("RemovingPH_Preparing", (int)RemovingPH_Preparing));
			printerStatusmap.insert(make_pair("RemovingPH", (int)RemovingPH));
			printerStatusmap.insert(make_pair("RemovingPH_WaitUser", (int)RemovingPH_WaitUser));

			printerStatusmap.insert(make_pair("InstallingPH_WaitInserting", (int)InstallingPH_WaitInserting));
			printerStatusmap.insert(make_pair("InstallingPH_InsertRetry", (int)InstallingPH_InsertRetry));
			printerStatusmap.insert(make_pair("InstallingPH_WaitApproval", (int)InstallingPH_WaitApproval));
			printerStatusmap.insert(make_pair("InstallingPH", (int)InstallingPH));
			printerStatusmap.insert(make_pair("InstallingPH_Preparing", (int)InstallingPH));

			printerStatusmap.insert(make_pair("PHLightRecovering", (int)PHLightRecovering));
			printerStatusmap.insert(make_pair("PHHeavyRecovering", (int)PHHeavyRecovering)); 

			printerStatusmap.insert(make_pair("Online", (int)Online));
			printerStatusmap.insert(make_pair("Service", (int)Service));
			printerStatusmap.insert(make_pair("PanelControl", (int)PanelControl));

			printerStatusmap.insert(make_pair("PrePrinting", (int)PrePrinting));
			printerStatusmap.insert(make_pair("Printing", (int)Printing));
			printerStatusmap.insert(make_pair("PostPrinting", (int)PostPrinting));

			printerStatusmap.insert(make_pair("Pausing", (int)Pausing));
			printerStatusmap.insert(make_pair("Paused", (int)Paused));
			printerStatusmap.insert(make_pair("DoorOpened", (int)DoorOpened));

			printerStatusmap.insert(make_pair("GettingError", (int)GettingError));
			printerStatusmap.insert(make_pair("PumpError", (int)GettingError));

			printerStatusmap.insert(make_pair("Printing_INK_LOW", (int)Printing_INK_LOW));
			printerStatusmap.insert(make_pair("Printing_INK_OUT", (int)Printing_INK_OUT));
			printerStatusmap.insert(make_pair("Printing_CARTRIDGE_MISSING", (int)Printing_CARTRIDGE_MISSING));			
			printerStatusmap.insert(make_pair("Fill_INK_To_Resume", (int)Fill_INK_To_Resume));

			printerStatusmap.insert(make_pair("WiperSetting", (int)WiperSetting));
			printerStatusmap.insert(make_pair("BinderRecovering", (int)BinderRecovering));
			printerStatusmap.insert(make_pair("Poweroff", (int)Poweroff));
			//printerStatusmap.insert(make_pair("", (int)None));


			/*Curing, Init_nPHin, Init_PH_user_manual, Init_opendoor,
			Init_user_installPH, Init_closedoor_PH, Init_PCOn,
			Init_wipermissing, Init_wiper_usermode, Init_closedoor_wiper*/

			printerStatusmap.insert(make_pair("Curing", (int)Curing));
			printerStatusmap.insert(make_pair("Init_nPHin", (int)Init_nPHin));
			printerStatusmap.insert(make_pair("Init_PH_user_manual", (int)Init_PH_user_manual));
			printerStatusmap.insert(make_pair("Init_opendoor", (int)Init_opendoor));
			printerStatusmap.insert(make_pair("Init_user_installPH", (int)Init_user_installPH));
			printerStatusmap.insert(make_pair("Init_closedoor_PH", (int)Init_closedoor_PH));
			printerStatusmap.insert(make_pair("Init_PCOn", (int)Init_PCOn));
			printerStatusmap.insert(make_pair("Init_wipermissing", (int)Init_wipermissing));
			printerStatusmap.insert(make_pair("Init_wiper_usermode", (int)Init_wiper_usermode));
			printerStatusmap.insert(make_pair("Init_closedoor_wiper", (int)Init_closedoor_wiper));


			errorStatusMap.insert(make_pair("Error_noErr", (int)Error_noErr));
			errorStatusMap.insert(make_pair("Error_other", (int)Error_other));
			errorStatusMap.insert(make_pair("Error_no_printer", (int)Error_no_printer));
			errorStatusMap.insert(make_pair("Error_too_many_UARTs", (int)Error_too_many_UARTs));
			errorStatusMap.insert(make_pair("Error_init_noPH", (int)Error_init_noPH));
			errorStatusMap.insert(make_pair("Error_FPGA_missing", (int)Error_FPGA_missing));
			errorStatusMap.insert(make_pair("Error_Sirius_missing", (int)Error_Sirius_missing));
			errorStatusMap.insert(make_pair("Error_from_Sirius", (int)Error_from_Sirius));		

		}

	}printer3dpString;
};



#endif