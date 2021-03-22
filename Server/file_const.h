#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cmath>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <map>
#include <sstream>
using namespace std;
namespace FS = std::filesystem;

typedef std::map <std::string, std::string> StrDict;

template <typename TP>
std::time_t to_time_t(TP tp)
{
    using namespace std::chrono;
    auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now()
              + system_clock::now());
    return system_clock::to_time_t(sctp);
}

FS::path FSRelPath(std::string StrPath)
{
	FS::path FSRelPath = StrPath;
	FS::path NewPath = FS::current_path();
	for(auto part = FSRelPath.begin(); part != FSRelPath.end(); ++part)
	{
		if (*part == "..")
			NewPath = NewPath.parent_path();
		else if (*part == ".");
		else
			NewPath /= *part;
	}
	return NewPath;
}

std::string KeyByValue(std::string Value, StrDict Dict)
{
	bool Found = false;
	std::string Key = "";
	for(auto &pair : Dict)
		if (pair.second == Value)
		{
			Key = pair.first;
			Found = true;
			break;
		}
	if (Found)
		return Key;
	else
		throw "Key was not found";
};

StrDict RusUTFToURL =
{
	// Capital russian chars
	{"&#1040;", "%D0%90"}, {"&#1041;", "%D0%91"}, {"&#1042;", "%D0%92"}, {"&#1043;", "%D0%93"}, {"&#1044;", "%D0%94"}, {"&#1045;", "%D0%95"}, {"&#1025;", "%D0%81"}, {"&#1046;", "%D0%96"},
	{"&#1047;", "%D0%97"}, {"&#1048;", "%D0%98"}, {"&#1049;", "%D0%99"}, {"&#1050;", "%D0%9A"}, {"&#1051;", "%D0%9B"}, {"&#1052;", "%D0%9C"}, {"&#1053;", "%D0%9D"}, {"&#1054;", "%D0%9E"},
	{"&#1055;", "%D0%9F"}, {"&#1056;", "%D0%A0"}, {"&#1057;", "%D0%A1"}, {"&#1058;", "%D0%A2"}, {"&#1059;", "%D0%A3"}, {"&#1060;", "%D0%A4"}, {"&#1061;", "%D0%A5"}, {"&#1062;", "%D0%A6"},
	{"&#1063;", "%D0%A7"}, {"&#1064;", "%D0%A8"}, {"&#1065;", "%D0%A9"}, {"&#1066;", "%D0%AA"}, {"&#1067;", "%D0%AB"}, {"&#1068;", "%D0%AC"}, {"&#1069;", "%D0%AD"}, {"&#1070;", "%D0%AE"},
	{"&#1071;", "%D0%AF"},

	
	// Lowercase russian chars
	{"&#1072;", "%D0%B0"}, {"&#1073;", "%D0%B1"}, {"&#1074;", "%D0%B2"}, {"&#1075;", "%D0%B3"}, {"&#1076;", "%D0%B4"}, {"&#1077;", "%D0%B5"}, {"&#1105;", "%D1%91"}, {"&#1078;", "%D0%B6"},
	{"&#1079;", "%D0%B7"}, {"&#1080;", "%D0%B8"}, {"&#1081;", "%D0%B9"}, {"&#1082;", "%D0%BA"}, {"&#1083;", "%D0%BB"}, {"&#1084;", "%D0%BC"}, {"&#1085;", "%D0%BD"}, {"&#1086;", "%D0%BE"},
	{"&#1087;", "%D0%BF"}, {"&#1088;", "%D1%80"}, {"&#1089;", "%D1%81"}, {"&#1090;", "%D1%82"}, {"&#1091;", "%D1%83"}, {"&#1092;", "%D1%84"}, {"&#1093;", "%D1%85"}, {"&#1094;", "%D1%86"},
	{"&#1095;", "%D1%87"}, {"&#1096;", "%D1%88"}, {"&#1097;", "%D1%89"}, {"&#1098;", "%D1%8A"}, {"&#1099;", "%D1%8B"}, {"&#1100;", "%D1%8C"}, {"&#1101;", "%D1%8D"}, {"&#1102;", "%D1%8E"},
	{"&#1103;", "%D1%8F"},
	
	// Space
	{"&#32;", "%20"},
	
	// Slash
	{"&#92;", "%2F"}
};

