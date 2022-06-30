#include "STDInclude.hpp"

namespace Utils
{
	void SetEnvironment()
	{
		wchar_t exeName[512];
		GetModuleFileName(GetModuleHandle(nullptr), exeName, sizeof(exeName) / 2);

		wchar_t* exeBaseName = wcsrchr(exeName, L'\\');
		exeBaseName[0] = L'\0';

		SetCurrentDirectory(exeName);
	}

	std::string LoadResource(int resId)
	{
		Utils::NT::Module module;
		HRSRC res = FindResource(module.getHandle(), MAKEINTRESOURCE(resId), RT_RCDATA);
		if (!res) return "";

		HGLOBAL handle = LoadResource(nullptr, res);
		if (!handle) return "";

		return std::string(LPSTR(LockResource(handle)), SizeofResource(nullptr, res));
	}
}
