#ifndef KT_APP_STRINGUTIL_H_
#define KT_APP_STRINGUTIL_H_

#include <cstdint>
#include <string>
#include <sstream>
#include <vector>

namespace kt {

/**
 * @func str_from_wstr
 * @brief Answer a wstring from a string. NOTE: Converts to utf8.
 */
std::string					str_from_wstr(const std::wstring&);

/**
 * @func wstr_from_str
 * @brief Answer a wstring from a string. NOTE: Assumes utf8.
 */
std::wstring				wstr_from_str(const std::string&);

/**
 * @func to_lower
 * @func to_upper
 * @brief change case
 */
std::string					to_lower(std::string);
std::string					to_upper(std::string);

/**
 * @func split_str
 * @brief Split a string based on a token.
 */
std::vector<std::string>	split(const std::string &input, const std::string &token);

/**
 * @func replace_substr
 * @brief Replace occurrences of search with replace. Kind of amazing this
 * is still missing from the library.
 */
void						replace_substr(	const std::string &search, const std::string &replace,
											std::string &haystack);

/**
 * @func trim_l
 * @brief Trim left whitespace.
 */
std::string&				trim_l(std::string&);

/**
 * @func trim_r
 * @brief Trim right whitespace.
 */
std::string&				trim_r(std::string&);

/**
 * @func trim
 * @brief Trim left and right whitespace.
 */
std::string&				trim(std::string&);

/**
 * CONVERSIONS
 */
template<typename T>
T							from_str(const std::string &input) {
	// XXX No error checking right now
	T						ans;
	std::istringstream		s(input);
	s >> ans;
	return ans;
}

// Bool string input is "true" or "false" (or more accurately, "t" or "f")
template<>
bool						from_str<bool>(const std::string &input);

template<>
std::int32_t				from_str<std::int32_t>(const std::string &input);

template<>
float						from_str<float>(const std::string &input);

} // namespace kt

#endif
