#include "http_server.h"
#include "http_headers.h"
#include "http_content_type.h"
#include "CLOUD.h"
#include "html.h"
#include <mutex>
#include <evhttp.h>
#include <cstring>
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
			
			// Declaring main variables
			Storage *CLOUD = new Storage;
			std::string Path = req->GetPath();
			DirStruct CurrentDir;
			auto ContentSize = req->GetContentSize();
			std::vector <char> *Buf = new std::vector <char> (ContentSize);
			std::string Referer = req->GetHeaderAttr(Http::Request::Header::Referer::Value),
						RefSite = "",
						RefPath = "";
			size_t CountBuf = 0;
			std::string Cookies = req->GetHeaderAttr(Http::Request::Header::Cookie::Value);
			
			
			// Processing and fulling them with data
			(*CLOUD).Path = FSRelPath(RootDir) / "yourcloud";
			
			Path = RusToUTF_decode(RootDir + Path + (Path == "/" ? DefaultPage : std::string()));
			FS::path FsPath = FSRelPath(Path);
			
			CurrentDir.Directory = FsPath.parent_path();
			CurrentDir.HtmlName = RusToHtml_encode(*(--(FsPath.end())));
			
			for(size_t i = Referer.find("://") + 3; i < Referer.length(); i++)
			{
				if (Referer[i] == '/')
				{
					CountBuf = i;
					break;
				}
			}
			for(size_t i = 0; i < CountBuf; i++)
				RefSite += Referer[i];
			for(size_t i = RefSite.length() + 1; i < Referer.length(); i++)
				RefPath += Referer[i];
			auto CookiesList = GetCookies(Cookies);
			
			
			// Making base output of got request
			{
				std::stringstream Io;
				time (&rawtime);
				timeinfo = localtime (&rawtime);
				strftime (buffer, 100 ,"Request got on %d.%m.%Y at %H:%M:%S",timeinfo);
				Io << buffer << std::endl
					<< "Path: " << FsPath << std::endl
					<< Http::Request::Header::Host::Name << ": "
					<< req->GetHeaderAttr(Http::Request::Header::Host::Value) << std::endl
					<< Http::Request::Header::Referer::Name << ": "
					<< req->GetHeaderAttr(Http::Request::Header::Referer::Value) << std::endl
					<< "Cookies: " << Cookies << std::endl;
				std::lock_guard<std::mutex> Lock(Mtx);
				std::cout << Io.str();
			}
			
			
			// Setting response base http headers
			req->SetResponseAttr(Http::Response::Header::Server::Value, "MyTestServer");
			req->SetResponseAttr(Http::Response::Header::ContentType::Value,
								 Http::Content::TypeFromFileName(Path));
			
			
			// Declaring variables for CLOUD request types
			std::string LogIn = "",
						PassIn = "",
						DeletePath = "",
						NewName = "",
						DeleteExtra = "";
						
			FS::path NewFolderPath = "",
					 RenameFromPath = "",
					 RenameToPath = "";
			
			bool SendFileRequest = false,
				 AuthRequest = false,
				 DeleteRequest = false,
				 GetFileRequest = false,
				 GoToDir = false,
				 NewFolderRequest = false,
				 RenameRequest = false,
				 OutSideRequest = false;
				 
			
			// Analyzing request type
			
			// Parsing of POST request if it exists
			if (ContentSize > 0)
			{
				req->GetContent(&(*Buf)[0], ContentSize, true);
				std::stringstream Buff;
				for(size_t i = 0; i < ContentSize && (int) (*Buf)[i] != 10 && (int) (*Buf)[i] != 13; i++)
				{
					Buff << (*Buf)[i];
				}
				
				// Checking for SendFileRequest
				if ((*Buf)[0] == '-' && (*Buf)[1] == '-')
				{
					std::string CheckBuf = "";
					for(int i = 0; i < 80; i++)
						CheckBuf += (*Buf)[i];
					if (CheckBuf.find("\n") != std::string::npos)
					{
						try
						{
							if (GetLines(Buf, 0, 2).find("filename=") != std::string::npos)
							{
								if (strcmp(GetLines(Buf, (*Buf).size() - GetLines(Buf, 0, 1).length() - 2, 1).c_str(), (GetLines(Buf, 0, 1) + "--\n").c_str()))
								{
									if (Referer.find("yourcloud") != std::string::npos)
									{
										SendFileRequest = true;
									}
									else
									{
										OutSideRequest = true;
									}
								}
								else
								{
									OutSideRequest = true;
								}
							}
							else
							{
								OutSideRequest = true;
							}
						}
						catch (const char* err)
						{
							OutSideRequest = true;
						}	
					}
					else
					{
						OutSideRequest = true;
					}
				}
				else
				{
					try
					{
						GetLines(Buf, 0, 2);
						OutSideRequest = true;
					}
					catch (const char* err){};
				}
				if (not SendFileRequest && not OutSideRequest)
				{
					CountBuf = 0;
					string RequestCheck = "";
					while (CountBuf<ContentSize)
					{
						RequestCheck += (*Buf)[CountBuf];
						CountBuf++;
					}
					
					// Checking for AuthRequest
					if (RequestCheck.find("Login=") == 0)
					{
						AuthRequest = true;
						CountBuf = 6; // in "Login=" 6
						while ((*Buf)[CountBuf] != '&')
						{
							LogIn += (*Buf)[CountBuf];
							CountBuf++;
						}
						CountBuf += 10; // in "Password=" 10
						while (CountBuf < ContentSize)
						{
							PassIn += (*Buf)[CountBuf];
							CountBuf++;
						}
					}
					else
					{
						if (not OutSideRequest)
						{
							for(size_t i = 0; i < ContentSize; i++)
								if (RequestCheck[i] == '+')
									RequestCheck[i] = ' ';
									
							// Checking for DeleteRequest
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
									DeletePath = RusToUTF_decode(DeletePath);
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
							
							// Checking for NewFolderRequest
							else if (RequestCheck.find("NewFolder=") == 0)
							{
								std::string NewFolderName = "";
								for(size_t i = 10; i < ContentSize; i++)
									NewFolderName += RequestCheck[i];
								NewFolderPath = RusToUTF2_decode((FSRelPath(RootDir) / RefPath / NewFolderName).string());
								NewFolderRequest = true;
							}
							
							// Checking for RenameRequest
							else if (RequestCheck.find("PathToRename=") == 0 && RequestCheck.find("NewName=") != std::string::npos)
							{
								CountBuf = 13; // in "PathToRename=" 13
								std::string OldName = "",
											NewName = "";
								RenameFromPath = FSRelPath(RootDir) / RefPath;
								while (CountBuf < RequestCheck.find("NewName=") - 1)
								{
									OldName += RequestCheck[CountBuf];
									CountBuf++;
								}
								RenameToPath = RenameFromPath;
								RenameFromPath /= OldName;
								CountBuf = RequestCheck.find("NewName=") + 8;
								while (CountBuf < RequestCheck.length())
								{
									NewName += RequestCheck[CountBuf];
									CountBuf++;
								}
								RenameToPath /= NewName;
								if (*(--(RenameFromPath.end())) == "..%2F")
								{
									RenameFromPath = RenameFromPath.parent_path() / "";
									RenameToPath = RenameToPath.parent_path().replace_filename(NewName);
								}
								try
								{
									RenameFromPath = RusToUTF2_decode(RenameFromPath.string());
								}
								catch (const char* err)
								{
									RenameFromPath = RusToUTF_decode(RenameFromPath.string());
								}
								try
								{
									RenameToPath = RusToUTF2_decode(RenameToPath.string());
								}
								catch (const char* err)
								{
									RenameToPath = RusToUTF_decode(RenameToPath.string());
								}
								RenameRequest = true;								
							}
							
							// If none of CLOUD requests was recognized in recieved one, it is OutSideRequest
							else
							{
								OutSideRequest = true;
							}
							AuthRequest = false;
						}
					}
					CountBuf = 0;
				}
			}
			
			// Checking for GoToDirRequest or GetFileRequest
			if (FsPath.string().find("yourcloud") != std::string::npos)
			{
				if (Path[Path.length()-1] != '/')
					GetFileRequest = true;
				else
					GoToDir = true;
			}
				
			
			// Making actions for calculated request type
			
			// Authorization cookies analyzing and processing (if client has been authorized), changing them for AuthRequest
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
			else
			{
				for(size_t i = 0; i<CookiesList.size(); i++)
				{
					if (CookiesList[i] == CookieLogin)
						LoginCorrect = true;
					if (CookiesList[i] == CookiePassword)
						PasswordCorrect = true;
				}
			}
			if (LoginCorrect && PasswordCorrect)
				Authorized = true;
			
			// Uploading file to CLOUD for SendFileRequest
			if(SendFileRequest && Authorized)
			{
				FS::path UpFileDir = FSRelPath(RootDir);
				std::string UpFileDirBuf = "";
				for(size_t i = RefSite.length() + 1; i < Referer.length(); i++)
				{
					UpFileDirBuf += Referer[i];
				}
				UpFileDir /= UpFileDirBuf;
				(*CLOUD).Upload(Buf, RusToUTF_decode(UpFileDir), ContentSize);
			}
			
			// Deleteing elements from CLOUD for DeleteRequest
			if (DeleteRequest && Authorized)
			{
				(*CLOUD).Delete(FS::path(RusToUTF_decode(DeletePath)), DeleteExtra);
			}
			
			// Creating new folder for NewFolderRequest
			if (NewFolderRequest && Authorized)
			{
				(*CLOUD).NewFolder(NewFolderPath);
			}
			
			// Changing name of CLOUD element for RenameRequest
			if (RenameRequest && Authorized)
			{
				(*CLOUD).Rename(RenameFromPath, RenameToPath);
			}
			
			
			// Updating CLOUD data and freeing up memory
			(*CLOUD).Update();
			delete Buf;
			
			
			// Building HTML response for client
			std::string HtmlOut = "";
			
			// Declaring HTML constants from html.h
			extern std::string AuthIndex,
							   NonAuthIndex,
							   OverSizeError;
							   
			// Building response for authorized client
			if (Authorized == true)
			{
				// Building base CLOUD page
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
								<script>\n\
									function htmlDecode(value)\n\
									{ \n\
									   var NewElement = document.createElement('div');\n\
									   NewElement.innerHTML = value;\n\
									   return NewElement.innerHTML;\n\
									}\n\
									function EnterNameOfFolder()\n\
									{\n\
										var NewFolderTD = document.getElementById(\"NewFolder\");\n\
										var OldElement = NewFolderTD.innerHTML;\n\
										if (OldElement.includes('Delete all'))\n\
										{\n\
											NewFolderTD.innerHTML = '<input type=\"text\" form=\"delete or create\" name=\"NewFolder\" required value=\"New folder\" maxlength=\"50\"><br><input type=\"submit\" form=\"delete or create\" value=\"Create new folder\">'\n\
										}\n\
										else if (OldElement.includes('NewFolder'))\n\
										{\n\
											NewFolderTD.innerHTML = '<div class=\"table-header-1\" style=\"width: 100%; height: 100%; font-size: 10px\"><input type=\"submit\" form=\"delete or create\" value=\"Delete all\" style=\"width: 100%; height: 100%; font-size: 30px\"><input type=\"text\" form=\"delete or create\" name=\"delete\" value=\"" + FsPath.string() + "\" style=\"display: none\"><input type=\"text\" form=\"delete or create\" name=\"extra\" value=\"Everything in folder\" style=\"display: none\"></div>'\n\
										}\n\
									}\n\
									function DeleteOrCreate(form)\n\
									{\n\
										var NewFolderTD = document.getElementById(\"NewFolder\");\n\
										var ElementChildHTML = NewFolderTD.innerHTML;\n\
										if (ElementChildHTML.includes('Delete all'))\n\
										{\n\
											if (confirm(htmlDecode(\"" + RusToHtml_encode("Треубется подтверждение для удаления всего содержимого данной директории\"") + ")))\n\
												form.submit();\n\
										}\n\
										else if (ElementChildHTML.includes('NewFolder'))\n\
										{\n\
											form.submit();\n\
										}\n\
									}\n\
									function EditNameOfF(id)\n\
									{\n\
										var Element = document.getElementById(id);\n\
										var OldElement = Element.innerHTML;\n\
										var OldName = Element.textContent.replace(/[/]/g, '');\n\
										if (OldElement.includes('<a '))\n\
										{\n\
											Element.innerHTML = ('<form action = \"" + RefSite + "/wait.html\" method=\"post\"><input value=\"' + id + '\" name=\"PathToRename\" type=\"text\" align=\"center\" style=\"display: none\"><br><input value=\"' + OldName + '\" name=\"NewName\" type=\"text\"><br><input value=\"Rename\" type=\"submit\"><br></form>');\n\
										}\n\
										else if (OldElement.includes('<input'))\n\
										{\n\
											if (id.includes('/'))\n\
											{\n\
												Element.innerHTML = '<div class=\"file-struct\"><a href=\"' + document.URL + id + '\">' + id + '</a></div>'\n\
											}\n\
											else\n\
											{\n\
												Element.innerHTML = '<div class=\"file-struct\"><a href=\"' + document.URL + id + '\" download>' + id + '</a></div>'\n\
											}\n\
										}\n\
									}\n\
								</script>\n\
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
												<form method=\"POST\" id=\"delete or create\" action=\"" + RefSite + "/wait.html\" onSubmit=\"DeleteOrCreate(this); return false;\">\n\
													<td width=\"5%\" height=\"15%\" align=\"center\">\n\
														<button type=\"button\" style=\"width: 100%; height: 100%; font-size: 20px\" onclick=\"EnterNameOfFolder()\">New<br>Folder</button>\n\
													</td>\n\
													<td width=\"7.7%\" id=\"NewFolder\" height=\"15%\" align=\"center\">\n\
														<div class=\"table-header-1\" style=\"width: 100%; height: 100%; font-size: 10px\">\n\
															<input type=\"submit\" form=\"delete or create\" value=\"Delete all\" style=\"width: 100%; height: 100%; font-size: 30px\">\n\
															<input type=\"text\" form=\"delete or create\" name=\"delete\" value=\"" + FsPath.string() + "\" style=\"display: none\">\n\
															<input type=\"text\" form=\"delete or create\" name=\"extra\" value=\"Everything in folder\" style=\"display: none\">\n\
														</div>\n\
													</td>\n\
													<td width=\"5%\" height=\"15%\" align=\"center\">\n\
														Rename\n\
													</td>\n\
													<td width=\"31.5%\" height=\"15%\" align=\"center\">\n\
														<div class=\"table-header-1\">\n\
															Name\n\
														</div>\n\
													</td>\n\
													<td width=\"15.4%\" height=\"15%\" align=\"center\">\n\
														<div class=\"table-header-1\">\n\
															Date\n\
														</div>\n\
													</td>\n\
													<td width=\"10.4%\" height=\"15%\" align=\"center\">\n\
														<div class=\"table-header-1\">\n\
															Size\n\
														</div>\n\
													</td>\n\
												</form>\n\
											</tr>\n";
					int FileCounter = 0;
					int DirCounter = 0;
					if (FsPath != (*CLOUD).Path / "")
						HtmlOut += "\
											<tr>\n\
												<form method=\"post\" action=\"" + RefSite + "/wait.html\">\n\
													<td width=\"5%\" height=\"10%\" align=\"center\">\n\
														<img src=\"/sources/images/Folder Icon.png\" style=\"position: inherit;display: block; height: 70px\">\n\
													</td>\n\
													<td width=\"7.7%\" height=\"10%\" align=\"center\">\n\
														<div class=\"file-struct\" style=\"width: 100%; height: 100%; font-size: 30px\">\n\
															<input type=\"submit\" value=\"Delete folder\" style=\"width: 100%; height: 100%; font-size: 30px\">\n\
															<input type=\"text\" name=\"delete\" value=\"" + (CurrentDir.Path() / "").string() + "\" style=\"display: none\">\n\
															<input type=\"text\" name=\"extra\" value=\"Folder is current\" style=\"display: none\">\n\
														</div>\n\
													</td>\n\
													<td width=\"5%\" height=\"15%\" align=\"center\">\n\
														<button type=\"button\" style=\"width: 100%; height: 100%; font-size: 20px\" onclick=\"EditNameOfF('../')\">Rename<br>Cur.<br>Folder</button>\n\
													</td>\n\
													<td width=\"31.5%\" id=\"../\" height=\"10%\" align=\"center\">\n\
														<div class=\"file-struct\">\n\
															<a href=\"" + RusToHtml_encode(RusToUTF_decode(req->GetPath())) + "..\">../<a/>\n\
														</div>\n\
													</td>\n\
													<td width=\"15.4%\" height=\"10%\" align=\"center\">\n\
														<div class=\"file-struct\">\n" + \
															CurrentDir.Date() + "\
														</div>\n\
													</td>\n\
													<td width=\"10.4%\" height=\"10%\" align=\"center\">\n\
														<div class=\"file-struct\">\n" + \
															CurrentDir.DiskSize().Rounded() + "\n\
														</div>\n\
													</td>\n\
												</form>\n\
											</tr>\n";
					for(DirStruct& Dir : (*CLOUD).Dirs)
					{
						if (Dir.Directory == FsPath)
						{
							DirCounter++;
							HtmlOut += "\
											<tr>\n\
												<form method=\"post\" action=\"" + RefSite + "/wait.html\" onSubmit='return confirm(\"" + RusToHtml_encode("Требуется подтверждение для удаления директории") + "\");'>\n\
													<td width=\"5%\" height=\"10%\" align=\"center\">\n\
														<img src=\"/sources/images/Folder Icon.png\" style=\"position: inherit;display: block; height: 70px\">\n\
													</td>\n\
													<td width=\"7.7%\" height=\"10%\" align=\"center\">\n\
														<div class=\"file-struct\" style=\"width: 100%; height: 100%; font-size: 30px\">\n\
															<input type=\"submit\" value=\"Delete folder\" style=\"width: 100%; height: 100%; font-size: 30px\">\n\
															<input type=\"text\" name=\"delete\" value=\"" + Dir.Path().string() + FS::path::preferred_separator + "\" style=\"display: none\">\n\
														</div>\n\
													</td>\n\
													<td width=\"5%\" height=\"15%\" align=\"center\">\n\
														<button type=\"button\" style=\"width: 100%; height: 100%; font-size: 20px\" onclick=\"EditNameOfF('" + Dir.HtmlName + "/')\">Rename<br>Folder</button>\n\
													</td>\n\
													<td width=\"31.5%\" id=\"" + Dir.HtmlName + "/\" height=\"10%\" align=\"center\">\n\
														<div class=\"file-struct\">\n\
															<a href=\"" + Dir.WebPath() + "\">" + Dir.HtmlName + "/<a/>\n\
														</div>\n\
													</td>\n\
													<td width=\"15.4%\" height=\"10%\" align=\"center\">\n\
														<div class=\"file-struct\">\n" + \
															Dir.Date() + "\
														</div>\n\
													</td>\n\
													<td width=\"10.4%\" height=\"10%\" align=\"center\">\n\
														<div class=\"file-struct\">\n" + \
															Dir.DiskSize().Rounded() + "\n\
														</div>\n\
													</td>\n\
												</form>\n\
											</tr>\n";
						}
												
					}
					for(FileStruct& File : (*CLOUD).Files)
					{
						if (File.Directory == FsPath)
						{
							FileCounter++;
							HtmlOut += "\
											<tr>\n\
												<form method=\"post\" action=\"" + RefSite + "/wait.html\" onSubmit='return confirm(\"" + RusToHtml_encode("Требуется подтверждение для удаления файла") + "\");'>\n\
													<td width=\"5%\" height=\"10%\" align=\"center\">\n\
														<img src=\"/sources/images/File Icon.png\" style=\"position: inherit;display: block; height: 70px\">\n\
													</td>\n\
													<td width=\"7.7%\" height=\"10%\" align=\"center\">\n\
														<div class=\"file-struct\" style=\"width: 100%; height: 100%; font-size: 30px\">\n\
															<input type=\"submit\" value=\"Delete file\" style=\"width: 100%; height: 100%; font-size: 30px\">\n\
															<input type=\"text\" name=\"delete\" value=\"" + File.Path().string() + "\" style=\"display: none\">\n\
														</div>\n\
													</td>\n\
													<td width=\"5%\" height=\"15%\" align=\"center\">\n\
														<button type=\"button\" style=\"width: 100%; height: 100%; font-size: 20px\" onclick=\"EditNameOfF('" + File.HtmlName + "')\">Rename<br>File</button>\n\
													</td>\n\
													<td width=\"31.5%\" id=\"" + File.HtmlName + "\" height=\"10%\" align=\"center\">\n\
														<div class=\"file-struct\">\n\
															<a href=\"" + File.WebPath() + "\" download>" + File.HtmlName + "<a/>\n\
														</div>\n\
													</td>\n\
													<td width=\"15.4%\" height=\"10%\" align=\"center\">\n\
														<div class=\"file-struct\">\n" + \
															File.Date() + "\
														</div>\n\
													</td>\n\
													<td width=\"10.4%\" height=\"10%\" align=\"center\">\n\
														<div class=\"file-struct\">\n" + \
															File.DiskSize().Rounded() + "\n\
														</div>\n\
													</td>\n\
												</form>\n\
											</tr>\n";
						}
					}
					if (FsPath == ((*CLOUD).Path / "") && FileCounter == 0 && DirCounter == 0)
					{
						HtmlOut += "\
											<tr>\n\
												<td width=\"100%\" align=\"center\" valign=\"center\" colspan=\"6\">\n\
													<div class=\"no-content\">\n\
														No files or dirs were found in CLOUD\n\
													</div>\n\
												</td>\n\
											</tr>\n";
					}
					else
					{
						HtmlOut += "\
											<tr>\n\
											</tr>\n";
					}
					HtmlOut += "\
										</table>\n\
									</div>\n\
									<div class=\"size-size\">\n\
										<div align=\"left\">\n\
											<div class=\"free-size\" align=\"center\">\n" + \
												(*CLOUD).FreeSpace().Rounded() + " left\n\
											</div>\n\
										</div>\n\
										<div align=\"right\">\n\
											<div class=\"used-size\" align=\"center\">\n" + \
												(*CLOUD).UsedSpace().Rounded() + " used\n\
											</div>\n\
										</div>\n\
										<div class=\"send-file\" align=\"center\">\n\
											<div class=\"form-file-sender\" align=\"center\">\n\
												<form enctype=\"multipart/form-data\" method=\"POST\" action=\"" + RefSite + "/wait.html\">\n\
													<input type=\"file\" name=\"ffff\" multiple><br><br>\n\
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
					if(ContentSize > (*CLOUD).FreeSpace().Bytes)
						req->SetResponseString(OverSizeError);
					else if (FsPath == FSRelPath(RootDir) / "wait.html")
					{
						std::cout << "Redirecting..." << std::endl;
						req->SetResponseString(Redirect(RusToHtml_encode(RusToUTF_decode(Referer))));
					}
				}
				else if (FsPath == FSRelPath(RootDir) / "index.html")
				{
					req->SetResponseString(AuthIndex);
				}
				else if (FsPath == FSRelPath(RootDir) / "wait.html")
				{
					if (DeleteRequest && DeleteExtra == "Folder is current")
					{
						req->SetResponseString(Redirect(RusToHtml_encode(RusToUTF_decode(Referer) + "..")));
					}
					else if (RenameRequest && RenameFromPath.string()[RenameFromPath.string().length()-1] == '/')
					{
						std::cout << "CHECK" << std::endl;
						req->SetResponseString(Redirect(RusToHtml_encode(RusToUTF_decode(Referer) + "..")));
					}
					else
					{
						req->SetResponseString(Redirect(RusToHtml_encode(RusToUTF_decode(Referer))));
					}
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
					{
						req->SetResponseString(Redirect(RusToHtml_encode(RusToUTF_decode(Referer))));
					}
				}
				else
					req->SetResponseFile(FsPath);
			}
			delete CLOUD;
		});
		std::cin.get();
	}
	catch (std::exception const &e)
	{
		std::cout << e.what() << std::endl;
	}
	return 0;
}
