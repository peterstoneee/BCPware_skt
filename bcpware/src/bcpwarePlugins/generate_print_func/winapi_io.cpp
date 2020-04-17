#ifndef WINAPI_IO_CPP
#define WINAPI_IO_CPP
//#ifndef WIN32_LEAN_AND_MEAN
//#define WIN32_LEAN_AND_MEAN
//#endif
#define _UNICODE
#define UNICODE
#include <windows.h>



#include <tchar.h>
#include <stdio.h>
#include <setupapi.h>               // for SetupDiEnumDeviceInterfaces(), etc.
#include <initguid.h>               // for DEFINE_GUID() macro
#include <assert.h>     
#include <qDebug>
#include <comdef.h>

DEFINE_GUID(GUID_CLASS_USBPRINT,
	0x28d78fad, 0x5a12, 0x11d1, 0xae, 0x5b, 0x00, 0x00, 0xf8, 0x03, 0xa8, 0xc2);


static const char PJL_UEL[] = "\x1b%-12345X";
static const char PJL_ENTERLANG[] = "@PJL ENTER LANGUAGE=ACL\r\n";
static const char PJL_INITIALIZE[] = "@PJL INITIALIZE\r\n";
static const char PJL_ENTERLANG_BOREALIS[] = "@PJL ENTER LANGUAGE=BOR\r\n";

static const UINT32 BOR_END_JOB = 1016;
static const UINT32 BOR_HEADER_SIZE = 12;

static void test_writefile(char *writebuffer);
static bool USBlink();
static void _cdecl fatalExit(LPCTSTR pszFormat, ...);
static HANDLE OpenUSBDevice(TCHAR * pszControllerSerialNum);
static BOOL transportFile(HANDLE hDevice, HANDLE hFile);
static void test_writefile2(char *writebuffer);
static DWORD transportTOUSBDevice(HANDLE usbHandle, char *printbuf, size_t count);
static DWORD offsetRecord = 0;

static bool USBlink()
{

	HANDLE  hUSBDevice = INVALID_HANDLE_VALUE;
	TCHAR  * pchControllerSerialNum = NULL;
	TCHAR  * pchPrintJobFilename = NULL;
	HANDLE  hFile = INVALID_HANDLE_VALUE;
	//  Connect with the specified USB device.
	hUSBDevice = OpenUSBDevice(pchControllerSerialNum);
	if (INVALID_HANDLE_VALUE == hUSBDevice)
	{
		/*_tprintf(TEXT(
			"FATAL ERROR: couldn't find the requested USB device, err %u\n"),
			GetLastError());*/
		qDebug() << "usb_link error";
		//system("pause");
		return 1;
	}

	pchPrintJobFilename = L"d:\\aMSDNnm.tmp";

	if (pchPrintJobFilename != NULL)
	{
		// Open the print job file.
		//_tprintf(_T("Create the file %s \n"), pchPrintJobFilename);
		hFile = CreateFile(pchPrintJobFilename,
			GENERIC_READ, 0, NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED
			, NULL);

		if (INVALID_HANDLE_VALUE == hFile)
		{
			//wprintf((L"Failed to open file '%s', err %u\n"),pchPrintJobFilename,		GetLastError());
			CloseHandle(hUSBDevice);
			return 1;
		}


		//printf(("Sending print job from file %s\n"),pchPrintJobFilename);

		transportFile(hUSBDevice, hFile);

		CloseHandle(hFile);

		// You cannot use SetCommTimeouts() to make ReadFile() non-blocking on
		// a read from a USB device and therefore I have to make the call from
		// a separate thread.
	}

}

