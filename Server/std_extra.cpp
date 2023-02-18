#include "std_extra.h"
#include <string>

void STD_FGC::changeStrChar(std::string *str, size_t pos, char c)
{
	char &old = (*str)[pos];
	old = c;
}

std::string STD_FGC::getSizeStr(size_t Bytes)
{
	double XBytes = (double) Bytes;
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

std::string STD_FGC::RoundTo(std::string number, int n)
{
	size_t float_point = number.find(".");
	std::string int_part = number.substr(0, float_point);
	std::string float_part = (float_point == std::string::npos) ? "" : number.substr(float_point + 1);
	
	if (n > 0)
	{
		for(int i = float_part.length()-2; i >= n - 1; i--)
		{
			if((int)(float_part[i+1] - '0') > 4)
			{
				if(float_part[i] == '9')
				{
					float_part = std::to_string(std::stoi(float_part.substr(0, i)) + 1);
				}
				else
				{
					STD_FGC::changeStrChar(&float_part, i, (char) (float_part[i]+1));
				}
			}
			float_part.pop_back();
		}
		if (float_part == "") float_part += "0";
		return int_part + "." + float_part;
	}
	else
	{
		float_part = "";
		for(int i = int_part.length()-2; i >= int_part.length() + n - 1; i--)
		{
			if((int)(int_part[i+1] - '0') > 4)
			{
				if(int_part[i] == '9')
				{
					std::string zeros = "";
					for(int j = i; j < int_part.length(); j++)
						zeros += "0";
					int_part = std::to_string(std::stoi(int_part.substr(0, i)) + 1) + zeros;
				}
				else
				{
					STD_FGC::changeStrChar(&int_part, i, (char) (int_part[i]+1));
				}
			}
			STD_FGC::changeStrChar(&int_part, i+1, '0');
		}
		return int_part;
	}
}

