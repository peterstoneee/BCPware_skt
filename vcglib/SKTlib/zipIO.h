


#include "zip.h"
#include <exception>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
using namespace std;


class zipException :std::exception
{
public:
	zipException(const char* err) :std::exception(err)	{	}
};

struct buffer_t
{
	char* data;
	size_t size;
};

static size_t on_extract(void *arg, unsigned long long offset, const void *data,
	size_t size) {
	struct buffer_t *buf = (struct buffer_t *)arg;
	buf->data = static_cast<char *>(realloc(buf->data, buf->size + size + 1));
	assert(NULL != buf->data);

	memcpy(&(buf->data[buf->size]), data, size);
	buf->size += size;
	buf->data[buf->size] = 0;

	return size;
}
struct EntryNames
{
	std::vector<string> entryName;
};
static int on_extract2file(const char *filename, void *arg)
{
	struct EntryNames *getArgFromLibAndSendBack = (struct EntryNames *)arg;
	getArgFromLibAndSendBack->entryName.push_back(filename);

	return getArgFromLibAndSendBack->entryName.size();
}

class ZipIO
{
public:
	ZipIO(const char* _fileName) :zipFileName(NULL)
	{
		zipFileName = _fileName;
	}

	void NewZipFileAndEntry(const char *entryName)
	{
		zip = zip_open(zipFileName, ZIP_DEFAULT_COMPRESSION_LEVEL, 'w');
		if (zip == NULL)throw zipException("zip open fail");		
		if (zip_entry_open(zip, entryName) != 0) throw zipException("zip Entry Open fail");
		
		//assert(0 == zip_entry_write(zip, buff, BUFFSIZE));
	}
	

	void writeBuffertoEntry(const void * buff, size_t bufsize)
	{
		if (zip_entry_write(zip, buff, bufsize) != 0)throw zipException("write zip fail");
	}
	void closeEntry()
	{
		if (zip_entry_close(zip) != 0)throw zipException("close entry fail");
	}
	

	void ReadZipFileOnEntry(char *entryName, buffer_t &outbuf)
	{
		zip = zip_open(zipFileName, ZIP_DEFAULT_COMPRESSION_LEVEL, 'r');
		if (zip == NULL)throw zipException("zip open fail");
		if (zip_entry_open(zip, entryName) != 0) throw zipException("zip Entry Open fail");
		if (zip_entry_extract(zip, on_extract, &outbuf) != 0)throw zipException("extract zip entry fail");		
	}

	bool extract2File(const char *dir, EntryNames *fileInZip)
	{
		zip = zip_open(zipFileName, ZIP_DEFAULT_COMPRESSION_LEVEL, 'r');
		if (zip == NULL){
			return 0;
			throw zipException("zip open fail");

		}
		if (zip_extract(zipFileName, dir, on_extract2file, fileInZip) != 0)
		{
			return 0;
			throw zipException("zip Entry Open fail");
		}
		else return 1;
		//if (zip_entry_extract(zip, on_extract, &outbuf) != 0)throw zipException("extract zip entry fail");

	}

	void appendZipFile(const char *entryName)
	{
		zip = zip_open(zipFileName, ZIP_DEFAULT_COMPRESSION_LEVEL, 'a');
		if (zip == NULL)throw zipException("zip open fail");
		if (zip_entry_open(zip, entryName) != 0)
		{
			throw zipException("zip Entry Open fail");
		}
	}


	void closeZip()
	{
		zip_close(zip);
	}

	~ZipIO()
	{
		//if(zip!=NULL)zip_close(zip);		
	}

private:
	
	const char *zipFileName;
	struct zip_t *zip;
	//static void WriteDataToZip
	


};
