

#ifndef _USETOOL_CPP_
#define _USETOOL_CPP_
#endif // !_USETOOL_CPP_

#include "include.h"
#include "usetool.h"

namespace NString
{
	std::string WtoA(std::wstring wStr)
	{
		if (wStr == L"") return "";
		auto byte{ ::WideCharToMultiByte(CP_ACP, 0, wStr.c_str(), -1, nullptr, 0, nullptr, nullptr) }; //byte include '\0'
		if (!byte) return "";
		std::shared_ptr<char> buffer{ new char[byte]{} };
		if (!buffer) return "";
		std::string result{ (::WideCharToMultiByte(CP_ACP, 0, wStr.c_str(), -1, buffer.get(), byte, nullptr, nullptr) ? buffer.get() : "")};
		return result;
	}

	std::wstring AtoW(std::string aStr)
	{
		if (aStr == "") return L"";
		auto count = ::MultiByteToWideChar(CP_ACP, 0, aStr.c_str(), -1, nullptr, 0); //wide char count include '\0'
		if (!count) return L"";
		std::shared_ptr<wchar_t> buffer{ new wchar_t[count * sizeof(wchar_t)]{} };
		if (!buffer) return L"";
		std::wstring result{ (::MultiByteToWideChar(CP_ACP, 0, aStr.c_str(), -1, buffer.get(), count) ? buffer.get() : L"")};
		return result;
	}

	int ToInt(std::string str)
	{
		int result{};
		std::stringstream{ str } >> result;
		return result;
	}
	long long ToLonglong(std::string str)
	{
		long long result{};
		std::stringstream{ str } >> result;
		return result;
	}
	float ToFloat(std::string str)
	{
		float result{};
		std::stringstream{ str } >> result;
		return result;
	}
	double ToDouble(std::string str)
	{
		double result{};
		std::stringstream{ str } >> result;
		return result;
	}
	std::string ToString(double number, std::streamsize place)
	{
		std::stringstream ss{};
		ss << std::fixed << std::setprecision(place) << number;
		return ss.str();
	}

	std::string Trim(std::string str)
	{
		str.erase(0, str.find_first_not_of(' ')); //erase second param can overflow
		str.erase(str.find_last_not_of(' ') + 1); //erase first param cannot overflow, size_t max + 1 = 0，"" [0] is '\0'
		return str;
	}

	bool Split(std::string str, _OUT_ std::vector<std::string>& data, std::vector<std::string> delimiterList, bool trim)
	{
		std::vector<std::string> result{};
		size_t location{};
		for (const auto& traverse : delimiterList)
		{
			location = str.find(traverse);
			if (location == std::string::npos) return false;
			result.emplace_back(trim ? NString::Trim(str.substr(0, location)) : str.substr(0, location));
			str.erase(0, location + traverse.size());
		}
		result.emplace_back(trim ? NString::Trim(str) : str);
		data = result;
		return true;
	}
}

namespace NSystem
{
	std::string ExeDir()
	{
		char exePath[_MAX_PATH + 1]{};
		if (!::GetModuleFileNameA(nullptr, exePath, sizeof(exePath))) return "";
		return NFileSystem::GetDir(exePath);
	}

	void Hang()
	{
		while (true) ::Sleep(static_cast<unsigned long>(-1L));
	}

