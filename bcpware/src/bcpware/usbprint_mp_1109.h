//*****************************************************************************
//2016.10 Timmy updated it from usbprint.c
//*****************************************************************************
//  Copyright (C) 2012 Cambridge Silicon Radio Ltd.
//  $Header: //depot/imgeng/sw/inferno/appsrc/testing/tools/usbprint/usbprint.c#6 $
//  $Change: 201569 $ $Date: 2012/08/10 $
//  
/// @file
/// usbprint - cmdline tool to print to connected usb printer
///
/// also used to download firmware files for reflashing, etc.
///
/// @ingroup Testing
///
/// @author Brian Dodge
//  
//*****************************************************************************
#ifndef USBPRINTMP_H
#define USBPRINTMP_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <io.h>
#include <fcntl.h>
#include <comdef.h>



#include <conio.h> // for getche
#include <time.h> //for time

#define Windows 1
#define pr printf

#define PCJR_CMD_LENGTH 16 //2016.10.26 Timmy sets length = 16

static int i_progress = 0;

#if defined(Windows)
#include <Windows.h>
#include <Setupapi.h>
#include <shlwapi.h>
#include <rpc.h>
#elif defined(Linux)
#include <errno.h>
#include <usb.h>    // libusb-dev must be installed!
#include <unistd.h>
#else
unimplemented
#endif

#ifdef Windows

#define USB_PTP_BULK_WR_EP      0
#define USB_PTP_BULK_RD_EP      1
#define USB_PTP_EVENT_RD_EP     2

static const GUID s_guid_printer =
{ 0x28d78fad, 0x5a12, 0x11D1, { 0xae, 0x5b, 0x00, 0x00, 0xf8, 0x03, 0xa8, 0xc2 } };

static const GUID s_guid_digital_camera =
{ 0x6bdd1fc6, 0x810f, 0x11d0, { 0xbe, 0xc7, 0x08, 0x00, 0x2b, 0xe2, 0x09, 0x2f } };

static const GUID s_guid_pbcamera_channel =
{ 0x6bdd1fc6, 0x810f, 0x11d0, { 0xbe, 0xc7, 0x08, 0x00, 0x2b, 0xe2, 0x09, 0x2f } };

#define MALLOC malloc
#define FREE   free

#endif
#ifdef Linux
#define USB_CLASS_PRINTER       7   

#define MALLOC malloc
#define FREE   free

#define MAX_PATH    512
#define O_BINARY    0

typedef unsigned char BYTE, *PBYTE;
#endif

#define USBIO_READ_BUFFER_SIZE  65536
#define USBIO_WRITE_BUFFER_SIZE 65536 //2016.10.18 Timmy increased from 32768
#define USBIO_INT_BUFFER_SIZE   4096





// USB device descriptor
//
typedef struct tas_usb_dev_desc
{
	int     in_use;
	int     rep, wep, iep;

#ifdef Windows
	HANDLE  hio_wr;
	HANDLE  hio_rd;
	HANDLE  hio_ev;
#elif defined Linux
	struct usb_dev_handle* hio;
	int     busn;
	int     devn;
	int     inum;
#endif
	int     vid;
	int     pid;

	BYTE*   rbuf;
	PBYTE   rp;
	int     nrbuf;

	BYTE*   ibuf;
	PBYTE   ip;
	int     nibuf;

	BYTE*   wbuf;
}
USB_DEV_DESC, *PUSB_DEV_DESC;




static USB_DEV_DESC s_usb =
{
	0,
	0, 0, 0,
#ifdef Windows
	INVALID_HANDLE_VALUE,
	INVALID_HANDLE_VALUE,
	INVALID_HANDLE_VALUE,
#elif defined(Linux)
	NULL,
	0,
	0,
	0,
#endif
	0, 0,

	NULL,
	NULL,
	0,

	NULL,
	NULL,
	0,

	NULL
};

//**************************************************************************
//Timmy added
static void Wait(int ms)
{
	pr("wait %d ms; ", ms);
	Sleep(ms);
}




////***20160429
//auto c_Name = [&](TCHAR* t_x)->char*
//{
//	int size = wcslen(t_x);
//	//char *ansi_string = NULL;
//	char *ansi_string = new char[150];
//	wcstombs(ansi_string, t_x, size + 1);
//	return ansi_string;
//};
//auto t_Name = [&](char* c_x)->TCHAR*
//{
//	int size = strlen(c_x);
//	TCHAR *unicode_string = new TCHAR[200];
//	mbstowcs(unicode_string, c_x, size + 1);
//	return unicode_string;
//};



