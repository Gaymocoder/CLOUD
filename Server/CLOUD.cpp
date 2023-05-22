#include "CLOUD.h"
#include "en_de_coders.h"
#include <cmath>

std::vector <std::string> GetCookies(std::string CookiesList)
{
	int k = 1;
    for(size_t i = 0; i<CookiesList.length(); i++)
    {
        if(CookiesList[i] == ';')
            k++;
    }
    int j = 0;
    std::vector <std::string> Cookies (k);
    for(size_t i = 0; i<CookiesList.length(); i++)
    {
        while (i != CookiesList.length())
        {
            if (CookiesList[i] == ';')
            {
                i++;
                break;
            }
            Cookies[j] += CookiesList[i];
            i++;
        }
        j++;
    }
    return Cookies;
}

DataStruct::DataStruct(DSType type, std::string name, FS::path dir)
{
	this->type = type;
	this->normal_name = name;
	this->directory = dir;
}

std::string DataStruct::getHtmlName()
{
	return RusToHtml_encode(this->normal_name);
}

std::string DataStruct::getVisualName()
{
	return this->normal_name;
}

std::string DataStruct::getWebPath()
{
	bool WebPathBegin = false;
	FS::path WebPath = "/";
	for(auto& PathElement : this->directory)
	{
		if (PathElement == "yourcloud")
			WebPathBegin = true;
		if (WebPathBegin)
			WebPath /= PathElement;
	}
	WebPath /= getVisualName() + (this->type == DSType::DIR ? "/" : "");
	return RusToHtml_encode(WebPath.string());
}

FS::path DataStruct::getPath()
{
	return this->directory / this->normal_name;
}

std::string DataStruct::LastChanged()
{
	char buffer[30];
	std::time_t TTTime = STD_FGC::to_time_t(FS::last_write_time(this->getPath()));
	struct tm *timeinfo = std::gmtime(&TTTime);
	std::strftime(buffer, 30, "%d.%m.%Y\n%H:%M:%S", timeinfo);
	return std::string(buffer);
}

void Storage::configFileParse()
{
	ifstream configFile(this->configFile);
	
	// Start of parsing config.ini with initializing main variables
	printf("Starting config file parsing\n");
	for(std::string Line; getline(configFile, Line);)
	{
		if (Line.find("Server IP:") == 0)
		{
			this->SrvAddress = Line.substr(11, Line.length()-11);
			printf("Found server IPv4 of CLOUD: %s \n", SrvAddress.c_str());
			continue;
		}
		if (Line.find("Port:") == 0)
		{
			this->SrvPort = std::stoi(Line.substr(6, Line.length()-6));
			printf("Found server port of CLOUD: %i \n", SrvPort);
			continue;
		}
		if (Line.find("Threads count:") == 0)
		{
			this->SrvThreadCount = std::stoi(Line.substr(15, Line.length()-15));
			printf("Found server port of CLOUD: %i \n", SrvThreadCount);
			continue;
		}
		if (Line.find("Root Directory:") == 0)
		{
			this->RootDir = FS::absolute(Line.substr(16, Line.length()-16));
			printf("Found server directory: %s \n", RootDir.c_str());
			continue;
		}
		if (Line.find("Default Page:") == 0)
		{
			this->DefaultPage = Line.substr(14, Line.length()-14);
			printf("Found name of default page file: %s \n", DefaultPage.c_str());
			continue;
		}
		if (Line.find("CLOUD Login:") == 0)
		{
			this->Login = Line.substr(13, Line.length()-13);
			printf("Found login of CLOUD: %s \n", Login.c_str());
			continue;
		}
		if (Line.find("CLOUD Password:") == 0)
		{
			this->Password = Line.substr(16, Line.length()-16);
			printf("Found password of CLOUD: %s \n", Password.c_str());
			continue;
		}
	}
}

Storage::Storage(std::string configFile)
{
	this->configFile = FS::absolute(FS::path(configFile));
	this->configFileParse();
}
