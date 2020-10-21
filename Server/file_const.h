#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cmath>
using namespace std;

vector <string> GetCookies(string CookiesList)
{
    int k = 1;
    for(size_t i = 0; i<CookiesList.length(); i++)
    {
        if(CookiesList[i] == ';')
            k++;
    }
    int j = 0;
    vector <string> Cookies (k);
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

class file_struct
{
	public:
		string public_name;
		string server_name;
		string file_path;
		string date;
		string size;
		string used_size;
		int byte_size;
		int byte_used_size;
};		

string round_d(string a)
{
	for(int i = a.length()-1; i>=0; i--)
	{
		if(a[i]=='0')
			a[i] = 0;
		else
			break;
	}
	for(size_t i = 0; i<a.length(); i++)
		if(a[i]=='.'&&a[i+1]==0)
			a[i+1] = '0';
	return a;
} 

vector <file_struct> sort(string sn, string pn, string sort, string size, string date)
{
	double used_size_d = 0;
	string used_size;
	double size_double;
	int count = 0;
	int i = 0;
	string prom;
	string ss;
	ifstream server_name(sn);
	ifstream public_name(pn);
	ifstream sorted(sort);
	ifstream sizes(size);
	ifstream dates(date);
	while(getline(server_name,ss))
	{
		count++;
	}
	server_name.close();
	server_name.open(sn);
	vector <file_struct> file_list (count);
	vector <string> s (count);
	if(count>0)
	{
		while(getline(sorted,ss))
		{
			s[i] = ss;
			i++;
		}
		i = 0;
		while(getline(server_name,ss))
		{
			file_list[i].server_name = ss;
			i++;
		}
		i = 0;
		while(getline(public_name,ss))
		{
			file_list[i].public_name = ss;
			i++;
		}
		for(int i = 0; i<count; i++)
		{
			for(int j = 0; j<count; j++)
			{
				if(file_list[j].server_name==s[i])
				{
					prom = file_list[i].server_name;
					file_list[i].server_name = file_list[j].server_name;
					file_list[j].server_name = prom;
					prom = file_list[i].public_name;
					file_list[i].public_name = file_list[j].public_name;
					file_list[j].public_name = prom;
				}
			}
		}
		i = 0;
		while(getline(sizes,ss))
		{	
			if(i>0)
			{
				size_double = stod(ss);
				file_list[i-1].byte_size = stoi(ss);
				used_size_d += size_double;
				file_list[0].byte_used_size += stoi(ss);
				if(size_double<1024)
				{	
					file_list[i-1].size = round_d(to_string(size_double)) + " B";
				}
				else
				{
					size_double = round(size_double/1024*10)/10;
					if(size_double<1024)
					{
						file_list[i-1].size = round_d(to_string(size_double)) + " KB";
					}
					else
					{
						size_double = round(size_double/1024*10)/10;
						if(size_double<1024)
						{
							
							file_list[i-1].size = round_d(to_string(size_double)) + " MB";
						}
						else
						{
							size_double = round(size_double/1024*10)/10;
							file_list[i-1].size = round_d(to_string(size_double)) + " GB";
						}
					}
				}
			}
			i++;
		}
		i = 0;
		while(getline(dates,ss))
		{
			if(i>0)
				file_list[i-1].date = ss;
			i++;
		}
		if(used_size_d>=0.1)
		{
			if(used_size_d<1024)
			{
				file_list[0].used_size = round_d(to_string(used_size_d)) + " B";
			}
			else
			{
				used_size_d = round(used_size_d/1024*10)/10;
				if(used_size_d<1024)
				{
					file_list[0].used_size = round_d(to_string(used_size_d)) + " KB";
				}
				else
				{
					used_size_d = round(used_size_d/1024*10)/10;
					if(used_size_d<1024)
					{
						
						file_list[0].used_size = round_d(to_string(used_size_d)) + " MB";
					}
					else
					{
						used_size_d = round(used_size_d/1024*10)/10;
						file_list[0].used_size = round_d(to_string(used_size_d)) + " GB";
					}						
				}
			}
		}
		else
		{
			file_list[0].used_size = "0 B";
		}
		for(int i = 0; i<count; i++)
			for(size_t j = 0; j<file_list[i].date.length(); j++)
				if(file_list[i].date[j] == '_')
					file_list[i].date[j] = ' ';
	}
	return file_list;
}

vector<file_struct> file_delete(string num, vector<file_struct> file_list)
{
	if(num == "all")
	{
		system("rm ../Interface/yourcloud/*");
		ofstream public_name("../Interface/sources/content/public_name.txt");
		public_name<<"";
		public_name.close();
		ofstream server_name("../Interface/sources/content/server_name.txt");
		server_name<<"";
		server_name.close();
		for(size_t i = 0; i<file_list.size(); i++)
		{
			file_list[i].server_name = "";
			file_list[i].public_name = "";
			file_list[i].size = "";
			file_list[i].date = "";
			file_list[i].file_path = "";
			file_list[i].used_size = "";
			file_list[i].byte_size = 0;
		}
	}
	else
	{
		int numm = stoi(num);
		num = "";
		char command_buf[200];
		for(int i = 0; i<200; i++)
			command_buf[i] = 0;
		strcat(command_buf, "rm ../Interface/yourcloud/");
		strcat(command_buf, file_list[numm].server_name.c_str());
		system(command_buf);
		ifstream sn("../Interface/sources/content/server_name.txt");
		ofstream sn_n("../Interface/sources/content/server_name_new.txt");
		ifstream pn("../Interface/sources/content/public_name.txt");
		ofstream pn_n("../Interface/sources/content/public_name_new.txt");
		while(getline(sn,num))
		{
			if(num==file_list[numm].server_name);
			else
				sn_n<<num<<endl;
		}
		while(getline(pn,num))
		{
			if(num==file_list[numm].public_name);
			else
				pn_n<<num<<endl;
		}
		system("rm ../Interface/sources/content/server_name.txt");
		system("mv ../Interface/sources/content/server_name_new.txt ../Interface/sources/content/server_name.txt");
		system("rm ../Interface/sources/content/public_name.txt");
		system("mv ../Interface/sources/content/public_name_new.txt ../Interface/sources/content/public_name.txt");
		file_list[0].byte_used_size -= file_list[numm].byte_size;
		for(size_t i = numm+1; i<file_list.size(); i++)
		{
			file_list[i-1].server_name = file_list[i].server_name;
			file_list[i-1].public_name = file_list[i].public_name;
			file_list[i-1].size = file_list[i].size;
			file_list[i-1].date = file_list[i].date;
			file_list[i-1].file_path = file_list[i].file_path;
			file_list[i-1].byte_size = file_list[i].byte_size;
		}
	}
	system("ls ../Interface/yourcloud  -l --time-style=\"+%Y-%m-%d_%H:%M:%S\" | awk '{print $6}' > ../Interface/sources/content/dates.txt");
	system("ls -l ../Interface/yourcloud | awk '{print $5}' > ../Interface/sources/content/sizes.txt");
	system("df ../Interface/yourcloud/ | awk '{print $4}' > ../Interface/sources/content/free_size.txt");
	system("ls -1 ../Interface/yourcloud > ../Interface/sources/content/sorted.txt");
	return file_list;
}
