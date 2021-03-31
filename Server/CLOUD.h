#include <fstream>
#include <vector>
#include <cmath>
#include "en_de_coders.h"
using namespace std;

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
		std::string Date()
		{
			struct tm * timeinfo;
			char buffer[100];
			auto FSTime = FS::last_write_time(Path());
			std::time_t TTTime = to_time_t(FSTime);
			timeinfo = std::localtime(&TTTime);
			strftime(buffer, 100 ,"%d.%m.%Y\n%H:%M:%S", timeinfo);
			std::string Date = buffer;
			return Date;
		}
		Size DiskSize()
		{
			Size FileSize;
			FileSize.Bytes = FS::file_size(Path());
			return FileSize;
		}
};

class DirStruct
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
			WebPath /= VisualName() + FS::path::preferred_separator;
			return RusToHtml_encode(WebPath.string());
		}
		FS::path Path()
		{
			return Directory / VisualName();
		}
		std::string Date()
		{
			struct tm * timeinfo;
			char buffer[100];
			time_t max = 0;
			std::time_t TTTime;
			FS::file_time_type FSTime;
			for(auto& File: FS::recursive_directory_iterator(Path()))
				if (not(File.is_directory()))
				{
					FSTime = FS::last_write_time(File.path());
					TTTime = to_time_t(FSTime);
					if ((int) TTTime > max)
						max = TTTime;
				}
			timeinfo = std::localtime(&max);
			strftime(buffer, 100 ,"%d.%m.%Y\n%H:%M:%S", timeinfo);
			std::string Date = buffer;
			return Date;
		}
		Size DiskSize()
		{
			Size DirSize;
			DirSize.Bytes = 0;
			for(auto& File: FS::recursive_directory_iterator(Path()))
				if (not(File.is_directory()))
					DirSize.Bytes += File.file_size();
			return DirSize;
		}

};

class NewFile
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
		FS::path Path()
		{
			return Directory / VisualName();
		}
		size_t ContentStart;
		size_t ContentEnd;
		void Record(std::vector <char> *src)
		{
			ofstream FileUp(Path().string());
			for(size_t i = ContentStart; i <= ContentEnd; i++)
				FileUp << (*src)[i];
			FileUp.close();
		}
};

