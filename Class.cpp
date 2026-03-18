#include "Class.h"

namespace NString
{
	extern long long ToLonglong(std::string str);
	extern std::string ToString(double number, std::streamsize place);
}

namespace NDebug
{
	extern bool Assert(bool expression, std::string text);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// CEvent
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

CEvent::operator bool()
{
	return CEvent::hEvent;
}
bool CEvent::operator!()
{
	return (!CEvent::hEvent);
}

bool CEvent::Create(bool initSet, bool manualReset)
{
	return (CEvent::Close() && (CEvent::hEvent = ::CreateEventA(nullptr, manualReset, initSet, nullptr)));
}
bool CEvent::Set()
{
	return (CEvent::hEvent && ::SetEvent(CEvent::hEvent));
}
bool CEvent::Wait(DWORD waitTimeMs)
{
	if (!(CEvent::hEvent)) return false;
	DWORD result{ ::WaitForSingleObject(CEvent::hEvent, waitTimeMs) };
	return ((result == WAIT_OBJECT_0) || (result == WAIT_TIMEOUT));
}
bool CEvent::Wait(DWORD waitTimeMs, bool& timeOut)
{
	if (!(CEvent::hEvent)) return false;
	DWORD result{ ::WaitForSingleObject(CEvent::hEvent, waitTimeMs) };
	if (result == WAIT_OBJECT_0)
	{
		timeOut = false;
	}
	else if (result == WAIT_TIMEOUT)
	{
		timeOut = true;
	}
	else
	{
		return false;
	}
	return true;
}
bool CEvent::Reset()
{
	return (CEvent::hEvent && ::ResetEvent(CEvent::hEvent));
}
bool CEvent::Close()
{
	if (!(CEvent::hEvent)) return true;
	if (!::CloseHandle(CEvent::hEvent)) return false;
	CEvent::hEvent = nullptr;
	return true;
}

CEvent::CEvent() : hEvent{ nullptr }
{}

CEvent::~CEvent()
{
	CEvent::Close();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// CSemaphore
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

CSemaphore::operator bool()
{
	return CSemaphore::hSemaphore;
}
bool CSemaphore::operator!()
{
	return (!CSemaphore::hSemaphore);
}

bool CSemaphore::Create(LONG initCount, LONG maxCount)
{
	if (initCount < 0L) initCount = 0L;
	if (maxCount < 1L) maxCount = 1L;
	CSemaphore::initCount = initCount;
	CSemaphore::maxCount = maxCount;
	return (CSemaphore::Close() && (CSemaphore::hSemaphore = ::CreateSemaphoreW(nullptr, initCount, maxCount, nullptr)));
}
bool CSemaphore::Add(LONG addCount)
{
	return (CSemaphore::hSemaphore && ::ReleaseSemaphore(CSemaphore::hSemaphore, addCount, nullptr));
}
bool CSemaphore::Wait(DWORD waitTimeMs)
{
	if (!CSemaphore::hSemaphore) return false;
	DWORD result{ ::WaitForSingleObject(CSemaphore::hSemaphore, waitTimeMs) };
	return ((result == WAIT_OBJECT_0) || (result == WAIT_TIMEOUT));
}
bool CSemaphore::Wait(DWORD waitTimeMs, bool& timeOut)
{
	if (!CSemaphore::hSemaphore) return false;
	DWORD result{ ::WaitForSingleObject(CSemaphore::hSemaphore, waitTimeMs) };
	if (result == WAIT_OBJECT_0)
	{
		timeOut = false;
	}
	else if (result == WAIT_TIMEOUT)
	{
		timeOut = true;
	}
	else
	{
		return false;
	}
	return true;
}
bool  CSemaphore::Close()
{
	if (!(CSemaphore::hSemaphore)) return true;
	if (!::CloseHandle(CSemaphore::hSemaphore)) return false;
	CSemaphore::hSemaphore = nullptr;
	CSemaphore::initCount = 0U;
	CSemaphore::maxCount = 0U;
	return true;
}

CSemaphore::CSemaphore() : hSemaphore{ nullptr }, initCount{}, maxCount{}
{}
CSemaphore::~CSemaphore()
{
	CSemaphore::Close();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// CNumber
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

const long long& CNumber::Data() const
{
	return CNumber::data;
}
const CNumber::SPlace& CNumber::PointPlace() const
{
	return CNumber::pointPlace;
}

bool CNumber::operator!() const
{
	return (!(CNumber::data));
}
CNumber::operator bool() const
{
	return CNumber::data;
}
CNumber::operator long long() const
{
	if (CNumber::pointPlace == 0U) return CNumber::data;
	std::string temp{ (std::string)(*this) };
	return NString::ToLonglong(temp.substr(0, temp.find(".")).c_str());
}
CNumber::operator int() const
{
	return static_cast<int>((long long)(*this));
}
CNumber::operator double() const
{
	return (CNumber::data / ::pow(10.0, CNumber::pointPlace));
}
CNumber::operator float() const
{
	return static_cast<float>((double)(*this));
}
CNumber::operator std::string() const
{
	std::string result{ std::to_string(CNumber::data) };
	if (CNumber::pointPlace == 0) return result;
	std::string sign{};
	if (result.at(0) == '-')
	{
		sign = "-";
		result.erase(0, 1);
	}
	if (result.size() > CNumber::pointPlace)
	{
		result.insert(result.end() - CNumber::pointPlace, '.');
	}
	else
	{
		for (CNumber::SPlace i{ static_cast<CNumber::SPlace>(CNumber::pointPlace - result.size()) }; i > 0; --i)
		{
			result.insert(result.begin(), '0');
		}
		result.insert(result.begin(), '.');
		result.insert(result.begin(), '0');
	}
	CNumber::SPlace pointPosition{ static_cast<CNumber::SPlace>(result.size() - CNumber::pointPlace - 1) };
	CNumber::SPlace noZeroPosition{ static_cast<CNumber::SPlace>(result.find_last_not_of('0')) };
	return (sign + result.substr(0, static_cast<size_t>((noZeroPosition == pointPosition) ? pointPosition : (noZeroPosition + 1))));
}
std::string CNumber::ToString(CNumber::SPlace outputPlace) const
{
	std::string result{ std::to_string(CNumber::data) };
	std::string sign{};
	if (result.at(0) == '-')
	{
		sign = "-";
		result.erase(0, 1);
	}
	if (result.size() > CNumber::pointPlace)
	{
		result.insert(result.end() - CNumber::pointPlace, '.');
	}
	else
	{
		for (CNumber::SPlace i{ static_cast<CNumber::SPlace>(CNumber::pointPlace - result.size()) }; i > 0; --i)
		{
			result.insert(result.begin(), '0');
		}
		result.insert(result.begin(), '.');
		result.insert(result.begin(), '0');
	}
	CNumber::SPlace pointPosition{ static_cast<CNumber::SPlace>(result.size() - CNumber::pointPlace - 1) };
	if (outputPlace == 0)
	{
		result.erase(pointPosition);
	}
	else if (outputPlace < CNumber::pointPlace)
	{
		result.erase(static_cast<size_t>(pointPosition + outputPlace + 1));
	}
	else
	{
		for (CNumber::SPlace i{ outputPlace - CNumber::pointPlace }; i > 0; --i)
		{
			result.append("0");
		}
	}
	return ((result == "0") ? result : (sign + result));
}

CNumber& CNumber::operator=(_IN_ const CNumber& number)
{
	CNumber::data = number.CNumber::data;
	CNumber::pointPlace = number.CNumber::pointPlace;
	return *this;
}
CNumber& CNumber::operator=(std::string number)
{
	auto pointPosition{ number.find('.') };
	if ((pointPosition == std::string::npos) || ((pointPosition + 1U) == number.size()))
	{
		CNumber::pointPlace = 0U;
		CNumber::data = NString::ToLonglong(number.substr(0, pointPosition).c_str());
		return *this;
	}
	CNumber::pointPlace = static_cast<CNumber::SPlace>(number.size() - pointPosition - 1U);
	CNumber::data = NString::ToLonglong(number.erase(pointPosition, 1).c_str());
	return *this;
}
CNumber& CNumber::operator=(const char* number)
{
	return (*this = CNumber(std::string{ number }));
}
CNumber& CNumber::FromDouble(double number)
{
	return (*this = NString::ToString(number, CNumber::pointPlace));
}
CNumber& CNumber::FromDouble(double number, std::streamsize newPlace)
{
	return (*this = NString::ToString(number, newPlace));
}

CNumber& CNumber::operator++()
{
	std::string temp{ "1." };
	for (CNumber::SPlace i{ CNumber::pointPlace }; i > 0; --i)
	{
		temp.append("0");
	}
	return (*this += CNumber{ temp });
}
const CNumber CNumber::operator++(int)
{
	CNumber result{ *this };
	++(*this);
	return result;
}
CNumber& CNumber::operator--()
{
	std::string temp{ "1." };
	for (CNumber::SPlace i{ CNumber::pointPlace }; i > 0; --i)
	{
		temp.append("0");
	}
	return (*this -= CNumber{ temp });
}
const CNumber CNumber::operator--(int)
{
	CNumber result{ *this };
	--(*this);
	return result;
}

CNumber CNumber::operator-() const
{
	CNumber result{ *this };
	result.CNumber::data = -(result.CNumber::data);
	return result;
}

CNumber CNumber::operator<<(unsigned placeMove) const
{
	CNumber result{ *this };
	for (auto i{ placeMove }; placeMove > 0U; --placeMove)
	{
		result.CNumber::data *= 10LL;
	}
	return result;
}
CNumber CNumber::operator>>(unsigned placeMove) const
{
	CNumber result{ *this };
	for (auto i{ placeMove }; placeMove > 0U; --placeMove)
	{
		result.CNumber::data /= 10LL;
	}
	return result;
}
CNumber& CNumber::operator<<=(unsigned placeMove)
{
	for (auto i{ placeMove }; placeMove > 0U; --placeMove)
	{
		CNumber::data *= 10LL;
	}
	return *this;
}
CNumber& CNumber::operator>>=(unsigned placeMove)
{
	for (auto i{ placeMove }; placeMove > 0U; --placeMove)
	{
		CNumber::data /= 10LL;
	}
	return *this;
}

CNumber CNumber::operator+(_IN_ const CNumber& number) const
{
	NDebug::Assert((CNumber::pointPlace == number.CNumber::pointPlace), __FUNCTION__ ": pointPlace cannot different");
	CNumber result{ *this };
	result.CNumber::data = (CNumber::data + number.CNumber::data);
	return result;
}
CNumber CNumber::operator-(_IN_ const CNumber& number) const
{
	NDebug::Assert((CNumber::pointPlace == number.CNumber::pointPlace), __FUNCTION__ ": pointPlace cannot different");
	CNumber result{ *this };
	result.CNumber::data = (CNumber::data - number.CNumber::data);
	return result;
}
CNumber& CNumber::operator+=(_IN_ const CNumber& number)
{
	NDebug::Assert((CNumber::pointPlace == number.CNumber::pointPlace), __FUNCTION__ ": pointPlace cannot different");
	CNumber::data += number.CNumber::data;
	return *this;
}
CNumber& CNumber::operator-=(_IN_ const CNumber& number)
{
	NDebug::Assert((CNumber::pointPlace == number.CNumber::pointPlace), __FUNCTION__ ": pointPlace cannot different");
	CNumber::data -= number.CNumber::data;
	return *this;
}

bool CNumber::operator==(_IN_ const CNumber& number) const
{
	NDebug::Assert((CNumber::pointPlace == number.CNumber::pointPlace), __FUNCTION__ ": pointPlace cannot different");
	return (CNumber::data == number.CNumber::data);
}
bool CNumber::operator!=(_IN_ const CNumber& number) const
{
	NDebug::Assert((CNumber::pointPlace == number.CNumber::pointPlace), __FUNCTION__ ": pointPlace cannot different");
	return (CNumber::data != number.CNumber::data);
}
bool CNumber::operator>(_IN_ const CNumber& number) const
{
	NDebug::Assert((CNumber::pointPlace == number.CNumber::pointPlace), __FUNCTION__ ": pointPlace cannot different");
	return (CNumber::data > number.CNumber::data);
}
bool CNumber::operator<(_IN_ const CNumber& number) const
{
	NDebug::Assert((CNumber::pointPlace == number.CNumber::pointPlace), __FUNCTION__ ": pointPlace cannot different");
	return (CNumber::data < number.CNumber::data);
}
bool CNumber::operator>=(_IN_ const CNumber& number) const
{
	NDebug::Assert((CNumber::pointPlace == number.CNumber::pointPlace), __FUNCTION__ ": pointPlace cannot different");
	return (CNumber::data >= number.CNumber::data);
}
bool CNumber::operator<=(_IN_ const CNumber& number) const
{
	NDebug::Assert((CNumber::pointPlace == number.CNumber::pointPlace), __FUNCTION__ ": pointPlace cannot different");
	return (CNumber::data <= number.CNumber::data);
}

CNumber::CNumber(std::string number) : data{}, pointPlace{}
{
	*this = number;
}
CNumber::CNumber(const char* number) : data{}, pointPlace{}
{
	*this = number;
}
CNumber::CNumber(_IN_ const CNumber& number) : data{ number.CNumber::data }, pointPlace{ number.CNumber::pointPlace }
{}
CNumber::~CNumber()
{}

std::ostream& operator<<(_IN_ std::ostream& cout, _IN_ const CNumber& number)
{
	return (cout << ((std::string)number));
}
std::istream& operator>>(_IN_ std::istream& cin, _OUT_ CNumber& number)
{
	std::string input{};
	if (cin >> input) number = input;
	return cin;
}