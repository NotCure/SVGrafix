#pragma once
#include <string>
#include <string_view>

namespace utility::str {
	[[nodiscard]]
	std::string to_lower(std::string_view input);

}