StrDict RusUTFToChar = 
{
	// Capital russian chars
	{"&#1040;", "А"}, {"&#1041;", "Б"}, {"&#1042;", "В"}, {"&#1043;", "Г"}, {"&#1044;", "Д"}, {"&#1045;", "Е"}, {"&#1025;", "Ё"}, {"&#1046;", "Ж"}, {"&#1047;", "З"}, {"&#1048;", "И"}, {"&#1049;", "Й"},
	{"&#1050;", "К"}, {"&#1051;", "Л"}, {"&#1052;", "М"}, {"&#1053;", "Н"}, {"&#1054;", "О"}, {"&#1055;", "П"}, {"&#1056;", "Р"}, {"&#1057;", "С"}, {"&#1058;", "Т"}, {"&#1059;", "У"}, {"&#1060;", "Ф"},
	{"&#1061;", "Х"}, {"&#1062;", "Ц"},	{"&#1063;", "Ч"}, {"&#1064;", "Ш"},	{"&#1065;", "Щ"}, {"&#1066;", "Ъ"}, {"&#1067;", "Ы"}, {"&#1068;", "Ь"}, {"&#1069;", "Э"}, {"&#1070;", "Ю"}, {"&#1071;", "Я"},
	
	// Lowercase russian chars
	{"&#1072;", "а"}, {"&#1073;", "б"}, {"&#1074;", "в"}, {"&#1075;", "г"}, {"&#1076;", "д"}, {"&#1077;", "е"}, {"&#1105;", "ё"}, {"&#1078;", "ж"}, {"&#1079;", "з"}, {"&#1080;", "и"}, {"&#1081;", "й"},
	{"&#1082;", "к"}, {"&#1083;", "л"}, {"&#1084;", "м"}, {"&#1085;", "н"}, {"&#1086;", "о"}, {"&#1087;", "п"}, {"&#1088;", "р"}, {"&#1089;", "с"}, {"&#1090;", "т"}, {"&#1091;", "у"}, {"&#1092;", "ф"},
	{"&#1093;", "х"}, {"&#1094;", "ц"}, {"&#1095;", "ч"}, {"&#1096;", "ш"}, {"&#1097;", "щ"}, {"&#1098;", "ъ"}, {"&#1099;", "ы"}, {"&#1100;", "ь"}, {"&#1101;", "э"}, {"&#1102;", "ю"}, {"&#1103;", "я"},
	
	// Space
	{"&#32;", " "},
	
	//Slash
	{"&#92;", "/"}
};

std::string RusToUTF_decode(std::string Str)
{
	std::string Out = "";
	char Buf[Str.length()];
	for(size_t i = 0; i < Str.length(); i++)
	{
		if (i + 5 < Str.length())
		{
			if (Str[i] == '%')
			{
				Str.copy(Buf, 3, i);
				if (strcmp(Buf, "%20") == 0)
				{
					Out += RusUTFToChar[KeyByValue(std::string(Buf), RusUTFToURL)];
					i += 2;
				}
				else if (strcmp(Buf, "%2F") == 0)
				{
					Out += RusUTFToChar[KeyByValue(std::string(Buf), RusUTFToURL)];
					i += 2;
				}
				else if (Str[i+3] == '%')
				{
					Str.copy(Buf, 6, i);
					i += 5;
					Out += RusUTFToChar[KeyByValue(std::string(Buf), RusUTFToURL)];
				}
				else
				{
					Out += Str[i];
				}
			}
			else
			{
				Out += Str[i];
			}
		}
		else if (i + 2 < Str.length())
		{
			Str.copy(Buf, 3, i);
			if (strcmp(Buf, "%20") == 0)
			{
				Out += RusUTFToChar[KeyByValue(std::string(Buf), RusUTFToURL)];
				i += 2;
			}
			else if (strcmp(Buf, "%2F") == 0)
			{
				Out += RusUTFToChar[KeyByValue(std::string(Buf), RusUTFToURL)];
				i += 2;
			}
			else
			{
				Out += Str[i];
			}
		}
		else
		{
			Out += Str[i];
		}
		for(size_t i = 0; i < Str.length(); i++)
			Buf[i] = 0;
	}
	return Out;
}

std::string RusToHtml_encode(std::string src)
{
	std::string Encoded;
	for(size_t i = 0; i < src.length(); i++)
	{
		if (src.length() - i > 1)
		{
			std::stringstream Char;
			Char << src[i] << src[i+1];
			try
			{
				std::string HtmlCode = KeyByValue(Char.str(), RusUTFToChar);
				Encoded += HtmlCode;
				i++;
			}
			catch (char const* err)
			{
				if (strcmp(err, "Key was not found") == 0)
					Encoded += src[i];
			}
		}
		else
			Encoded += src[i];
	}
	return Encoded;
}

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