static char* c_Name(TCHAR* t_x)
{	
	_bstr_t b(t_x);
	
	char sz[200];
	//char *ansi_string = b;
	wcstombs(sz, t_x, wcslen(t_x));
	return sz;
}
static TCHAR* t_Name(char* c_x)
{
	int size = strlen(c_x);
	TCHAR *unicode_string = new TCHAR[size+1];
	mbstowcs(unicode_string, c_x, size + 1);
	return unicode_string;
}
static void _cdecl fatalExit(LPCTSTR pszFormat, ...)
{
	//***20160429
	/*auto c_Name = [&](TCHAR* t_x)->char*
	{
		int size = wcslen(t_x);
		char *ansi_string = NULL;
		wcstombs(ansi_string, t_x, size + 1);
		return ansi_string;
	};*/
	/*auto t_Name = [&](char* c_x)->TCHAR*
	{
		int size = strlen(c_x);
		TCHAR *unicode_string = NULL;
		mbstowcs(unicode_string, c_x, size + 1);
		return unicode_string;
	};*/
	TCHAR    szText[256];
	va_list  vaList;

	va_start(vaList, pszFormat);
	wvsprintf(szText, pszFormat, vaList);
	va_end(vaList);

	fputs(c_Name(szText), stderr);
	OutputDebugString(szText);

	//_stprintf(szText, TEXT(": error %u\n"), GetLastError());

	fputs(c_Name(szText), stderr);
	OutputDebugString(szText);

	exit(1);
} // fatalExit()
static HANDLE OpenUSBDevice(TCHAR * pszControllerSerialNum)
{
	//***20160429
	/*auto c_Name = [&](TCHAR* t_x)->char*
	{
		int size = wcslen(t_x);
		char *ansi_string = NULL;
		wcstombs(ansi_string, t_x, size + 1);
		return ansi_string;
	};
	auto t_Name = [&](char* c_x)->TCHAR*
	{
		int size = strlen(c_x);
		TCHAR *unicode_string = NULL;
		mbstowcs(unicode_string, c_x, size + 1);
		return unicode_string;
	};*/


	HDEVINFO hDevInfoSet;
	DWORD    iDevice = 0;
	int      iMatch = -1;
	HANDLE   hUSBDevice = INVALID_HANDLE_VALUE; // return value of this func
	TCHAR    szDevName[256];
	//char *c_szControllerSerialNumLC = "###################";
	//TCHAR    *szControllerSerialNumLC = t_Name(c_szControllerSerialNumLC);
	TCHAR    szControllerSerialNumLC[20] = L"###################";//backup

	if (pszControllerSerialNum)
	{
		// Even when we specify upper-case characters with the "-wu" switch
		// to set the Controller serial number, the USB device name ends up holding
		// the corresponding lower-case characters.  Therefore to spot the
		// requested device, we need to watch for the lower-case characters.
		// Furthermore, the Controller serial number is bracketed by '#' characters
		// in the USB device name.  Therefore we include these characters
		// in our search in order to avoid "finding" the serial number in
		// other parts of the USB device name.

		TCHAR  * pch = pszControllerSerialNum;
		TCHAR  * pchLC = &szControllerSerialNumLC[1];

		while (*pch != TEXT('\0'))
			*pchLC++ = (TCHAR)tolower((int)*(pch++));

		*pchLC++ = TEXT('#');
		*pchLC++ = TEXT('\0');
	}

	hDevInfoSet = SetupDiGetClassDevs((LPGUID)&GUID_CLASS_USBPRINT,
		NULL,
		NULL,
		DIGCF_PRESENT | DIGCF_INTERFACEDEVICE);

	if (INVALID_HANDLE_VALUE == hDevInfoSet)
	{
		fatalExit(TEXT("Failed SetupDiGetClassDevs()\n"));
	}

	// The device information set returned by SetupDiGetClassDevs()
	// must eventually be deleted via a call to SetupDiDestroyDeviceInfoList().

	SP_DEVICE_INTERFACE_DATA  devInfoData;

	// Each call to SetupDiEnumDeviceInterfaces() returns information
	// about one device interface.

	memset(&devInfoData, 0, sizeof(devInfoData));
	devInfoData.cbSize = sizeof(devInfoData);

	while (SetupDiEnumDeviceInterfaces(hDevInfoSet, NULL,
		(LPGUID)&GUID_CLASS_USBPRINT,
		iDevice,          // member index
		&devInfoData))
	{
		// Since the detailed information has variable length,
		// we make two calls to SetupDiGetInterfaceDeviceDetail(),
		// one to get the required size and another to actually
		// fill in some allocated storage.

		DWORD   dwLen = 0;

		SetupDiGetInterfaceDeviceDetail(hDevInfoSet, &devInfoData,
			NULL, 0, &dwLen, NULL);

		if (dwLen == 0)
		{
			SetupDiDestroyDeviceInfoList(hDevInfoSet);
			fatalExit(TEXT(
				"Failed first call to SetupDiGetInterfaceDeviceDetail()"));
		}

		SP_DEVICE_INTERFACE_DETAIL_DATA *pDetail =
			(SP_DEVICE_INTERFACE_DETAIL_DATA*)malloc(dwLen);

		// This memory allocation must be freed !!

		if (!pDetail)
		{
			SetupDiDestroyDeviceInfoList(hDevInfoSet);
			fatalExit(TEXT("Failed to alloc buffer for device detail"));
		}

		pDetail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

		if (SetupDiGetInterfaceDeviceDetail(hDevInfoSet,
			&devInfoData,
			pDetail,
			dwLen, &dwLen, NULL))
		{	
			if (wcslen(pDetail->DevicePath)<	sizeof(szDevName) / sizeof(TCHAR)-1)
			{
				//                _tprintf( TEXT( "Device # %u has the name %s\n" ),
				//                          iDevice, pDetail->DevicePath );
				wcscpy(szDevName, pDetail->DevicePath);
			}
			else
			{
				free(pDetail);
				SetupDiDestroyDeviceInfoList(hDevInfoSet);
				fatalExit(TEXT("Insufficient device name buffer"));
			}
		}
		else
		{
			free(pDetail);
			SetupDiDestroyDeviceInfoList(hDevInfoSet);
			fatalExit(TEXT(
				"Failed second call to SetupDiGetInterfaceDeviceDetail()"));
		}

		// To detect whether this USB device is the particular USB device
		// indicated by the pszControllerSerialNum passed parameter, I look for
		// pszControllerSerialNum inside szDevName.  An alternative implementation
		// would call CreateFile() to get the USB HANDLE, then issue a
		// UEL and an "@PJL ENTER LANGUAGE=ACL" command, and then call into
		// ReadControllerSerialNum() to get the actual serial number.  But this
		// is not only slower, it also won't work when some of the connected
		// devices did not support the
		// ACL language.

		if (pszControllerSerialNum)
		{
			TCHAR  * pch = wcsstr(szDevName, szControllerSerialNumLC);
			//char  * pch = strstr(c_Name(szDevName), c_Name(szControllerSerialNumLC));
			

			if ((pch != NULL))
			{
				iMatch = (int)iDevice;
				//printf("Found it at iMatch = %d, szDevName = %s\n", iMatch, szDevName );
			}
		}

		// We now prepare for the next call to SetupDiEnumDeviceInterfaces().

		iDevice++;
		free(pDetail);
		memset(&devInfoData, 0, sizeof(devInfoData));
		devInfoData.cbSize = sizeof(devInfoData);

	} // end of while ( SetupDiEnumDeviceInterfaces() )

	SetupDiDestroyDeviceInfoList(hDevInfoSet);

	if (iDevice == 1)
	{
		//printf("going with szDevName =%s\n", szDevName);
		hUSBDevice = CreateFile(szDevName, GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ, NULL,
			OPEN_EXISTING, 0, NULL);
		return hUSBDevice;
	}
#pragma region device_bigger_than_1
	if (iDevice > 1)
	{
		TCHAR  szResp[100];
		UINT   iDesiredDevice;

		if (iMatch < 0)
		{
			// Since there are more than one device that are using the same
			// USB class GUID, and the passed pszControllerSerialNum did not identify
			// a particular one of these devices, we must ask the operator to
			// explicitly identify which device he wants to talk to.

			// In order to display all the choices, we repeat the enumeration.

			hDevInfoSet = SetupDiGetClassDevs(
				(LPGUID)&GUID_CLASS_USBPRINT,
				NULL,
				NULL,
				DIGCF_PRESENT | DIGCF_INTERFACEDEVICE);

			if (INVALID_HANDLE_VALUE == hDevInfoSet)
			{
				fatalExit(TEXT("Failed SetupDiGetClassDevs()\n"));
			}

			iDevice = 0;

			memset(&devInfoData, 0, sizeof(devInfoData));
			devInfoData.cbSize = sizeof(devInfoData);

			while (SetupDiEnumDeviceInterfaces(
				hDevInfoSet, NULL,
				(LPGUID)&GUID_CLASS_USBPRINT,
				iDevice,          // member index
				&devInfoData))
			{
				DWORD   dwLen = 0;

				SetupDiGetInterfaceDeviceDetail(hDevInfoSet, &devInfoData,
					NULL, 0, &dwLen, NULL);
				if (dwLen == 0)
				{
					SetupDiDestroyDeviceInfoList(hDevInfoSet);
					fatalExit(TEXT(
						"Failed first call to SetupDiGetInterfaceDeviceDetail()"));
				}

				SP_DEVICE_INTERFACE_DETAIL_DATA *pDetail =
					(SP_DEVICE_INTERFACE_DETAIL_DATA*)malloc(dwLen);

				// This memory allocation must be freed !!

				if (!pDetail)
				{
					SetupDiDestroyDeviceInfoList(hDevInfoSet);
					fatalExit(TEXT(
						"Failed to alloc buffer for device detail"));
				}

				pDetail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

				if (SetupDiGetInterfaceDeviceDetail(
					hDevInfoSet,
					&devInfoData,
					pDetail,
					dwLen, &dwLen, NULL))
				{
					int y = _tcslen((pDetail->DevicePath));
					int x = sizeof(szDevName) / sizeof(TCHAR)-1;
					
					if (_tcslen((pDetail->DevicePath)) <
						sizeof(szDevName) / sizeof(TCHAR)-1)
					{
						_tprintf(_T("Device # %u has the name %s\n"),
							iDevice, pDetail->DevicePath);
						_tcscpy((szDevName), (pDetail->DevicePath));
					}
					else
					{
						free(pDetail);
						SetupDiDestroyDeviceInfoList(hDevInfoSet);
						fatalExit(TEXT("Insufficient device name buffer"));
					}
				}
				else
				{
					free(pDetail);
					SetupDiDestroyDeviceInfoList(hDevInfoSet);
					fatalExit(TEXT(
						"Failed second call to SetupDiGetInterfaceDeviceDetail()"));
				}

				// We now prepare for the next call to
				// SetupDiEnumDeviceInterfaces().

				iDevice++;
				free(pDetail);
				memset(&devInfoData, 0, sizeof(devInfoData));
				devInfoData.cbSize = sizeof(devInfoData);

			} // end of while ( SetupDiEnumDeviceInterfaces() )

			SetupDiDestroyDeviceInfoList(hDevInfoSet);

			do
			{
				_tprintf(_T(
					"Enter the index of the device you wish to send to: "));
				_fgetts((szResp), sizeof(szResp) / sizeof(TCHAR), stdin);
				iDesiredDevice = _tcstoul((szResp), NULL, 10);

			} while (iDesiredDevice >= iDevice);
		}
		else
		{
			iDesiredDevice = (UINT)iMatch;
		}

		// We now repeat the enumeration.

		hDevInfoSet = SetupDiGetClassDevs(
			(LPGUID)&GUID_CLASS_USBPRINT,
			NULL,
			NULL,
			DIGCF_PRESENT | DIGCF_INTERFACEDEVICE);

		if (INVALID_HANDLE_VALUE == hDevInfoSet)
		{
			fatalExit(TEXT("Failed SetupDiGetClassDevs()\n"));
		}

		iDevice = 0;

		memset(&devInfoData, 0, sizeof(devInfoData));
		devInfoData.cbSize = sizeof(devInfoData);

		while (SetupDiEnumDeviceInterfaces(
			hDevInfoSet, NULL,
			(LPGUID)&GUID_CLASS_USBPRINT,
			iDevice,          // member index
			&devInfoData))
		{
			DWORD   dwLen = 0;

			SetupDiGetInterfaceDeviceDetail(hDevInfoSet, &devInfoData,
				NULL, 0, &dwLen, NULL);
			if (dwLen == 0)
			{
				SetupDiDestroyDeviceInfoList(hDevInfoSet);
				fatalExit(TEXT(
					"Failed first call to SetupDiGetInterfaceDeviceDetail()"));
			}

			SP_DEVICE_INTERFACE_DETAIL_DATA *pDetail =
				(SP_DEVICE_INTERFACE_DETAIL_DATA*)malloc(dwLen);

			// This memory allocation must be freed !!

			if (!pDetail)
			{
				SetupDiDestroyDeviceInfoList(hDevInfoSet);
				fatalExit(TEXT("Failed to alloc buffer for device detail"));
			}

			pDetail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

			if (SetupDiGetInterfaceDeviceDetail(hDevInfoSet,
				&devInfoData,
				pDetail,
				dwLen, &dwLen, NULL))
			{
				if (_tcslen((pDetail->DevicePath)) <
					sizeof(szDevName) / sizeof(TCHAR)-1)
				{
					_tcscpy((szDevName), (pDetail->DevicePath));

					if (iDevice == iDesiredDevice)
						break;  // break rather than return because we
					//   still have work to do
				}
				else
				{
					free(pDetail);
					SetupDiDestroyDeviceInfoList(hDevInfoSet);
					fatalExit(TEXT("Insufficient device name buffer"));
				}
			}
			else
			{
				free(pDetail);
				SetupDiDestroyDeviceInfoList(hDevInfoSet);
				fatalExit(TEXT(
					"Failed second call to SetupDiGetInterfaceDeviceDetail()"));
			}

			// We now prepare for the next call to
			// SetupDiEnumDeviceInterfaces().

			iDevice++;
			free(pDetail);
			memset(&devInfoData, 0, sizeof(devInfoData));
			devInfoData.cbSize = sizeof(devInfoData);

		} // end of while ( SetupDiEnumDeviceInterfaces() )
	}
#pragma endregion device_bigger_than_1

	SetupDiDestroyDeviceInfoList(hDevInfoSet);

	//printf("going with szDevName =%s\n", szDevName);
	hUSBDevice = CreateFile(szDevName, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ, NULL,
		OPEN_EXISTING, 0, NULL);
	return hUSBDevice;
} // OpenUSBDevice()
static DWORD transportToUSBDevice(HANDLE usbHandle,char *printbuf,size_t count)
{
	DWORD nBytesWritten = 0;
	BOOL  bRetVal;
	//if (nBytesToWrite > MAX_BYTES)
	//{
	//	//  _tprintf(TEXT("Request for too many bytes %d max %d\n"),nBytesToWrite,MAX_BYTES);
	//	nBytesToWrite = MAX_BYTES;
	//}

	bRetVal = WriteFile(usbHandle, printbuf, count, &nBytesWritten, NULL);

	if (!bRetVal)
	{
		/*_tprintf(TEXT("WriteFile() Failed, error = %d\n"),
			GetLastError());
		ReportError(TEXT("Write Failed."));*/
		qDebug() << "usb_transport_error" << GetLastError() << endl;
	}

	assert(bRetVal>0);
	if (bRetVal)
	return nBytesWritten;
	else return 0;

}

