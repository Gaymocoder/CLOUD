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
  char const SrvAddress[] = "127.0.0.1";
  std::uint16_t SrvPort = 5555;
  std::uint16_t SrvThreadCount = 4;
  std::string const RootDir = "../Interface";
  std::string const DefaultPage = "index.html";
  std::mutex Mtx;
  const string login = "admin";
  const string password = "admin";
  string login_input = "";
  string password_input = "";
  file_struct file;
  try
  {
    using namespace Network;
    HttpServer Srv(SrvAddress, SrvPort, SrvThreadCount,
      [&] (IHttpRequestPtr req) 
      {
        std::string Path = req->GetPath();
        Path = RootDir + Path + (Path == "/" ? DefaultPage : std::string());
        {
          std::stringstream Io;
          Io << "Path: " << Path << std::endl
             << Http::Request::Header::Host::Name << ": "
                  << req->GetHeaderAttr(Http::Request::Header::Host::Value) << std::endl
             << Http::Request::Header::Referer::Name << ": "
                  << req->GetHeaderAttr(Http::Request::Header::Referer::Value) << std::endl;
          std::lock_guard<std::mutex> Lock(Mtx);
          std::cout << Io.str() << std::endl;
        }
        req->SetResponseAttr(Http::Response::Header::Server::Value, "MyTestServer");
        req->SetResponseAttr(Http::Response::Header::ContentType::Value,
                             Http::Content::TypeFromFileName(Path));
        req->SetResponseFile(Path);
        auto Params = req->GetParams();
        auto RequestType = req->GetRequestType();
        string req_type;
        switch (RequestType)
        {
			case IHttpRequest::Type::GET:
				req_type = "GET";
			case IHttpRequest::Type::HEAD:
				req_type = "HEAD";
			case IHttpRequest::Type::PUT:
				req_type = "PUT";	
			case IHttpRequest::Type::POST:
				req_type = "POST";
		}
		string delete_num;
		bool delete_bool = false;
		for(const auto& i : Params)
		{
			if (i.first == "Login")
				login_input = i.second;
			if (i.first == "Password")
				password_input = i.second;
			if (i.first == "delete")
			{
				delete_num = i.second;
				delete_bool = true;
			}
		}
		auto ContentSize = req->GetContentSize();
		string ru_let;
		string ru_var;
		string public_name = "../Interface/sources/content/public_name.txt";
		string server_name = "../Interface/sources/content/server_name.txt";
		string dates = "../Interface/sources/content/dates.txt";
		string sizes = "../Interface/sources/content/sizes.txt";
		string free_size = "../Interface/sources/content/free_size.txt";
		string s_buf;
		double f_size_byte = 0;
		double f_size;
		string fs_size;
		char command_buf[200];
		for(int i = 0; i<200; i++)
			command_buf[i] = 0;
		strcat(command_buf, "df ../Interface/yourcloud/ | awk '{print $4}' > ");
		strcat(command_buf, free_size.c_str());
		system(command_buf);
		for(int i = 0; i<200; i++)
			command_buf[i] = 0;
		ifstream fs(free_size);
		int i = 0;
		while(getline(fs,s_buf))
		{
			if(i>0)
			{
				f_size_byte = stod(s_buf)*1024;
				f_size = stod(s_buf);
				if(f_size<1024)
				{	
					fs_size = round_d(to_string(f_size)) + " KB";
				}
				else
				{
					f_size = round(f_size/1024*10)/10;
					if(f_size<1024)
					{
						fs_size = round_d(to_string(f_size)) + " MB";
					}
					else
					{
						f_size = round(f_size/1024*10)/10;
						if(f_size<1024)
						{
							
							fs_size = round_d(to_string(f_size)) + " GB";
						}
						else
							fs_size = round_d(to_string(f_size)) + " TB";
					}
				}
			}
			i++;
		}
        if(ContentSize > 0)					
        {
			if(ContentSize > f_size_byte);
			else
			{
				file.file_path = "";
				file.server_name = "";
				file.public_name = "";				
				std::vector <char> Buf(ContentSize);
				req->GetContent(&Buf[0], ContentSize, true);
				int i = 0;
				while(Buf[i]!='\n')
					i++;
				const int first_buf_line = i;
				size_t num = first_buf_line+56;
				bool contain = false;
				while(Buf[num]!='"')
				{
					file.public_name += Buf[num];
					num++;
				}
				num = first_buf_line+56;
				while(Buf[num]!='"')
				{
					if(Buf[num]=='&'&&Buf[num+1]=='#')
					{
						ru_let = "";
						for(size_t i = num; i<num+7; i++)
							ru_let+=Buf[i];
						if(ru_let == "&#1025;")
							ru_var+="Yo";
						if(ru_let == "&#1105;")
							ru_var+="yo";
						if(ru_let == "&#1040;")
							ru_var+="A";
						if(ru_let == "&#1041;")
							ru_var+="B";
						if(ru_let == "&#1042;")
							ru_var+="V";
						if(ru_let == "&#1043;")
							ru_var+="G";
						if(ru_let == "&#1044;")
							ru_var+="D";
						if(ru_let == "&#1045;")
							ru_var+="E";
						if(ru_let == "&#1046;")
							ru_var+="J";
						if(ru_let == "&#1047;")
							ru_var+="Z";
						if(ru_let == "&#1048;")
							ru_var+="I";
						if(ru_let == "&#1049;")
							ru_var+="Y";
						if(ru_let == "&#1050;")
							ru_var+="K";
						if(ru_let == "&#1051;")
							ru_var+="L";
						if(ru_let == "&#1052;")
							ru_var+="M";
						if(ru_let == "&#1053;")
							ru_var+="N";
						if(ru_let == "&#1054;")
							ru_var+="O";
						if(ru_let == "&#1055;")
							ru_var+="P";
						if(ru_let == "&#1056;")
							ru_var+="R";
						if(ru_let == "&#1057;")
							ru_var+="S";
						if(ru_let == "&#1058;")
							ru_var+="T";
						if(ru_let == "&#1059;")
							ru_var+="U";
						if(ru_let == "&#1060;")
							ru_var+="F";
						if(ru_let == "&#1061;")
							ru_var+="H";
						if(ru_let == "&#1062;")
							ru_var+="Cc";
						if(ru_let == "&#1063;")
							ru_var+="Ch";
						if(ru_let == "&#1064;")
							ru_var+="Sh";
						if(ru_let == "&#1065;")
							ru_var+="Sh'";
						if(ru_let == "&#1066;");
						if(ru_let == "&#1067;")
							ru_var+="I";
						if(ru_let == "&#1068;");
						if(ru_let == "&#1069;")
							ru_var+="E";
						if(ru_let == "&#1070;")
							ru_var+="Yu";
						if(ru_let == "&#1071;")
							ru_var+="Ya";
						if(ru_let == "&#1072;")
							ru_var+="a";
						if(ru_let == "&#1073;")
							ru_var+="b";
						if(ru_let == "&#1074;")
							ru_var+="v";
						if(ru_let == "&#1075;")
							ru_var+="g";
						if(ru_let == "&#1076;")
							ru_var+="d";
						if(ru_let == "&#1077;")
							ru_var+="e";
						if(ru_let == "&#1078;")
							ru_var+="j";
						if(ru_let == "&#1079;")
							ru_var+="z";
						if(ru_let == "&#1080;")
							ru_var+="i";
						if(ru_let == "&#1081;")
							ru_var+="y";
						if(ru_let == "&#1082;")
							ru_var+="k";
						if(ru_let == "&#1083;")
							ru_var+="l";
						if(ru_let == "&#1084;")
							ru_var+="m";
						if(ru_let == "&#1085;")
							ru_var+="n";
						if(ru_let == "&#1086;")
							ru_var+="o";
						if(ru_let == "&#1087;")
							ru_var+="p";
						if(ru_let == "&#1088;")
							ru_var+="r";
						if(ru_let == "&#1089;")
							ru_var+="s";
						if(ru_let == "&#1090;")
							ru_var+="t";
						if(ru_let == "&#1091;")
							ru_var+="u";
						if(ru_let == "&#1092;")
							ru_var+="f";
						if(ru_let == "&#1093;")
							ru_var+="h";
						if(ru_let == "&#1094;")
							ru_var+="cc";
						if(ru_let == "&#1095;")
							ru_var+="ch";
						if(ru_let == "&#1096;")
							ru_var+="sh";
						if(ru_let == "&#1097;")
							ru_var+="sh'";
						if(ru_let == "&#1098;");
						if(ru_let == "&#1099;")
							ru_var+="i";
						if(ru_let == "&#1100;");
						if(ru_let == "&#1101;")
							ru_var+="e";
						if(ru_let == "&#1102;")
							ru_var+="yu";
						if(ru_let == "&#1103;")
							ru_var+="ya";
						contain = true;
						num++;
					}	
					else
						if(contain==true)
						{
							num+=6;
							contain = false;
						}
						else
						{
							ru_var+=Buf[num];
							num++;
						}
				}
				file.server_name = ru_var;
				for(size_t i = 0; i<ru_var.length(); i++)
					if (ru_var[i] == ' ')
						file.server_name[i] = '_';
				file.file_path = "../Interface/yourcloud/" + file.server_name;
				size_t content_start = 0;
				for(size_t i = 0; i<ContentSize; i++)
				{
					if(Buf[i]=='C'&&Buf[i+6]=='t'&&Buf[i+13]==' ')
					{
						for(size_t k = i+14; k<ContentSize; k++)
						{
							if(Buf[k]=='\n')
							{
								content_start = k+3;
								break;	
							}
						}
						break;
					}
				}
				size_t content_end = ContentSize - first_buf_line - 4;
				ofstream fileup(file.file_path);
				for(size_t i = content_start; i<content_end; i++)
				{
					fileup<<Buf[i];
				}
				string public_name_new = "../Interface/sources/content/public_name_new.txt";
				string server_name_new = "../Interface/sources/content/server_name_new.txt";
				ifstream sn(server_name);
				ifstream pn(public_name);
				ofstream sn_n(server_name_new);
				ofstream pn_n(public_name_new);
				string file_buf;
				while(getline(sn,file_buf))
				{
					if(file_buf==file.server_name);
					else
						sn_n<<file_buf<<endl;
				}
				sn_n<<file.server_name<<endl;
				strcat(command_buf, "rm ");
				strcat(command_buf, server_name.c_str());
				system(command_buf);
				for(int i = 0; i<200; i++)
					command_buf[i] = 0;
				strcat(command_buf, "mv ");
				strcat(command_buf, server_name_new.c_str());
				strcat(command_buf, " ");
				strcat(command_buf, server_name.c_str());
				system(command_buf);
				for(int i = 0; i<200; i++)
					command_buf[i] = 0;
				while(getline(pn,file_buf))
				{
					if(file_buf==file.public_name);
					else
						pn_n<<file_buf<<endl;
				}
				pn_n<<file.public_name<<endl;
				strcat(command_buf, "rm ");
				strcat(command_buf, public_name.c_str());
				system(command_buf);
				for(int i = 0; i<200; i++)
					command_buf[i] = 0;
				strcat(command_buf, "mv ");
				strcat(command_buf, public_name_new.c_str());
				strcat(command_buf, " ");
				strcat(command_buf, public_name.c_str());
				system(command_buf);
				for(int i = 0; i<200; i++)
					command_buf[i] = 0;
			}
		}
		string sorted = "../Interface/sources/content/sorted.txt";
		strcat(command_buf, "ls -1 ../Interface/yourcloud > ");
		strcat(command_buf, sorted.c_str());
		system(command_buf);
		for(int i = 0; i<200; i++)
			command_buf[i] = 0;
		strcat(command_buf, "ls ../Interface/yourcloud  -l --time-style=\"+%Y-%m-%d_%H:%M:%S\" | awk '{print $6}' > ");
		strcat(command_buf, dates.c_str());
		system(command_buf);
		for(int i = 0; i<200; i++)
			command_buf[i] = 0;
		strcat(command_buf, "ls -l ../Interface/yourcloud | awk '{print $5}' > ");
		strcat(command_buf, sizes.c_str());
		system(command_buf);
		for(int i = 0; i<200; i++)
			command_buf[i] = 0;
		vector<file_struct> file_list;
		file_list = sort(server_name, public_name, sorted, sizes, dates);
		if(delete_bool == true)
		{
			file_list = file_delete(delete_num, file_list);
			file_list = sort(server_name, public_name, sorted, sizes, dates);
		}
		string used_size;
		if(file_list.size()==0)
			used_size = "0 B";
		else
			used_size = file_list[0].used_size;
		ofstream index("../Interface/index.html");
		ofstream delay_web("../Interface/delay.html");
		ofstream access_error("../Interface/access_error.html");
		ofstream send_file("../Interface/send-file.html");
		ofstream exit("../Interface/exit.html");
		exit<<"\
			<html>"<<endl<<"\
				<head>"<<endl<<"\
					<link rel=\"shortcut icon\" href=\"/sources/images/favicon.ico\" type=\"image/x-icon\">"<<endl<<"\
					<meta http-equiv=\"refresh\" content=\"0;URL=index.html\" />"<<endl<<"\
				</head>"<<endl<<"\
				<body>"<<endl<<"\
				</body>"<<endl<<"\
			</html>"<<endl<<"\
		";
		if (login==login_input&&password==password_input)
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
							<a href=\"exit.html?Login=sss&Password=fff\">Exit</a>"<<endl<<"\
						</div>"<<endl<<"\
                        <div class=\"files\" style=\"height: 90%; border: 0; background: 0\">"<<endl<<"\
                            <div class=\"files\" style=\"width: 99%; top: 0; left: 0\">"<<endl<<"\
                                <table cellspacing=\"0\" cellpadding=\"0\" width=\"100%\" height=\"100%\" border=\"3\" bgcolor=\"white\">"<<endl<<"\
                                    <tr>"<<endl<<"\
                                        <td width=\"7.7%\" height=\"15%\" align=\"center\">"<<endl<<"\
                                            <div class=\"table-header-1\">"<<endl<<"\
                                                <a href=\"delay.html?delete=all\">Delete all</a>"<<endl<<"\
                                            </div>"<<endl<<"\
                                        </td>"<<endl<<"\
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
			if(file_list.size()>0)
			{
				for(size_t i = 0; i<file_list.size(); i++)
				{	
					index<<"\
                                    <tr>"<<endl<<"\
                                        <td width=\"7.7%\" height=\"10%\" align=\"center\">"<<endl<<"\
                                            <div class=\"file-struct\">"<<endl<<"\
                                                <a href=\"delay.html?delete="<<i<<"\">Delete file</a>"<<endl<<"\
                                            </div>"<<endl<<"\
                                        <td width=\"38.5%\" height=\"10%\" align=\"center\">"<<endl<<"\
                                            <div class=\"file-struct\">"<<endl<<"\
                                                <a href=\"yourcloud/"<<file_list[i].server_name<<"\" download>"<<file_list[i].public_name<<"<a/>"<<endl<<"\
                                            </div>"<<endl<<"\
                                        </td>"<<endl<<"\
                                        <td width=\"15.4%\" height=\"10%\" align=\"center\">"<<endl<<"\
                                            <div class=\"file-struct\">"<<endl<<\
                                                file_list[i].date<<endl<<"\
                                            </div>"<<endl<<"\
                                        </td>"<<endl<<"\
                                        <td width=\"15.4%\" height=\"10%\" align=\"center\">"<<endl<<"\
                                            <div class=\"file-struct\">"<<endl<<\
                                                file_list[i].size<<endl<<"\
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
                                        fs_size<<" left"<<endl<<"\
                                    </div>"<<endl<<"\
                                </div>"<<endl<<"\
                                <div align=\"right\">"<<endl<<"\
                                    <div class=\"used-size\" align=\"center\">"<<endl<<\
                                        used_size<<" used"<<endl<<"\
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
						<meta http-equiv=\"refresh\" content=\"0;URL=index.html\" />"<<endl<<"\
					</head>"<<endl<<"\
					<body>"<<endl<<"\
					</body>"<<endl<<"\
				</html>"<<endl;
			if(ContentSize > f_size_byte)
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
							<meta http-equiv=\"refresh\" content=\"0;URL=index.html\" />"<<endl<<"\
						</head>"<<endl<<"\
						<body>"<<endl<<"\
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
                                <form action=\"delay.html\">"<<endl<<"\
                                    <table cellspacing=\"0\" align=\"center\" border=\"3\" width=\"300\" height=\"300\" bgcolor=\"white\">"<<endl<<"\
                                        <tr height=\"50\">"<<endl<<"\
                                            <td valing=\"middle\">"<<endl<<"\
                                                <h1 align=\"center\"><a name=\"Try-Again\">LOG IN</a></h1>"<<endl<<"\
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
						<form style=\"padding-top:25%\" action=\"delay.html\">"<<endl<<"\
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
    });
    std::cin.get();
  }
  catch (std::exception const &e)
  {
    std::cout << e.what() << std::endl;
  }

  return 0;
}
