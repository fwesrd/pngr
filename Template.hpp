#pragma once
#ifndef _TEMPLATE_H_
#define _TEMPLATE_H_

template <typename BasicType>
std::vector<BasicType> GetInputs(std::istream& in, BasicType type)
{
	std::string input{};
	std::getline(in, input);
	std::stringstream sstream{ input };
	std::vector<BasicType> result{};
	BasicType basicType{};
	while (sstream >> basicType) result.emplace_back(basicType);
	return result;
}

#endif // !_TEMPLATE_H_