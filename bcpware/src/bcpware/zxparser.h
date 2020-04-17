#ifndef ZXPARSER_H
#define ZXPARSER_H

#include "ZjRCA.h"
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <vector>


class ZxParser
{
	union Val32{ uint32_t val; unsigned char arr[4]; };
	std::ifstream zxin;
	bool bytesSwap;
	bool isLittleEndian;


private:
	bool getOneChunckHeader(ZJ_HEADER &chunckHeader, std::vector<char> &chunckData)
	{
		static char headerbuf[sizeof(ZJ_HEADER)];
		//read chunck header
		if (!zxin.read(headerbuf, sizeof(ZJ_HEADER)))
			return false;

		if (bytesSwap)
		{
			char *ptr = (char*)&chunckHeader;
			char *head, *tail;
			//swap ZJ_HEADER.cbSize
			head = headerbuf;
			tail = head + (sizeof(chunckHeader.cbSize) - 1);
			*(ptr++) = *(tail--); *(ptr++) = *(tail--); *(ptr++) = *(tail--); *(ptr++) = *tail;
			//swap ZJ_HEADER.Type
			head = (char*)&(((ZJ_HEADER*)headerbuf)->Type);
			tail = head + (sizeof(chunckHeader.Type) - 1);
			*(ptr++) = *(tail--); *(ptr++) = *(tail--); *(ptr++) = *(tail--); *(ptr++) = *tail;
			//swap ZJ_HEADER.dwParam
			head = (char*)&(((ZJ_HEADER*)headerbuf)->dwParam);
			tail = head + (sizeof(chunckHeader.dwParam) - 1);
			*(ptr++) = *(tail--); *(ptr++) = *(tail--); *(ptr++) = *(tail--); *(ptr++) = *tail;
			//swap ZJ_HEADER.wParam
			head = (char*)&(((ZJ_HEADER*)headerbuf)->wParam);
			tail = head + (sizeof(chunckHeader.wParam) - 1);
			*(ptr++) = *(tail--); *(ptr++) = *(tail--);
			//ZJ_HEADER.Signature no need to swap(it contents 'ZZ')
			head = (char*)&(((ZJ_HEADER*)headerbuf)->Signature);
			memcpy(ptr, head, sizeof(chunckHeader.Signature));
		}
		else
			memcpy(&chunckHeader, headerbuf, sizeof(ZJ_HEADER));

		int gg=zxin.tellg();
		//store chunck data to originalChunck
		int nChunckData = chunckHeader.cbSize - sizeof(ZJ_HEADER);
		chunckData.resize(nChunckData);
		char *vecdata = chunckData.data();
		if (!zxin.read(vecdata, nChunckData))
			return false;

		return true;
	}
	bool getOneZJItem(ZJ_ITEM_HEADER &itemHeader, uint32_t &val, const char *data, int ndata)
	{
		if (ndata < sizeof(ZJ_ITEM_HEADER))
			return false;

		if (bytesSwap)
		{
			char *ptr = (char*)&itemHeader;
			const char *head, *tail;
			//swap ZJ_ITEM_HEADER.cbSize
			head = data;
			tail = head + (sizeof(itemHeader.cbSize) - 1);
			*(ptr++) = *(tail--); *(ptr++) = *(tail--); *(ptr++) = *(tail--); *(ptr++) = *tail;
			//swap ZJ_ITEM_HEADER.Item
			head = (const char*)&(((const ZJ_ITEM_HEADER*)data)->Item);
			tail = head + (sizeof(itemHeader.Item) - 1);
			*(ptr++) = *(tail--); *(ptr++) = *tail;
			//ZJ_ITEM_HEADER.Type and ZJ_ITEM_HEADER.bParam no need to swap(They both are only one byte)
			head = (const char*)&(((const ZJ_ITEM_HEADER*)data)->Type);
			memcpy(ptr, head, 2);
		}
		else
			memcpy(&itemHeader, data, sizeof(ZJ_ITEM_HEADER));

		//I guess values of all zjitems are uint32 type, for writing code more easily, I assume all zjitem values are uint32 type.
		//if it's not, return false
		if (itemHeader.cbSize - sizeof(ZJ_ITEM_HEADER) != sizeof(uint32_t))
			return false;
		if (sizeof(uint32_t)+sizeof(ZJ_ITEM_HEADER) > ndata)
			return false;
		const char *itemValBuf = data + sizeof(ZJ_ITEM_HEADER);
		if (bytesSwap)
		{
			char *ptr = (char*)&val;
			*(ptr++) = itemValBuf[3]; *(ptr++) = itemValBuf[2]; *(ptr++) = itemValBuf[1]; *ptr = itemValBuf[0];
		}
		else
			memcpy(&val, itemValBuf, sizeof(uint32_t));

		return true;
	}

public:
	int getZxNPage(const char *zxFileName)
	{
		//open zx file
		zxin.open(zxFileName, std::ios_base::in | std::ios_base::binary);
		if (zxin.fail()) return -1;

		//get zjstream signature
		char signature[5];
		signature[4] = '\0';
		zxin.read(signature, 4);
		if (strcmp(signature, "ZJZJ") == 0) bytesSwap = isLittleEndian ? false : true;
		else if (strcmp(signature, "JZJZ") == 0) bytesSwap = isLittleEndian ? true : false;
		else
		{
			zxin.close();
			return -1;
		}


		ZJ_HEADER zh;
		std::vector<char> vchunckdata;
		//const char *data;
		//int ndata;
		//ZJ_ITEM_HEADER itemHeader;
		//uint32_t val;
		unsigned retval = 0;
		while (getOneChunckHeader(zh, vchunckdata))
		{
			/*if (zh.Type == ZJT_START_DOC)
			{
				data = vchunckdata.data();
				ndata = vchunckdata.size();

				while (true)
				{
					if (!getOneZJItem(itemHeader, val, data, ndata)) break;
					if (itemHeader.Item == ZJI_PAGECOUNT)
					{
						zxin.close();
						return val;
					}

					data += itemHeader.cbSize;
					ndata -= itemHeader.cbSize;
				}
			}*/

			if (zh.Type == ZJT_START_PAGE) ++retval;


			/*if (retval == 505 && i == 4623)
			{
				printf("x");
			}

			fprintf(fff, "i=%d, page=%d\n", i++, retval);
			fflush(fff);*/
		}

		zxin.close();
		return retval;
	}

	//@@@@
	//private:
		//FILE *fff;

public:
	ZxParser()
	{
		//check pc is big or little endian
		Val32 u;
		u.arr[0] = 0x01;
		u.arr[1] = 0x02;
		u.arr[2] = 0x03;
		u.arr[3] = 0x04;
		switch (u.val)
		{
		case UINT32_C(0x01020304): isLittleEndian = false; break;
		case UINT32_C(0x04030201): isLittleEndian = true; break;
		default: break;
		}

		//@@@
		//fff = fopen("whatThe.txt", "wb");
	}
};

#endif