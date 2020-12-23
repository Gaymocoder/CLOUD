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

using namespace std;

int main()
{
    std::string SrvAddress, RootDir, DefaultPage, Login, Password, Line, LineBuf;
    std::uint16_t SrvPort = 0, SrvThreadCount = 0;
    ifstream Config("../config.ini");
    // Start of parsing config.ini with initializing main variables
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
        }
        if (Line.find("Port:") == 0)
        {
            for(size_t i = 6; i<Line.length(); i++)
            {
                LineBuf += Line[i];
            }
            SrvPort = std::stoi(LineBuf);
            LineBuf = "";
        }
        if (Line.find("Threads count:") == 0)
        {
            for(size_t i = 15; i<Line.length(); i++)
            {
                LineBuf += Line[i];
            }
            SrvThreadCount = std::stoi(LineBuf);
            LineBuf = "";
        }
        if (Line.find("Root Directory:") == 0)
        {
            for(size_t i = 16; i<Line.length(); i++)
            {
                LineBuf += Line[i];
            }
            RootDir = LineBuf;
            LineBuf = "";
        }
        if (Line.find("Default Page:") == 0)
        {
            for(size_t i = 14; i<Line.length(); i++)
            {
                LineBuf += Line[i];
            }
            DefaultPage = LineBuf;
            LineBuf = "";
        }
        if (Line.find("CLOUD Login:") == 0)
        {
            for(size_t i = 13; i<Line.length(); i++)
            {
                LineBuf += Line[i];
            }
            Login = LineBuf;
            LineBuf = "";
        }
        if (Line.find("CLOUD Password:") == 0)
        {
            for(size_t i = 16; i<Line.length(); i++)
            {
                LineBuf += Line[i];
            }
            Password = LineBuf;
            LineBuf = "";
        }
    }
    std::mutex Mtx;
    const std::string CookieLogin = "login="+Login;
    const std::string CookiePassword = "password="+Password;
    file_struct File;
    
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
            if (Http::Content::TypeFromFileName(Path) == Http::Content::Type::html::Value)
            {
                {
                    std::stringstream Io;
                    Io << "Path: " << Path << std::endl
                        << Http::Request::Header::Host::Name << ": "
                            << req->GetHeaderAttr(Http::Request::Header::Host::Value) << std::endl
                        << Http::Request::Header::Referer::Name << ": "
                            << req->GetHeaderAttr(Http::Request::Header::Referer::Value) << std::endl;
                    std::lock_guard<std::mutex> Lock(Mtx);
                    std::cout << Io.str();
                }
            }
            req->SetResponseAttr(Http::Response::Header::Server::Value, "MyTestServer");
            req->SetResponseAttr(Http::Response::Header::ContentType::Value,
                                 Http::Content::TypeFromFileName(Path));
                                 
            // Parsing of POST request
            std::string LogIn = "",
                        PassIn = "",
                        DeleteNumber = "";
            bool FileRequest = false,
                 AuthRequest = false,
                 DeleteRequest = false;
            size_t CountBuf = 0;
            auto ContentSize = req->GetContentSize();
            std::vector <char> Buf(ContentSize);
            req->GetContent(&Buf[0], ContentSize, true);
            for(size_t i = 0; i<ContentSize; i++)
            {
                if (Buf[i] == '\n')
                {
                    FileRequest = true;
                    break;
                }
            }
            if (not FileRequest)
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
            if (Http::Content::TypeFromFileName(Path) == Http::Content::Type::html::Value)
                cout<<"Cookies: "<<Cookies<<endl<<endl;
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
                        RoundedFS = round_d(to_string(FS)) + " KB";
                     else
                    {
                        FS = round(FS/1024*10)/10;
                        if(FS<1024)
                            RoundedFS = round_d(to_string(FS)) + " MB";
                        else
                        {
                            FS = round(FS/1024*10)/10;
                            if(FS<1024)
                                RoundedFS = round_d(to_string(FS)) + " GB";
                            else
                                RoundedFS = round_d(to_string(FS)) + " TB";
                        }
                    }
                }
                CountBuf++;
            }
            if(ContentSize > 0 && FileRequest)
            {
                if(ContentSize > BytedFS);
                else
                {
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
                    while(Buf[CountBuf]!='"')
                    while(Buf[CountBuf]!='"')
                    {
                        File.PublicName += Buf[CountBuf];
                        CountBuf++;
                    }
                    CountBuf = 0; 
                    while(CountBuf < File.PublicName.length())
                    {
                        CharIsRus = false;
                        if(File.PublicName[CountBuf]=='&' && File.PublicName[CountBuf + 1] == '#')
                        {
                            CharIsRus = true;
                            for(size_t i = CountBuf; i<CountBuf+7; i++)
                                RusChar+=File.PublicName[i];
                            if(RusChar == "&#1025;")
                                File.ServerName += "Yo";
                            else if(RusChar == "&#1105;")
                                File.ServerName += "yo";
                            else if(RusChar == "&#1040;")
                                File.ServerName += "A";
                            else if(RusChar == "&#1041;")
                                File.ServerName += "B";
                            else if(RusChar == "&#1042;")
                                File.ServerName += "V";
                            else if(RusChar == "&#1043;")
                                File.ServerName += "G";
                            else if(RusChar == "&#1044;")
                                File.ServerName += "D";
                            else if(RusChar == "&#1045;")
                                File.ServerName += "E";
                            else if(RusChar == "&#1046;")
                                File.ServerName += "J";
                            else if(RusChar == "&#1047;")
                                File.ServerName += "Z";
                            else if(RusChar == "&#1048;")
                                File.ServerName += "I";
                            else if(RusChar == "&#1049;")
                                File.ServerName += "Y";
                            else if(RusChar == "&#1050;")
                                File.ServerName += "K";
                            else if(RusChar == "&#1051;")
                                File.ServerName += "L";
                            else if(RusChar == "&#1052;")
                                File.ServerName += "M";
                            else if(RusChar == "&#1053;")
                                File.ServerName += "N";
                            else if(RusChar == "&#1054;")
                                File.ServerName += "O";
                            else if(RusChar == "&#1055;")
                                File.ServerName += "P";
                            else if(RusChar == "&#1056;")
                                File.ServerName += "R";
                            else if(RusChar == "&#1057;")
                                File.ServerName += "S";
                            else if(RusChar == "&#1058;")
                                File.ServerName += "T";
                            else if(RusChar == "&#1059;")
                                File.ServerName += "U";
                            else if(RusChar == "&#1060;")
                                File.ServerName += "F";
                            else if(RusChar == "&#1061;")
                                File.ServerName += "H";
                            else if(RusChar == "&#1062;")
                                File.ServerName += "Cc";
                            else if(RusChar == "&#1063;")
                                File.ServerName += "Ch";
                            else if(RusChar == "&#1064;")
                                File.ServerName += "Sh";
                            else if(RusChar == "&#1065;")
                                File.ServerName += "Sh'";
                            else if(RusChar == "&#1066;");
                            else if(RusChar == "&#1067;")
                                File.ServerName += "I";
                            else if(RusChar == "&#1068;");
                            else if(RusChar == "&#1069;")
                                File.ServerName += "E";
                            else if(RusChar == "&#1070;")
                                File.ServerName += "Yu";
                            else if(RusChar == "&#1071;")
                                File.ServerName += "Ya";
                            else if(RusChar == "&#1072;")
                                File.ServerName += "a";
                            else if(RusChar == "&#1073;")
                                File.ServerName += "b";
                            else if(RusChar == "&#1074;")
                                File.ServerName += "v";
                            else if(RusChar == "&#1075;")
                                File.ServerName += "g";
                            else if(RusChar == "&#1076;")
                                File.ServerName += "d";
                            else if(RusChar == "&#1077;")
                                File.ServerName += "e";
                            else if(RusChar == "&#1078;")
                                File.ServerName += "j";
                            else if(RusChar == "&#1079;")
                                File.ServerName += "z";
                            else if(RusChar == "&#1080;")
                                File.ServerName += "i";
                            else if(RusChar == "&#1081;")
                                File.ServerName += "y";
                            else if(RusChar == "&#1082;")
                                File.ServerName += "k";
                            else if(RusChar == "&#1083;")
                                File.ServerName += "l";
                            else if(RusChar == "&#1084;")
                                File.ServerName += "m";
                            else if(RusChar == "&#1085;")
                                File.ServerName += "n";
                            else if(RusChar == "&#1086;")
                                File.ServerName += "o";
                            else if(RusChar == "&#1087;")
                                File.ServerName += "p";
                            else if(RusChar == "&#1088;")
                                File.ServerName += "r";
                            else if(RusChar == "&#1089;")
                                File.ServerName += "s";
                            else if(RusChar == "&#1090;")
                                File.ServerName += "t";
                            else if(RusChar == "&#1091;")
                                File.ServerName += "u";
                            else if(RusChar == "&#1092;")
                                File.ServerName += "f";
                            else if(RusChar == "&#1093;")
                                File.ServerName += "h";
                            else if(RusChar == "&#1094;")
                                File.ServerName += "cc";
                            else if(RusChar == "&#1095;")
                                File.ServerName += "ch";
                            else if(RusChar == "&#1096;")
                                File.ServerName += "sh";
                            else if(RusChar == "&#1097;")
                                File.ServerName += "sh'";
                            else if(RusChar == "&#1098;");
                            else if(RusChar == "&#1099;")
                                File.ServerName += "i";
                            else if(RusChar == "&#1100;");
                            else if(RusChar == "&#1101;")
                                File.ServerName += "e";
                            else if(RusChar == "&#1102;")
                                File.ServerName += "yu";
                            else if(RusChar == "&#1103;")
                                File.ServerName += "ya";
                            CountBuf++;
                        }	
                        if(CharIsRus)
                        {
                            CountBuf += 6;
                            RusChar = "";
                        }
                        else
                        {
                            File.ServerName += File.PublicName[CountBuf];
                            CountBuf++;
                        }
                    }
                    for(size_t i = 0; i<File.ServerName.length(); i++)
                        if (File.ServerName[i] == ' ')
                            File.ServerName[i] = '_';
                    File.Path = "../Interface/yourcloud/" + File.ServerName;
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
                    ofstream FileUpload(File.Path);
                    for(size_t i = FileStart; i<FileEnd; i++)
                    {
                        FileUpload<<Buf[i];
                    }
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
            }
            req->SetResponseFile(Path);
        });
        std::cin.get();
    }
    catch (std::exception const &e)
    {
        std::cout << e.what() << std::endl;
    }
    return 0;
}