std::string round_d(std::string a, int n)
{
	std::string b = "";
	size_t num = 0;
	if (n == -1)
	{
		for(int i = a.length()-1; i>=0; i--)
		{
			if(a[i]=='0');
			else
			{
				if (a[i] == '.')
					num = i-1;
				else
					num = i;
				break;
			}
		}
		for(size_t i = 0; i<=num; i++)
			b += a[i];
	}
	else if (n > 0)
	{
		for(int i = 0; a[i] != '.'; i++)
			b += a[i];
		for(size_t i = b.length()+1; i<b.length()+1+n; i++)
			b += a[i];
	}
	else if (n == 0)
		for(int i = 0; a[i] != '.'; i++)
			b += a[i];
	else
		std::printf("Not acceptable amount of chars after point %i", n);
	return b;
} 

class Size
{
	public:
		std::string Rounded()
		{
			double XBytes = (double)Bytes;
			std::string RoundedSize;
			if (XBytes < 1024)
				RoundedSize = round_d(to_string(XBytes), -1) + " B";
			else
			{
				XBytes = round(XBytes/1024*10)/10;
				if (XBytes < 1024)
					RoundedSize = round_d(to_string(XBytes), -1) + " KB";
				else
				{
					XBytes = round(XBytes/1024*10)/10;
					if (XBytes < 1024)
						RoundedSize = round_d(to_string(XBytes), -1) + " MB";
					else
					{
						XBytes = round(XBytes/1024*10)/10;
						if (XBytes < 1024)
							RoundedSize = round_d(to_string(XBytes), -1) + " GB";
						else
						{
							XBytes = round(XBytes/1024*10)/10;
							if (XBytes < 1024)
								RoundedSize = round_d(to_string(XBytes), -1) + " TB";
							else
							{
								XBytes = round(XBytes/1024*10)/10;
								if (XBytes < 1024)
									RoundedSize = round_d(to_string(XBytes), -1) + " PB";
								else
								{
									XBytes = round(XBytes/1024*10)/10;
									if (XBytes < 1024)
										RoundedSize = round_d(to_string(XBytes), -1) + " EB";
									else
									{
										XBytes = round(XBytes/1024*10)/10;
										if (XBytes < 1024)
											RoundedSize = round_d(to_string(XBytes), -1) + " ZB";
										else
										{
											XBytes = round(XBytes/1024*10)/10;
											RoundedSize = round_d(to_string(XBytes), -1) + " YB";
										}
									}
								}
							}
						}
					}
				}
			}
			return RoundedSize;
		}
		size_t Bytes;
};

class FileStruct
{
	public:
		std::string HtmlName;
		std::string VisualName()
		{
			std::string VisualName = "";
			std::string RusChar = "";
			for(size_t i = 0; i < HtmlName.length(); i++)
			{
				if(HtmlName[i] == '&' && HtmlName[i + 1] == '#' && HtmlName[i + 6] == ';')
				{
					for(size_t j = i; j<i+7; j++)
						RusChar += HtmlName[j];
					VisualName += RusUTFToChar[RusChar];
					RusChar = "";
					i += 6;
				}
				else
				{
					VisualName += HtmlName[i];
				}
			}
			return VisualName;
		}
		FS::path Directory;
		std::string WebPath()
		{
			bool WebPathBegin = false;
			FS::path WebPath = "/";
			for(auto& PathElement : Directory)
			{
				if (PathElement == "yourcloud")
					WebPathBegin = true;
				if (WebPathBegin)
					WebPath /= PathElement;
			}
			WebPath /= VisualName();
			return RusToHtml_encode(WebPath.string());
		}
		FS::path Path()
		{
			return Directory / VisualName();
		}
		std::string Date;
		Size DiskSize()
		{
			Size FileSize;
			FileSize.Bytes = FS::file_size(Path());
			return FileSize;
		}
};