	bool RegCreate(std::string keyName, std::string processCmd)
	{
		char exePath[_MAX_PATH + 1]{};
		if (!::GetModuleFileNameA(nullptr, exePath, sizeof(exePath))) return false;
		std::string regValue{ "\"" + std::string{ exePath } + "\" " + processCmd }; //key value
		HKEY hkey{};
		if (::RegOpenKeyA(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", &hkey) != ERROR_SUCCESS) return false;
		bool result = (::RegSetValueExA(hkey, keyName.c_str(), 0, REG_SZ, reinterpret_cast<const BYTE*>(regValue.c_str()),
			static_cast<DWORD>(regValue.size() + sizeof(regValue.at(0)))) == ERROR_SUCCESS); //create key value
		::RegCloseKey(hkey);
		return result;
	}

	HANDLE MutexCreate(std::string mutexName)
	{
		HANDLE handle{};
		if (handle = ::OpenMutexA(MUTEX_ALL_ACCESS, false, mutexName.c_str()))
		{
			::CloseHandle(handle);
			return nullptr;
		}
		return ::CreateMutexA(nullptr, true, mutexName.c_str());
	}

	void MutexClose(_INOUT_ HANDLE& mutexHandle)
	{
		if (!mutexHandle) return;
		::CloseHandle(mutexHandle);
		mutexHandle = nullptr;
	}

	bool ClipboardCopy(std::string text)
	{
		if (!::OpenClipboard(nullptr)) return false;
		if (!::EmptyClipboard())
		{
			::CloseClipboard();
			return false;
		}
		if (text.empty()) return true;
		size_t byte{ (text.size() + 1) * sizeof(text.at(0)) };
		//global memory
		HGLOBAL hGlobal{ ::GlobalAlloc(GMEM_MOVEABLE, byte) };
		if (!hGlobal)
		{
			::CloseClipboard();
			return false;
		}
		{ //write text
			LPVOID pWrite{ ::GlobalLock(hGlobal) };
			if (!pWrite)
			{
				::GlobalFree(hGlobal);
				::CloseClipboard();
				return false;
			}
			if (!::memcpy(pWrite, text.c_str(), byte))
			{
				::GlobalUnlock(hGlobal);
				::GlobalFree(hGlobal);
				::CloseClipboard();
				return false;
			}
			::GlobalUnlock(hGlobal);
		}
		if (!::SetClipboardData(CF_TEXT, hGlobal))
		{
			::GlobalFree(hGlobal);
			::CloseClipboard();
			return false;
		}
		::CloseClipboard();
		return true;
	}

	bool DPIScale()
	{
		static bool set{ false };
		if (set) return true;
		if (!::SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE)) return false;
		set = true;
		return true;
	}
}

namespace NConsole
{
	bool Create()
	{
		FILE* fp{};
		return (::AllocConsole() && (!::freopen_s(&fp, "CON", "w", stdout)) && (!::freopen_s(&fp, "CON", "r", stdin)));
	}

	bool Minimize()
	{
		HWND hWnd{ ::GetConsoleWindow() };
		if (!hWnd) return false;
		::ShowWindow(hWnd, SW_MINIMIZE);
		return true;
	}

	bool TextColor(int colorNumber)
	{
		HANDLE handle{ ::GetStdHandle(STD_OUTPUT_HANDLE) };
		return (handle && ::SetConsoleTextAttribute(handle, colorNumber));
	}
}

namespace NFileSystem
{
	bool IsExist(std::string path)
	{
		return !::_access(path.c_str(), 0); //success return 0
	}

	bool OnlyRead(std::string path, bool onlyRead)
	{
		return ::SetFileAttributesA(path.c_str(), (onlyRead ? FILE_ATTRIBUTE_READONLY : FILE_ATTRIBUTE_NORMAL));
	}

	bool IsOnlyRead(std::string path)
	{
		return ::_access(path.c_str(), 2); //can write return 0
	}

	bool GetFileSize(std::string path, _OUT_ size_t& byte)
	{
		std::fstream file{ path, std::ios::in | std::ios::binary | std::ios::ate };
		if (!file.is_open()) return false;
		byte = file.tellg();
		file.close();
		return true;
	}

