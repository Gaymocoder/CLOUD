#include "http_server.h"
#include "http_headers.h"
#include "http_content_type.h"
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <mutex>
#include <evhttp.h>
#include <vector>
#include "file_const.h"
#include <cstring>
#include <ctime>

using namespace std;

int main()
{
	std::string SrvAddress, RootDir, DefaultPage, Login, Password, Line, LineBuf;
	std::uint16_t SrvPort = 0, SrvThreadCount = 0;
	ifstream Config("../config.ini");
	
	// Start of parsing config.ini with initializing main variables
	printf("Starting config file parsing\n");
	while (getline(Config, Line))
	{
		if (Line.find("Server IP:") == 0)
		{
			for(size_t i = 11; i<Line.length(); i++)
			{
				LineBuf += Line[i];
			}
			SrvAddress = LineBuf;
			LineBuf = "";
			printf("Found server IPv4 of CLOUD: %s \n", SrvAddress.c_str());
		}
		if (Line.find("Port:") == 0)
		{
			for(size_t i = 6; i<Line.length(); i++)
			{
				LineBuf += Line[i];
			}
			SrvPort = std::stoi(LineBuf);
			LineBuf = "";
			printf("Found server port of CLOUD: %i \n", SrvPort);
		}
		if (Line.find("Threads count:") == 0)
		{
			for(size_t i = 15; i<Line.length(); i++)
			{
				LineBuf += Line[i];
			}
			SrvThreadCount = std::stoi(LineBuf);
			LineBuf = "";
			printf("Found server port of CLOUD: %i \n", SrvThreadCount);
		}
		if (Line.find("Root Directory:") == 0)
		{
			for(size_t i = 16; i<Line.length(); i++)
			{
				LineBuf += Line[i];
			}
			RootDir = LineBuf;
			LineBuf = "";
			printf("Found server directory: %s \n", RootDir.c_str());
		}
		if (Line.find("Default Page:") == 0)
		{
			for(size_t i = 14; i<Line.length(); i++)
			{
				LineBuf += Line[i];
			}
			DefaultPage = LineBuf;
			LineBuf = "";
			printf("Found name of default page file: %s \n", DefaultPage.c_str());
		}
		if (Line.find("CLOUD Login:") == 0)
		{
			for(size_t i = 13; i<Line.length(); i++)
			{
				LineBuf += Line[i];
			}
			Login = LineBuf;
			LineBuf = "";
			printf("Found login of CLOUD: %s \n", Login.c_str());
		}
		if (Line.find("CLOUD Password:") == 0)
		{
			for(size_t i = 16; i<Line.length(); i++)
			{
				LineBuf += Line[i];
			}
			Password = LineBuf;
			LineBuf = "";
			printf("Found password of CLOUD: %s \n", Password.c_str());
		}
	}
	std::mutex Mtx;
	const std::string CookieLogin = "login="+Login;
	const std::string CookiePassword = "password="+Password;
	file_struct File;
	time_t rawtime;
	struct tm * timeinfo;
	char buffer[100];
	time (&rawtime);
	timeinfo = localtime (&rawtime);
	strftime (buffer, 100 ,"Server started on %d.%m.%Y at %H:%M:%S\n\n",timeinfo);
	printf("%s", buffer);
	
	// Server initializing, cycle starting
	try
	{
		using namespace Network;
		HttpServer Srv(SrvAddress, SrvPort, SrvThreadCount,
		[&] (IHttpRequestPtr req) 
		{
			
			// Cout of execution
			std::string Path = req->GetPath();
			Path = RootDir + Path + (Path == "/" ? DefaultPage : std::string());
			{
				std::stringstream Io;
				time (&rawtime);
				timeinfo = localtime (&rawtime);
				strftime (buffer, 100 ,"Request got on %d.%m.%Y at %H:%M:%S",timeinfo);
				Io << buffer << std::endl
					<< "Path: " << Path << std::endl
					<< Http::Request::Header::Host::Name << ": "
					<< req->GetHeaderAttr(Http::Request::Header::Host::Value) << std::endl
					<< Http::Request::Header::Referer::Name << ": "
					<< req->GetHeaderAttr(Http::Request::Header::Referer::Value) << std::endl;
				std::lock_guard<std::mutex> Lock(Mtx);
				std::cout << Io.str();
			}
			req->SetResponseAttr(Http::Response::Header::Server::Value, "MyTestServer");
			req->SetResponseAttr(Http::Response::Header::ContentType::Value,
								 Http::Content::TypeFromFileName(Path));
								 
			// Parsing of POST request
			std::string LogIn = "",
						PassIn = "",
						DeleteNumber = "";
			bool SendFileRequest = false,
				 AuthRequest = false,
				 DeleteRequest = false,
				 GetFileRequest = false;
			size_t CountBuf = 0;
			auto ContentSize = req->GetContentSize();
			std::vector <char> Buf(ContentSize);
			req->GetContent(&Buf[0], ContentSize, true);
			for(size_t i = 0; i<ContentSize; i++)
			{
				if (Buf[i] == '\n')
				{
					SendFileRequest = true;
					break;
				}
			}
			if (not SendFileRequest)
			{
				string RequestCheck = "";
				while (CountBuf<ContentSize)
				{
					RequestCheck += Buf[CountBuf];
					CountBuf++;
				}
				if (RequestCheck.find("Login=") == 0)
				{
					AuthRequest = true;
					CountBuf = 6; // in "Login=" 6
					while (Buf[CountBuf] != '&')
					{
						LogIn += Buf[CountBuf];
						CountBuf++;
					}
					CountBuf += 10; // in "Password=" 10
					while (CountBuf < ContentSize)
					{
						PassIn += Buf[CountBuf];
						CountBuf++;
					}
				}
				else
				{
					if (RequestCheck.find("delete=") == 0)
					{
						CountBuf = 7; // in "delete=" 7
						while (CountBuf < ContentSize)
						{
							DeleteNumber += Buf[CountBuf];
							CountBuf++;
						}
						DeleteRequest = true;
					}
					AuthRequest = false;
				}
				CountBuf = 0;
			}
			
			// If request is connected to file in CLOUD
			if (Path.find("yourcloud") == 13)
				GetFileRequest = true;
				
			// Cookies analyzing and processing
			string Cookie;
			bool Authorized = false;
			if (AuthRequest == true)
			{
				string LogInCookie = "login=" + LogIn;
				string PassInCookie = "password=" + PassIn;
				req->SetResponseAttr(Http::Response::Header::SetCookie::Value, LogInCookie);
				req->SetResponseAttr(Http::Response::Header::SetCookie::Value, PassInCookie);
			}
			std::string Cookies = req->GetHeaderAttr(Http::Request::Header::Cookie::Value);
			auto CookiesList = GetCookies(Cookies);
			std::cout<<"Cookies: "<<Cookies<<endl<<endl;
			
			// Login and password checking
			bool LoginCorrect = false, PasswordCorrect = false;
			for(size_t i = 0; i<CookiesList.size(); i++)
			{
				if (CookiesList[i] == CookieLogin)
					LoginCorrect = true;
				if (CookiesList[i] == CookiePassword)
					PasswordCorrect = true;
			}
			if (LoginCorrect && PasswordCorrect)
				Authorized = true;
			   
			// Place on the disk analyzing
			std::string PublicNamesPath = "../Interface/sources/content/public_name.txt",
						ServerNamesPath = "../Interface/sources/content/server_name.txt",
						DatesPath = "../Interface/sources/content/dates.txt",
						SizesPath = "../Interface/sources/content/sizes.txt",
						FSPath = "../Interface/sources/content/free_size.txt";
			double BytedFS = 0,
				   FS = 0;
			string RoundedFS;
			char CommandBuf[200];
			for(int i = 0; i<200; i++)
				CommandBuf[i] = 0;
			strcat(CommandBuf, "df ../Interface/yourcloud/ | awk '{print $4}' > ");
			strcat(CommandBuf, FSPath.c_str());
			system(CommandBuf);
			for(int i = 0; i<200; i++)
				CommandBuf[i] = 0;
			ifstream FSFile(FSPath);
			while(getline(FSFile,Line))
			{
				if(CountBuf>0)
				{
					BytedFS = stod(Line)*1024;
					FS = stod(Line);
					if(FS<1024)
						RoundedFS = round_d(to_string(FS), -1) + " KB";
					 else
					{
						FS = round(FS/1024*10)/10;
						if(FS<1024)
							RoundedFS = round_d(to_string(FS), -1) + " MB";
						else
						{
							FS = round(FS/1024*10)/10;
							if(FS<1024)
								RoundedFS = round_d(to_string(FS), -1) + " GB";
							else
								RoundedFS = round_d(to_string(FS), -1) + " TB";
						}
					}
				}
				CountBuf++;
			}
			
			// Uploading file to CLOUD
			if(SendFileRequest)
			{
				printf("Trying to upload file to CLOUD\n");
				if(ContentSize > BytedFS)
					printf("The size of uploading file resizes size of free place on the CLOUD\n");
				else
				{
					
					// Building uploading file structure by parsing POST request
					printf("Parsing received POST-request... \n");
					File.Path = "";
					File.ServerName = "";
					File.PublicName = "";				
					CountBuf = 0;
					while(Buf[CountBuf]!='\n')
						CountBuf++;
					const int FirstBufLine = CountBuf++;
					Line = "nothing";
					CountBuf = 0;
					while (Line.find("Content-Disposition") != 0)
					{
						Line = "";
						while (Buf[CountBuf] != '\n')
						{
							Line += Buf[CountBuf];
							CountBuf++;
						}
						CountBuf++;
					}
					CountBuf = FirstBufLine + Line.find("filename=") + 11; //in "filename=" 9 chars + 1 cause of '"' and + 1 case of 0 count in s.find
					bool CharIsRus;
					std::string RusChar = "";
					std::string EnChar = "";
					while(Buf[CountBuf]!='"')
					while(Buf[CountBuf]!='"')
					{
						File.PublicName += Buf[CountBuf];
						CountBuf++;
					}
					printf("Found Public name: %s \n", File.PublicName.c_str());
					CountBuf = 0; 
					
					// Building server file name for uploading file
					printf("Building name for server for uploading file: ");
					while(CountBuf < File.PublicName.length())
					{
						CharIsRus = false;
						if(File.PublicName[CountBuf]=='&' && File.PublicName[CountBuf + 1] == '#')
						{
							CharIsRus = true;
							for(size_t i = CountBuf; i<CountBuf+7; i++)
								RusChar+=File.PublicName[i];
							if(RusChar == "&#1025;")
								EnChar = "Yo";
							else if(RusChar == "&#1105;")
								EnChar = "yo";
							else if(RusChar == "&#1040;")
								EnChar = "A";
							else if(RusChar == "&#1041;")
								EnChar = "B";
							else if(RusChar == "&#1042;")
								EnChar = "V";
							else if(RusChar == "&#1043;")
								EnChar = "G";
							else if(RusChar == "&#1044;")
								EnChar = "D";
							else if(RusChar == "&#1045;")
								EnChar = "E";
							else if(RusChar == "&#1046;")
								EnChar = "J";
							else if(RusChar == "&#1047;")
								EnChar = "Z";
							else if(RusChar == "&#1048;")
								EnChar = "I";
							else if(RusChar == "&#1049;")
								EnChar = "Y";
							else if(RusChar == "&#1050;")
								EnChar = "K";
							else if(RusChar == "&#1051;")
								EnChar = "L";
							else if(RusChar == "&#1052;")
								EnChar = "M";
							else if(RusChar == "&#1053;")
								EnChar = "N";
							else if(RusChar == "&#1054;")
								EnChar = "O";
							else if(RusChar == "&#1055;")
								EnChar = "P";
							else if(RusChar == "&#1056;")
								EnChar = "R";
							else if(RusChar == "&#1057;")
								EnChar = "S";
							else if(RusChar == "&#1058;")
								EnChar = "T";
							else if(RusChar == "&#1059;")
								EnChar = "U";
							else if(RusChar == "&#1060;")
								EnChar = "F";
							else if(RusChar == "&#1061;")
								EnChar = "H";
							else if(RusChar == "&#1062;")
								EnChar = "Cc";
							else if(RusChar == "&#1063;")
								EnChar = "Ch";
							else if(RusChar == "&#1064;")
								EnChar = "Sh";
							else if(RusChar == "&#1065;")
								EnChar = "Sh'";
							else if(RusChar == "&#1066;");
							else if(RusChar == "&#1067;")
								EnChar = "I";
							else if(RusChar == "&#1068;");
							else if(RusChar == "&#1069;")
								EnChar = "E";
							else if(RusChar == "&#1070;")
								EnChar = "Yu";
							else if(RusChar == "&#1071;")
								EnChar = "Ya";
							else if(RusChar == "&#1072;")
								EnChar = "a";
							else if(RusChar == "&#1073;")
								EnChar = "b";
							else if(RusChar == "&#1074;")
								EnChar = "v";
							else if(RusChar == "&#1075;")
								EnChar = "g";
							else if(RusChar == "&#1076;")
								EnChar = "d";
							else if(RusChar == "&#1077;")
								EnChar = "e";
							else if(RusChar == "&#1078;")
								EnChar = "j";
							else if(RusChar == "&#1079;")
								EnChar = "z";
							else if(RusChar == "&#1080;")
								EnChar = "i";
							else if(RusChar == "&#1081;")
								EnChar = "y";
							else if(RusChar == "&#1082;")
								EnChar = "k";
							else if(RusChar == "&#1083;")
								EnChar = "l";
							else if(RusChar == "&#1084;")
								EnChar = "m";
							else if(RusChar == "&#1085;")
								EnChar = "n";
							else if(RusChar == "&#1086;")
								EnChar = "o";
							else if(RusChar == "&#1087;")
								EnChar = "p";
							else if(RusChar == "&#1088;")
								EnChar = "r";
							else if(RusChar == "&#1089;")
								EnChar = "s";
							else if(RusChar == "&#1090;")
								EnChar = "t";
							else if(RusChar == "&#1091;")
								EnChar = "u";
							else if(RusChar == "&#1092;")
								EnChar = "f";
							else if(RusChar == "&#1093;")
								EnChar = "h";
							else if(RusChar == "&#1094;")
								EnChar = "cc";
							else if(RusChar == "&#1095;")
								EnChar = "ch";
							else if(RusChar == "&#1096;")
								EnChar = "sh";
							else if(RusChar == "&#1097;")
								EnChar = "sh'";
							else if(RusChar == "&#1098;");
							else if(RusChar == "&#1099;")
								EnChar = "i";
							else if(RusChar == "&#1100;");
							else if(RusChar == "&#1101;")
								EnChar = "e";
							else if(RusChar == "&#1102;")
								EnChar = "yu";
							else if(RusChar == "&#1103;")
								EnChar = "ya";
							File.ServerName += EnChar;
							CountBuf++;
						}
						if(CharIsRus)
						{
							printf("%s", EnChar.c_str());
							CountBuf += 6;
							RusChar = "";
							EnChar = "";
						}
						else
						{
							if (File.PublicName[CountBuf] == ' ')
							{
								printf("_");
								File.ServerName += '_';
							}
							else
							{
								printf("%c", File.PublicName[CountBuf]);
								File.ServerName += File.PublicName[CountBuf];
							}
							CountBuf++;
						}
					}
					File.Path = "../Interface/yourcloud/" + File.ServerName;
					printf("\nPath for uploading file: %s \n", File.Path.c_str());
					
					// Separating file-data from request information in POST request
					size_t FileStart = 0;
					CountBuf = 0;
					for(size_t i = 0; i<ContentSize; i++)
					{
						if (Buf[i] == '\n')
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
					ofstream FileUpload(File.Path);
					for(size_t i = FileStart; i<FileEnd; i++)
					{
						FileUpload<<Buf[i];
					}
					printf("Done.\n");
					
					// Updating data in system files
					ifstream ServerNames(ServerNamesPath);
					ifstream PublicNames(PublicNamesPath);
					Line = "";
					std::string ServerNamesBuf = "",
								PublicNamesBuf = "";
					while(getline(ServerNames, Line))
					{
						if(Line==File.ServerName);
						else
							ServerNamesBuf += Line + '\n';
					}
					ServerNamesBuf += File.ServerName;
					while(getline(PublicNames, Line))
					{
						if(Line==File.PublicName);
						else
							PublicNamesBuf += Line + '\n';
					}
					ofstream ServerNamesNew(ServerNamesPath);
					ofstream PublicNamesNew(PublicNamesPath);
					PublicNamesBuf += File.PublicName;
					ServerNamesNew<<ServerNamesBuf;
					PublicNamesNew<<PublicNamesBuf;
				}
			}
			std::string SortedPath = "../Interface/sources/content/sorted.txt";
			strcat(CommandBuf, "ls -1 ../Interface/yourcloud > ");
			strcat(CommandBuf, SortedPath.c_str());
			system(CommandBuf);
			for(int i = 0; i<200; i++)
				CommandBuf[i] = 0;
			strcat(CommandBuf, "ls ../Interface/yourcloud  -l --time-style=\"+%Y-%m-%d_%H:%M:%S\" | awk '{print $6}' > ");
			strcat(CommandBuf, DatesPath.c_str());
			system(CommandBuf);
			for(int i = 0; i<200; i++)
				CommandBuf[i] = 0;
			strcat(CommandBuf, "ls -l ../Interface/yourcloud | awk '{print $5}' > ");
			strcat(CommandBuf, SizesPath.c_str());
			system(CommandBuf);
			for(int i = 0; i<200; i++)
				CommandBuf[i] = 0;
			vector <file_struct> Files;
			Files = Sort(ServerNamesPath, PublicNamesPath, SortedPath, SizesPath, DatesPath);
			if (DeleteRequest)
			{
				Files = FileDelete(DeleteNumber, Files);
				Files = Sort(ServerNamesPath, PublicNamesPath, SortedPath, SizesPath, DatesPath);
			}
			std::string UsedSize;
			if (Files.size() == 0)
				UsedSize = "0 B";
			else
				UsedSize = Files[0].UsedSize;
				
			printf("Remaking html pages... ");
			ofstream index("../Interface/index.html");
			ofstream delay_web("../Interface/delay.html");
			ofstream access_error("../Interface/access_error.html");
			ofstream send_file("../Interface/send-file.html");
			ofstream exit("../Interface/exit.html");
			ofstream delay_web1("../Interface/delay1.html");
			exit<<"\
				<html>"<<endl<<"\
					<head>"<<endl<<"\
						<link rel=\"shortcut icon\" href=\"/sources/images/favicon.ico\" type=\"image/x-icon\">"<<endl<<"\
						<meta http-equiv=\"refresh\" content=\"0;URL=index.html\">"<<endl<<"\
					</head>"<<endl<<"\
					<body>"<<endl<<"\
					</body>"<<endl<<"\
				</html>"<<endl<<"\
			";
			delay_web1<<"\
				<html>"<<endl<<"\
					<head>"<<endl<<"\
						<link rel=\"shortcut icon\" href=\"/sources/images/favicon.ico\" type=\"image/x-icon\">"<<endl<<"\
						<meta http-equiv=\"refresh\" content=\"0;URL=delay.html\">"<<endl<<"\
					</head>"<<endl<<"\
					<body>"<<endl<<"\
					</body>"<<endl<<"\
				</html>"<<endl<<"\
			";
			if (Authorized == true)
			{
				index<<"\
					<html>"<<endl<<"\
						<head>"<<endl<<"\
							<title>"<<endl<<"\
								Your CLOUD"<<endl<<"\
							</title>"<<endl<<"\
							<link rel=\"shortcut icon\" href=\"/sources/images/favicon.ico\" type=\"image/x-icon\">"<<endl<<"\
							<link rel=\"stylesheet\" type=\"text/css\" href=\"/sources/css/style.css\">"<<endl<<"\
						</head>"<<endl<<"\
						<body background = \"/sources/images/Cubes_Back.jpg\">"<<endl<<"\
							<div class=\"exit\">"<<endl<<"\
								<form action=\"exit.html\" method=\"post\">"<<endl<<"\
									<input value=\"Exit\" type=\"submit\" style=\"width: 100px; height: 50px; font-size: 30px\">"<<endl<<"\
									<input value=\""<<Login<<"incorrect\" name=\"Login\" type=\"text\" align=\"center\" style=\"display: none\"><br>"<<endl<<"\
									<input value=\""<<Password<<"incorrect\" name=\"Password\" type=\"password\" align=\"center\" style=\"display: none\"><br>"<<endl<<"\
								</form>"<<endl<<"\
							</div>"<<endl<<"\
							<div class=\"files\" style=\"height: 90%; border: 0; background: 0\">"<<endl<<"\
								<div class=\"files\" style=\"width: 99%; top: 0; left: 0\">"<<endl<<"\
									<table cellspacing=\"0\" cellpadding=\"0\" width=\"100%\" height=\"100%\" border=\"3\" bgcolor=\"white\">"<<endl<<"\
										<tr>"<<endl<<"\
											<form action=\"delay.html\" method=\"post\">"<<endl<<"\
												<td width=\"7.7%\" height=\"15%\" align=\"center\">"<<endl<<"\
													<div class=\"table-header-1\" style=\"width: 100%; height: 100%; font-size: 30px\">"<<endl<<"\
														<input type=\"submit\" value=\"Delete all\" style=\"width: 100%; height: 100%; font-size: 30px\">"<<endl<<"\
														<input type=\"text\" name=\"delete\" value=\"all\" style=\"display: none\">"<<endl<<"\
													</div>"<<endl<<"\
												</td>"<<endl<<"\
											</form>"<<endl<<"\
											<td width=\"38.5%\" height=\"15%\" align=\"center\">"<<endl<<"\
												<div class=\"table-header-1\">"<<endl<<"\
													Name"<<endl<<"\
												</div>"<<endl<<"\
											</td>"<<endl<<"\
											<td width=\"15.4%\" height=\"15%\" align=\"center\">"<<endl<<"\
												<div class=\"table-header-1\">"<<endl<<"\
													Date"<<endl<<"\
												</div>"<<endl<<"\
											</td>"<<endl<<"\
											<td width=\"15.4%\" height=\"15%\" align=\"center\">"<<endl<<"\
												<div class=\"table-header-1\">"<<endl<<"\
													Size"<<endl<<"\
												</div>"<<endl<<"\
											</td>"<<endl<<"\
										</tr>"<<endl;
				if(Files.size()>0)
				{
					for(size_t i = 0; i<Files.size(); i++)
					{	
						index<<"\
										<tr>"<<endl<<"\
											<form action=\"delay.html\" method=\"post\">"<<endl<<"\
												<td width=\"7.7%\" height=\"10%\" align=\"center\">"<<endl<<"\
													<div class=\"file-struct\" style=\"width: 100%; height: 100%; font-size: 30px\">"<<endl<<"\
														<input type=\"submit\" value=\"Delete file\" style=\"width: 100%; height: 100%; font-size: 30px\">"<<endl<<"\
														<input type=\"text\" name=\"delete\" value=\""<<i<<"\" style=\"display: none\">"<<endl<<"\
													</div>"<<endl<<"\
												<td width=\"38.5%\" height=\"10%\" align=\"center\">"<<endl<<"\
											</form>"<<endl<<"\
												<div class=\"file-struct\">"<<endl<<"\
													<a href=\"yourcloud/"<<Files[i].ServerName<<"\" download>"<<Files[i].PublicName<<"<a/>"<<endl<<"\
												</div>"<<endl<<"\
											</td>"<<endl<<"\
											<td width=\"15.4%\" height=\"10%\" align=\"center\">"<<endl<<"\
												<div class=\"file-struct\">"<<endl<<\
													Files[i].Date<<endl<<"\
												</div>"<<endl<<"\
											</td>"<<endl<<"\
											<td width=\"15.4%\" height=\"10%\" align=\"center\">"<<endl<<"\
												<div class=\"file-struct\">"<<endl<<\
													Files[i].Size<<endl<<"\
												</div>"<<endl<<"\
											</td>"<<endl<<"\
										</tr>"<<endl;
					}
					index<<"\
										<tr>"<<endl<<"\
										</tr>"<<endl;
				}
				else
				{
					index<<"\
										<tr>"<<endl<<"\
											<td width=\"100%\" align=\"center\" valign=\"center\" colspan=\"4\">"<<endl<<"\
												<div class=\"no-content\">"<<endl<<"\
													No uploaded files found"<<endl<<"\
												</div>"<<endl<<"\
											</td>"<<endl<<"\
										</tr>"<<endl;
				}
				index<<"\
									</table>"<<endl<<"\
								</div>"<<endl<<"\
								<div class=\"size-size\">"<<endl<<"\
									<div align=\"left\">"<<endl<<"\
										<div class=\"free-size\" align=\"center\">"<<endl<<\
											RoundedFS<<" left"<<endl<<"\
										</div>"<<endl<<"\
									</div>"<<endl<<"\
									<div align=\"right\">"<<endl<<"\
										<div class=\"used-size\" align=\"center\">"<<endl<<\
											UsedSize<<" used"<<endl<<"\
										</div>"<<endl<<"\
									</div>"<<endl<<"\
									<div class=\"send-file\" align=\"center\">"<<endl<<"\
										<div class=\"form-file-sender\" align=\"center\">"<<endl<<"\
											<form enctype=\"multipart/form-data\" method=\"POST\" action=\"send-file.html\">"<<endl<<"\
												<input type=\"file\" name=\"ffff\"><br><br>"<<endl<<"\
												<input type=\"submit\" value=\"Send file\"><br>"<<endl<<"\
											</form>"<<endl<<"\
										</div>"<<endl<<"\
									</div>"<<endl<<"\
								</div>"<<endl<<"\
							</div>"<<endl<<"\
						</body>"<<endl<<"\
					</html>"<<endl;
				delay_web<<"\
					<html>"<<endl<<"\
						<head>"<<endl<<"\
							<title>"<<endl<<"\
								Your Cloud"<<endl<<"\
							</title>"<<endl<<"\
							<meta http-equiv=\"refresh\" content=\"0;URL=index.html\">"<<endl<<"\
						</head>"<<endl<<"\
						<body>"<<endl<<"\
						</body>"<<endl<<"\
					</html>"<<endl;
				if(ContentSize > BytedFS)
				{
					send_file<<"\
						<html>"<<endl<<"\
							<head>"<<endl<<"\
								<title>"<<endl<<"\
									No place for file"<<endl<<"\
								</title>"<<endl<<"\
							</head>"<<endl<<"\
							<body>"<<endl<<"\
								<div class=\"size-error\" align=\"center\">"<<endl<<"\
									Uploading file resizes free place in your CLOUD"<<endl<<"\
								</div>"<<endl<<"\
							</body>"<<endl<<"\
						</html>"<<endl;	
				}
				else
				{
					send_file<<"\
						<html>"<<endl<<"\
							<head>"<<endl<<"\
								<meta http-equiv=\"refresh\" content=\"0;URL=index.html\">"<<endl<<"\
							</head>"<<endl<<"\
							<body>"<<endl<<"\
								Wait..."<<endl<<"\
							</body>"<<endl<<"\
						</html>"<<endl;
				}
				req->SetResponseFile(Path);
			}
			else
			{
				access_error<<"\
					<html>"<<endl<<"\
						<head>"<<endl<<"\
							<title>"<<endl<<"\
								Access denied!"<<endl<<"\
							</title>"<<endl<<"\
							<link rel=\"shortcut icon\" href=\"/sources/images/favicon.ico\" type=\"image/x-icon\">"<<endl<<"\
							<link rel=\"stylesheet\" type=\"text/css\" href=\"/sources/css/style.css\">"<<endl<<"\
						</head>"<<endl<<"\
						<body background=\"/sources/images/Cubes_Back.jpg\">"<<endl<<"\
							<div align=\"center\">"<<endl<<"\
								<div class=\"page-header\" align=\"center\">"<<endl<<"\
									Authorization failed"<<endl<<"\
								</div>"<<endl<<"\
							</div>"<<endl<<"\
							<div class=\"files\">"<<endl<<"\
								<img src=\"/sources/images/Access_Error.jpg\" width=\"100%\">"<<endl<<"\
								<div class=\"autherror\">"<<endl<<"\
									<div class=\"left-if\" align=\"center\" style=\"position: absolute\">"<<endl<<"\
										<div class=\"if\" align=\"center\">"<<endl<<"\
											But if you're truly the owner..."<<endl<<"\
										</div>"<<endl<<"\
									</div>"<<endl<<"\
									<div class=\"right-if\" align=\"center\" style=\"position: absolute\">"<<endl<<"\
										<div class=\"if\" align=\"center\">"<<endl<<"\
											...or the confident of the owner..."<<endl<<"\
										</div>"<<endl<<"\
									</div>"<<endl<<"\
									<img src=\"/sources/images/Logo.jpg\" style=\"position: relative; left: 35%\" width=\"30%\">"<<endl<<"\
								</div>"<<endl<<"\
								<div class=\"center-if\" align=\"center\">"<<endl<<"\
									...try again. With correct login and password."<<endl<<"\
								</div>"<<endl<<"\
								<div class=\"log-in-error\" align=\"center\">"<<endl<<"\
									<form action=\"delay1.html\" method=\"post\">"<<endl<<"\
										<table cellspacing=\"0\" align=\"center\" border=\"3\" width=\"300px\" height=\"300px\" bgcolor=\"white\">"<<endl<<"\
											<tr height=\"50\">"<<endl<<"\
												<td valing=\"middle\">"<<endl<<"\
													<h1 align=\"center\">LOG IN</h1>"<<endl<<"\
												</td>"<<endl<<"\
											</tr>"<<endl<<"\
											<tr border=\"0\">"<<endl<<"\
												<td valign=\"top\" align=\"center\">"<<endl<<"\
													<br>"<<endl<<"\
													Login<br>"<<endl<<"\
													<input name=\"Login\" type=\"text\" align=\"center\"><br>"<<endl<<"\
													<br>"<<endl<<"\
													Password<br>"<<endl<<"\
													<input name=\"Password\" type=\"password\" align=\"center\"><br>"<<endl<<"\
													<br><br>"<<endl<<"\
													<input value=\"Log in\" type=\"submit\">"<<endl<<"\
												</td>"<<endl<<"\
											</tr>"<<endl<<"\
										</table>"<<endl<<"\
									</form>"<<endl<<"\
								</div>"<<endl<<"\
							</div>"<<endl<<"\
						</body>"<<endl<<"\
					</html>"<<endl<<"\
				";
				index<<"\
					<html>"<<endl<<"\
						<head>"<<endl<<"\
							<title>"<<endl<<"\
								CLOUD"<<endl<<"\
							</title>"<<endl<<"\
							<link rel=\"shortcut icon\" href=\"/sources/images/favicon.ico\" type=\"image/x-icon\">"<<endl<<"\
							<link rel=\"stylesheet\" type=\"text/css\" href=\"/sources/css/style.css\">"<<endl<<"\
						</head>"<<endl<<"\
						<body background = \"/sources/images/Cubes_Back.jpg\">"<<endl<<"\
							<div class=\"title_logo\">"<<endl<<"\
								<img src=\"/sources/images/Logo.jpg\" alt=\"No image found\" align=\"right\" width=\"20%\">"<<endl<<"\
								<img src=\"/sources/images/Logo.png\" alt=\"No image found\" align=\"left\" width=\"77%\">"<<endl<<"\
							</div>"<<endl<<"\
							<form style=\"padding-top:25%\" action=\"delay1.html\" method=\"post\">"<<endl<<"\
								<table cellspacing=\"0\" align=\"center\" border=\"3\" width=\"300px\" height=\"300px\" bgcolor=\"white\">"<<endl<<"\
									<tr height=\"50\">"<<endl<<"\
										<td valing=\"middle\">"<<endl<<"\
											<h1 align=\"center\">LOG IN</h1>"<<endl<<"\
										</td>"<<endl<<"\
									</tr>"<<endl<<"\
									<tr border=\"0\">"<<endl<<"\
										<td valign=\"top\" align=\"center\">"<<endl<<"\
											<br>"<<endl<<"\
											Login<br>"<<endl<<"\
											<input name=\"Login\" type=\"text\" align=\"center\"><br>"<<endl<<"\
											<br>"<<endl<<"\
											Password<br>"<<endl<<"\
											<input name=\"Password\" type=\"password\" align=\"center\"><br>"<<endl<<"\
											<br><br>"<<endl<<"\
											<input value=\"Log in\" type=\"submit\">"<<endl<<"\
										</td>"<<endl<<"\
									</tr>"<<endl<<"\
								</table>"<<endl<<"\
							</form>"<<endl<<"\
						</body>"<<endl<<"\
					</html>"<<endl<<"\
				";
				delay_web<<"\
					<html>"<<endl<<"\
						<head>"<<endl<<"\
							<title>"<<endl<<"\
								Your Cloud"<<endl<<"\
							</title>"<<endl<<"\
							<link rel=\"shortcut icon\" href=\"/sources/images/favicon.ico\" type=\"image/x-icon\">"<<endl<<"\
							<meta http-equiv=\"refresh\" content=\"0;URL=access_error.html\" />"<<endl<<"\
						</head>"<<endl<<"\
						<body>"<<endl<<"\
						</body>"<<endl<<"\
					</html>"<<endl;
				if (GetFileRequest)
				{
					std::stringstream GetFileError;
					GetFileError << ifstream("../Interface/index.html").rdbuf();
					throw HttpRequestException(GetFileError.str());
				}
				else
					req->SetResponseFile(Path);
			}
			printf("Done.\n\n");
		});
		std::cin.get();
	}
	catch (std::exception const &e)
	{
		std::cout << e.what() << std::endl;
	}
	return 0;
}