class Storage
{
	public:
		std::vector <FileStruct> Files;
		FS::path Path;
		Size FreeSpace()
		{
			Size FreeSpace;
			FreeSpace.Bytes = FS::space(Path).free;
			return FreeSpace;
		}
		Size UsedSpace()
		{
			Size UsedPlace;
			UsedPlace.Bytes = 0;
			for(size_t i = 0; i < Files.size(); i++)
				UsedPlace.Bytes += Files[i].DiskSize().Bytes;
			return UsedPlace;
		}
		void Upload(std::vector <char> RequestBuf, FS::path Directory)
		{
			size_t ContentSize = RequestBuf.size();
			printf("Trying to upload file to CLOUD\n");
			if(ContentSize > FreeSpace().Bytes)
				printf("The size of uploading file resizes size of free place on the CLOUD\n");
			else
			{
				
				// Building uploading file structure by parsing POST request
				FileStruct File;
				printf("Parsing received POST-request... \n");
				File.Directory = Directory;
				File.HtmlName = "";
				size_t CountBuf = 0;
				while(RequestBuf[CountBuf]!='\n')
					CountBuf++;
				const int FirstBufLine = CountBuf++;
				std::string Line = "nothing";
				CountBuf = 0;
				while (Line.find("Content-Disposition") != 0)
				{
					Line = "";
					while (RequestBuf[CountBuf] != '\n')
					{
						Line += RequestBuf[CountBuf];
						CountBuf++;
					}
					CountBuf++;
				}
				CountBuf = FirstBufLine + Line.find("filename=") + 11; //in "filename=" 9 chars + 1 cause of '"' and + 1 case of 0 count in s.find
				std::stringstream HtmlBuf;
				while(RequestBuf[CountBuf]!='"')
				{
				    if ((int) RequestBuf[CountBuf] < 0)
				    {
				        HtmlBuf << "&#" << (int)RequestBuf[CountBuf] + 1104 << ";";
					}
				    else
				    {
				        HtmlBuf << RequestBuf[CountBuf];
					}
					CountBuf++;
				}
				File.HtmlName = HtmlBuf.str();
				printf("Found HTML name: %s \n", File.HtmlName.c_str());
				CountBuf = 0; 
				printf("Path for uploading file: %s \n", (File.Path().c_str()));
				
				// Separating file-data from request information in POST request
				size_t FileStart = 0;
				CountBuf = 0;
				for(size_t i = 0; i<ContentSize; i++)
				{
					if (RequestBuf[i] == '\n')
						CountBuf++;
					if (CountBuf == 4)
					{
						FileStart = i+1;
						break;
					}
				}
				size_t FileEnd = ContentSize - FirstBufLine - 5;
				size_t FileSize = ContentSize - (ContentSize - FileEnd + FileStart);
				
				//Recording file-data to file, using server name
				printf("Recording received file data (%li bytes) to file... ", FileSize);
				ofstream FileUpload(File.Path().string());
				for(size_t i = FileStart; i<FileEnd; i++)
				{
					FileUpload<<RequestBuf[i];
				}
				printf("Done.\n");
			}
		}
		void Delete(FS::path ElementPath, std::string extra)
		{
			std::cout << ElementPath << std::endl;
			std::cout << extra << std::endl;
			if (extra == "Everything in folder")
			{
				FS::remove_all(ElementPath);
				FS::create_directory(ElementPath);
			}
			else if (FS::is_directory(ElementPath))
			{
				FS::remove_all(ElementPath);
			}
			else
			{
				FS::remove(ElementPath);
			}
			Update();
		}
		void Update()
		{
			Files.erase(Files.begin(), Files.end());
			int Counter = 0;
			FileStruct EmptyFile;
			for(auto& File: FS::recursive_directory_iterator(Path))
			{
				if (not(File.is_directory()))
				{
					Files.push_back(EmptyFile);
				
					// Date modified file making
					struct tm * timeinfo;
					char buffer[100];
					auto FSTime = FS::last_write_time(File.path());
					std::time_t TTTime = to_time_t(FSTime);
					timeinfo = std::localtime(&TTTime);
					strftime(buffer, 100 ,"%d.%m.%Y\n%H:%M:%S", timeinfo);
					Files[Counter].Date = buffer;
					
					// File Names modifiying
					std::string FileName = File.path().filename().string();
					Files[Counter].HtmlName = RusToHtml_encode(FileName);
					
					Files[Counter].Directory = FSRelPath(File.path().string()).remove_filename();
					Counter++;
				}
			}
		}
};

std::string Redirect(std::string ToPath)
{
	std::string HtmlCode = "\
	<html>\n\
		<head>\n\
			<meta http-equiv=\"refresh\" content=\"0; URL=" + ToPath + "\" />\n\
		</head>\n\
		<body>\n\
			Wait...\n\
		</body>\n\
	<html>";
	return HtmlCode;
}

