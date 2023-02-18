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
	this.type = type;
	this.normal_name = name;
	this.directory = dir;
}

DataStruct::getHtmlName()
{
	return EnDecoders::CharToHtml(this.normal_name);
}

DataStruct::getVisualName()
{
	return this.normal_name;
}

DataStruct::getWebPath()
{
	bool WebPathBegin = false;
	FS::path WebPath = "/";
	for(auto& PathElement : this.directory)
	{
		if (PathElement == "yourcloud")
			WebPathBegin = true;
		if (WebPathBegin)
			WebPath /= PathElement;
	}
	WebPath /= VisualName() + (this.type == DSType::DIR ? "/" : "");
	return RusToHtml_encode(WebPath.string());
}

DataStruct::getPath()
{
	return this.directory / this.normal_name;
}

DataStruct::LastChanged()
{
	char buffer[30];
	struct tm *timeinfo = gmtime(STD_FGC::to_time_t(FS::last_write_time(this.getPath())));;
	std::strftime(buffer, 30, "%d.%m.%Y\n%H:%M:%S", timeinfo);
	return std::string(buffer);
}
