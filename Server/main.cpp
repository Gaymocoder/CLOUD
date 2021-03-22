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
	Config.close();
	std::mutex Mtx;
	const std::string CookieLogin = "login="+Login;
	const std::string CookiePassword = "password="+Password;
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
			
			auto ContentSize = req->GetContentSize();
			std::vector <char> Buf(ContentSize);
			req->GetContent(&Buf[0], ContentSize, true);
			// Cout of execution
			Storage CLOUD;
			CLOUD.Path = FSRelPath(RootDir) / "yourcloud";
			std::string Path = req->GetPath();
			Path = RootDir + Path + (Path == "/" ? DefaultPage : std::string());
			Path = RusToUTF_decode(Path);
			FS::path FsPath = FSRelPath(Path);
			std::string RefSite = "";
			std::string Referer = req->GetHeaderAttr(Http::Request::Header::Referer::Value);
			size_t CountBuf = 0;
			{
				std::stringstream Io;
				time (&rawtime);
				timeinfo = localtime (&rawtime);
				strftime (buffer, 100 ,"Request got on %d.%m.%Y at %H:%M:%S",timeinfo);
				for(size_t i = Referer.find("://") + 3; i < Referer.length(); i++)
					if (Referer[i] == '/')
					{
						CountBuf = i;
						break;
					}
				for(size_t i = 0; i < CountBuf; i++)
					RefSite += Referer[i];
				Io << buffer << std::endl
					<< "Path: " << FsPath << std::endl
					<< Http::Request::Header::Host::Name << ": "
					<< req->GetHeaderAttr(Http::Request::Header::Host::Value) << std::endl
					<< Http::Request::Header::Referer::Name << ": "
					<< req->GetHeaderAttr(Http::Request::Header::Referer::Value) << std::endl;
				std::lock_guard<std::mutex> Lock(Mtx);
				std::cout << Io.str();
			}
			CountBuf = 0;
			req->SetResponseAttr(Http::Response::Header::Server::Value, "MyTestServer");
			req->SetResponseAttr(Http::Response::Header::ContentType::Value,
								 Http::Content::TypeFromFileName(Path));
								 
			// Parsing of POST request
			std::string LogIn = "",
						PassIn = "",
						DeletePath = "",
						DeleteExtra = "";
			bool SendFileRequest = false,
				 AuthRequest = false,
				 DeleteRequest = false,
				 GetFileRequest = false,
				 GoToDir = false;
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
					RequestCheck = RusToUTF_decode(RequestCheck);
					for(size_t i = 0; i < RequestCheck.length(); i++)
						if (RequestCheck[i] == '+')
							RequestCheck[i] = ' ';
					if (RequestCheck.find("delete=") == 0)
					{
						if (RequestCheck.find("extra=") != std::string::npos)
						{
							CountBuf = 7; // in "delete=" 7
							while (CountBuf < RequestCheck.find("extra=") - 1)
							{
								DeletePath += RequestCheck[CountBuf];
								CountBuf++;
							}
							CountBuf = RequestCheck.find("extra=") + 6;
							while (CountBuf < RequestCheck.length())
							{
								DeleteExtra += RequestCheck[CountBuf];
								CountBuf++;
							}
						}
						else
						{
							CountBuf = 7; // in "delete=" 7
							while (CountBuf < RequestCheck.length())
							{
								DeletePath += RequestCheck[CountBuf];
								CountBuf++;
							}
						}
						DeleteRequest = true;
					}
					AuthRequest = false;
				}
				CountBuf = 0;
			}
			
			// If request is connected to file in CLOUD or dir in CLOUD
			if (Path.find("yourcloud") == 13)
			{
				if (Path[Path.length()-1] != '/')
					GetFileRequest = true;
				else
					GoToDir = true;
			}
				
			// Cookies analyzing and processing
			string Cookie;
			bool Authorized = false,
				 LoginCorrect = false,
				 PasswordCorrect = false;
			if (AuthRequest)
			{
				std::string LogInCookie = "login=" + LogIn;
				std::string PassInCookie = "password=" + PassIn;
				req->SetResponseAttr(Http::Response::Header::SetCookie::Value, LogInCookie);
				req->SetResponseAttr(Http::Response::Header::SetCookie::Value, PassInCookie);
				if (LogInCookie == CookieLogin)
					LoginCorrect = true;
				if (PassInCookie == CookiePassword)
					PasswordCorrect = true;
			}
			std::string Cookies = req->GetHeaderAttr(Http::Request::Header::Cookie::Value);
			auto CookiesList = GetCookies(Cookies);
			std::cout<<"Cookies: "<<Cookies<<endl<<endl;
			
			// Login and password checking
			if (not(AuthRequest))
				for(size_t i = 0; i<CookiesList.size(); i++)
				{
					if (CookiesList[i] == CookieLogin)
						LoginCorrect = true;
					if (CookiesList[i] == CookiePassword)
						PasswordCorrect = true;
				}
			if (LoginCorrect && PasswordCorrect)
				Authorized = true;
			
			// Uploading file to CLOUD
			if(SendFileRequest)
			{
				FS::path UpFileDir = FSRelPath(RootDir);
				std::string UpFileDirBuf = "";
				for(size_t i = RefSite.length() + 1; i < Referer.length(); i++)
				{
					UpFileDirBuf += Referer[i];
				}
				UpFileDir /= UpFileDirBuf;
				CLOUD.Upload(Buf, UpFileDir);
			}
			
			// Deleting elements
			if (DeleteRequest)
				CLOUD.Delete(FSRelPath(DeletePath), DeleteExtra);
				
			extern std::string AuthIndex,
							   NonAuthIndex,
							   OverSizeError;
			std::string HtmlOut = "";
			CLOUD.Update();
			if (Authorized == true)
			{
				if (GoToDir)
				{
					HtmlOut += "\
						<html>\n\
							<head>\n\
								<title>\n\
									Your CLOUD\n\
								</title>\n\
								<link rel=\"shortcut icon\" href=\"/sources/images/favicon.ico\" type=\"image/x-icon\">\n\
								<link rel=\"stylesheet\" type=\"text/css\" href=\"/sources/css/style.css\">\n\
							</head>\n\
							<body background = \"/sources/images/Cubes_Back.jpg\">\n\
								<div class=\"exit\">\n\
									<form action = \"" + RefSite + "/wait.html\" method=\"post\">\n\
										<input value=\"Exit\" type=\"submit\" style=\"width: 100px; height: 50px; font-size: 30px\">\n\
										<input value=\"" + Login + "incorrect\" name=\"Login\" type=\"text\" align=\"center\" style=\"display: none\"><br>\n\
										<input value=\"" + Password + "incorrect\" name=\"Password\" type=\"password\" align=\"center\" style=\"display: none\"><br>\n\
									</form>\n\
								</div>\n\
								<div class=\"files\" style=\"height: 90%; border: 0; background: 0\">\n\
									<div class=\"files\" style=\"width: 99%; top: 0; left: 0\">\n\
										<table cellspacing=\"0\" cellpadding=\"0\" width=\"100%\" height=\"100%\" border=\"3\" bgcolor=\"white\">\n\
											<tr>\n\
												<form method=\"post\">\n\
													<td width=\"7.7%\" height=\"15%\" align=\"center\">\n\
														<div class=\"table-header-1\" style=\"width: 100%; height: 100%; font-size: 30px\">\n\
															<input type=\"submit\" value=\"Delete all\" style=\"width: 100%; height: 100%; font-size: 30px\">\n\
															<input type=\"text\" name=\"delete\" value=\"" + FsPath.string() + "\" style=\"display: none\">\n\
															<input type=\"text\" name=\"extra\" value=\"Everything in folder\" style=\"display: none\">\n\
														</div>\n\
													</td>\n\
												</form>\n\
												<td width=\"38.5%\" height=\"15%\" align=\"center\">\n\
													<div class=\"table-header-1\">\n\
														Name\n\
													</div>\n\
												</td>\n\
												<td width=\"15.4%\" height=\"15%\" align=\"center\">\n\
													<div class=\"table-header-1\">\n\
														Date\n\
													</div>\n\
												</td>\n\
												<td width=\"15.4%\" height=\"15%\" align=\"center\">\n\
													<div class=\"table-header-1\">\n\
														Size\n\
													</div>\n\
												</td>\n\
											</tr>\n";
					int FileCounter = 0;
					for(FileStruct& File : CLOUD.Files)
					{
						if (File.Directory == FsPath)
						{
							FileCounter++;
							HtmlOut += "\
											<tr>\n\
												<form method=\"post\">\n\
													<td width=\"7.7%\" height=\"10%\" align=\"center\">\n\
														<div class=\"file-struct\" style=\"width: 100%; height: 100%; font-size: 30px\">\n\
															<input type=\"submit\" value=\"Delete file\" style=\"width: 100%; height: 100%; font-size: 30px\">\n\
															<input type=\"text\" name=\"delete\" value=\"" + (File.Directory / File.VisualName()).string() + "\" style=\"display: none\">\n\
														</div>\n\
													<td width=\"38.5%\" height=\"10%\" align=\"center\">\n\
												</form>\n\
													<div class=\"file-struct\">\n\
														<a href=\"" + File.WebPath() + "\" download>" + File.HtmlName + "<a/>\n\
													</div>\n\
												</td>\n\
												<td width=\"15.4%\" height=\"10%\" align=\"center\">\n\
													<div class=\"file-struct\">\n" + \
														File.Date + "\
													</div>\n\
												</td>\n\
												<td width=\"15.4%\" height=\"10%\" align=\"center\">\n\
													<div class=\"file-struct\">\n" + \
														File.DiskSize().Rounded() + "\n\
													</div>\n\
												</td>\n\
											</tr>\n";
						}
					}
					if (FileCounter == 0)
					{
						HtmlOut += "\
											<tr>\n\
												<td width=\"100%\" align=\"center\" valign=\"center\" colspan=\"4\">\n\
													<div class=\"no-content\">\n\
														No uploaded files found\n\
													</div>\n\
												</td>\n\
											</tr>\n";
					}
					else
						HtmlOut += "\
											<tr>\n\
											</tr>\n";
					HtmlOut += "\
										</table>\n\
									</div>\n\
									<div class=\"size-size\">\n\
										<div align=\"left\">\n\
											<div class=\"free-size\" align=\"center\">\n" + \
												CLOUD.FreeSpace().Rounded() + " left\n\
											</div>\n\
										</div>\n\
										<div align=\"right\">\n\
											<div class=\"used-size\" align=\"center\">\n" + \
												CLOUD.UsedSpace().Rounded() + " used\n\
											</div>\n\
										</div>\n\
										<div class=\"send-file\" align=\"center\">\n\
											<div class=\"form-file-sender\" align=\"center\">\n\
												<form enctype=\"multipart/form-data\" method=\"POST\" action=\"" + RefSite + "/wait.html\">\n\
													<input type=\"file\" name=\"ffff\"><br><br>\n\
													<input type=\"submit\" value=\"Send file\"><br>\n\
												</form>\n\
											</div>\n\
										</div>\n\
									</div>\n\
								</div>\n\
							</body>\n\
						</html>\n";
					req->SetResponseString(HtmlOut);
				}
				else if (SendFileRequest)
				{
					if(ContentSize > CLOUD.FreeSpace().Bytes)
						req->SetResponseString(OverSizeError);
					else if (FsPath == FSRelPath(RootDir) / "wait.html")
					{
						std::cout << "Redirecting..." << std::endl;
						req->SetResponseString(Redirect(Referer));
					}
				}
				else if (FsPath == FSRelPath(RootDir) / "index.html")
				{
					req->SetResponseString(AuthIndex);
				}
				else if (FsPath == FSRelPath(RootDir) / "wait.html")
				{
					req->SetResponseString(Redirect(Referer));
				}
				else
					req->SetResponseFile(FsPath);
			}
			else
			{
				if (GetFileRequest || GoToDir)
				{
					req->SetResponseString(AccessError(RefSite + "/wait.html"));
				}
				else if (FsPath == FSRelPath(RootDir) / "index.html")
				{
					req->SetResponseString(NonAuthIndex);
				}
				else if (FsPath == FSRelPath(RootDir) / "wait.html")
				{
					if (Referer == RefSite + "/")
					{
						req->SetResponseString(Redirect(RefSite + "/yourcloud/"));
					}
					else if (Referer == RefSite + "/yourcloud/")
					{
						req->SetResponseString(Redirect(RefSite));
					}
					else
						req->SetResponseString(Redirect(Referer));
				}
				else
					req->SetResponseFile(FsPath);
			}
		});
		std::cin.get();
	}
	catch (std::exception const &e)
	{
		std::cout << e.what() << std::endl;
	}
	return 0;
}
