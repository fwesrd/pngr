#pragma once
#ifndef _IFILE_HPP_
#define _IFILE_HPP_

#include "IFile.h"

namespace NFileSystem
{
	extern bool IsExist(std::string path);
	extern bool OnlyRead(std::string path, bool onlyRead);
	extern bool IsOnlyRead(std::string path);
}

template<typename DataType, typename STLType>
inline const std::fstream& IFile<DataType, STLType>::File()
{
	return IFile<DataType, STLType>::m_file;
}

template<typename DataType, typename STLType>
inline const std::string& IFile<DataType, STLType>::Path()
{
	return IFile<DataType, STLType>::m_path;
}

template<typename DataType, typename STLType>
inline const bool& IFile<DataType, STLType>::IsOnlyRead()
{
	return IFile<DataType, STLType>::m_onlyRead;
}

template<typename DataType, typename STLType>
bool IFile<DataType, STLType>::Open(std::string path, std::ios_base::openmode mode, bool onlyRead)
{
	//check old file attribute
	if (IFile<DataType, STLType>::m_path == path) IFile<DataType, STLType>::Close();
	bool attribute = (NFileSystem::IsExist(path) && NFileSystem::IsOnlyRead(path));
	NFileSystem::OnlyRead(path, false);
	std::fstream tryFile{ path, mode };
	//fail
	if (!tryFile.is_open())
	{
		NFileSystem::OnlyRead(path, attribute);
		return false;
	}
	//success
	IFile<DataType, STLType>::Close();
	IFile<DataType, STLType>::m_file.swap(tryFile);
	IFile<DataType, STLType>::m_path = path;
	IFile<DataType, STLType>::m_onlyRead = onlyRead;
	return true;
}

template<typename DataType, typename STLType>
inline void IFile<DataType, STLType>::Close()
{
	IFile<DataType, STLType>::m_file.close();
	NFileSystem::OnlyRead(IFile<DataType, STLType>::m_path, IFile<DataType, STLType>::m_onlyRead);
	IFile<DataType, STLType>::m_path = "";
}

template<typename DataType, typename STLType>
inline bool IFile<DataType, STLType>::IsOpen()
{
	return IFile<DataType, STLType>::m_file.is_open();
}

template<typename DataType, typename STLType>
inline void IFile<DataType, STLType>::OnlyRead(bool onlyRead)
{
	IFile<DataType, STLType>::m_onlyRead = onlyRead;
}

template<typename DataType, typename STLType>
size_t IFile<DataType, STLType>::ByteSize()
{
	if (!IFile<DataType, STLType>::IsOpen()) return 0;
	IFile<DataType, STLType>::m_file.clear();
	IFile<DataType, STLType>::m_file.seekg(0, std::ios::end);
	size_t byteSize{ IFile<DataType, STLType>::m_file.tellg() };
	IFile<DataType, STLType>::m_file.seekg(0, std::ios::beg);
	IFile<DataType, STLType>::m_file.clear();
	return byteSize;
}

template<typename DataType, typename STLType>
bool IFile<DataType, STLType>::Clear(size_t bufferByte)
{
	if (!IFile<DataType, STLType>::IsOpen()) return false;
	size_t fileByte{ IFile<DataType, STLType>::ByteSize() };
	if (!fileByte) return true;
	NFileSystem::OnlyRead(IFile<DataType, STLType>::m_path, false);
	IFile<DataType, STLType>::m_file.clear();
	IFile<DataType, STLType>::m_file.seekp(0, std::ios::beg);
	char* buffer{ new char[bufferByte] {} };
	if (!buffer) return false;
	bool result = true;
	while (fileByte >= bufferByte)
	{
		if (!IFile<DataType, STLType>::m_file.write(buffer, bufferByte)) result = false;
		fileByte -= bufferByte;
	}
	if (!IFile<DataType, STLType>::m_file.write(buffer, fileByte)) result = false;
	delete[] buffer;
	buffer = nullptr;
	IFile<DataType, STLType>::m_file.flush();
	IFile<DataType, STLType>::m_file.clear();
	IFile<DataType, STLType>::m_file.seekp(0, std::ios::beg);
	IFile<DataType, STLType>::m_file.clear();
	return result;
}

template<typename DataType, typename STLType>
bool IFile<DataType, STLType>::Read(_OUT_ STLType& stl)
{
	if (!IFile<DataType, STLType>::IsOpen()) return false;
	IFile<DataType, STLType>::m_file.clear();
	IFile<DataType, STLType>::m_file.seekg(0, std::ios::beg);
	stl.clear();
	this->Get(stl);
	IFile<DataType, STLType>::m_file.seekg(0, std::ios::beg);
	IFile<DataType, STLType>::m_file.clear();
	return true;
}

template<typename DataType, typename STLType>
bool IFile<DataType, STLType>::Write(_IN_ const STLType& stl)
{
	if (!IFile<DataType, STLType>::IsOpen()) return false;
	IFile<DataType, STLType>::m_file.close();
	NFileSystem::OnlyRead(IFile<DataType, STLType>::m_path, false);
	IFile<DataType, STLType>::m_file.open(IFile<DataType, STLType>::m_path, std::ios::trunc | std::ios::out | std::ios::in | std::ios::binary);
	if (!IFile<DataType, STLType>::m_file.is_open()) return false;
	this->Put(stl);
	IFile<DataType, STLType>::m_file.flush();
	IFile<DataType, STLType>::m_file.clear();
	IFile<DataType, STLType>::m_file.seekp(0, std::ios::beg);
	IFile<DataType, STLType>::m_file.clear();
	return true;
}

template<typename DataType, typename STLType>
bool IFile<DataType, STLType>::Append(_IN_ const STLType& stl)
{
	if (!IFile<DataType, STLType>::IsOpen()) return false;
	NFileSystem::OnlyRead(IFile<DataType, STLType>::m_path, false);
	IFile<DataType, STLType>::m_file.clear();
	IFile<DataType, STLType>::m_file.seekp(0, std::ios::end);
	this->Put(stl);
	IFile<DataType, STLType>::m_file.flush();
	IFile<DataType, STLType>::m_file.clear();
	IFile<DataType, STLType>::m_file.seekp(0, std::ios::beg);
	IFile<DataType, STLType>::m_file.clear();
	return true;
}

template<typename DataType, typename STLType>
IFile<DataType, STLType>::IFile() : m_onlyRead{ false }
{}

template<typename DataType, typename STLType>
IFile<DataType, STLType>::~IFile()
{
	IFile<DataType, STLType>::Close();
}

#endif // !_IFILE_HPP_