	bool Clear(std::string path, size_t bufferByte)
	{
		if (!NFileSystem::IsExist(path)) return false;
		bool readOnly{ NFileSystem::IsOnlyRead(path) };
		if (!NFileSystem::OnlyRead(path, false))
		{
			NFileSystem::OnlyRead(path, readOnly);
			return false;
		}
		std::fstream file{ path, std::ios::in | std::ios::out | std::ios::binary | std::ios::ate };
		if (!file.is_open())
		{
			NFileSystem::OnlyRead(path, readOnly);
			return false;
		}
		size_t byte{ static_cast<size_t>(file.tellg()) };
		if (!byte)
		{
			NFileSystem::OnlyRead(path, readOnly);
			return true;
		}
		if (!file.seekp(0, std::ios::beg)) //no GetFileSize because close again open
		{
			NFileSystem::OnlyRead(path, readOnly);
			return false;
		}
		file.clear();
		bool result{ true };
		std::shared_ptr<char> buffer{ new char[bufferByte]{} };
		if (!buffer)
		{
			file.close();
			NFileSystem::OnlyRead(path, readOnly);
			return false;
		}
		while (byte >= bufferByte)
		{
			if (!file.write(buffer.get(), bufferByte))
			{
				result = false;
				break;
			}
			byte -= bufferByte;
		}
		if (!file.write(buffer.get(), byte)) result = false;
		file.close();
		NFileSystem::OnlyRead(path, readOnly);
		return result;
	}

	bool Copy(std::string sourcePath, std::string destinationPath)
	{
		return ::CopyFileA(sourcePath.c_str(), destinationPath.c_str(), true);
	}

	bool Rename(std::string oldPath, std::string newPath)
	{
		return !::rename(oldPath.c_str(), newPath.c_str());
	}

	bool Remove(std::string path)
	{
		if (!NFileSystem::IsExist(path)) return true;
		bool attribute{ NFileSystem::IsOnlyRead(path) };
		NFileSystem::OnlyRead(path, false);
		bool result{ !::remove(path.c_str()) };
		if (!result) NFileSystem::OnlyRead(path, attribute);
		return result;
	}

	bool FileNameIllegal(std::string fileName)
	{
		for (const auto& ch : fileName)
		{
			if ((ch == '\\') || (ch == '/') || (ch == ':') || (ch == '*') || (ch == '?') || (ch == '"') || (ch == '<') || (ch == '>') || (ch == '|'))
			{
				return false;
			}
		}
		return true;
	}

	std::string GetFileNameIllegal(std::string fileName)
	{
		std::string result{};
		for (const char& ch : fileName)
		{
			if ((ch != '\\') && (ch != '/') && (ch != ':') && (ch != '*') && (ch != '?') && (ch != '"') && (ch != '<') && (ch != '>') && (ch != '|'))
			{
				result += ch;
			}
		}
		return result;
	}

	std::string GetDir(std::string path)
	{
		auto dir{ path.rfind("\\") };
		return ((dir == std::string::npos) ? "" : path.substr(0, dir));
	}

	std::string GetSuf(std::string path)
	{
		auto point{ path.rfind('.') };
		if ((point == std::string::npos) || (path.rfind('\\') + 1 > point)) return "";
		return path.substr(point);
	}

	std::string GetFileName(std::string path)
	{
		auto point{ path.rfind('.') };
		auto dir{ path.rfind('\\') };
		if ((point != std::string::npos) && (point >= dir + 1)) path.erase(point);
		return path.substr(dir + 1);
	}

	bool MkDir(std::string dirPath)
	{
		auto result{ ::GetFileAttributesA(dirPath.c_str()) };
		return ((!::_mkdir(dirPath.c_str())) || (NFileSystem::IsExist(dirPath) && (result != INVALID_FILE_ATTRIBUTES) && (result & FILE_ATTRIBUTE_DIRECTORY)));
	}

	bool MkAllDir(std::string dirPath)
	{
		std::stack<std::string> dirs{};
		std::string temp{ dirPath };
		dirs.push(temp);
		while ((temp = NFileSystem::GetDir(temp)) != "")
		{
			dirs.push(temp);
		}
		while (!dirs.empty())
		{
			NFileSystem::MkDir(dirs.top());
			dirs.pop();
		}
		return NFileSystem::MkDir(dirPath);
	}

	bool RmDir(std::string dirPath)
	{
		return (!::_rmdir(dirPath.c_str()) || (errno == ENOENT));
	}

