export module rsp;

import std;
import utils;

export namespace rsp
{
	bool IsMacroArg(const std::string& arg)
	{
		return arg.starts_with("@") && arg.ends_with(".rsp");
	}

	std::string Expand(const std::string& arg)
	{
		std::string path = arg.substr(1);
		std::u16string u16 = readUtf16File(path);
		return u16_to_utf8(u16);
	}
}