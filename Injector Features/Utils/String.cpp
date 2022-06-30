#include "STDInclude.hpp"

namespace Utils
{
	const char *String::VA(const char *fmt, ...)
	{
		static
#ifndef VA_DONT_USE_THREAD_STORAGE
			thread_local
#endif
			String::VAProvider<8, 256> provider;

		va_list ap;
		va_start(ap, fmt);

		const char* result = provider.get(fmt, ap);

		va_end(ap);
		return result;
	}

	std::string String::DumpHex(std::string data, std::string separator)
	{
		std::string result;

		for (unsigned int i = 0; i < data.size(); ++i)
		{
			if (i > 0)
			{
				result.append(separator);
			}

			result.append(Utils::String::VA("%02X", data[i] & 0xFF));
		}

		return result;
	}

	std::vector<std::string> String::Explode(const std::string& str, char delim)
	{
		std::vector<std::string> result;
		std::istringstream iss(str);

		for (std::string token; std::getline(iss, token, delim);)
		{
			result.push_back(std::move(token));
		}

		return result;
	}

	String String::toLower()
	{
		String out(*this);
		std::transform(out.begin(), out.end(), out.begin(), ::tolower);
		return out;
	}

	String String::toUpper()
	{
		String out(*this);
		std::transform(out.begin(), out.end(), out.begin(), ::toupper);
		return out;
	}

	bool String::endsWith(Utils::String otherString)
	{
		if (otherString.size() > this->size()) return false;
		return std::equal(otherString.begin(), otherString.end(), this->begin() + (this->size() - otherString.size()));
	}

	bool String::startsWith(Utils::String otherString)
	{
		if (otherString.size() > this->size()) return false;
		return std::equal(otherString.begin(), otherString.end(), this->begin());
	}

	String String::replaceAll(std::string find, std::string replace)
	{
		size_t pos = 0;
		String string(*this);
		while ((pos = string.find(find, pos)) != std::string::npos)
		{
			string = string.replace(pos, find.size(), replace);
			pos += replace.length();
		}

		return string;
	}
}