	bool RmAllDir(std::string dirPath)
	{
		if (!NFileSystem::IsExist(dirPath)) return true;
		__finddata64_t fInfo{};
		intptr_t fHandle{ ::_findfirst64((dirPath + "\\*").c_str(), &fInfo) };
		if (fHandle == -1) return false;
		do
		{
			if (!::strcmp(fInfo.name, ".") || !::strcmp(fInfo.name, "..") || !(fInfo.attrib & _A_SUBDIR)) continue;
			NFileSystem::RmAllDir(dirPath + "\\" + fInfo.name);
		} while (!::_findnext64(fHandle, &fInfo));
		::_findclose(fHandle);
		return NFileSystem::RmDir(dirPath);
	}

	bool RmAllDirAndFile(std::string dirPath)
	{
		if (!NFileSystem::IsExist(dirPath)) return true;
		__finddata64_t fInfo{};
		intptr_t fHandle{ ::_findfirst64((dirPath + "\\*").c_str(), &fInfo) };
		if (fHandle == -1) return false;
		do
		{
			if (!::strcmp(fInfo.name, ".") || !::strcmp(fInfo.name, "..")) continue;
			if (fInfo.attrib & _A_SUBDIR)
			{
				NFileSystem::RmAllDirAndFile(dirPath + "\\" + fInfo.name);
			}
			else
			{
				NFileSystem::Remove(dirPath + "\\" + fInfo.name);
			}
		} while (!::_findnext64(fHandle, &fInfo));
		::_findclose(fHandle);
		return NFileSystem::RmDir(dirPath);
	}

	bool GetDirSize(std::string dirPath, _OUT_ size_t& byte)
	{
		size_t result{};
		auto callbackFunc{ [](std::string dirPath, __finddata64_t& fInfo, void* data) -> void
		{
			if (fInfo.size >= 0) (*(static_cast<size_t*>(data))) += fInfo.size;
		} };
		if (!NFileSystem::TravDir(dirPath, callbackFunc, &result, true, false, false, "")) return false;
		byte = result;
		return true;
	}

	bool GetFileCount(std::string dirPath, _OUT_ size_t& count, bool onlyFile, bool noRecursion)
	{
		size_t result{};
		static auto callbackFunc{ [](std::string dirPath, __finddata64_t& fInfo, void* data) -> void
		{
			++(*(static_cast<size_t*>(data)));
		} };
		if (!NFileSystem::TravDir(dirPath, callbackFunc, &result, onlyFile, noRecursion, false, "")) return false;
		count = result;
		return true;
	}

