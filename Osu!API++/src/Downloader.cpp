/*
http://www.dreamincode.net/forums/topic/101532-download-file-from-url/
An Internet File Donloader
Can download (large) files from internet and resume the download after an
interuption

Based on an example from the book "The Art of C++" by Herbert Schildt

Jos de Jong, sept 2007
c++ program

*/

// A file download subsystem
#include "../header/Downloader.h"
#define CHAR(STR) (char*)STR.c_str()

Downloader* Downloader::Instance = new Downloader;

Downloader& Downloader::getInstance()
{
	return *Downloader::Instance;
}

/**
Download a file

Pass the URL of the file to url

To specify an update function that is called after each buffer is read, pass a
pointer to that function as the third parameter. If no update function is
desired, then let the third parameter default to null.
*/
bool Downloader::download(string url, bool reload)
{
	ofstream fout;              // output stream
	unsigned char buf[BUF_SIZE];// input buffer
	unsigned long numrcved;     // number of bytes read
	unsigned long filelen;      // length of the file on disk
	HINTERNET hIurl = nullptr, 
			  hInet = nullptr;  // internet handles
	unsigned long contentlen;   // length of content
	unsigned long len;          // length of contentlen
	unsigned long total = 0;    // running total of bytes received

	try
	{
		if (!ishttp(CHAR(url)))
			throw DLExc("Must be HTTP url");

		/*
		Open the file spcified by url.
		The open stream will be returned in fout. If reload is true, then any
		preexisting file will be truncated. The length of any preexisting file
		(after possible truncation) is returned.
		*/
		filelen = openfile(CHAR(url), true, fout);

		// See if internet connection is available
		if (InternetAttemptConnect(0) != ERROR_SUCCESS)
			throw DLExc("Can't connect");

		// Open internet connection
		hInet = InternetOpen("downloader", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
		if (hInet == NULL)
			throw DLExc("Can't open connection");

		hIurl = InternetOpenUrl(hInet, CHAR(url), NULL, 0, INTERNET_FLAG_NO_CACHE_WRITE, 0);
		if (hIurl == NULL)
			throw DLExc("Can't open url");

		// Confirm that HTTP/1.1 or greater is supported
		if (!httpverOK(hIurl))
			throw DLExc("HTTP/1.1 not supported");

		// Get content length
		len = sizeof contentlen;
		HttpQueryInfo(hIurl, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, &contentlen, &len, NULL);

		// If existing file (if any) is not complete, then finish downloading
		if (filelen != contentlen && contentlen)
		{
			#ifdef _DEBUG
				cout << "downloading..." << endl;
			#endif
			do
			{
				// Read a buffer of info
				if (!InternetReadFile(hIurl, &buf, BUF_SIZE, &numrcved))
					throw DLExc("Error occurred during download");

				// Write buffer to disk
				fout.write((const char *)buf, numrcved);
				if (!fout.good())
					throw DLExc("Error writing file");

				// update running total
				total += numrcved;

				
			} while (numrcved > 0);
		}
	}
	catch (DLExc)
	{
		fout.close();
		InternetCloseHandle(hIurl);
		InternetCloseHandle(hInet);

		// rethrow the exception for use by the caller
		throw;
	}

	fout.close();
	InternetCloseHandle(hIurl);
	InternetCloseHandle(hInet);

	return true;
}


// Return true if HTTP version of 1.1 or greater
bool Downloader::httpverOK(HINTERNET hIurl)
{
	char str[80];
	unsigned long len = 79;

	if (!HttpQueryInfo(hIurl, HTTP_QUERY_VERSION, &str, &len, NULL))
		return false;

	// First, check major version number
	char *p = strchr(str, '/');
	p++;
	if (*p == '0')
		return false;       // can't use HTTP 0.x

	// Now, find start of minor HTTP version number
	p = strchr(str, '.');
	p++;

	// convert to int
	int minorVerNum = atoi(p);

	if (minorVerNum > 0)
		return true;

	return false;
}

/*
Open the output file, initialize the output stream, and return the file's
length. If reload is true, first truncate any preexisting file
*/
unsigned long Downloader::openfile(char *url, bool reload, ofstream &fout)
{
	char fname[MAX_FILENAME_SIZE] = "data.txt";

	//if (!getfname(url, fname))
	//	throw DLExc("File name error");

	if (!reload)
		fout.open(fname, ios::binary | ios::out | ios::app | ios::ate);
	else
		fout.open(fname, ios::binary | ios::out | ios::trunc);

	if (!fout)
		throw DLExc("Can't open output file");

	// get current file length
	return (unsigned long)fout.tellp();
}

// Confirm that the URL specifies HTTP
bool Downloader::ishttp(char *url)
{
	char str[5] = "";

	// get the first four characters from the URL
	strncpy_s(str, url, 4);

	// convert to lowercase
	for (char *p = str; *p; p++)
		*p = tolower(*p);

	return !strcmp("http", str);
}

Downloader::Downloader(){}
Downloader::~Downloader(){ delete Downloader::Instance; }
Downloader::Downloader(Downloader const&){}
Downloader& Downloader::operator=(Downloader const&){ return *this; }