//**************************************************************************
static int USBgetPort(
#ifdef Windows
	const GUID *devGUID,    ///< [in] guid of device to find
	const GUID *portGUID,   ///< [in] port on device to get, NULL means use device guid
#endif
#ifdef Linux
	int   devClass,         ///< [in] device class to look for
#endif    
	int   nDevice,          ///< [in] which device to attache to by index
	char *pPortName,        ///< [out] port name to use for device
	int  *pvid,             ///< [out] VID of device
	int  *ppid              ///< [out] PID of device
	)
{
#ifdef Windows
	// Windows usb (setupapi)
	BOOL                            rv;
	HDEVINFO                        hDev;
	SP_DEVICE_INTERFACE_DATA        devIntData;
	SP_DEVICE_INTERFACE_DETAIL_DATA *devIntDetail;

	DWORD   cbNeeded;
	BYTE    ditBuf[16384];
	int     index, devdex;
	int     err;

	char  vidstr[32], pidstr[32];
	int   vid, pid;

	pPortName[0] = '\0';

	if (!portGUID)
	{
		portGUID = devGUID;
	}
	// setup api, get class devices: list all devices belonging to the interface
	// or setup clas that are present
	//
	hDev = SetupDiGetClassDevs(
		devGUID,
		NULL,
		NULL,
		DIGCF_PRESENT | DIGCF_INTERFACEDEVICE
		);
	if (hDev == INVALID_HANDLE_VALUE)
	{
		err = GetLastError();
		return err;
	}
	// device index in enumeration
	//
	index = 0;
	devdex = 0;

	do
	{
		// enumerate the device interfaces in the set created above
		//
		devIntData.cbSize = sizeof(devIntData);
		rv = SetupDiEnumDeviceInterfaces(hDev, NULL, portGUID, devdex, &devIntData);

		if (rv)
		{
			devIntDetail = (PSP_DEVICE_INTERFACE_DETAIL_DATA)ditBuf;
			devIntDetail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

			rv = SetupDiGetDeviceInterfaceDetail(hDev, &devIntData, devIntDetail, sizeof(ditBuf), &cbNeeded, NULL);

			if (rv)
			{
				char* ps;

				// extract vid and pid from device path
				// &devIntDetail->DevicePath;
				//
				vidstr[0] = '\0';
				pidstr[0] = '\0';
				_bstr_t b(devIntDetail->DevicePath);
				char *c = b;
				if ((ps = strstr(c, "vid_")) != NULL)
				{
					vid = strtol(ps + 4, NULL, 16);
					_snprintf(vidstr, 32, "%d", vid);
				}
				if ((ps = strstr(c, "pid_")) != NULL)
				{
					pid = strtol(ps + 4, NULL, 16);
					_snprintf(pidstr, 32, "%d", pid);
				}
				strcpy(pPortName, c);
				index++;
			}
			devdex++;
		}
		else
		{
			err = GetLastError();
		}
	} while (rv && (index != (nDevice + 1)));

	SetupDiDestroyDeviceInfoList(hDev);

	// if found a device and at index, all set, err = 0
	//
	err = ((index == (nDevice + 1)) && (pPortName[0] != '\0')) ? 0 : 1;

	if (!err)
	{
		if (pvid)
		{
			*pvid = vid;
		}
		if (ppid)
		{
			*ppid = pid;
		}
	}
#elif defined(Linux)
	// Linux libusb
	static int isinit = 0;

	struct usb_bus    *bus;
	struct usb_device *dev;

	int devn, busn;

	int err = 1;

	if(! isinit)
	{
		usb_init();
		isinit = 1;
	}
	usb_find_busses();
	usb_find_devices();

	for(bus = usb_busses, busn = 0; bus; bus = bus->next, busn++)
	{
		for (dev = bus->devices, devn = 0; dev; dev = dev->next, devn++)
		{
			/*
			printf(
			"pollusb dev %d vid:%d pid:%d cls:%d sc:%d\n",
			devn,
			dev->descriptor.idVendor, dev->descriptor.idProduct
			dev->descriptor.bDeviceClass, dev->descriptor.bDeviceSubClass
			);
			*/
			if(dev->descriptor.bDeviceClass == 0 && dev->descriptor.idVendor != 0)
			{
				int c, i, a;

				/*
				printf("\npoll_usb: found Device %d:%d\n",
				dev->descriptor.idVendor, dev->descriptor.idProduct);
				*/
				for(c = 0; c < dev->descriptor.bNumConfigurations; c++)
				{
					for(i = 0; i < dev->config[c].bNumInterfaces; i++)
					{
						for(a = 0; a < dev->config[c].interface[i].num_altsetting; a++)
						{
							if(dev->config[c].interface[i].altsetting[a].bInterfaceClass == devClass)
							{
								int n, rep, wep, iep;

								rep = 0x81;
								wep = 0;
								iep = 0x82;

								for(n = 0; n < dev->config[c].interface[i].altsetting[a].bNumEndpoints; n++)
								{
									switch(dev->config[c].interface[i].altsetting[a].endpoint[n].bmAttributes & USB_ENDPOINT_TYPE_MASK)
									{
									case USB_ENDPOINT_TYPE_BULK:
										switch(dev->config[c].interface[i].altsetting[a].endpoint[n].bEndpointAddress & USB_ENDPOINT_DIR_MASK)
										{
										case USB_ENDPOINT_IN:
											rep = dev->config[c].interface[i].altsetting[a].endpoint[n].bEndpointAddress;
											break;
										case USB_ENDPOINT_OUT:
											wep = dev->config[c].interface[i].altsetting[a].endpoint[n].bEndpointAddress;
											break;
										}
										break;
									case USB_ENDPOINT_TYPE_INTERRUPT:
										switch(dev->config[c].interface[i].altsetting[a].endpoint[n].bEndpointAddress & USB_ENDPOINT_DIR_MASK)
										{
										case USB_ENDPOINT_IN:
											iep = dev->config[c].interface[i].altsetting[a].endpoint[n].bEndpointAddress;
											break;
										}
										break;
									default:
										break;
									}
								}
								sprintf(pPortName, "%02d:%02d:%02d:%02d:%02d:%02X:%02X:%02X", busn, devn, c, i, a, rep, wep, iep);

								err = 0;
								if (pvid)
								{
									*pvid = dev->descriptor.idVendor;
								}
								if (ppid)
								{
									*ppid = dev->descriptor.idProduct;
								}
								break;
							}
						}
					}
				}
			}
		}
	}
#endif
	return err;
}

