#include <iostream>
#include <unordered_map>
       


namespace Utility
{
	namespace Parser
	{
		void parseAttributes(
			const std::string& tagBody,
			std::unordered_map<std::string, std::string>& outAttrs
		);
	}


	namespace File
	{

		std::string getFileContent(std::string filePath);
		std::string checkExtension(const std::string& path);
		std::string readFile(const std::string& path);
	} 

	namespace String
	{
		std::string convertToLowercase(const std::string& str);

	}
	
}

