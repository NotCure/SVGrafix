#pragma once
#include <string>
#include <string_view>

namespace utility::file {

	[[nodiscard]]
	std::string read_file(std::string_view path);

	[[nodiscard]]
	std::string extension(std::string_view path);

}