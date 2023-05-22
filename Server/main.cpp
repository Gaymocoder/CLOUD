#include "http_server.h"
#include "http_headers.h"
#include "http_content_type.h"
#include "CLOUD.h"
#include <mutex>
#include <evhttp.h>
#include <cstring>
#include <iostream>

int main()
{
	Storage *CLOUD = new Storage("../config.ini");
	
	std::mutex Mtx;
	const std::string CookieLogin = "login=" + CLOUD->Login;
	const std::string CookiePassword = "password=" + CLOUD->Password;
	
	time_t rawtime;
	struct tm * timeinfo;
	char buffer[100];
	time (&rawtime);
	timeinfo = localtime (&rawtime);
	strftime (buffer, 100 ,"Server started on %d.%m.%Y at %H:%M:%S\n\n", timeinfo);
	printf("%s", buffer);
	
	// Server initializing, cycle starting
	try
	{
		using namespace Network;
		HttpServer Srv(CLOUD->SrvAddress, CLOUD->SrvPort, CLOUD->SrvThreadCount,
		[&] (IHttpRequestPtr req) 
		{
			
			// Declaring main variables
			std::string Path = req->GetPath();
			auto ContentSize = req->GetContentSize();
			std::vector <char> *Buf = new std::vector <char> (ContentSize);
			std::string Referer = req->GetHeaderAttr(Http::Request::Header::Referer::Value);
			std::string Cookies = req->GetHeaderAttr(Http::Request::Header::Cookie::Value);
			
			//Path = RusToUTF_decode(RootDir + Path + (Path == "/" ? DefaultPage : std::string()));
			//FS::path FsPath = FS::absolute(Path);
			
			auto CookiesList = GetCookies(Cookies);
			
			// Making base output of got request
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
					<< req->GetHeaderAttr(Http::Request::Header::Referer::Value) << std::endl
					<< "Cookies: " << Cookies << std::endl;
				std::lock_guard<std::mutex> Lock(Mtx);
				std::cout << Io.str();
			}
			
			
			// Setting response base http headers
			req->SetResponseAttr(Http::Response::Header::Server::Value, "MyTestServer");
			req->SetResponseAttr(Http::Response::Header::ContentType::Value,
								 Http::Content::TypeFromFileName(Path));
			
			//req->SetResponseFile(FsPath);
			
		});
		std::cin.get();
	}
	catch (std::exception const &e)
	{
		std::cout << e.what() << std::endl;
	}
	return 0;
}