#if 1 //Timmy turns on
//**************************************************************************
static int ReadUSBdata(
	PUSB_DEV_DESC pusb,
	int bEventChannel,
	unsigned char *pBuffer,
	int nBuffer,
	int* nRead
	)
{
	int     rv;
	int     attempts;
	//int     nRoom, nGotten;
	unsigned long     nRoom, nGotten;

#ifdef Windows
	OVERLAPPED overlap;
	int        timer;
	//#elif defined(Linux)
#endif

	nRoom = nBuffer;
	rv = 1;
	attempts = 0;

	//pr("bEventChannel=%d\n",bEventChannel);
	if (bEventChannel)
	{
		do
		{
			// copy any buffered bytes first
			if (pusb->nibuf > 0)
			{
				if (pusb->nibuf >= nRoom)
				{
					pusb->nibuf -= nRoom;
					memcpy(pBuffer, pusb->ip, nRoom);
					pusb->ip += nRoom;
					nRoom = 0;
				}
				else
				{
					memcpy(pBuffer, pusb->ip, pusb->nibuf);
					pBuffer += pusb->nibuf;
					nRoom -= pusb->nibuf;
					pusb->nibuf = 0;
				}
			}
			if (pusb->nibuf <= 0)
			{
				pusb->nibuf = 0;
				pusb->ip = pusb->ibuf;
			}
			if (nRoom > 0)
			{
#ifdef Windows
				// event endpoint read directly in recvevent
				nGotten = 0;
				//#elif defined(Linux)
				//                nGotten = usb_bulk_read(pusb->hio, pusb->iep, (char *)pusb->ibuf, USBIO_INT_BUFFER_SIZE, 100);
				//                if(nGotten == -ETIMEDOUT)
				//                {
				//                    return 0;
				//                }
				//                else if(nGotten < 0)
				//                {
				//                    return nGotten;
				//                }
#endif
				if (nGotten > 0)
				{
					//printf("usbre ng=%d toget=%d\n", nGotten, nRoom);
					attempts = 0;
					pusb->nibuf = nGotten;
				}
				else if (nGotten < 0)
				{
					// TODO - unget partial buffer?
					return 0;
				}
				else
				{
					if (nRoom == nBuffer)
					{
						// nothing read, and 0 pending, return 0
						//
						return 0;
					}
					else
					{
						attempts++;
					}
				}
			}
		} while (nRoom > 0 && rv > 0 && attempts < 100);
	}
	else
	{
		do
		{

			// copy any buffered bytes first
			if (pusb->nrbuf > 0)
			{
				if (pusb->nrbuf >= nRoom)
				{
					pusb->nrbuf -= nRoom;
					memcpy(pBuffer, pusb->rp, nRoom);
					pusb->rp += nRoom;
					nRoom = 0;
				}
				else
				{
					memcpy(pBuffer, pusb->rp, pusb->nrbuf);
					pBuffer += pusb->nrbuf;
					nRoom -= pusb->nrbuf;
					pusb->nrbuf = 0;
				}
			}
			if (pusb->nrbuf <= 0)
			{
				pusb->nrbuf = 0;
				pusb->rp = pusb->rbuf;
			}
			if (nRoom > 0)
			{
#ifdef Windows
				memset(&overlap, 0, sizeof(OVERLAPPED));
				rv = ReadFile(pusb->hio_rd, (LPVOID)pusb->rbuf, USBIO_READ_BUFFER_SIZE, &nGotten, &overlap);

				//for(timer = 0; ! rv && (timer < 500); timer++)
				for (timer = 0; !rv && (timer < 60); timer++) // timmy set time out 60 sec

				{
					rv = GetOverlappedResult(pusb->hio_rd, &overlap, &nGotten, FALSE);
					if (timer % 20 == 0)
						pr("\n");
					//pr(" %d) %d ", timer, rv); //Timmy traced
					pr("%2d ", timer + 1); //Timmy traced


					if (!rv)
					{
						int err = GetLastError();
						if (err != ERROR_IO_INCOMPLETE && err != ERROR_IO_PENDING)
						{
							CancelIo(pusb->hio_rd);
							break;
						}
					}
					//Wait(10);
					Wait(1000); //Timmy increased wait time from 10 ms to 1 sec

				}
				//#elif defined(Linux)
				//                nGotten = usb_bulk_read(pusb->hio, pusb->rep, (char *)pusb->rbuf, USBIO_READ_BUFFER_SIZE, 5000);
				//                if(nGotten < 0 && nGotten != -ETIMEDOUT)
				//                {
				//                    rv = 0;
				//                }
#endif
				if (nGotten >= 0)
				{
					attempts = 0;
					pusb->nrbuf = nGotten;
				}
			}
			//pr("temp trace: pBuffer=[%s], nRoom=%d, attempts=%d\n", pBuffer,nRoom, attempts);//timmy traced
		} while (nRoom > 0 && rv > 0 && attempts++ < 30);
		//while(nRoom > 0 && rv <= 0 && attempts++ < 3);//testonly
	}
	nBuffer -= nRoom;

	if (!rv)
	{
		printf("USB Read err\n");
		*nRead = 0;  //debug
		//==============
		//timmy added for debug temp!!!!!!!!!
		//*nRead = nBuffer;
		//pBuffer = 0; 
		//==============
	}
	else
	{
		*nRead = nBuffer;
		/*
		printf("USB Read ends with %d bytes leaves %d\n",
		nBuffer, bEventChannel ? pusb->nibuf : pusb->nrbuf);
		*/
	}
	return rv ? 0 : -1;
}
#endif

//**************************************************************************
static int WriteUSBdata(
	PUSB_DEV_DESC pusb,
	int bEventChannel,
	unsigned char *pBuffer,
	int nBuffer,
	int *nWrote
	)
{

#ifdef Windows
	BOOL        rv;
	int         err;
	int         timer;
	OVERLAPPED  overlap;
	HANDLE      hio;
	DWORD       wc;

	hio = bEventChannel ? pusb->hio_ev : pusb->hio_wr;

	if (hio == INVALID_HANDLE_VALUE)
	{
		return -1;
	}
	memset(&overlap, 0, sizeof(OVERLAPPED));
	rv = WriteFile(pusb->hio_wr, (LPCVOID)pBuffer, nBuffer, &wc, &overlap);
	for (timer = 0; !rv && (timer < 40000); timer++)
	{
		rv = GetOverlappedResult(pusb->hio_wr, &overlap, &wc, FALSE);
		if (!rv)
		{
			err = GetLastError();
			if (err != ERROR_IO_INCOMPLETE && err != ERROR_IO_PENDING)
			{
				break;
			}
		}
		//Wait(2);
		Sleep(2);
	}
	*nWrote = (int)wc;
#elif defined(Linux)

	int rv;
	int attempts;
	int nWritten, nTotal;

	rv = 1;
	attempts = 0;
	nTotal = 0;
	do
	{
		nWritten = usb_bulk_write(pusb->hio, pusb->wep, (char*)pBuffer, nBuffer, 5000);
		if(nWritten >= 0)
		{
			nBuffer -= nWritten;
			pBuffer += nWritten;
			nTotal  += nWritten;
		}
		else
		{
			rv = 0;
		}
	}
	while(nBuffer > 0 && rv > 0 && attempts++ < 30);

	*nWrote = nTotal;
#endif
	if (!rv)
	{
		printf("USB Write err\n");
	}
	/*
	else
	{
	printf("USB wrote %d bytes\n", *nWrote);
	}
	*/
	return rv ? 0 : -1;
}

