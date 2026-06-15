#include "FileSystem.h"

CFileSystem fs;

std::string CFileSystem::GetDirectory(const std::string& path)
{
	size_t pos = path.find_last_of("\\/");
	return pos == std::string::npos ? "" : path.substr(0, pos);
}

bool CFileSystem::CheckDirectory(const char* path)
{
	if (!path || !*path)
		return false;

	std::string dir = GetDirectory(path);

	if (!dir.empty())
	{
		if (!CreateFolder(dir.c_str()))
			return false;
	}

	return true;
}

bool CFileSystem::Exists(const char* path)
{
	return std::ifstream(path).good();
}

bool CFileSystem::CreateFolder(const char* path)
{
	if (!path || !*path)
		return false;

	std::string current;

	for (size_t i = 0; path[i]; ++i)
	{
		current += path[i];

		const bool isSlash = (path[i] == '\\' || path[i] == '/');
		const bool isLast = (path[i + 1] == '\0');

		if (isSlash || isLast)
		{
			// skip roots like "D:\"
			if (current.size() <= 3 && current.find(':') != std::string::npos)
				continue;

			if (!CreateDirectory(current.c_str(), nullptr))
			{
				DWORD err = GetLastError();

				if (err != ERROR_ALREADY_EXISTS)
					return false;
			}
		}
	}

	return true;
}
