#pragma once

class CFileSystem
{
public:
	std::string GetDirectory(const std::string& path);

	bool CheckDirectory(const char* path);
	bool CheckDirectory(const std::string& path) { return CheckDirectory(path.c_str()); }

	bool Exists(const char* path);
	bool Exists(const std::string& path) { return Exists(path.c_str()); }

	bool CreateFolder(const char* path);
	bool CreateFolder(const std::string& path) { return CreateFolder(path.c_str()); }
};

extern CFileSystem fs;