//**************************************************************************
static int CloseUSB(PUSB_DEV_DESC pusb)
{
#ifdef Windows
	// Windows close
	if (pusb->hio_rd != INVALID_HANDLE_VALUE)
	{
		CloseHandle(pusb->hio_rd);
	}
	if (pusb->hio_wr != INVALID_HANDLE_VALUE && pusb->hio_wr != pusb->hio_rd)
	{
		CloseHandle(pusb->hio_wr);
	}
	if (pusb->hio_ev != INVALID_HANDLE_VALUE)
	{
		CloseHandle(pusb->hio_ev);
	}
	pusb->hio_rd = INVALID_HANDLE_VALUE;
	pusb->hio_wr = INVALID_HANDLE_VALUE;
	pusb->hio_ev = INVALID_HANDLE_VALUE;
#elif defined(Linux)
	// libusb close
	usb_release_interface(pusb->hio, pusb->inum);
	if(pusb->hio)
	{
		usb_close(pusb->hio);
	}
	pusb->hio = NULL;
#endif
	s_usb.in_use = 0;





	//========timmy added
	s_usb.in_use = 0;
	s_usb.rep = 0;
	s_usb.wep = 0;
	s_usb.iep = 0;
	s_usb.hio_ev = INVALID_HANDLE_VALUE;
	s_usb.hio_wr = INVALID_HANDLE_VALUE;
	s_usb.hio_rd = INVALID_HANDLE_VALUE;

	s_usb.vid = 0;
	s_usb.pid = 0;

	s_usb.rbuf = NULL;
	s_usb.rp = NULL;
	s_usb.nrbuf = 0;

	s_usb.ibuf = NULL;
	s_usb.ip = NULL;
	s_usb.nibuf = 0;
	s_usb.wbuf = NULL;
	//s_usb = 0;
	//=========



	return 0;
}

//**************************************************************************
static int OpenUSB(PUSB_DEV_DESC pusb, const char* portBase)
{
#ifdef Windows
	// Windows open
	char          portName[MAX_PATH];
	int           err = 0;

	if (pusb->hio_rd == INVALID_HANDLE_VALUE)
	{
		CloseHandle(pusb->hio_rd);
	}
	if (pusb->hio_wr == INVALID_HANDLE_VALUE)
	{
		CloseHandle(pusb->hio_wr);
	}
	pusb->hio_rd = INVALID_HANDLE_VALUE;
	pusb->hio_wr = INVALID_HANDLE_VALUE;

	pusb->rep = USB_PTP_BULK_RD_EP;
	pusb->wep = USB_PTP_BULK_WR_EP;
	pusb->iep = USB_PTP_EVENT_RD_EP;

	// open bulk channel port
	//
	if (USB_PTP_BULK_RD_EP >= 0)
	{
		_snprintf(portName, MAX_PATH, "%s\\%d", portBase, USB_PTP_BULK_RD_EP);

		pusb->hio_rd = CreateFile(
			(LPCTSTR)portName,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			(LPSECURITY_ATTRIBUTES)NULL,
			OPEN_EXISTING,
			FILE_FLAG_OVERLAPPED,
			NULL
			);
		if (pusb->hio_rd == INVALID_HANDLE_VALUE)
		{
			return -1;
		}
	}
	// open control channel port
	//
	_snprintf(portName, MAX_PATH, "%s\\%d", portBase, USB_PTP_BULK_WR_EP);

	pusb->hio_wr = CreateFile(
		(LPCTSTR)portName,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		(LPSECURITY_ATTRIBUTES)NULL,
		OPEN_EXISTING,
		FILE_FLAG_OVERLAPPED,
		NULL
		);
	if (pusb->hio_wr == INVALID_HANDLE_VALUE)
	{
		CloseHandle(pusb->hio_rd);
		pusb->hio_rd = INVALID_HANDLE_VALUE;
		return -1;
	}
	if (USB_PTP_BULK_RD_EP < 0)
	{
		pusb->hio_rd = pusb->hio_wr;
	}
	// open event channel port
	//
	_snprintf(portName, MAX_PATH, "%s\\%d", portBase, USB_PTP_EVENT_RD_EP);

	pusb->hio_ev = CreateFile(
		(LPCTSTR)portName,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		(LPSECURITY_ATTRIBUTES)NULL,
		OPEN_EXISTING,
		FILE_FLAG_OVERLAPPED,
		NULL
		);
	if (pusb->hio_ev == INVALID_HANDLE_VALUE)
	{
		CloseHandle(pusb->hio_rd);
		CloseHandle(pusb->hio_wr);
		pusb->hio_rd = INVALID_HANDLE_VALUE;
		pusb->hio_wr = INVALID_HANDLE_VALUE;
		return -1;
	}
	printf("Opened USB printer vid=%X pid=%X\n", pusb->vid, pusb->pid);

#elif defined(Linux)
	// libusb open
	struct usb_bus    *bus;
	struct usb_device *dev;

	int busn = strtol(portBase, NULL, 10);
	int devn = strtol(portBase + 3, NULL, 10);
	int cnfn = strtol(portBase + 6, NULL, 10);
	int intn = strtol(portBase + 9, NULL, 10);
	int altn = strtol(portBase + 13, NULL, 10);
	int err;

	pusb->busn  = busn;
	pusb->devn  = devn;

	for(bus = usb_busses; busn > 0 && bus; bus = bus->next)
		busn--;
	if(! bus)
		return -1;
	for(dev = bus->devices; devn > 0 && dev; dev = dev->next)
		devn--;
	if(! dev)
		return -1;

	pusb->hio = usb_open(dev);
	if(! pusb->hio)
	{
		return -1;
	}           
	pusb->rep   = strtol(portBase + 15, NULL, 16);
	pusb->wep   = strtol(portBase + 18, NULL, 16);
	pusb->iep   = strtol(portBase + 21, NULL, 16);
	pusb->inum  = dev->config[cnfn].interface[intn].altsetting[altn].bInterfaceNumber;

	pusb->nrbuf = 0;
	pusb->rp    = pusb->rbuf;
	pusb->nibuf = 0;
	pusb->ip    = pusb->ibuf;

	err = usb_claim_interface(pusb->hio, pusb->inum);
	if(err < 0)
	{
		fprintf(stderr, "Can't claim USB interface %d [%s]\n",
			pusb->inum, strerror(-err));
		usb_close(pusb->hio);
		pusb->hio = NULL;
	}
	else
	{
		printf("Opened USB printer vid=%X pid=%X\n", pusb->vid, pusb->pid);
		err = 0;
	}
#endif
	return err;
}

