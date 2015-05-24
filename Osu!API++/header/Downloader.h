/*
http://www.dreamincode.net/forums/topic/101532-download-file-from-url/
An Internet File Donloader
Can download (large) files from internet and resume the download after an
interuption

Based on an example from the book "The Art of C++" by Herbert Schildt

Jos de Jong, sept 2007
c++ program
*/

// Header file for downloader.
#if defined(_MSC_VER)
	#pragma comment(lib, "Wininet")
#endif

#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#ifdef _DEBUG
	#include <iostream> 
#endif
#include <string>
#include <windows.h>  // sorry... windows depended for now :(
#include <wininet.h>
#include <fstream>

using namespace std;

const int MAX_ERRMSG_SIZE = 80;
const int MAX_FILENAME_SIZE = 512;
const int BUF_SIZE = 10240;             // 10 KB


// Exception class for donwload errors;
class DLExc
{
	private:
		char err[MAX_ERRMSG_SIZE];
	public:
		DLExc(char *exc)
		{
			if (strlen(exc) < MAX_ERRMSG_SIZE)
				strcpy_s(err, exc);
		}

		// Return a pointer to the error message
		const char *geterr()
		{
			return err;
		}
};


// A class for downloading files from the internet
class Downloader
{
	public:
		static Downloader& getInstance();
		bool download(string url, bool reload = false);

	private:
		Downloader();
		~Downloader();
		Downloader(Downloader const&);         
		Downloader& operator=(Downloader const&);
		static Downloader* Instance;

		bool ishttp(char *url);
		bool httpverOK(HINTERNET hIurl);
		unsigned long openfile(char *url, bool reload, ofstream &fout);
};

#endif