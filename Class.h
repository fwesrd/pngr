#pragma once
#ifndef _CLASS_H_
#define _CLASS_H_

#include "include.h"

class CEvent
{
protected:
	HANDLE hEvent;
public:
	operator bool();
	bool operator!();
public:
	bool Create(bool initSet, bool manualReset = false);
	bool Set();
	bool Wait(DWORD waitTimeMs = INFINITE);
	bool Wait(DWORD waitTimeMs, bool& timeOut);
	bool Reset(); //use for manualReset
	bool Close();
public:
	CEvent();
	virtual ~CEvent();
};

class CSemaphore
{
protected:
    HANDLE hSemaphore;
    LONG initCount;
    LONG maxCount;
public:
    operator bool();
    bool operator!();
public:
    bool Create(LONG initCount, LONG maxCount); //initCount >= 0, maxCount >= 1
	bool Add(LONG addCount = 1L);
	bool Wait(DWORD waitTimeMs = INFINITE);
	bool Wait(DWORD waitTimeMs, bool& timeOut);
	bool Close();
public:
	CSemaphore();
	~CSemaphore();
};

class CNumber
{
	using SPlace = unsigned int;
protected:
	long long data;
	SPlace pointPlace;

public:
	const long long& Data() const;
	const SPlace& PointPlace() const;

public:
	bool operator!() const;
	operator bool() const;
	operator long long() const;
	operator int() const;
	operator double() const;
	operator float() const;
	operator std::string() const;
	std::string ToString(SPlace outputPlace) const;

public: //change place
	CNumber& operator=(_IN_ const CNumber& number);
	CNumber& operator=(std::string number);
	CNumber& operator=(const char* number);
	CNumber& FromDouble(double number);
	CNumber& FromDouble(double number, std::streamsize newPlace);
	
public:
	CNumber& operator++();
	const CNumber operator++(int);
	CNumber& operator--();
	const CNumber operator--(int);

	CNumber operator-() const;

	CNumber operator<<(unsigned placeMove) const;
	CNumber operator>>(unsigned placeMove) const;
	CNumber& operator<<=(unsigned placeMove);
	CNumber& operator>>=(unsigned placeMove);

public: //Assert
	CNumber operator+(_IN_ const CNumber& number) const;
	CNumber operator-(_IN_ const CNumber& number) const;
	CNumber& operator+=(_IN_ const CNumber& number);
	CNumber& operator-=(_IN_ const CNumber& number);

	bool operator==(_IN_ const CNumber& number) const;
	bool operator!=(_IN_ const CNumber& number) const;
	bool operator>(_IN_ const CNumber& number) const;
	bool operator<(_IN_ const CNumber& number) const;
	bool operator>=(_IN_ const CNumber& number) const;
	bool operator<=(_IN_ const CNumber& number) const;

public:
	friend std::ostream& operator<<(_IN_ std::ostream& cout, _IN_ const CNumber& number);
	friend std::istream& operator>>(_IN_ std::istream& cin, _OUT_ CNumber& number);

public:
	CNumber(std::string number);
	CNumber(const char* number);
	CNumber(_IN_ const CNumber& number);
	virtual ~CNumber();
};

#endif // !_CLASS_H_