//**************************************************************************
static int USBopen(int devdex)
{
	int     rc;
	char    portName[MAX_PATH];

	// allow only one user
	//
	if (s_usb.in_use)
	{
		fprintf(stderr, "Port in use\n");
		return -1;
	}
	// get the Nth instance of channel on USB
	//
	rc = USBgetPort(
#ifdef Windows
		&s_guid_printer,
		NULL,
#endif
#ifdef Linux
		USB_CLASS_PRINTER,
#endif
		devdex,
		portName,
		&s_usb.vid,
		&s_usb.pid
		);
	if (rc)
	{
		fprintf(stderr, "No USB device attached\n");
		return rc;
	}
	s_usb.in_use = 1;

	// init buffering
	//
	s_usb.rbuf = (BYTE*)MALLOC(USBIO_READ_BUFFER_SIZE);
	s_usb.ibuf = (BYTE*)MALLOC(USBIO_INT_BUFFER_SIZE);
	s_usb.wbuf = (BYTE*)MALLOC(USBIO_WRITE_BUFFER_SIZE);

	if (!s_usb.rbuf || !s_usb.ibuf || !s_usb.wbuf)
	{
		fprintf(stderr, "Can't alloc I/O buffers\n");
		return -1;
	}
	s_usb.nrbuf = 0;
	s_usb.rp = s_usb.rbuf;

	s_usb.nibuf = 0;
	s_usb.ip = s_usb.ibuf;

	// open the usb channel for r/w
	//
	rc = OpenUSB(&s_usb, portName);
	if (rc)
	{
		fprintf(stderr, "USB port open failed\n");
		s_usb.in_use = 0;
		return rc;
	}
	return 0;
}

//**************************************************************************
static int USBclose()
{
	if (s_usb.rbuf)
		free(s_usb.rbuf);
	if (s_usb.ibuf)
		free(s_usb.ibuf);
	if (s_usb.wbuf)
		free(s_usb.wbuf);


	if (s_usb.in_use)
	{
		//pr("	close inused usb \n");
		CloseUSB(&s_usb);
		s_usb.in_use = 0;
	}
	return 0;
}

//**************************************************************************
static int Useage(const char *progname)
{
	fprintf(stderr, "Use: %s filename [filename ...]\n"
		"  or %s (no arguments) to use standard input as source\n",
		progname, progname);
	return 1;
}




//**************************************************************************
//int Backup_main(int argc, char **argv)
//{
//	char *progname = *argv++;
//	char *fname;
//	int inf;
//	int rc, wc, err;
//	int usestdin;
//
//	char iobuf[USBIO_WRITE_BUFFER_SIZE];
//	printf("Start \n");
//
//	i_progress = 0;
//
//	usestdin = (argc == 1);
//	if (argc-- < 1)
//	{
//		return Useage(progname);
//	}
//	// find the usb printer
//	//
//	err = USBopen(0);
//	if (err)
//	{
//		return err;
//	}
//	while (argc-- > 0 || usestdin)
//	{
//		if (usestdin)
//		{
//			fname = "stdin";
//			inf = _fileno(stdin);
//		}
//		else
//		{
//			fname = *argv++;
//			inf = _open(fname, O_RDONLY | O_BINARY, 0);
//		}
//		if (inf >= 0)
//		{
//			int rtot, wtot;
//
//			printf("Downloading file: [%s]...\n", fname); //Timmy update 
//
//			rtot = wtot = 0;
//			do
//			{
//				rc = _read(inf, iobuf, sizeof(iobuf));
//
//				if (rc > 0)
//				{
//					rtot += rc;
//					err = WriteUSBdata(&s_usb, 0, (PBYTE)iobuf, rc, &wc);
//
//					if (err || (wc != rc))
//					{
//						fprintf(stderr, "Write error %d, wrote %d of %d\n", err, wc, rc);
//						break;
//					}
//					wtot += wc;
//
//					i_progress++;
//					if (i_progress % 10 == 0)
//					{
//						//pr("%d %d %d\n", inf, iobuf, sizeof(iobuf));
//						pr("R: %8d KB, W: %8d KB\n", rtot / 1000, wtot / 1000); //timmy traced                       
//						//pr("RC: %8d KB, WC: %8d KB\n", rc, wc);                       
//					}
//					//Wait(100);
//
//				}
//			} while (rc > 0);
//			printf("complete. %d bytes\n", wtot);
//			if (!usestdin)
//			{
//				_close(inf);
//			}
//		}
//		else
//		{
//			if (!usestdin)
//			{
//				fprintf(stderr, "Can't open %s\n", fname);
//			}
//		}
//		if (usestdin)
//		{
//			break;
//		}
//	}
//	USBclose();
//	pr("End \n"); //Timmy traced
//	return 0;
//}


