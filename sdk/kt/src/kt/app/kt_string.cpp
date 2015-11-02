#include "kt_string.h"

#include <codecvt>
#include <regex>

namespace kt {

/**
 * @func str_from_wstr
 */
std::string					str_from_wstr(const std::wstring& wstr) {
	// From stack overflow
	// http://stackoverflow.com/questions/4804298/how-to-convert-wstring-into-string
    typedef std::codecvt_utf8<wchar_t> convert_typeX;
    std::wstring_convert<convert_typeX, wchar_t> converterX;

    return converterX.to_bytes(wstr);
}

/**
 * @func wstr_from_str
 */
std::wstring				wstr_from_str(const std::string& str) {
	// From stack overflow
	// http://stackoverflow.com/questions/4804298/how-to-convert-wstring-into-string
	typedef std::codecvt_utf8<wchar_t> convert_typeX;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

    return converterX.from_bytes(str);
}

/**
 * change case
 */
std::string					to_lower(std::string s) {
	// Not safe. I really should just standardize on wstring like I have in the past.
	std::transform(s.begin(), s.end(), s.begin(), ::tolower);
	return s;
}

std::string					to_upper(std::string s) {
	// Not safe. I really should just standardize on wstring like I have in the past.
	std::transform(s.begin(), s.end(), s.begin(), ::toupper);
	return s;
}

/**
 * @func split_str
 */
std::vector<std::string>	split(const std::string &input, const std::string &_token) {
	// From stack overflow
	// http://stackoverflow.com/questions/12153573/c11-regex-token-iterator

	// passing -1 as the submatch index parameter performs splitting
	std::regex				token(_token);
	std::sregex_token_iterator
			first(input.begin(), input.end(), token, -1),
			last;
	return {first, last};
}

/**
 * @func replace_substr
 */
void						replace_substr(	const std::string &old_s, const std::string &new_s,
											std::string &haystack) {
	for( size_t pos = 0; ; pos += new_s.length() ) {
		pos = haystack.find(old_s, pos);
		if (pos == std::string::npos) return;

		haystack.erase(pos, old_s.length());
		haystack.insert(pos, new_s);
	}
}

/**
 * @func trim_l
 */
std::string&				trim_l(std::string &str) {
	// From https://gist.github.com/superwills/7723313
	if( !str.size() ) return str;
	// find first non-whitespace, then delete from begin to there.
	std::string::iterator iter = str.begin() ;
	while( iter != str.end() && isspace( *iter ) ) ++iter;
	str.erase( str.begin(), iter ) ;
	return str ;
}

/**
 * @func trim_r
 */
std::string&				trim_r(std::string &str) {
	// From https://gist.github.com/superwills/7723313
	if( !str.size() ) return str;
	// find the last whitespace chr, then delete from there to end.
	std::string::iterator iter = --str.end() ;
	while( isspace( *iter ) ) --iter;
	str.erase( ++iter, str.end() ) ;
	return str;
}

/**
 * @func trim
 */
std::string&				trim(std::string &str) {
	return trim_r(trim_l(str));
}

/**
 * CONVERSIONS
 */
template<>
bool						from_str<bool>(const std::string &input) {
	if (!input.empty() && (input[0] == 't' || input[0] == 'T')) return true;
	return false;
}

template<>
std::int32_t				from_str<std::int32_t>(const std::string &input) {
	return std::stoi(input);
}

template<>
float						from_str<float>(const std::string &input) {
	return std::stof(input);
}

} // namespace kt
