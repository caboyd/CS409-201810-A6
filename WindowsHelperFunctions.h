#pragma once
#include <iomanip> // setprecision
#include <sstream> // stringstream

//Helper function to get all the files in a folder on windows
#ifdef _WIN32
#include <Windows.h>
inline std::vector<std::string> getFileNamesInFolder(const std::string& folder)
{
	std::vector<std::string> names;
	std::string search_path = folder + "/*.*";
	WIN32_FIND_DATAA fd;
	HANDLE file_handle = FindFirstFileA(search_path.c_str(), &fd);
	if (file_handle != INVALID_HANDLE_VALUE)
	{
		do
		{
			// read all (real) files in current folder
			// , delete '!' read other 2 default folder . and ..
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				names.emplace_back(fd.cFileName);
			}
		} while (::FindNextFileA(file_handle, &fd));
		::FindClose(file_handle);
	}
	return names;
}
#endif

inline std::string realToString(const float x, int precision)
{
	std::stringstream stream;
	stream << std::fixed << std::setprecision(precision) << x;
	return stream.str();
}
inline std::string realToString(const double x, int precision)
{
	std::stringstream stream;
	stream << std::fixed << std::setprecision(precision) << x;
	return stream.str();
}