#include "Utility.h"
#include <filesystem>     
#include <sstream>     
#include <cctype>
#include <fstream>
#include <string>

using namespace Utility;

// made the code shorter by chatgpt
void Utility::Parser::parseAttributes(const std::string& tagBody, std::unordered_map<std::string, std::string>& attrs)
{
	size_t i = 0, n = tagBody.size();
	while (i < n) {
		while (i < n && std::isspace((unsigned char)tagBody[i])) ++i;
		if (i >= n) break;

		size_t nameStart = i;
		while (i < n && (std::isalnum((unsigned char)tagBody[i]) || tagBody[i] == '-' || tagBody[i] == ':'))
			++i;
		if (i == nameStart) break;             // no name found
		std::string name = tagBody.substr(nameStart, i - nameStart);

		while (i < n && std::isspace((unsigned char)tagBody[i])) ++i;
		if (i >= n || tagBody[i] != '=') break;
		++i;

		while (i < n && std::isspace((unsigned char)tagBody[i])) ++i;
		if (i >= n || (tagBody[i] != '"' && tagBody[i] != '\'')) break;
		char quote = tagBody[i++];

		size_t valueStart = i;
		while (i < n && tagBody[i] != quote) ++i;
		if (i >= n) break; 
		std::string value = tagBody.substr(valueStart, i - valueStart);
		++i;  

		attrs.emplace(std::move(name), std::move(value));


	}
}


std::string File::getFileContent(std::string filePath)
{

	if (filePath.empty() || std::filesystem::exists(filePath) == false)
	{
		std::cerr << "File does not exist!" << std::endl;
		return "";
	}
	else
	{
		std::string extension = checkExtension(filePath);
		std::string content = readFile(filePath);
		
		return content;
	}

	
}

std::string File::checkExtension(const std::string& path)
{
	auto ext = std::filesystem::path(path).extension().string();
	if (ext == ".svg")
	{

		return ext;
	}
	else
	{
		return "Empty";
	}
}

std::string File::readFile(const std::string& path)
{
	std::ifstream file(path);
	if (!file.is_open() || !file)
	{
		std::cerr << "Could not open the file!" << std::endl;
		return "";
	}

	std::ostringstream buffer;
	buffer << file.rdbuf();
	file.close();
	return buffer.str();
}
std::string String::convertToLowercase(const std::string& str)
{
	std::string lowerStr = str;
	for (char& c : lowerStr) {
		c = std::tolower(c);
	}
	return lowerStr;
}