	bool TravDir(std::string dirPath,
		_IN_ void (*callbackFunc)(std::string dirPath, _IN_ __finddata64_t& fInfo, _IN_OPT_ void* data),
		_IN_OPT_ void* data,
		bool onlyFile,
		bool noRecursion,
		bool printResult, std::string text)
	{
		if (!callbackFunc) return false;
		if (printResult)
		{
			size_t byteTotal{}, countTotal{};
			size_t byteCur{ 0 }, countCur{ 0 };
			void* ptrArr[]{
				static_cast<void*>(&byteTotal),
				static_cast<void*>(&countTotal),
				static_cast<void*>(&byteCur),
				static_cast<void*>(&countCur),
				static_cast<void*>(callbackFunc),
				static_cast<void*>(data)
			};
			static auto func{ [](std::string dirPath, __finddata64_t& fInfo, void* pPtrArr) -> void
			{
				void** ptrArr{ static_cast<void**>(pPtrArr) };
				size_t& byteTotal{ *(static_cast<size_t*>(ptrArr[0])) };
				size_t& countTotal{ *(static_cast<size_t*>(ptrArr[1])) };
				size_t& byteCur{ *(static_cast<size_t*>(ptrArr[2])) };
				size_t& countCur{ *(static_cast<size_t*>(ptrArr[3])) };
				void (*callbackFunc)(std::string dirPath, __finddata64_t & fInfo, void* data)
				{
					reinterpret_cast<void (*)(std::string, __finddata64_t&, void*)>(ptrArr[4])
				};
				void* data{ static_cast<void*>(ptrArr[5]) };
				++countCur;
				::system("cls");
				std::cout << "Size: " << byteCur / 1024.0 / 1024.0 << " MB / " << byteTotal / 1024.0 / 1024.0 << " MB    "; ::printf("%4.2lf %%\n", (byteCur * 1.0 / byteTotal) * 100);
				std::cout << "Count: " << countCur << " / " << countTotal << std::endl;
				std::cout << "Current: " << fInfo.name << "    Size: " << fInfo.size / 1024.0 / 1024.0 << " MB" << std::endl;
				byteCur += fInfo.size;
				(*callbackFunc)(dirPath, fInfo, data);
			} };
			time_t timeStart{ ::time(nullptr) };
			if (!NFileSystem::GetDirSize(dirPath, byteTotal) ||
				!NFileSystem::GetFileCount(dirPath, countTotal, onlyFile, noRecursion) ||
				!NFileSystem::TravDir(dirPath, func, static_cast<void*>(ptrArr), onlyFile, noRecursion, false, text)
				) return false;
			::system("cls");
			std::cout << (text == "" ? "Finish" : "Finish: " + text) << std::endl;
			std::cout << "Size: " << byteTotal / 1024.0 / 1024.0 << " MB" << std::endl;
			std::cout << "Count: " << countTotal << std::endl;
			NOther::PrintTime(timeStart, ::time(nullptr));
			return true;
		}
		__finddata64_t fInfo{};
		intptr_t fHandle{ ::_findfirst64((dirPath + "\\*").c_str(), &fInfo) };
		if (fHandle == -1) return false; //no this directory
		do //first file
		{
			if (!::strcmp(fInfo.name, "..") || !::strcmp(fInfo.name, ".")) continue;
			if (!onlyFile || !(fInfo.attrib & _A_SUBDIR)) (*callbackFunc)(dirPath, fInfo, data);
			if (!noRecursion && (fInfo.attrib & _A_SUBDIR)) NFileSystem::TravDir(dirPath + "\\" + fInfo.name, callbackFunc, data, onlyFile, noRecursion, false, "");
		} while (!::_findnext64(fHandle, &fInfo)); //next file
		::_findclose(fHandle);
		return true;
	}
}

namespace NKey
{
	bool Down(UINT uMsg, WPARAM wParam, int nVirtKey)
	{
		return ((uMsg == WM_KEYDOWN) && (wParam == nVirtKey));
	}

	bool DownHold(UINT uMsg, WPARAM wParam, LPARAM lParam, int nVirtKey)
	{
		return ((uMsg == WM_KEYDOWN) && (wParam == nVirtKey) && (!(HIWORD(lParam) & KF_REPEAT)));
	}

	bool Up(UINT uMsg, WPARAM wParam, int nVirtKey)
	{
		return ((uMsg == WM_KEYUP) && (wParam == nVirtKey) && (!NKey::IsPushing(nVirtKey)));
	}

	bool IsPushing(int nVirtKey)
	{
		return (::GetKeyState(nVirtKey) & 0x8000);
	}
}

namespace NCommonDialog
{
	bool OpenFileDialog(_OUT_ std::string& path, std::vector<COMDLG_FILTERSPEC> fileSpec)
	{
		NSystem::DPIScale();
		if (FAILED(::CoInitializeEx(nullptr, tagCOINIT::COINIT_APARTMENTTHREADED | tagCOINIT::COINIT_DISABLE_OLE1DDE | tagCOINIT::COINIT_SPEED_OVER_MEMORY))) return false;
		CComPtr<IFileDialog> pFileDialog{};
		if (FAILED(pFileDialog.CoCreateInstance(__uuidof(FileOpenDialog)))) return false;
		if (fileSpec.empty()) fileSpec.emplace_back(COMDLG_FILTERSPEC{ L"", L"*" });
		if (FAILED(pFileDialog->SetFileTypes(static_cast<UINT>(fileSpec.size()), fileSpec.data()))) return false;
		if (FAILED(pFileDialog->Show(nullptr))) return false;
		CComPtr<IShellItem> pItem{};
		if (FAILED(pFileDialog->GetResult(&pItem))) return false;
		LPWSTR itemWPath{};
		if (FAILED(pItem->GetDisplayName(SIGDN::SIGDN_FILESYSPATH, &itemWPath))) return false;
		path = NString::WtoA(itemWPath);
		if (itemWPath) ::CoTaskMemFree(itemWPath);
		::CoUninitialize();
		return true;
	}