//**************************************************************************
//updated from original main() of usbprint.c
static void WriteFileOuter(char *fname)
{
	//char *fname;
	int inf;
	int rc, wc, err;

	char iobuf[USBIO_WRITE_BUFFER_SIZE];
	//printf("Start \n");
	//fname = "1.tif";
	i_progress = 0;

	// find the usb printer
	err = USBopen(0);
	if (err)
	{
		//return err;
	}

	inf = _open(fname, O_RDONLY | O_BINARY, 0);


	if (inf >= 0)
	{
		int rtot, wtot;

		printf("	Downloading file: [%s]...\n", fname); //Timmy update 

		rtot = wtot = 0;
		do
		{
			rc = _read(inf, iobuf, sizeof(iobuf));

			if (rc > 0)
			{
				rtot += rc;
				err = WriteUSBdata(&s_usb, 0, (PBYTE)iobuf, rc, &wc);

				if (err || (wc != rc))
				{
					fprintf(stderr, "Write error %d, wrote %d of %d\n", err, wc, rc);
					break;
				}
				wtot += wc;

				i_progress++;
				if (i_progress % 10 == 0)
				{
					//pr("%d %d %d\n", inf, iobuf, sizeof(iobuf));
					pr("		R: %8d KB, W: %8d KB\n", rtot / 1000, wtot / 1000); //timmy traced                      
					//pr("RC: %8d KB, WC: %8d KB\n", rc, wc);                       
				}
				//Wait(100);

			}
		} while (rc > 0);
		printf("	complete. %d bytes\n", wtot);
		_close(inf);

	}
	else
	{
		pr("Error! can not open file \n"); //
	}
	USBclose();
	//pr("End \n");

}


//**************************************************************************
//2016.10.26 Timmy created
static void WriteCmd(char *sCmdShort)
{
	//Write 16 bytes cmd string
	//ex. "1234567890123456"
	//    "PCJRxxxxrrrrCEND"

	//int inf;
	int rc, wc, err;

	char iobuf[USBIO_WRITE_BUFFER_SIZE];
	//char sCmdShort[16] = "PCJRxxxxrrrrCEND";   <= Char Array caused in ERROR!
	//char *sCmdShort = "xxxxrrrr";
	//sCmdShort = "?;xxrrrr";

	printf("write cmd [%s] \n", sCmdShort);

	// find the usb printer
	err = USBopen(0);
	if (err)
	{
		pr("USB OPEN ERROR\n");
		//return err;
	}
	//pr("	USB OPEN OK\n");


	sprintf(iobuf, "PCJR%sCEND", sCmdShort);
	pr("		{%s}\n", iobuf);

	rc = PCJR_CMD_LENGTH;
	err = WriteUSBdata(&s_usb, 0, (PBYTE)iobuf, rc, &wc);
	//pr("	wc = %d\n", wc); 
	if (err)
	{
		pr("	ERROR in write!!!!!\n");
	}
	else
	{
		//pr("	Write OK\n");
	}
	if (wc != rc)
	{
		pr("	Warning w/r counters are different\n");
	}


	//close connection
	USBclose();
	//pr("End \n");
}

//**************************************************************************
static int ReadFlagFromFW()
{
	//int inf;
	int tempc, rc, err;
	int iretry;
	char iobuf[16] = " ";// = "________________";
	tempc = 16;
	rc = 0;

	// find the usb printer
	//printf("USB open \n");
	err = USBopen(0);
	if (err)
	{
		printf("use open err: %d\n", err);
		return err;
	}
	else
	{
		iretry = 0;
		do
		{
			//pr("Read USB buffer: \n");
			err = ReadUSBdata(&s_usb, 0, (PBYTE)iobuf, tempc, &rc);
			if (err)
			{
				pr("ReadUSBdata error or time out \n");
				iretry++;
			}
			else
			{
				//pr("0x%x [%s] %d %d\n", iobuf, iobuf ,tempc ,rc);
				pr("\n		Callback from fw [%s], Addr: 0x%x  Data len = %d, %d\n", iobuf, iobuf, tempc, rc);

				//check status context

				if (strcmp("Status=6;", iobuf) == 0)
				{
					//idle
					pr("		idle\n");
				}
				else
				{
					//status is not idle
					pr("		Status is not idle\n");
					USBclose();
					return 2;
				}

			}
		} while (err && iretry < 30);
	}

	//pr("USB Close \n");
	USBclose();
	return err;  //0
}




