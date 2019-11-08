
//
//#include "../FTPconnector/lib/stdafx.h"
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <stdio.h>
#include <Windows.h>
#include <WinInet.h>
#include <stdio.h> // printf()
#include <stdlib.h> // exit()
#include <conio.h>
#include <atlbase.h>
#include <string>
#include <filesystem>
#include <string>
#include <iostream>
#include <sys/types.h>
#include <errno.h>
#include <vector>
#include <fstream>
#include <windows.h>
#include <RestartManager.h>
#include <ctime>
#include <windows.data.json.h>
#include <atlbase.h>
#include <atlconv.h>
#include <fstream>
#include <iostream>
#include <string> // подключаем строки
#include <vector>//для "масивов" переменной длины
#include <map>// для структур "ключ"-"значение"
#include <sstream> //для разделения строки на слова
#include <filesystem>
#include <string>
#include <iostream>
#include <sys/types.h>
#include <errno.h>
#include <vector>
#include <fstream>
#include <windows.h>
#include <RestartManager.h>
#include <ctime>


#include <atlbase.h>
#include <atlconv.h>



#pragma comment(lib,"Rstrtmgr.lib")
#pragma comment(lib, "runtimeobject.lib")
using namespace ABI::Windows::Data::Json;
using std::string;
using std::cout;
using std::cin;
using std::endl;
#pragma comment (lib, "wininet.lib")

string logfile="log.json";
string lastlog="lastlog.json";
std::int16_t firstelement = 1;
std::map<string, std::vector<string>> FtpInfoMap;
std::map<string, std::vector<string>> OldInfoMap;
std::map<string, string> ExistMAP;
std::map<string, string> DownloadMAP;
std::map<string, string> DeletingMAP;


