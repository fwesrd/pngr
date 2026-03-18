#pragma once
#ifndef _USETOOL_H_
#define _USETOOL_H_

#include "include.h"

#include "Template.hpp"

#include "Class.h"
#include "IFile.h"
#include "Control.h"
#include "IDialog.h"
#include "IWinUI.h"
#include "IStrUI.h"

#define _USETOOL_VERSION_ "usetool7.1"
#ifndef _USETOOL_CPP_
#ifdef _DEBUG //can not path of ".\"
#pragma comment(lib, "C:\\MyDirC\\Code\\cpp\\usetool\\" _USETOOL_VERSION_ "\\x64\\Debug\\usetool.lib")
#else
#pragma comment(lib, "C:\\MyDirC\\Code\\cpp\\usetool\\" _USETOOL_VERSION_ "\\x64\\Release\\usetool.lib")
#endif // _DEBUG
#endif // !_USETOOL_CPP_
#undef _USETOOL_VERSION_

namespace NString
{
	std::string WtoA(std::wstring wStr);
	std::wstring AtoW(std::string aStr);

	int ToInt(std::string str);
	long long ToLonglong(std::string str);
	float ToFloat(std::string str);
	double ToDouble(std::string str);
	std::string ToString(double number, std::streamsize place);

	std::string Trim(std::string str); //trim blank at both ends
	bool Split(std::string str, _OUT_ std::vector<std::string>& data, std::vector<std::string> delimiter, bool trim = true); //trim is for each string
}

namespace NSystem
{
	std::string ExeDir(); //exe file in where for .\

	void Hang(); //hang thread forever

	bool RegCreate(std::string keyName, std::string processCmd = "");

	HANDLE MutexCreate(std::string mutexName); //no mutex return true
	void MutexClose(_INOUT_ HANDLE& mutexHandle);

	bool ClipboardCopy(std::string text); //"" is clear

	bool DPIScale();
}

namespace NConsole
{
	bool Create();

	bool Minimize();

	bool TextColor(int colorNumber = 7); //color number refer for SetConsoleTextAttribute
}

namespace NFileSystem
{
	bool IsExist(std::string path);
	bool OnlyRead(std::string path, bool onlyRead);
	bool IsOnlyRead(std::string path);

	bool GetFileSize(std::string path, _OUT_ size_t& byte);
	bool Clear(std::string path, size_t bufferByte = (1024 * 1024)); //write 0

	bool Copy(std::string sourcePath, std::string destinationPath); //destinationPath exist fail
	bool Rename(std::string oldPath, std::string newPath); //newPath exist fail
	bool Remove(std::string path); //path unexist success

	bool FileNameIllegal(std::string fileName); //has no \/:*?"<>|
	std::string GetFileNameIllegal(std::string fileName); //erase \/:*?"<>|
	std::string GetDir(std::string path); //without "\"
	std::string GetSuf(std::string path); //with "."
	std::string GetFileName(std::string path); //without directory and suffix

	bool MkDir(std::string dirPath); //only one layer directory
	bool MkAllDir(std::string dirPath); //all layer directory
	bool RmDir(std::string dirPath); //only empty directory
	bool RmAllDir(std::string dirPath); //only empty directory
	bool RmAllDirAndFile(std::string dirPath); //directory and file

	bool GetDirSize(std::string dirPath, _OUT_ size_t& byte);
	bool GetFileCount(std::string dirPath, _OUT_ size_t& count, bool onlyFile = true, bool noRecursion = false);

	bool TravDir(std::string dirPath, //no directory fail
		_IN_ void (*callbackFunc)(std::string dirPath, _IN_ __finddata64_t& fInfo, _IN_OPT_ void* data), //work when printResult false
		_IN_OPT_ void* data = nullptr, //work when printResult false
		bool onlyFile = true,
		bool noRecursion = false,
		bool printResult = false,
		std::string text = ""); //work when printResult true
}

namespace NKey
{
	bool Down(UINT uMsg, WPARAM wParam, int nVirtKey);

	bool DownHold(UINT uMsg, WPARAM wParam, LPARAM lParam, int nVirtKey); //no repeat

	bool Up(UINT uMsg, WPARAM wParam, int nVirtKey);

	bool IsPushing(int nVirtKey = VK_CONTROL);
}

namespace NCommonDialog
{
	bool OpenFileDialog(_OUT_ std::string& path, std::vector<COMDLG_FILTERSPEC> fileSpec = std::vector<COMDLG_FILTERSPEC>{ COMDLG_FILTERSPEC{ L"all file", L"*" } });

	bool SaveFileDialog(_OUT_ std::string& path, std::vector<COMDLG_FILTERSPEC> fileSpec = std::vector<COMDLG_FILTERSPEC>{ COMDLG_FILTERSPEC{ L"all file", L"*" } });

	bool OpenFolderDialog(_OUT_ std::string& path);
}

namespace NMath
{
	long long Random(long long lowerLimit, long long upperLimit); //lowerLimit > upperLimit = upperLimit

	double Sum(_IN_ const std::vector<double>& data);
	double Average(_IN_ const std::vector<double>& data);
	double Variance(_IN_ const std::vector<double>& data);
	double StandardDeviation(_IN_ const std::vector<double>& data);

	double Gauss(double x, double average, double variance);
	double Gauss(double x, _IN_ const std::vector<double>& data);
}

namespace NDebug //only work in DEBUG
{
	void Output(std::string text); //only work when using debuger

	void ErrorInfo(); //only work when using debuger

	bool Assert(bool expression, std::string text); //expression false will assert and return false
}

namespace NOther
{
	tm GetTime(time_t stamp = -1); //-1 is current time

	void PrintTime(time_t start, time_t end); //start > end = no do

	SIZE GetSystemMetrics(bool subtractTaskbar = false, bool subtractMenu = false); //get srceen size

	bool CoInit(DWORD dwCoInit = (COINIT::COINIT_APARTMENTTHREADED |
			COINIT::COINIT_DISABLE_OLE1DDE |
			COINIT::COINIT_SPEED_OVER_MEMORY));
	void UnCoInit(); //one CoInit need by one UnCoInit
}

#endif // _USETOOL_H_