class Storage
{
	public:
		std::vector <FileStruct> Files;
		std::vector <DirStruct> Dirs;
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
		void Upload(std::vector <char> *RequestBuf, FS::path Directory, size_t ContentSize)
		{
			printf("Trying to upload file to CLOUD\n");
			if(ContentSize > FreeSpace().Bytes)
				printf("The size of uploading file resizes size of free place on the CLOUD\n");
			else
			{
				std::vector <NewFile> NewFiles;
				NewFile EmptyFile;
				int FilesCount = 0;
				size_t CountBuf = 0;
				std::string Separator = GetLines(RequestBuf, 0, 1);
				printf("Found Separator of POST-request: %s", Separator.c_str());
				for(size_t i = 0; i + Separator.length() < ContentSize - 2; i++)
				{
					if ((int) (*RequestBuf)[i] == 10 || (int) (*RequestBuf)[i] == 13 || i == 0)
					{
						std::string CheckBuf = "";
						if ((int) (*RequestBuf)[i] == 10)
							i++;
						else if ((int) (*RequestBuf)[i] == 13)
						{
							if ((int) (*RequestBuf)[i+1] == 10)
							{
								i += 2;
							}
							else
							{
								i++;
							}
						}
						CountBuf = i;
						for(; i < CountBuf + Separator.length(); i++)
							CheckBuf += (*RequestBuf)[i];
						if (LinesSame(Separator.c_str(), CheckBuf.c_str()))
						{
							if (CountBuf != 0)
							{
								if ((int) (*RequestBuf)[i - CheckBuf.length() - 2] == 13)
									if ((int) (*RequestBuf)[i - CheckBuf.length() - 4] == 13)
										NewFiles[FilesCount].ContentEnd = i - CheckBuf.length() - 5;
									else
										NewFiles[FilesCount].ContentEnd = i - CheckBuf.length() - 4;
								else
									if ((int) (*RequestBuf)[i - CheckBuf.length() - 3] == 13)
										NewFiles[FilesCount].ContentEnd = i - CheckBuf.length() - 4;
									else
										NewFiles[FilesCount].ContentEnd = i - CheckBuf.length() - 3;
								FilesCount++;
							}
							if ((int) (*RequestBuf)[i]	== 10)
								i++;
							NewFiles.push_back(EmptyFile);
							CheckBuf = GetLines(RequestBuf, i, 1);
							std::stringstream HtmlBuf;
							for(int j = CheckBuf.find("filename=") + 10; CheckBuf[j] != '"'; j++)
							{
								int IntCheckBuf = (int) CheckBuf[j];
								printf("%i", IntCheckBuf);
								if (IntCheckBuf < 0)
								{
									if (IntCheckBuf == -88)
										HtmlBuf << "&#" << 1025 << ";";
									else if (IntCheckBuf == -72)
										HtmlBuf << "&#" << 1105 << ";";
									else 
										HtmlBuf << "&#" << IntCheckBuf + 1104 << ";";
								}
								else
									HtmlBuf << CheckBuf[j];
							}
							NewFiles[FilesCount].HtmlName = HtmlBuf.str();
							std::cout << NewFiles[FilesCount].HtmlName << std::endl;
							std::cout << NewFiles[FilesCount].VisualName() << std::endl;
							NewFiles[FilesCount].Directory = Directory;
							i += CheckBuf.length();
							i += GetLines(RequestBuf, i, 2).length();
							NewFiles[FilesCount].ContentStart = i;
						}
						else
						{
							if ((*RequestBuf)[i-1] == '-' && (*RequestBuf)[i-2] == '-' && i == ContentSize - 2)
							{
								if ((int) (*RequestBuf)[i - CheckBuf.length() - 2] == 13)
								{
									if ((int) (*RequestBuf)[i - CheckBuf.length() - 4] == 13)
									{
										NewFiles[FilesCount].ContentEnd = i - CheckBuf.length() - 4;
									}
									else
									{
										NewFiles[FilesCount].ContentEnd = i - CheckBuf.length() - 3;
									}
								}
								else
								{
									if ((int) (*RequestBuf)[i - CheckBuf.length() - 3] == 13)
									{
										NewFiles[FilesCount].ContentEnd = i - CheckBuf.length() - 3;
									}
									else
									{
										NewFiles[FilesCount].ContentEnd = i - CheckBuf.length() - 2;
									}
								}
								break;
							}
							else
							{
								i = CountBuf - 1;
							}
						}
					}
				}
				printf((std::string("Detected %ld file") + std::string(NewFiles.size() == 1 ? "":"s") + std::string("\n")).c_str(), NewFiles.size());
				for(NewFile& File : NewFiles)
				{
					printf("Recording file with path \"%s\" (%ld bytes)... ", File.Path().string().c_str(), File.ContentEnd - File.ContentStart + 2);
					fflush(stdout);
					File.Record(RequestBuf);
					printf("Done.\n");
				}
				
			}
		}
		void Delete(FS::path ElementPath, std::string extra)
		{
			if (extra == "Everything in folder")
			{
				printf("Deleting everything from directory \"%s\"... ", ElementPath.string().c_str());
				fflush(stdout);
				FS::remove_all(ElementPath);
				FS::create_directory(ElementPath);
				printf("Done.\n");
			}
			else if (FS::is_directory(ElementPath))
			{
				printf("Deleting directory \"%s\"... ", ElementPath.string().c_str());
				fflush(stdout);
				FS::remove_all(ElementPath);
				printf("Done.\n");
			}
			else
			{
				printf("Deleting file \"%s\"... ", ElementPath.string().c_str());
				fflush(stdout);
				FS::remove(ElementPath);
				printf("Done.\n");
			}
			Update();
		}
		void NewFolder(FS::path NewFolderPath)
		{
			printf("Creating folder (\"%s\"... ", NewFolderPath.string().c_str());
			fflush(stdout);
			FS::create_directory(NewFolderPath);
			printf("Done.\n");
		}
		void Rename(FS::path OldPath, FS::path NewPath)
		{
			printf("Directory \"%s\": Renaming file \"%s\" to \"%s\"... ", (OldPath.parent_path() / "").string().c_str(), OldPath.filename().string().c_str(), NewPath.filename().string().c_str());
			fflush(stdout);
			try
			{
				FS::rename(OldPath, NewPath);
			}
			catch(FS::filesystem_error &err)
			{
				printf("\n%s", err.what());
			}
			printf("Done.\n");
			
		}
		void Update()
		{
			std::cout << "Updating CLOUD..." << std::endl;
			
			Files.erase(Files.begin(), Files.end());
			Dirs.erase(Dirs.begin(), Dirs.end());
			
			int FileCounter = 0;
			int DirCounter = 0;
			FileStruct EmptyFile;
			DirStruct EmptyDir;
			
			for(auto& File: FS::recursive_directory_iterator(Path))
			{
				if (not(File.is_directory()))
				{
					Files.push_back(EmptyFile);
					
					// FileStruct.HtmlName modifiying
					std::string FileName = File.path().filename().string();
					Files[FileCounter].HtmlName = RusToHtml_encode(FileName);
										
					// FileStruct.Directory building
					Files[FileCounter].Directory = FSRelPath(File.path().string()).remove_filename();
					
					printf("Detected file (\"%s\")\n", Files[FileCounter].Path().string().c_str());
					
					FileCounter++;
				}
				else
				{
					Dirs.push_back(EmptyDir);
					
					// DirStruct.HtmlName modifiying
					std::string DirName = File.path().filename().string();
					Dirs[DirCounter].HtmlName = RusToHtml_encode(DirName);
										
					//DirStruct.Directory building
					Dirs[DirCounter].Directory = FSRelPath(File.path().string()).remove_filename();
					
					printf("Detected directory (\"%s/\")\n", Dirs[DirCounter].Path().string().c_str());
					
					DirCounter++;
				}
			}
			
			printf("\n");
		}
};