static void picassojrprintfunc()
{
	char sfilename[20];
	char ch;
	const int ReadRetryLimit = 30;
	int i = 0;
	int j = 0;
	int rc = 0;
	int iRetest = 0;
	int ipage = 0;
	int itotalpage = 10;
	time_t t0, t1, t2;

	//if (argc > 1)
	//	ch = *argv[1];


	//for (i=0; i < 10000; i++)
	/*while (1)
	{*/

	pr("USB Print Multi-functions - Rev. 2016.1109.A \n");
	pr("\nInput command, press the 'q' to exit:\n");
	pr("  '1' send 1.tif, '2' send 2.tif,  'h': get help from FW \n");
	pr("  's' : check status, 'r': read\n");
	pr("  't' : 10 different pages, 1.tif ~ 10.tif \n");
	pr("  '0' : Reset\n");
	//pr(" 'm': multi-pages 500 pages\n"); 
	pr(">>");

	//ch = _getche();
	pr("\n");



	//test different 10 pages
	itotalpage = 490;

	WriteCmd("tp=490;   "); //check status again
	Wait(1000);

	WriteCmd("status; ");
	rc = ReadFlagFromFW();
	if (rc != 0)
	{
		pr("IO is busy\n");
		//break;
	}



	t0 = time(NULL);


	for (ipage = 1; ipage <= itotalpage; ipage++)
	{
		pr("PAGE [%d] / [%d]\n", ipage, itotalpage);

		t1 = time(NULL);
		t2 = time(NULL);
		pr("	total time: %d \n", t2 - t0);

		//send image
		//WriteFileOuter("1.tif");

		//sprintf(sfilename, "%d.tif", ipage);
		sprintf(sfilename, "./image/%04d.tif", ipage);

		WriteFileOuter(sfilename);

		Wait(5000);

		iRetest = 0;
		// send cmd [call back status] to FW, for check status
		//        12345678
		WriteCmd("status; ");
		Wait(1000);

		//read status and retry
		do
		{
			//read callback from FW
			rc = ReadFlagFromFW();
			pr("[Page #%d], rc=%d, iRetest=%d/%d \n", ipage, rc, iRetest, ReadRetryLimit);

			t2 = time(NULL);
			pr("	total time: %d, ", t2 - t0);
			pr("	page time: %d \n", t2 - t1);


			if (rc == 1)
			{
				pr("	Read Error\n");
				//need to do ?
				iRetest = 9999; //cut test?
				break;
			}
			else if (rc == 2)
			{
				pr("	Unexpect Status, retry...\n");
				//Wait(5000);
				Wait(1000);
				Wait(1000);
				Wait(1000);
				Wait(1000);
				Wait(1000);

				WriteCmd("status; "); //check status again
			}
			else
			{  //rc == 0
				pr("Read status Pass\n");
			}
			iRetest++;

		} while (rc != 0 && iRetest < ReadRetryLimit);


		if (iRetest >= ReadRetryLimit)
		{
			pr("Error: Cancel print due to Over Retest boundary\n");
			break;
		}

		t2 = time(NULL);
		pr("Acc. time: %d, ", t2 - t0);
		pr(" Single page time: [%d] \n", t2 - t1);

		pr("\n---------------------------------------\n");
		Wait(3000);
	}

	pr("END OF MULTIPAGES TEST\n");
	//-------------------------------------------------------
}
//**************************************************************************
//void main_ooo(int argc, char **argv)
//{
//	char sfilename[20];
//	char ch;
//	const int ReadRetryLimit = 30;
//	int i = 0;
//	int j = 0;
//	int rc = 0;
//	int iRetest = 0;
//	int ipage = 0;
//	int itotalpage = 10;
//	time_t t0, t1, t2;
//
//	if (argc > 1) 
//		ch = *argv[1];
//
//
//	//for (i=0; i < 10000; i++)
//	/*while (1)
//	{*/
//
//		pr("USB Print Multi-functions - Rev. 2016.1109.A \n");
//		pr("\nInput command, press the 'q' to exit:\n");
//		pr("  '1' send 1.tif, '2' send 2.tif,  'h': get help from FW \n");
//		pr("  's' : check status, 'r': read\n");
//		pr("  't' : 10 different pages, 1.tif ~ 10.tif \n");
//		pr("  '0' : Reset\n");
//		//pr(" 'm': multi-pages 500 pages\n"); 
//		pr(">>");
//
//		//ch = _getche();
//		pr("\n");
//		switch (ch)  //exit
//		{
//		case ('q') :
//			pr("\nGoodby!\n");
//			return;
//			break;
//
//		case ('1') :
//			WriteFileOuter("0001.tif");
//			break;
//
//		case ('2') :
//			WriteFileOuter("0002.tif");
//			break;
//
//		case ('3') :
//			WriteFileOuter("0003.tif");
//			break;
//
//		case ('m') :
//			//multi-pages 500
//			//-------------------------------------------------------
//			//pr("END OF MULTIPAGES TEST\n");
//			//-------------------------------------------------------
//			break;
//
//		case ('r') :
//			rc = ReadFlagFromFW();
//			if (rc)
//			{
//				pr("Read Error\n");
//			}
//			else if (rc == 2)
//			{
//				pr("Not IDLE Status\n");
//			}
//			else
//			{
//				//pr("Read Pass\n");
//			}
//			break;
//
//		case ('h') :
//			//help from FW
//
//			// send cmd [help;] or [?;] to FW
//			//        12345678
//			WriteCmd("?;______");
//			//WriteCmd("?;xxrrrr");
//			break;
//
//		case ('s') :
//			//status callback
//
//			// send cmd [status;] status callback to FW, for check status
//			//        12345678
//			WriteCmd("status; ");
//			//Wait(1000);
//			rc = ReadFlagFromFW();
//			if (rc)
//			{
//				pr("Read Error\n");
//			}
//			else if (rc == 2)
//			{
//				pr("Not IDLE Status\n");
//			}
//			else
//			{
//				//pr("Read Pass\n");
//			}
//
//
//			break;
//
//		case ('t') :
//			//test different 10 pages
//			itotalpage = 490;
//
//			WriteCmd("tp=490;   "); //check status again
//			Wait(1000);
//
//			WriteCmd("status; ");
//			rc = ReadFlagFromFW();
//			if (rc != 0)
//			{
//				pr("IO is busy\n");
//				break;
//			}
//
//
//
//			t0 = time(NULL);
//
//
//			for (ipage = 1; ipage <= itotalpage; ipage++)
//			{
//				pr("PAGE [%d] / [%d]\n", ipage, itotalpage);
//
//				t1 = time(NULL);
//				t2 = time(NULL);
//				pr("	total time: %d \n", t2 - t0);
//
//				//send image
//				//WriteFileOuter("1.tif");
//
//				//sprintf(sfilename, "%d.tif", ipage);
//				sprintf(sfilename, "./image/%04d.tif", ipage);
//
//				WriteFileOuter(sfilename);
//
//				Wait(5000);
//
//				iRetest = 0;
//				// send cmd [call back status] to FW, for check status
//				//        12345678
//				WriteCmd("status; ");
//				Wait(1000);
//
//				//read status and retry
//				do
//				{
//					//read callback from FW
//					rc = ReadFlagFromFW();
//					pr("[Page #%d], rc=%d, iRetest=%d/%d \n", ipage, rc, iRetest, ReadRetryLimit);
//
//					t2 = time(NULL);
//					pr("	total time: %d, ", t2 - t0);
//					pr("	page time: %d \n", t2 - t1);
//
//
//					if (rc == 1)
//					{
//						pr("	Read Error\n");
//						//need to do ?
//						iRetest = 9999; //cut test?
//						break;
//					}
//					else if (rc == 2)
//					{
//						pr("	Unexpect Status, retry...\n");
//						//Wait(5000);
//						Wait(1000);
//						Wait(1000);
//						Wait(1000);
//						Wait(1000);
//						Wait(1000);
//
//						WriteCmd("status; "); //check status again
//					}
//					else
//					{  //rc == 0
//						pr("Read status Pass\n");
//					}
//					iRetest++;
//
//				} while (rc != 0 && iRetest < ReadRetryLimit);
//
//
//				if (iRetest >= ReadRetryLimit)
//				{
//					pr("Error: Cancel print due to Over Retest boundary\n");
//					break;
//				}
//
//				t2 = time(NULL);
//				pr("Acc. time: %d, ", t2 - t0);
//				pr(" Single page time: [%d] \n", t2 - t1);
//
//				pr("\n---------------------------------------\n");
//				Wait(3000);
//			}
//
//			pr("END OF MULTIPAGES TEST\n");
//			//-------------------------------------------------------
//
//			break;
//		case ('b') :
//			//set band done
//			WriteCmd("b;    ");
//
//			break;
//
//		case ('0') :
//			//reset
//			WriteCmd("reset;  ");
//
//			break;
//
//
//		case ('p') :
//			//page list
//			WriteCmd("pl;    ");
//
//			break;
//
//
//		default:
//			pr(" => unknow cmd!\n");
//
//		}
//		pr("\n");
//	//}
//}