int Getsize(string path)
{
	int answer = 0;
	HANDLE fH;
	
	std::wstring ws2(path.begin(), path.end());
	fH = CreateFile(ws2.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (fH != INVALID_HANDLE_VALUE)
	{
		unsigned long FileSize = 0;
		FileSize = GetFileSize(fH, NULL);
		answer = FileSize;
		CloseHandle(fH);
	}
	return answer;

}
void showFiles(std::string currentFolder)
{
	std::string newpath = currentFolder + "\\*";
	const char* str = newpath.c_str();

	std::wstring stemp = std::wstring(newpath.begin(), newpath.end());
	LPCWSTR sw = stemp.c_str();

	//std::wcout << str << std::endl;

	WIN32_FIND_DATAW wfd;

	HANDLE const hFind = FindFirstFileW(sw, &wfd);

	setlocale(LC_ALL, "");

	if (INVALID_HANDLE_VALUE != hFind)
	{

		do
		{


			if ((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)//если файл
			{
				std::wstring ws(&wfd.cFileName[0]);
				string str(ws.begin(), ws.end());
				std::string newway = currentFolder + "/" + str;
				std::wstring newwayWS = std::wstring(newway.begin(), newway.end());


				
				ExistMAP.insert({ newway ,std::to_string(Getsize(newway)) });

				
			}
			else //если папка

			{
				WCHAR* t = wfd.cFileName;

				if (t[0] != '.')//файлы НЕ начинаються на "." и ".."
				{
					std::wstring ws(&wfd.cFileName[0]);
					string str(ws.begin(), ws.end());
					std::string newway = currentFolder + "//" + str;
					std::wstring newwayWS = std::wstring(newway.begin(), newway.end());

					//std::wcout << newwayWS << endl;

					//cout << "::::::::::" << newway << endl;


					//ExistMAP.insert({ newway ,newway });
					showFiles(newway);
				}


			}

		} while (NULL != FindNextFileW(hFind, &wfd));

		FindClose(hFind);
	}

}

void download(LPCWSTR server, LPCWSTR login, LPCWSTR pass, LPCWSTR local_file, LPCWSTR remote_file)//Скачивает указаный файл в указаное место
{
	HINTERNET hOpen, hConnection;
	hOpen = InternetOpen(NULL, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
	hConnection = InternetConnectW(hOpen, server, 21, login, pass, INTERNET_SERVICE_FTP, 0, 0);
	if (hConnection)
	{
		cout << "Success hConnection" << endl;
	}
	if (FtpGetFileW(hConnection, local_file, remote_file, true, 0, FTP_TRANSFER_TYPE_UNKNOWN, 0))
	{
		cout << "Success Story" << endl;
	}
	else
	{
		cout << "Epic Fail!" << endl;
	}
	InternetCloseHandle(hConnection);
	InternetCloseHandle(hOpen);
}
void Folderinfo(LPCWSTR server, LPCWSTR login, LPCWSTR pass, LPCWSTR folder)//записывает в джейсон файл информацию о всех файлах в указанном фтп сервере
{
	HINTERNET hOpen, hConnection;
	
	hOpen = InternetOpen(NULL, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
	hConnection = InternetConnectW(hOpen, server, 21, login, pass, INTERNET_SERVICE_FTP, 0, 0);
	if (hConnection)
	{
		cout << "Success hConnection" << endl;


		HINTERNET hFind;
		WIN32_FIND_DATA fileInfo;

		hFind = FtpFindFirstFile(hConnection, folder, &fileInfo, 0, 0);


		std::wstring ws(fileInfo.cFileName);
		std::string curent(ws.begin(), ws.end());


		if ((curent != ".") && (curent != "..")) {

			if ((curent != ".") && (curent != "..")) {

				const char* filefolder = (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ? "FOLDER" : "FILE");
				if (filefolder == "FOLDER")
				{
					std::wstring ws1(fileInfo.cFileName);
					std::string curent(ws1.begin(), ws1.end());
					std::wstring wsfolder = (std::wstring)folder;
					std::string sfolder(wsfolder.begin(), wsfolder.end());
					std::string s = sfolder + "//" + curent;
					std::wstring ws2(s.begin(), s.end());
					LPCWSTR pcstr = ws2.c_str();
					std::wcout << ws2 << endl;

					Folderinfo(server, login, pass, pcstr);
				}
				else
				{

					ULONGLONG FileSize = fileInfo.nFileSizeHigh;
					FileSize <<= sizeof(fileInfo.nFileSizeHigh) * 8; // Push by count of bits
					FileSize |= fileInfo.nFileSizeLow;


					HANDLE fH;
					FILETIME ftWrite;
					FILETIME creationTime;

					SYSTEMTIME createTime;
					SYSTEMTIME updateTime;

					DWORD dwFileSize;



					FileTimeToSystemTime(&fileInfo.ftCreationTime, &createTime);
					FileTimeToSystemTime(&fileInfo.ftLastWriteTime, &updateTime);

					std::wcout << fileInfo.cFileName << "  " << FileSize << "\tCreate=" << createTime.wDay << "." << createTime.wMonth << "." << createTime.wYear <<
						" " << createTime.wHour << ":" << createTime.wMinute << ":" << createTime.wSecond << "\t" << "Update=" << updateTime.wDay << "." << updateTime.wMonth << "." << updateTime.wYear <<
						" " << updateTime.wHour << ":" << updateTime.wMinute << ":" << updateTime.wSecond << std::endl;;
					std::ofstream out(logfile, std::ios::app);
					if (out.is_open())
					{
						std::wstring ws1(fileInfo.cFileName);
						std::string curent(ws1.begin(), ws1.end());

						if (firstelement == 1)
						{
							firstelement = 0;
							out << "\"" << curent << "\":{" << "\"name\":\"" << curent << "\",\"Size\":\"" << FileSize << "\" " <<
								",\"Create\":\"" << createTime.wDay << "." << createTime.wMonth << "." << createTime.wYear << "-" << createTime.wHour << ":" << createTime.wMinute << ":" << createTime.wSecond << "\" " <<
								",\"Update\":\"" << updateTime.wDay << "." << updateTime.wMonth << "." << updateTime.wYear << "-" << updateTime.wHour << ":" << updateTime.wMinute << ":" << updateTime.wSecond << "\"}";


						}
						else
						{


							out << "," << std::endl << "\"" << curent << "\":{" << "\"name\":\"" << curent << "\",\"Size\":\"" << FileSize << "\" " <<
								",\"Create\":\"" << createTime.wDay << "." << createTime.wMonth << "." << createTime.wYear << "-" << createTime.wHour << ":" << createTime.wMinute << ":" << createTime.wSecond << "\" " <<
								",\"Update\":\"" << updateTime.wDay << "." << updateTime.wMonth << "." << updateTime.wYear << "-" << updateTime.wHour << ":" << updateTime.wMinute << ":" << updateTime.wSecond << "\"}";
						}
					}
					out.close();

				}
			}
		}
		

		while (InternetFindNextFile(hFind, &fileInfo) == TRUE)
		{
			
			std::wstring ws(fileInfo.cFileName);
			std::string curent(ws.begin(), ws.end());

			if ((curent != ".") && (curent != "..")) {

				const char* filefolder = (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ? "FOLDER" : "FILE");
				if (filefolder == "FOLDER")
				{
					std::wstring ws1(fileInfo.cFileName);
					std::string curent(ws1.begin(), ws1.end());
					std::wstring wsfolder = (std::wstring)folder;
					std::string sfolder(wsfolder.begin(), wsfolder.end());
					std::string s = sfolder + "/" + curent;
					std::wstring ws2(s.begin(), s.end());
					LPCWSTR pcstr = ws2.c_str();
					std::wcout << ws2 << endl;

					Folderinfo(server, login, pass, pcstr);
				}
				else
				{

					ULONGLONG FileSize = fileInfo.nFileSizeHigh;
					FileSize <<= sizeof(fileInfo.nFileSizeHigh) * 8; // Push by count of bits
					FileSize |= fileInfo.nFileSizeLow;


					HANDLE fH;
					FILETIME ftWrite;
					FILETIME creationTime;

					SYSTEMTIME createTime;
					SYSTEMTIME updateTime;

					DWORD dwFileSize;


					std::wstring wsfolder = (std::wstring)folder;
					std::string sfolder(wsfolder.begin(), wsfolder.end());

					std::string loadpath = sfolder + "/" + curent;
					

					

					FileTimeToSystemTime(&fileInfo.ftCreationTime, &createTime);
					FileTimeToSystemTime(&fileInfo.ftLastWriteTime, &updateTime);

					std::wcout << fileInfo.cFileName << "  " << FileSize <<  "\tCreate=" << createTime.wDay << "." << createTime.wMonth << "." << createTime.wYear <<
						" " << createTime.wHour << ":" << createTime.wMinute << ":" << createTime.wSecond << "\t" << "Update=" << updateTime.wDay << "." << updateTime.wMonth << "." << updateTime.wYear <<
						" " << updateTime.wHour << ":" << updateTime.wMinute << ":" << updateTime.wSecond << std::endl;;

					std::ofstream out(logfile, std::ios::app);
					if (out.is_open())
					{
						std::wstring ws1(fileInfo.cFileName);
						std::string curent(ws1.begin(), ws1.end());

						if (firstelement == 1)
						{
							firstelement = 0;
							out <<"\"" << loadpath << "\":{" << "\"name\":\"" << loadpath << "\",\"Size\":\"" << FileSize << "\" " <<
								",\"Create\":\"" << createTime.wDay << "." << createTime.wMonth << "." << createTime.wYear << "-" << createTime.wHour << ":" << createTime.wMinute << ":" << createTime.wSecond << "\" " <<
								",\"Update\":\"" << updateTime.wDay << "." << updateTime.wMonth << "." << updateTime.wYear << "-" << updateTime.wHour << ":" << updateTime.wMinute << ":" << updateTime.wSecond << "\"}";

						
						}
						else
						{


							out << "," << std::endl << "\"" << loadpath << "\":{" << "\"name\":\"" << loadpath << "\",\"Size\":\"" << FileSize << "\" " <<
								",\"Create\":\"" << createTime.wDay << "." << createTime.wMonth << "." << createTime.wYear << "-" << createTime.wHour << ":" << createTime.wMinute << ":" << createTime.wSecond << "\" " <<
								",\"Update\":\"" << updateTime.wDay << "." << updateTime.wMonth << "." << updateTime.wYear << "-" << updateTime.wHour << ":" << updateTime.wMinute << ":" << updateTime.wSecond << "\"}";
						}
					}
					out.close();



				}
			}
			

		}

		InternetCloseHandle(hFind);
	}
}



void AllFiles(LPCWSTR server, LPCWSTR login, LPCWSTR pass, LPCWSTR folder, std::string* exept)//Скачивает все файлы с соблюдением корневой структуры
{
	HINTERNET hOpen, hConnection;
	hOpen = InternetOpen(NULL, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
	hConnection = InternetConnectW(hOpen, server, 21, login, pass, INTERNET_SERVICE_FTP, 0, 0);
	if (hConnection)
	{
		HINTERNET hFind;
		WIN32_FIND_DATA fileInfo;

		hFind = FtpFindFirstFile(hConnection, folder, &fileInfo, 0, 0);

		PTSTR currentfile = fileInfo.cFileName;
		
		std::wstring ws(fileInfo.cFileName);
		std::string curent(ws.begin(), ws.end());
		

		if ((curent != ".") && (curent != ".."))
		{

			const char* filefolder = (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ? "FOLDER" : "FILE");
			if (filefolder == "FOLDER")
			{
				
				std::wstring wsfolder = (std::wstring)folder;
				std::string sfolder(wsfolder.begin(), wsfolder.end());
				std::string s = sfolder + "//" + curent;
				std::wstring ws(s.begin(), s.end());
				LPCWSTR pcstr = ws.c_str();


				std::wcout << ws << endl;
				if (CreateDirectory(ws.c_str(), NULL))
					cout << "directory create" << endl;
				else
					cout << "error create directory" << endl;
				AllFiles(server, login, pass, pcstr, exept);
			}
			else
			{
				std::wstring wsfolder = (std::wstring)folder;
				std::string sfolder(wsfolder.begin(), wsfolder.end());
				std::string s = sfolder + "//" + curent;
				std::wstring ws(s.begin(), s.end());

				LPCWSTR pcstr = ws.c_str();

				download(server, login, pass, pcstr, pcstr);

			}
		}

		std::int16_t counter=0;

		while (InternetFindNextFile(hFind, &fileInfo) == TRUE)
		{
			std::wstring ws(fileInfo.cFileName);
			std::string curent(ws.begin(), ws.end());


			if ((curent != ".") && (curent != ".."))
			{
				
				const char* filefolder = (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ? "FOLDER" : "FILE");
				if (filefolder == "FOLDER")
				{
					std::wstring wsfolder=(std::wstring)folder;
					std::string sfolder(wsfolder.begin(), wsfolder.end());								   

					std::string s = sfolder+"//"+curent;
					std::wstring ws(s.begin(), s.end());


					LPCWSTR pcstr = ws.c_str();
					
					std::wcout << ws << endl;


					if (CreateDirectory(ws.c_str(), NULL))
						cout << "directory create" << endl;
					else
						cout << "error create directory" << endl;


					AllFiles(server,login,pass, pcstr, exept);
				}
				else
				{

					std::wstring wsfolder = (std::wstring)folder;
					std::string sfolder(wsfolder.begin(), wsfolder.end());
					std::string s = sfolder + "//" + curent;
					std::wstring ws(s.begin(), s.end());

					LPCWSTR pcstr = ws.c_str();
	
					
					
					
					download(server,login, pass, pcstr, pcstr);

				}
			}
		
		}

		InternetCloseHandle(hFind);
	}
}
void CREATETREE(LPCWSTR server, LPCWSTR login, LPCWSTR pass, LPCWSTR folder)
{
	HINTERNET hOpen, hConnection;
	hOpen = InternetOpen(NULL, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
	hConnection = InternetConnectW(hOpen, server, 21, login, pass, INTERNET_SERVICE_FTP, 0, 0);
	if (hConnection)
	{
		HINTERNET hFind;
		WIN32_FIND_DATA fileInfo;

		hFind = FtpFindFirstFile(hConnection, folder, &fileInfo, 0, 0);

		PTSTR currentfile = fileInfo.cFileName;

		std::wstring ws(fileInfo.cFileName);
		std::string curent(ws.begin(), ws.end());


		if ((curent != ".") && (curent != ".."))
		{

			const char* filefolder = (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ? "FOLDER" : "FILE");
			if (filefolder == "FOLDER")
			{

				std::wstring wsfolder = (std::wstring)folder;
				std::string sfolder(wsfolder.begin(), wsfolder.end());
				std::string s;

				if (sfolder != "")
				{
					s = sfolder + "//" + curent;
				}
				else
				{
					s = curent;
				}
				std::wstring ws(s.begin(), s.end());
				LPCWSTR pcstr = ws.c_str();


				std::wcout << ws << endl;
				if (CreateDirectory(ws.c_str(), NULL))
					cout << "directory create" << endl;
				else
					cout << "error create directory" << endl;
				//CREATETREE(server, login, pass, pcstr);
			}
		}

		std::int16_t counter = 0;

		while (InternetFindNextFile(hFind, &fileInfo) == TRUE)
		{
			std::wstring ws(fileInfo.cFileName);
			std::string curent(ws.begin(), ws.end());


			if ((curent != ".") && (curent != ".."))
			{

				const char* filefolder = (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ? "FOLDER" : "FILE");
				if (filefolder == "FOLDER")
				{
					std::wstring wsfolder = (std::wstring)folder;
					std::string sfolder(wsfolder.begin(), wsfolder.end());

					std::string s;
					
					if (sfolder != "")
					{
						s = sfolder + "//" + curent;
					}
					else
					{
						s = curent;
					}
					
					
					
					std::wstring ws(s.begin(), s.end());


					LPCWSTR pcstr = ws.c_str();

					std::wcout << ws << endl;


					if (CreateDirectory(ws.c_str(), NULL))
						cout << "directory create" << endl;
					else
						cout << "error create directory" << endl;


					//CREATETREE(server, login, pass, pcstr);
				}
				
			}

		}

		InternetCloseHandle(hFind);
	}
}
void tokenize(std::string const& str, const char delim,
	std::vector<std::string>& out)
{
	size_t start;
	size_t end = 0;

	while ((start = str.find_first_not_of(delim, end)) != std::string::npos)
	{
		end = str.find(delim, start);
		out.push_back(str.substr(start, end - start));
	}
}

void CreateFTPMAP()
{
	string word; // сюда будем класть считанные строки
	std::ifstream file(logfile); // файл из которого читаем (для линукс путь будет выглядеть по другому)
	const char delim = '\"';
	while (getline(file, word))
	{ // пока не достигнут конец файла класть очередную строку в переменную (s)
		//cout << word << endl; // выводим на экран
		int i = 0, n = 0;
		string  answer;

		while (word[i])
		{
			if (word[i] == '"') word[i] = ' ';
			i++;
		}
		std::stringstream s1(word);
		while (s1 >> answer) { n++; }
		//std::wcout << "-SIZE-" <<n << "-----" << endl;//тут смотрим сколько слов
		if (n == 18)//если строка с данными
		{
			string* words = new string[n];
			i = 0;
			std::stringstream s2(word);
			while (s2 >> answer)
			{
				words[i] = answer;
				//cout << words[i]<<"  " <<i<<"\n";
				i++;
			}
			//	cout << "RESULT=" << words[0] <<"  " << words[4] << "  " << words[8] << "  " << words[12] << "  " << words[16] << "  " <<endl;
			FtpInfoMap.insert({ words[0], {words[4],words[8],words[12],words[16]} });
			delete[]words;
		}
		//std::wcout << "------------------------" << endl;//тут смотрим сколько слов
	}
	file.close(); // обязательно закрываем файл что бы не повредить его
}
void CreateOLDMAP()
{
	string word; // сюда будем класть считанные строки
	std::ifstream file(lastlog); // файл из которого читаем (для линукс путь будет выглядеть по другому)
	const char delim = '\"';
	while (getline(file, word))
	{ // пока не достигнут конец файла класть очередную строку в переменную (s)
		//cout << word << endl; // выводим на экран
		int i = 0, n = 0;
		string  answer;

		while (word[i])
		{
			if (word[i] == '"') word[i] = ' ';
			i++;
		}
		std::stringstream s1(word);
		while (s1 >> answer) { n++; }
		//std::wcout << "-SIZE-" <<n << "-----" << endl;//тут смотрим сколько слов
		if (n == 18)//если строка с данными
		{
			string* words = new string[n];
			i = 0;
			std::stringstream s2(word);
			while (s2 >> answer)
			{
				words[i] = answer;
				//cout << words[i]<<"  " <<i<<"\n";
				i++;
			}
			//	cout << "RESULT=" << words[0] <<"  " << words[4] << "  " << words[8] << "  " << words[12] << "  " << words[16] << "  " <<endl;
			OldInfoMap.insert({ words[0], {words[4],words[8],words[12],words[16]} });
			delete[]words;
		}
		//std::wcout << "------------------------" << endl;//тут смотрим сколько слов
	}
	file.close(); // обязательно закрываем файл что бы не повредить его
}

int main()
{
	///upload((LPCWSTR)"ftp.eimb.ru", (LPCWSTR)"anonymous", (LPCWSTR)"anonymous", L"COMP430D.DOC", L"C://COMP430D.DOC");
	//upload((LPCWSTR)"90.130.70.73", NULL,NULL, L"5MB.zip", L"5MB.zip");
	//download(L"ftp.eimb.ru", NULL, NULL, L"COMP430D.DOC", L"COMP430D.DOC");
	//download(L"90.130.70.73", NULL, NULL, L"5MB.zip", L"5MB.zip");

	//download(L"speedtest.tele2.net", NULL, NULL, L"5MB.zip", L"5MB.zip");

	//download(L"test.rebex.net", L"demo", L"password", L"pub//example//ConsoleClient.png", L"ConsoleClient.png");
	//download(L"vps32180.backup.ukraine.com.ua", L"vps_32180", L"0a47xfYllY2U8R1P", L"static//PLYLIST//20191025.txt", L"20191025.txt");

	//Folderinfo(L"vps32180.backup.ukraine.com.ua", L"vps_32180", L"0a47xfYllY2U8R1P", L"static//PLYLIST//");
	//Folderinfo(L"speedtest.tele2.net", NULL, NULL, L"");
	//Tester();


	std::string exept[] = { "folder1","folder2" };

	//AllFiles(L"speedtest.tele2.net", NULL, NULL, "", exept);




	LPCWSTR FTPSERVER = L"vps32180.backup.ukraine.com.ua";
	LPCWSTR FTPUSER = L"vps_32180";
	LPCWSTR FTPLOGIN = L"0a47xfYllY2U8R1P";
	LPCWSTR folder = L"static//PLYLIST";
	//LPCWSTR folder = L"";


	
	//if (CreateDirectory(folder, NULL))
	//	cout << "directory create" << endl;
	//else
	//	cout << "error create directory" << endl;
		//AllFiles(L"vps32180.backup.ukraine.com.ua", L"vps_32180", L"0a47xfYllY2U8R1P", folder, exept);



		/////////////////////////////////////////////////////////////////////////////////////////////////////

	///////////* СОЗДАЕМ ЛОГФАЙЛ ТОГО, ЧТО СЕЙЧАС НА СЕРВЕРЕ*//////////////  
		std::ofstream out;
		out.open(logfile);
		if (out.is_open()){out<<"{\n";}
		out.close();
		firstelement = 1;

		Folderinfo(FTPSERVER, FTPUSER, FTPLOGIN, folder);
		std::ofstream out2(logfile, std::ios::app);
		if (out2.is_open())
		{
			out2 << "\n}";
		}
		out2.close();
		cout << endl << "logfile from ftp was create" << endl;
		//////////////////


	
	system("pause");


	///////////////////////////*СОЗДАЕТЬСЯ В ПАПКЕ КОРНЕВАЯ СТРУКТУРА, КАК НА ФТП СЕРВЕРЕ*////////////////////////////////////////
	CreateDirectory(folder, NULL); // создаем корневую структуру
	//CREATETREE(FTPSERVER, FTPUSER, FTPLOGIN, folder);//создаем корневую структуру
	cout << endl << "struct was created" << endl;
	////////////////////////////////////////////////////////////////

	system("pause");


	//download(L"vps32180.backup.ukraine.com.ua", L"vps_32180", L"0a47xfYllY2U8R1P", L"static/JINGLES/AZS0145.mp3", L"AZS0145.mp3");

	std::wcout << "---------------FtpInfoMap--------------" << endl;
	CreateFTPMAP();
	for (auto it = FtpInfoMap.begin(); it != FtpInfoMap.end(); ++it)
	{
		cout << it->first << " : " << it->second[0] << "  " << it->second[1] << "  " << it->second[2] << "  " << it->second[3] << endl;
	}


	/////////////Create    log.txt
	std::ofstream rw;
	rw.open("log.txt");
	if (rw.is_open()) { rw << ""; }
	rw.close();
	std::ofstream WriteLog("log.txt", std::ios::app);
	if (WriteLog.is_open())
	{

		//name=C:\\searchtest\1.txt Size=13 Create=29.10.2019-12:8:0 Update=29.10.2019-12:29:11
		for (auto it = FtpInfoMap.begin(); it != FtpInfoMap.end(); ++it)
		{
			WriteLog << "name=" << it -> second[0] << "  Size=" << it->second[1] << "  Create=" << it->second[2] << "  Update=" << it->second[3]<<endl;
		}
	}
	WriteLog.close();



	std::wcout << "---------------OLDInfoMap--------------" << endl;
	CreateOLDMAP();
	for (auto it = OldInfoMap.begin(); it != OldInfoMap.end(); ++it)
	{
		cout << it->first << " : " << it->second[0] << "  " << it->second[1] << "  " << it->second[2] << "  " << it->second[3] << endl;
	}

	std::wcout << "---------------EXISTARRAY--------------" << endl;

	std::wstring folderws = folder;
	std::string folderS(folderws.begin(), folderws.end());
	showFiles(folderS);

	for (auto it = ExistMAP.begin(); it != ExistMAP.end(); ++it)
	{
		cout << it->first << " : " << it->second<< endl;
	}
	
	std::wcout << "---------------DELETEARRAY--------------" << endl;
	
	for (auto it = FtpInfoMap.begin(); it != FtpInfoMap.end(); ++it)
	{
		string key;
		//cout << it->first << " : " << it->second[0] << "  " << it->second[1] << "  " << it->second[2] << "  " << it->second[3] << endl;
		if(OldInfoMap.find(it->first) == OldInfoMap.end()) {
			//cout << "NOT FOUND" << endl;
		}
		else {
			if (it->second[1] != OldInfoMap.find(it->first)->second[1])
			{	
				//cout << it->second[1] << "-------" << OldInfoMap.find(it->first)->second[1] << endl;
				key = OldInfoMap.find(it->first)->second[0];
				//	cout << "FOUND=" <<key << endl;
				DeletingMAP.insert({ key,key });
			}
			else if(it->second[3] != OldInfoMap.find(it->first)->second[3])
			{
				//cout << it->second[3] << "-------" << OldInfoMap.find(it->first)->second[3] << endl;
				key = OldInfoMap.find(it->first)->second[0];
				//	cout << "FOUND=" <<key << endl;
				DeletingMAP.insert({ key,key });
			}
			
			
		}
	}
	for (auto it = OldInfoMap.begin(); it != OldInfoMap.end(); ++it)
	{
		string key;
		//cout << it->first << " : " << it->second[0] << "  " << it->second[1] << "  " << it->second[2] << "  " << it->second[3] << endl;
		if (FtpInfoMap.find(it->first) == FtpInfoMap.end()) {
			cout << it->first  << endl;
			key = OldInfoMap.find(it->first)->second[0];
			//	cout << "FOUND=" <<key << endl;
			DeletingMAP.insert({ key,key });
		}
	}
	///////////СМОТРИМ НА НОВЫЕ ФАЙЛЫ В ПАПКЕ И УДАЛЯЕМ ИХ НАФИГ
	for (auto it = ExistMAP.begin(); it != ExistMAP.end(); ++it)
	{
		string key;
		//cout << it->first << " : " << it->second << endl;
		if (FtpInfoMap.find(it->first) == FtpInfoMap.end())
		{
			key = it->first;
			cout << "NEW FILE ON PC" << it->first << endl;
			DeletingMAP.insert({ key,key });
		}
		else
		{
			if ((it->second != FtpInfoMap.find(it->first)->second[1]))
			{
				key = it->first;
				DeletingMAP.insert({ key,key });
			}
		}
	}



	//ВЫВОДИМ МАСИВ ФАЙЛОВ НА УДАЛЕНИЕ
	for (auto it = DeletingMAP.begin(); it != DeletingMAP.end(); ++it)
	{
		cout << it->first << " : " << it->second << endl;
	}

	std::wcout << "---------------DOWNLOADARRAY--------------" << endl;



		
	for (auto it = FtpInfoMap.begin(); it != FtpInfoMap.end(); ++it)
	{
		string key;
		
		if (OldInfoMap.find(it->first) == OldInfoMap.end()) //Если Файла нет у нас на компе
		{
			//NOT FOUND
			key = it->first;
			DownloadMAP.insert({ key,key });
		}

		else
		{
			if ((it->second[1] != OldInfoMap.find(it->first)->second[1])|| (it->second[3] != OldInfoMap.find(it->first)->second[3]))
			{
				key = it->first;
				DownloadMAP.insert({ key,key });
			}
		
		}

	}
	for (auto it = FtpInfoMap.begin(); it != FtpInfoMap.end(); ++it)
	{
		string key;

		if (ExistMAP.find(it->first) == ExistMAP.end()) //Если Файла нет у нас на компе
		{
			//NOT FOUND
			key = it->first;
			DownloadMAP.insert({ key,key });
		}

		else
		{
			//cout << "EXIST" << endl;
			if ((it->second[1] != ExistMAP.find(it->first)->second))
			{
				key = it->first;
				DownloadMAP.insert({ key,key });
			}

		}

	}



	for (auto it = DownloadMAP.begin(); it != DownloadMAP.end(); ++it)
	{
		cout << it->first << " : " << it->second << endl;
	}

	//std::wcout << "---------------NOW DELETE OLD FILES--------------" << endl;
	system("pause");
	for (auto it = DeletingMAP.begin(); it != DeletingMAP.end(); ++it)
	{
		//cout << it->first << " : " << it->second << endl;
		
		try {
			remove(it->first.c_str());
		}
		catch (int a)
		{}
	}

	std::wcout << "---------------NOW DOWNLOAD NEW  FILES--------------" << endl;
	system("pause");
	for (auto it = DownloadMAP.begin(); it != DownloadMAP.end(); ++it)
	{
		string s = it->first;
		std::wstring ws(s.begin(), s.end());

		LPCWSTR pcstr = ws.c_str();
		download(FTPSERVER, FTPUSER, FTPLOGIN, pcstr, pcstr);

		//CREATETREE(FTPSERVER, FTPUSER, FTPLOGIN, folder);//создаем корневую структуру

	}


	
	//копируем файловую структуру на момент запуска скрипта, для дальнейших действий
	std::ifstream    inFile(logfile);
	std::ofstream    outFile(lastlog);

	outFile << inFile.rdbuf();



	system("pause");
}