static BOOL transportFile(HANDLE hDevice, HANDLE hFile)
{

	DWORD  dwFileSize;
	DWORD  dwTotalBytes;
	DWORD  nBytesWritten, nBytesRead;
	BOOL   bRetVal;
	DWORD dwError;
	//char * pFileBuffer = NULL;
	bool   bBorealisDownload = false;

	

	//dwFileSize = GetFileSize(hFile, NULL);
	dwFileSize = 500;

	// Read the file into memory.

	//pFileBuffer = (char*)malloc(dwFileSize + 4);

	nBytesRead = 0;
	OVERLAPPED oRead = { 0 };
	oRead.OffsetHigh = 0;
	//oRead.Offset = 0x0;
	oRead.Offset = 0;//byte offset 64-bit file position


	//=========read file=====================================================================================
	
	//BYTE *bReadBuffer;
	char *bReadBuffer;
	DWORD dwBytesRead;
	//bRetVal = ReadFile(hFile, pFileBuffer, dwFileSize, &nBytesRead, &oWrite);
	bRetVal = ReadFile(hFile, bReadBuffer, offsetRecord, &nBytesRead, &oRead);

	if (!bRetVal || (nBytesRead != dwFileSize))
	{
		//_tprintf(c_Name(TEXT("ERROR: failed to read entire file.\n")));
	}
	else
	{
		// Is this a Borealis file with @PJL ENTER LANGUAGE=BOR... prepended
		/*if (_strnicmp(pFileBuffer, PJL_ENTERLANG_BOREALIS, strlen(PJL_ENTERLANG_BOREALIS)) == 0)
		{
		bBorealisDownload = true;
		}*/
	}
	dwError = GetLastError();
	if (!bRetVal && (dwError == ERROR_IO_PENDING))
	{
		
		WaitForSingleObject(hFile, INFINITE);		
		dwError = oRead.Internal;
		dwBytesRead = oRead.InternalHigh;

		if (SUCCEEDED(dwError))
		{
			bRetVal = TRUE;
		}
		else
		{
			SetLastError(dwError);
		}
		//_tprintf(c_Name(TEXT("last_error %u \n")), dwError);
	}


	//unsigned char *ucharr = (unsigned char *)pFileBuffer;
	//unsigned char ucharr[500];
	//convertZeroPadedHexIntoByte(pFileBuffer,ucharr);	
	test_writefile2(bReadBuffer);
	//==================================================================================


	// Now send the file.  Since WriteData() refuses to write more
	// than MAX_BYTES per call we must call it from inside a loop.

	//dwTotalBytes = 0;

	//while (dwTotalBytes < dwFileSize)
	//{
	//	nBytesWritten = WriteData(hDevice, pFileBuffer + dwTotalBytes,
	//		dwFileSize - dwTotalBytes);
	//	dwTotalBytes += nBytesWritten;
	//}w

	//// Check the last 12 bytes of the file for the BOR_END_JOB
	//if (bBorealisDownload &&
	//	(dwTotalBytes >= dwFileSize) &&
	//	(dwFileSize >= BOR_HEADER_SIZE))
	//{
	//	UINT32 lastTag = *(UINT32*)(pFileBuffer + dwTotalBytes - BOR_HEADER_SIZE);
	//	if (lastTag != BOR_END_JOB)
	//	{
	//		// No BOR_END_JOB tag, add one
	//		WriteBorJobEnd(hDevice);
	//	}
	//}

	//printf("File download complete, sent %u bytes\n",dwTotalBytes);

	return TRUE;

} // DownloadFile()

       
//void test_writefile(BYTE *writebuffer)
static void test_writefile_3(HANDLE hFile,char *writebuffer, size_t count)
{
	
	
	DWORD dwError;

	/////////////////////////////////////////////////////////////////////////

	if (hFile == INVALID_HANDLE_VALUE)
	{
		_tprintf(_T("Could not create file w/err 0x%08lx\n"), GetLastError());
		//hFile = CreateFile(szFileName, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);

		return;
	}

	DWORD dwBytesWritten;

	BOOL bWriteDone = WriteFile(hFile, writebuffer, count, &dwBytesWritten, 0);

	
}
static void test_writefile(char *writebuffer,size_t count)
{
	HANDLE hFile;
	TCHAR szFileName[] = (L"d:\\aMSDNnm.tmp");
	DWORD dwError;
	 

	

	/////////////////////////////////////////////////////////////////////////

	hFile = CreateFile(szFileName, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		_tprintf(_T("Could not create file w/err 0x%08lx\n"), GetLastError());
		//hFile = CreateFile(szFileName, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);

		return;
	}

	OVERLAPPED oWrite = { 0 };
	oWrite.OffsetHigh = 0;
	oWrite.Offset = offsetRecord;	
	offsetRecord += count;
	
	DWORD dwBytesWritten;

	BOOL bWriteDone = WriteFile(hFile, writebuffer, count, &dwBytesWritten,
		&oWrite);

	// If WriteFile returns TRUE, it indicates that the I/O request was  
	// performed synchronously. At this moment, dwBytesWritten is meaningful.
	// See http://support.microsoft.com/kb/156932

	// Else

	// If the I/O request was NOT performed synchronously (WriteFile returns 
	// FALSE), check to see whether an error occurred or whether the I/O is 
	// being performed asynchronously. (GetLastError() == ERROR_IO_PENDING)
	// At this moment, dwBytesWritten is meaningless.
	dwError = GetLastError();
	if (!bWriteDone && (dwError == ERROR_IO_PENDING))
	{
		
		WaitForSingleObject(hFile, INFINITE);

		// oWrite.Internal contains the I/O error
		// oWrite.InternalHigh contains the number of bytes transferred
		dwError = oWrite.Internal;
		dwBytesWritten = oWrite.InternalHigh;

		if (SUCCEEDED(dwError))
		{
			bWriteDone = TRUE;
		}
		else
		{
			SetLastError(dwError);
		}
	}

	// 4. Handle the result
	// The write operation was finished successfully or failed with an error
	if (bWriteDone)
	{
		// dwBytesWritten contains the number of written bytes
		// bWriteBuffer contains the written data

		// Print the result
		printf("%d bytes were written: %s\n", dwBytesWritten, writebuffer);
	}
	else
	{
		// An error occurred; see dwError

		// Print the error
		_tprintf(_T("AsyncIOWaitForDeviceObject(Write) failed w/err 0x%08lx\n"),
			dwError);
	}

	// 5. Close the file
	CloseHandle(hFile);
}