//**************************************************************************
//void main_back()
//{
//	char sfilename[20];
//	char ch;
//	const int ReadRetryLimit = 30;
//	int i = 0;
//	int j = 0;
//	int rc = 0;
//	int iRetest = 0;
//	int ipage = 0;
//	int itotalpage = 10;
//	time_t t0, t1, t2;
//
//
//
//	//for (i=0; i < 10000; i++)
//	while (1)
//	{
//
//		pr("USB Print Multi-functions - Rev. 2016.1109.A \n");
//		pr("\nInput command, press the 'q' to exit:\n");
//		pr("  '1' send 1.tif, '2' send 2.tif,  'h': get help from FW \n");
//		pr("  's' : check status, 'r': read\n");
//		pr("  't' : 10 different pages, 1.tif ~ 10.tif \n");
//		pr("  '0' : Reset\n");
//		//pr(" 'm': multi-pages 500 pages\n"); 
//		pr(">>");
//
//		ch = _getche();
//		pr("\n");
//		switch (ch)  //exit
//		{
//		case ('q') :
//			pr("\nGoodby!\n");
//			return;
//			break;
//
//		case ('1') :
//			WriteFileOuter("0001.tif");
//			break;
//
//		case ('2') :
//			WriteFileOuter("0002.tif");
//			break;
//
//		case ('3') :
//			WriteFileOuter("0003.tif");
//			break;
//
//		case ('m') :
//			//multi-pages 500
//			//-------------------------------------------------------
//			//pr("END OF MULTIPAGES TEST\n");
//			//-------------------------------------------------------
//			break;
//
//		case ('r') :
//			rc = ReadFlagFromFW();
//			if (rc)
//			{
//				pr("Read Error\n");
//			}
//			else if (rc == 2)
//			{
//				pr("Not IDLE Status\n");
//			}
//			else
//			{
//				//pr("Read Pass\n");
//			}
//			break;
//
//		case ('h') :
//			//help from FW
//
//			// send cmd [help;] or [?;] to FW
//			//        12345678
//			WriteCmd("?;______");
//			//WriteCmd("?;xxrrrr");
//			break;
//
//		case ('s') :
//			//status callback
//
//			// send cmd [status;] status callback to FW, for check status
//			//        12345678
//			WriteCmd("status; ");
//			//Wait(1000);
//			rc = ReadFlagFromFW();
//			if (rc)
//			{
//				pr("Read Error\n");
//			}
//			else if (rc == 2)
//			{
//				pr("Not IDLE Status\n");
//			}
//			else
//			{
//				//pr("Read Pass\n");
//			}
//
//
//			break;
//
//		case ('t') :
//			//test different 10 pages
//			itotalpage = 10;
//
//			WriteCmd("tp=10;   "); //check status again
//			Wait(1000);
//
//			WriteCmd("status; ");
//			rc = ReadFlagFromFW();
//			if (rc != 0)
//			{
//				pr("IO is busy\n");
//				break;
//			}
//
//
//
//			t0 = time(NULL);
//
//
//			for (ipage = 1; ipage <= itotalpage; ipage++)
//			{
//				pr("PAGE [%d] / [%d]\n", ipage, itotalpage);
//
//				t1 = time(NULL);
//				t2 = time(NULL);
//				pr("	total time: %d \n", t2 - t0);
//
//				//send image
//				//WriteFileOuter("1.tif");
//
//				//sprintf(sfilename, "%d.tif", ipage);
//				sprintf(sfilename, "%04d.tif", ipage);
//
//				WriteFileOuter(sfilename);
//
//				Wait(5000);
//
//				iRetest = 0;
//				// send cmd [call back status] to FW, for check status
//				//        12345678
//				WriteCmd("status; ");
//				Wait(1000);
//
//				//read status and retry
//				do
//				{
//					//read callback from FW
//					rc = ReadFlagFromFW();
//					pr("[Page #%d], rc=%d, iRetest=%d/%d \n", ipage, rc, iRetest, ReadRetryLimit);
//
//					t2 = time(NULL);
//					pr("	total time: %d, ", t2 - t0);
//					pr("	page time: %d \n", t2 - t1);
//
//
//					if (rc == 1)
//					{
//						pr("	Read Error\n");
//						//need to do ?
//						iRetest = 9999; //cut test?
//						break;
//					}
//					else if (rc == 2)
//					{
//						pr("	Unexpect Status, retry...\n");
//						//Wait(5000);
//						Wait(1000);
//						Wait(1000);
//						Wait(1000);
//						Wait(1000);
//						Wait(1000);
//
//						WriteCmd("status; "); //check status again
//					}
//					else
//					{  //rc == 0
//						pr("Read status Pass\n");
//					}
//					iRetest++;
//
//				} while (rc != 0 && iRetest < ReadRetryLimit);
//
//
//				if (iRetest >= ReadRetryLimit)
//				{
//					pr("Error: Cancel print due to Over Retest boundary\n");
//					break;
//				}
//
//				t2 = time(NULL);
//				pr("Acc. time: %d, ", t2 - t0);
//				pr(" Single page time: [%d] \n", t2 - t1);
//
//				pr("\n---------------------------------------\n");
//				Wait(3000);
//			}
//
//			pr("END OF MULTIPAGES TEST\n");
//			//-------------------------------------------------------
//
//			break;
//		case ('b') :
//			//set band done
//			WriteCmd("b;    ");
//
//			break;
//
//		case ('0') :
//			//reset
//			WriteCmd("reset;  ");
//
//			break;
//
//
//		case ('p') :
//			//page list
//			WriteCmd("pl;    ");
//
//			break;
//
//
//		default:
//			pr(" => unknow cmd!\n");
//
//		}
//		pr("\n");
//	}
//}
#endif