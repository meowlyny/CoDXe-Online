#pragma once

#include "Compression.h"
#include "Endianness.h"
#include "FileSystem.h"
#include "Hooking.h"
#include "Mth.h"
#include "String.h"

class CUtility
{
public:
	class CINIReader
	{
	public:
		bool LoadFile(const std::string& filePath)
		{
			std::ifstream f(filePath.c_str());
			if (!f.is_open())
				return false;

			std::string line;
			std::string currentSection;

			while (getline(f, line))
			{
				TrimStr(line);

				if (line.empty())
					continue;

				if (line[0] == ';')
					continue;

				if (line[0] == '[')
				{
					const auto end = line.find(']');
					if (end != std::string::npos)
						currentSection = line.substr(1, end - 1);

					continue;
				}

				const auto eq = line.find('=');
				if (eq == std::string::npos)
					continue;

				std::string key = line.substr(0, eq);
				std::string value = line.substr(eq + 1);

				TrimStr(key);
				TrimStr(value);

				m_data[currentSection][key] = value;
			}

			return true;
		}

		bool UnloadFile()
		{
			m_data.clear();
			return true;
		}

		std::string GetString(const std::string& section, const std::string& key, const std::string& def = "") const
		{
			auto itS = m_data.find(section);
			if (itS == m_data.end())
				return def;

			auto itK = itS->second.find(key);
			if (itK == itS->second.end())
				return def;

			return itK->second;
		}

		bool GetBoolean(const std::string& section, const std::string& key, bool def = false) const
		{
			const auto value = GetString(section, key);
			if (value.empty())
				return def;

			std::string string = value;
			for (size_t i = 0; i < string.size(); ++i)
				string[i] = (char)tolower((unsigned char)string[i]);

			return (string == "1" || string == "true");
		}

	private:
		typedef std::map<std::string, std::string> KeyMap;
		typedef std::map<std::string, KeyMap> SectionMap;

		SectionMap m_data;

		static void TrimStr(std::string& str)
		{
			size_t start = str.find_first_not_of(" \t");
			size_t end = str.find_last_not_of(" \t");

			if (start == std::string::npos)
			{
				str.clear();
				return;
			}

			str = str.substr(start, end - start + 1);
		}
	};
};

extern CUtility utility;
