#pragma once

#include <fstream>
#include <vector>
#include <filesystem>
#include "std_extra.h"

using namespace std;
namespace FS = std::filesystem;

typedef std::string serverSetting;
typedef std::string storageSetting;

std::vector <std::string> GetCookies(std::string CookiesList);

class DataStruct
{
	private:
		FS::path directory;
		std::string normal_name;
		DSType type;
		
	public:
		DataStruct(DSType type, std::string NormalName, FS::path Directory);
		std::string getHtmlName();
		std::string getVisualName();
		std::string getWebPath();
		FS::path getPath();
		std::string LastChanged();
		size_t getDiskSize();
};

class NewFile
{
	private:
		FS::path Directory;
		std::string NormalName;
	
	public:
		NewFile();
		const size_t ContentStart;
		const size_t ContentEnd;
		std::string getHtmlName();
		std::string getVisualName();
		FS::path getPath();
		void Record(std::vector <char> *src);
};

class Storage
{
	private:
		FS::path RootDir, configFile;
		void configFileParse();
				
	public:
		Storage(std::string configFile);
		std::string DefaultPage, Login, Password, SrvAddress;
		std::uint16_t SrvPort, SrvThreadCount;
		std::vector <DataStruct> Files, Dirs;
		size_t FreeSpace();
		size_t UsedSpace();
		void Upload(std::vector <char> *RequestBuf, FS::path Directory, size_t ContentSize);
		void Delete(FS::path ElementPath, std::string extra);
		void NewFolder(FS::path NewFolderPath);
		void Rename(FS::path OldPath, FS::path NewPath);
		void Update();
};