	bool SaveFileDialog(_OUT_ std::string& path, std::vector<COMDLG_FILTERSPEC> fileSpec)
	{
		NSystem::DPIScale();
		if (FAILED(::CoInitializeEx(nullptr, tagCOINIT::COINIT_APARTMENTTHREADED | tagCOINIT::COINIT_DISABLE_OLE1DDE | tagCOINIT::COINIT_SPEED_OVER_MEMORY))) return false;
		CComPtr<IFileDialog> pFileDialog{};
		if (FAILED(pFileDialog.CoCreateInstance(__uuidof(FileSaveDialog)))) return false;
		if (FAILED(pFileDialog->SetDefaultExtension(L""))) return false;
		if (fileSpec.empty()) fileSpec.emplace_back(COMDLG_FILTERSPEC{ L"", L"*" });
		if (FAILED(pFileDialog->SetFileTypes(static_cast<UINT>(fileSpec.size()), fileSpec.data()))) return false;
		if (FAILED(pFileDialog->Show(nullptr))) return false;
		CComPtr<IShellItem> pItem{};
		if (FAILED(pFileDialog->GetResult(&pItem))) return false;
		LPWSTR itemWPath{};
		if (FAILED(pItem->GetDisplayName(SIGDN::SIGDN_FILESYSPATH, &itemWPath))) return false;
		path = NString::WtoA(itemWPath);
		if (itemWPath) ::CoTaskMemFree(itemWPath);
		::CoUninitialize();
		return true;
	}

	bool OpenFolderDialog(_OUT_ std::string& path)
	{
		NSystem::DPIScale();
		if (FAILED(::CoInitializeEx(nullptr, tagCOINIT::COINIT_APARTMENTTHREADED | tagCOINIT::COINIT_DISABLE_OLE1DDE | tagCOINIT::COINIT_SPEED_OVER_MEMORY))) return false;
		CComPtr<IFileDialog> pFileDialog{};
		if (FAILED(pFileDialog.CoCreateInstance(__uuidof(FileOpenDialog)))) return false;
		if (FAILED(pFileDialog->SetOptions(_FILEOPENDIALOGOPTIONS::FOS_PICKFOLDERS))) return false;
		if (FAILED(pFileDialog->Show(nullptr))) return false;
		CComPtr<IShellItem> pItem{};
		if (FAILED(pFileDialog->GetResult(&pItem))) return false;
		LPWSTR itemWPath{};
		if (FAILED(pItem->GetDisplayName(SIGDN::SIGDN_FILESYSPATH, &itemWPath))) return false;
		path = NString::WtoA(itemWPath);
		if (itemWPath) ::CoTaskMemFree(itemWPath);
		::CoUninitialize();
		return true;
	}
}

namespace NMath
{
	long long Random(long long lowerLimit, long long upperLimit)
	{
		if (lowerLimit >= upperLimit) return upperLimit;
		static bool none{ true }; //this process only run one random seed
		if (none)
		{
			::srand(static_cast<unsigned int>(::time(nullptr)));
			none = false;
		}
		auto count{ upperLimit - lowerLimit + 1 };
		if (count <= 10) return (::rand() % count + lowerLimit);
		long long number{ 0 };
		for (long long i{ count / 10 }; i > 0; --i) number += (::rand() % 11);
		number += (::rand() % (count % 10));
		return (number + lowerLimit);
	}

	double Sum(_IN_ const std::vector<double>& data)
	{
		if (data.empty()) return 0.0;
		double sum{};
		for (const double& number : data)
		{
			sum += number;
		}
		return sum;
	}

	double Average(_IN_ const std::vector<double>& data)
	{
		if (data.empty()) return 0.0;
		return (NMath::Sum(data) / data.size());
	}

