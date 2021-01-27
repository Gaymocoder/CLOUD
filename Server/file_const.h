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
		string PublicName;
		string ServerName;
		string Path;
		string Date;
		string Size;
		string UsedSize;
		int BytedSize;
		int BytedUsedSize;
};		

string round_d(string a, int n)
{
	string b = "";
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
		printf("Not acceptable amount of chars after point %i", n);
	return b;
} 

vector <file_struct> Sort(string sn, string pn, string sort, string size, string date)
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
			file_list[i].ServerName = ss;
			i++;
		}
		i = 0;
		while(getline(public_name,ss))
		{
			file_list[i].PublicName = ss;
			i++;
		}
		for(int i = 0; i<count; i++)
		{
			for(int j = 0; j<count; j++)
			{
				if(file_list[j].ServerName==s[i])
				{
					prom = file_list[i].ServerName;
					file_list[i].ServerName = file_list[j].ServerName;
					file_list[j].ServerName = prom;
					prom = file_list[i].PublicName;
					file_list[i].PublicName = file_list[j].PublicName;
					file_list[j].PublicName = prom;
				}
			}
		}
		i = 0;
		while(getline(sizes,ss))
		{	
			if(i>0)
			{
				size_double = stod(ss);
				file_list[i-1].BytedSize = stoi(ss);
				used_size_d += size_double;
				file_list[0].BytedUsedSize += stoi(ss);
				if(size_double<1024)
				{	
					file_list[i-1].Size = round_d(to_string(size_double), -1) + " B";
				}
				else
				{
					size_double = round(size_double/1024*10)/10;
					if(size_double<1024)
					{
						file_list[i-1].Size = round_d(to_string(size_double), -1) + " KB";
					}
					else
					{
						size_double = round(size_double/1024*10)/10;
						if(size_double<1024)
						{
							
							file_list[i-1].Size = round_d(to_string(size_double), -1) + " MB";
						}
						else
						{
							size_double = round(size_double/1024*10)/10;
							file_list[i-1].Size = round_d(to_string(size_double), -1) + " GB";
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
				file_list[i-1].Date = ss;
			i++;
		}
		if(used_size_d>=0.1)
		{
			if(used_size_d<1024)
			{
				file_list[0].UsedSize = round_d(to_string(used_size_d), -1) + " B";
			}
			else
			{
				used_size_d = round(used_size_d/1024*10)/10;
				if(used_size_d<1024)
				{
					file_list[0].UsedSize = round_d(to_string(used_size_d), -1) + " KB";
				}
				else
				{
					used_size_d = round(used_size_d/1024*10)/10;
					if(used_size_d<1024)
					{
						
						file_list[0].UsedSize = round_d(to_string(used_size_d), -1) + " MB";
					}
					else
					{
						used_size_d = round(used_size_d/1024*10)/10;
						file_list[0].UsedSize = round_d(to_string(used_size_d), -1) + " GB";
					}						
				}
			}
		}
		else
		{
			file_list[0].UsedSize = "0 B";
		}
		for(int i = 0; i<count; i++)
			for(size_t j = 0; j<file_list[i].Date.length(); j++)
				if(file_list[i].Date[j] == '_')
					file_list[i].Date[j] = ' ';
	}
	return file_list;
}

vector<file_struct> FileDelete(string num, vector<file_struct> file_list)
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
			file_list[i].ServerName = "";
			file_list[i].PublicName = "";
			file_list[i].Size = "";
			file_list[i].Date = "";
			file_list[i].Path = "";
			file_list[i].UsedSize = "";
			file_list[i].BytedSize = 0;
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
		strcat(command_buf, file_list[numm].ServerName.c_str());
		system(command_buf);
		ifstream sn("../Interface/sources/content/server_name.txt");
		ofstream sn_n("../Interface/sources/content/server_name_new.txt");
		ifstream pn("../Interface/sources/content/public_name.txt");
		ofstream pn_n("../Interface/sources/content/public_name_new.txt");
		while(getline(sn,num))
		{
			if(num==file_list[numm].ServerName);
			else
				sn_n<<num<<endl;
		}
		while(getline(pn,num))
		{
			if(num==file_list[numm].PublicName);
			else
				pn_n<<num<<endl;
		}
		system("rm ../Interface/sources/content/server_name.txt");
		system("mv ../Interface/sources/content/server_name_new.txt ../Interface/sources/content/server_name.txt");
		system("rm ../Interface/sources/content/public_name.txt");
		system("mv ../Interface/sources/content/public_name_new.txt ../Interface/sources/content/public_name.txt");
		file_list[0].BytedUsedSize -= file_list[numm].BytedSize;
		for(size_t i = numm+1; i<file_list.size(); i++)
		{
			file_list[i-1].ServerName = file_list[i].ServerName;
			file_list[i-1].PublicName = file_list[i].PublicName;
			file_list[i-1].Size = file_list[i].Size;
			file_list[i-1].Date = file_list[i].Date;
			file_list[i-1].Path = file_list[i].Path;
			file_list[i-1].BytedSize = file_list[i].BytedSize;
		}
	}
	system("ls ../Interface/yourcloud  -l --time-style=\"+%Y-%m-%d_%H:%M:%S\" | awk '{print $6}' > ../Interface/sources/content/dates.txt");
	system("ls -l ../Interface/yourcloud | awk '{print $5}' > ../Interface/sources/content/sizes.txt");
	system("df ../Interface/yourcloud/ | awk '{print $4}' > ../Interface/sources/content/free_size.txt");
	system("ls -1 ../Interface/yourcloud > ../Interface/sources/content/sorted.txt");
	return file_list;
}