std::string NonAuthIndex = "\
<html>\n\
	<head>\n\
		<title>\n\
			CLOUD\n\
		</title>\n\
		<link rel=\"shortcut icon\" href=\"/sources/images/favicon.ico\" type=\"image/x-icon\">\n\
		<link rel=\"stylesheet\" type=\"text/css\" href=\"/sources/css/style.css\">\n\
	</head>\n\
	<body background = \"/sources/images/Cubes_Back.jpg\">\n\
		<div class=\"title_logo\">\n\
			<img src=\"/sources/images/Logo.jpg\" alt=\"No image found\" align=\"right\" width=\"20%\">\n\
			<img src=\"/sources/images/Logo.png\" alt=\"No image found\" align=\"left\" width=\"77%\">\n\
		</div>\n\
		<form style=\"padding-top:25%\" action=\"wait.html\" method=\"post\">\n\
			<table cellspacing=\"0\" align=\"center\" border=\"3\" width=\"300px\" height=\"300px\" bgcolor=\"white\">\n\
				<tr height=\"50\">\n\
					<td valing=\"middle\">\n\
						<h1 align=\"center\">LOG IN</h1>\n\
					</td>\n\
				</tr>\n\
				<tr border=\"0\">\n\
					<td valign=\"top\" align=\"center\">\n\
						<br>\n\
						Login<br>\n\
						<input name=\"Login\" type=\"text\" align=\"center\"><br>\n\
						<br>\n\
						Password<br>\n\
						<input name=\"Password\" type=\"password\" align=\"center\"><br>\n\
						<br><br>\n\
						<input value=\"Log in\" type=\"submit\">\n\
					</td>\n\
				</tr>\n\
			</table>\n\
		</form>\n\
	</body>\n\
</html>\n";

std::string AuthIndex = "\
<html>\n\
	<head>\n\
		<title>\n\
			Your Cloud\n\
		</title>\n\
		<link rel=\"shortcut icon\" href=\"/sources/images/favicon.ico\" type=\"image/x-icon\">\n\
		<meta http-equiv=\"refresh\" content=\"0;URL=/yourcloud/\" />\n\
	</head>\n\
	<body>\n\
	</body>\n\
<html>\n";

std::string AccessError(std::string ToPath)
{
	std::string HtmlCode = "\
	<html>\n\
		<head>\n\
			<title>\n\
				Access denied!\n\
			</title>\n\
			<link rel=\"shortcut icon\" href=\"/sources/images/favicon.ico\" type=\"image/x-icon\">\n\
			<link rel=\"stylesheet\" type=\"text/css\" href=\"/sources/css/style.css\">\n\
		</head>\n\
		<body background=\"/sources/images/Cubes_Back.jpg\">\n\
			<div align=\"center\">\n\
				<div class=\"page-header\" align=\"center\">\n\
					Authorization failed\n\
				</div>\n\
			</div>\n\
			<div class=\"files\">\n\
				<img src=\"/sources/images/Access_Error.jpg\" width=\"100%\">\n\
				<div class=\"autherror\">\n\
					<div class=\"left-if\" align=\"center\" style=\"position: absolute\">\n\
						<div class=\"if\" align=\"center\">\n\
							But if you're truly the owner...\n\
						</div>\n\
					</div>\n\
					<div class=\"right-if\" align=\"center\" style=\"position: absolute\">\n\
						<div class=\"if\" align=\"center\">\n\
							...or the confident of the owner...\n\
						</div>\n\
					</div>\n\
					<img src=\"/sources/images/Logo.jpg\" style=\"position: relative; left: 35%\" width=\"30%\">\n\
				</div>\n\
				<div class=\"center-if\" align=\"center\">\n\
					...try again. With correct login and password.\n\
				</div>\n\
				<div class=\"log-in-error\" align=\"center\">\n\
					<form action=\"" + ToPath + "\" method=\"post\">\n\
						<table cellspacing=\"0\" align=\"center\" border=\"3\" width=\"300px\" height=\"300px\" bgcolor=\"white\">\n\
							<tr height=\"50\">\n\
								<td valing=\"middle\">\n\
									<h1 align=\"center\">LOG IN</h1>\n\
								</td>\n\
							</tr>\n\
							<tr border=\"0\">\n\
								<td valign=\"top\" align=\"center\">\n\
									<br>\n\
									Login<br>\n\
									<input name=\"Login\" type=\"text\" align=\"center\"><br>\n\
									<br>\n\
									Password<br>\n\
									<input name=\"Password\" type=\"password\" align=\"center\"><br>\n\
									<br><br>\n\
									<input value=\"Log in\" type=\"submit\">\n\
								</td>\n\
							</tr>\n\
						</table>\n\
					</form>\n\
				</div>\n\
			</div>\n\
		</body>\n\
	</html>\n";
	return HtmlCode;
}
std::string OverSizeError = "\
<html>\n\
	<head>\n\
		<title>\n\
			No place for file\n\
		</title>\n\
	</head>\n\
	<body>\n\
		<div class=\"size-error\" align=\"center\">\n\
			Uploading file resizes free place in your CLOUD\n\
		</div>\n\
	</body>\n\
</html>\n";
