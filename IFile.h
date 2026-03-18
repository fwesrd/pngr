#pragma once
#ifndef _IFILE_H_
#define _IFILE_H_

#include "include.h"

template<typename DataType = std::string, typename STLType = std::vector<DataType>>
class IFile
{
protected:
	std::fstream m_file;
	std::string m_path;
	bool m_onlyRead;

public:
	const std::fstream& File();
	const std::string& Path();
	const bool& IsOnlyRead();

	bool IsOpen(); //can work
	void OnlyRead(bool onlyRead); //only read after close file

	size_t ByteSize(); //file byte size

	bool Open(std::string path, std::ios_base::openmode mode, bool onlyRead = false); //fail no close old file
	void Close();

	bool Read(_OUT_ STLType& stl);
	bool Write(_IN_ const STLType& stl); //may lead to no open
	bool Append(_IN_ const STLType& stl);

	bool Clear(size_t bufferByte = (1024 * 1024)); //write 0

private:
	virtual void Get(_OUT_ STLType& stl) = 0; //how to read
	virtual void Put(_IN_ const STLType& stl) = 0; //how to write and append

public:
	IFile();
	virtual ~IFile(); //auto close file
};

#include "IFile.hpp"

#endif // !_IFILE_H_