static void test_writefile2(char *writebuffer)
{
	HANDLE thFile;
	TCHAR szFileName[] = (L"d:\\USBOUTPUTPUTPUT.tmp");
	DWORD dwError;
	/////////////////////////////////////////////////////////////////////////

	thFile = CreateFile(szFileName, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);

	if (thFile == INVALID_HANDLE_VALUE)
	{
		//_tprintf(_T("Could not create file w/err 0x%08lx\n"), GetLastError());
		//hFile = CreateFile(szFileName, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);

		return;
	}

	OVERLAPPED oWrite = { 0 };
	oWrite.OffsetHigh = 0;
	oWrite.Offset = 0;
	
	//int buffersize = sizeof(writebuffer) / sizeof(char);

	DWORD dwBytesWritten;

	BOOL bWriteDone = WriteFile(thFile, writebuffer, offsetRecord, &dwBytesWritten,	&oWrite);
	//	
	dwError = GetLastError();
	if (!bWriteDone && (dwError == ERROR_IO_PENDING))
	{

		WaitForSingleObject(thFile, INFINITE);

		// oWrite.Internal contains the I/O error
		// oWrite.InternalHigh contains the number of bytes transferred
		dwError = oWrite.Internal;
		dwBytesWritten = oWrite.InternalHigh;

		if (SUCCEEDED(dwError))
		{
			bWriteDone = TRUE;
		}
		else
		{
			SetLastError(dwError);
		}
	}

	// 4. Handle the result
	// The write operation was finished successfully or failed with an error
	if (bWriteDone)
	{
		// dwBytesWritten contains the number of written bytes
		// bWriteBuffer contains the written data

		// Print the result
		//printf("%d bytes were written: %s\n", dwBytesWritten, writebuffer);
	}
	else
	{
		// An error occurred; see dwError

		// Print the error
		//_tprintf(_T("AsyncIOWaitForDeviceObject(Write) failed w/err 0x%08lx\n"),	dwError);
	}

	// 5. Close the file
	delete writebuffer;
	CloseHandle(thFile);
}
#endif