	double Variance(_IN_ const std::vector<double>& data)
	{
		if (data.empty()) return 0.0;
		double average{ NMath::Average(data) };
		double variance{};
		for (const double& number : data)
		{
			variance += ::pow((number - average), 2);
		}
		return (variance / data.size());
	}

	double StandardDeviation(_IN_ const std::vector<double>& data)
	{
		if (data.empty()) return 0.0;
		return ::sqrt(NMath::Variance(data));
	}

	double Gauss(double x, double average, double variance)
	{
		return (std::exp(-0.5 * ::pow((x - average), 2) / variance) / ::sqrt(8.0 * std::atan(1) * variance));
	}
	double Gauss(double x, _IN_ const std::vector<double>& data)
	{
		if (data.empty()) return 0.0;
		return NMath::Gauss(x, NMath::Average(data), NMath::Variance(data));
	}
}

namespace NDebug
{
	void Output(std::string text)
	{
		::OutputDebugStringA((__FUNCTION__ ": " + text).c_str());
	}

	void ErrorInfo()
	{
		DWORD dWord{ ::GetLastError() };
		LPSTR lpStr{};
		if (!::FormatMessageA((FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS),
			nullptr, dWord, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPSTR>(&lpStr), 0, nullptr))
		{
			::OutputDebugStringA(__FUNCTION__ ": " "FormatMessageA fail in " __FUNCTION__ "\n");
			return;
		}
		::OutputDebugStringA((__FUNCTION__ ": " + std::string(lpStr)).c_str());
		::LocalFree(lpStr);
	}

	bool Assert(bool expression, std::string text)
	{
#ifdef DEBUG
		if (expression) return true;
		::_wassert(NString::AtoW(text).c_str(), L"", 0);
		return false;
#else
		return true;
#endif // DEBUG
	}
}

namespace NOther
{
	tm GetTime(time_t stamp)
	{
		time_t t{ ((stamp < 0) ? ::time(nullptr) : stamp) };
		tm T{};
		::localtime_s(&T, &t);
		return T;
	}

	void PrintTime(time_t start, time_t end)
	{
		if (start > end) return;
		time_t timeSpend{ end - start };
		tm tmStart{ NOther::GetTime(start) };
		tm tmEnd{ NOther::GetTime(end)};
		time_t hour{ timeSpend / 3600 }, minute{ timeSpend % 3600 / 60 }, second{ timeSpend % 60 };
		std::cout << "Start: " << (tmStart.tm_year + 1900) << " Y / " << (tmStart.tm_mon + 1) << " M / " << tmStart.tm_mday << " D / " << tmStart.tm_hour << " h / " << tmStart.tm_min << " m / " << tmStart.tm_sec << " s" << std::endl;
		std::cout << "End: " << (tmEnd.tm_year + 1900) << " Y / " << (tmEnd.tm_mon + 1) << " M / " << tmEnd.tm_mday << " D / " << tmEnd.tm_hour << " h / " << tmEnd.tm_min << " m / " << tmEnd.tm_sec << " s" << std::endl;
		std::cout << "Spend: ";
		if (hour) ::printf("%lld h : %02lld m : %02lld s\n", hour, minute, second);
		else if (minute) ::printf("%lld m : %02lld s\n", minute, second);
		else ::printf("%lld s\n", second);
	}

	SIZE GetSystemMetrics(bool subtractTaskbar, bool subtractMenu)
	{
		SIZE size{};
		size.cx = ::GetSystemMetrics(subtractTaskbar ? SM_CXFULLSCREEN : SM_CXSCREEN);
		size.cy = ::GetSystemMetrics(subtractTaskbar ? SM_CYFULLSCREEN : SM_CYSCREEN);
		if (subtractMenu) size.cy -= ::GetSystemMetrics(SM_CYMENU);
		return size;
	}

	bool CoInit(DWORD dwCoInit)
	{
		return SUCCEEDED(::CoInitializeEx(nullptr, dwCoInit));
	}
	void UnCoInit()
	{
		::CoUninitialize();
	}
}
