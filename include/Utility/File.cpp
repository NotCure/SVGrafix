#include "File.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <iostream>

namespace utility::file {

    std::string read_file(std::string_view path_sv)
    {
		std::string extension = utility::file::extension(path_sv);
		if (extension == "Empty") 
            std::cout << "invalid extension" << extension << '\n';
		
        std::filesystem::path path(path_sv);
        std::ifstream file(path, std::ios::binary);
        if (!file) {
            std::cerr << "read_all: cannot open " << path << '\n';
            return {};
        }
        std::ostringstream oss;
        oss << file.rdbuf();
        return oss.str();
    }

    std::string extension(std::string_view path)
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

}