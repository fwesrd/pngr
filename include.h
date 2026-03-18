#pragma once
#ifndef _INCLUDE_H_
#define _INCLUDE_H_

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// head
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <conio.h> //_getch
#include <time.h>
#include <Windows.h>
#include <windowsx.h>
#include <direct.h> //_mkdir
#include <assert.h>
//c++
#include <iostream>
#include <fstream>
#include <sstream>
#include <string> //getline
#include <memory> //shared_ptr
#include <vector>
#include <list>
#include <stack>
#include <queue>
#include <io.h> //traverse directory, _access
#include <iomanip> //setprecision
#include <thread>
#include <functional>
#include <mutex>
//win
#include <ShObjIdl.h> //COM
#include <atlbase.h> //CComPtr
#include <wrl/client.h> //Microsoft::WRL::ComPtr
#include <wincodec.h>
#pragma comment(lib, "Windowscodecs")
#pragma comment(lib, "Ole32")
//directx
#include <DirectXMath.h>
#include <d2d1.h>
#pragma comment(lib, "d2d1")
#include <dwrite.h>
#pragma comment(lib, "dwrite")
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")
#include <d3dcompiler.h>
#pragma comment(lib, "D3DCompiler.lib")

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// comment
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#define _IN_
#define _OUT_
#define _INOUT_

#define _IN_OPT_
#define _OUT_OPT_
#define _INOUT_OPT_

#define _VIRTUAL_EMPTY_

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// test
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#define TESTA(out) (std::cout << out << std::endl)
#define TESTB(str) ::MessageBoxA(nullptr, (std::string{ str }).c_str(), "TEST", MB_OK)

#endif // !_INCLUDE_H_