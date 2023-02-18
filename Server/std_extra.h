#include "dictionaries.h"
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <vector>
#include <cmath>

using namespace std;
namespace FS = std::filesystem;

enum class DSType
{
	FILE,
	DIR
};

namespace STD_FGC
{
	void changeStrChar(std::string *str, size_t pos, char c);
	std::string getSizeStr(size_t Bytes);
	std::string RoundTo(std::string number, int n);

	template <typename TP>
	std::time_t to_time_t(TP tp)
	{
		using namespace std::chrono;
		auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now()
				  + system_clock::now());
		return system_clock::to_time_t(sctp);
	}

	FS::path FSRelPath(std::string StrPath)
	{
		FS::path FSRelPath = StrPath;
		FS::path NewPath = FS::current_path();
		for(auto part = FSRelPath.begin(); part != FSRelPath.end(); ++part)
		{
			if (*part == "..")
				NewPath = NewPath.parent_path();
			else if (*part == ".");
			else
				NewPath /= *part;
		}
		return NewPath;
	}

	std::string KeyByValue(std::string Value, StrDict Dict)
	{
		bool Found = false;
		std::string Key = "";
		for(auto &pair : Dict)
			if (pair.second == Value)
			{
				Key = pair.first;
				Found = true;
				break;
			}
		if (Found)
			return Key;
		else
			throw "Key was not found";
	};

	std::string GetLines(std::string src, size_t StartPosition, int k)
	{
		std::string Line = "";
		for(int j = 0; j < k; j++)
		{
			if (StartPosition >= src.length())
			{
				if (j == 0)
					throw "Argument \"StartPosition\" resizes length of source";
				else
					throw "Argument \"k\" resizes amount of lines in source";
			}
			else
			{
				for(; ((int) src[StartPosition] != 10 && (int) src[StartPosition] != 13 && StartPosition < src.length()); StartPosition++)
					Line += src[StartPosition];
				Line += src[StartPosition];
				if ((int) src[StartPosition] == 13)
				{
					if (StartPosition < src.length() - 1)
					{
						if ((int) src[StartPosition + 1] == 10)
						{
							Line += '\n';
							StartPosition++;
						}
					}
				}
				StartPosition++;
			}
		}
		return Line;
	}

	std::string GetLines(std::vector <char> *src, size_t StartPosition, int k)
	{
		std::string Line = "";
		for(int j = 0; j < k; j++)
		{
			if (StartPosition >= (*src).size())
			{
				if (j == 0)
					throw "Argument \"StartPosition\" resizes length of source";
				else
					throw "Argument \"k\" resizes amount of lines in source";
			}
			else
			{
				for(; ((int) (*src)[StartPosition] != 10 && (int) (*src)[StartPosition] != 13 && StartPosition < (*src).size()); StartPosition++)
					Line += (*src)[StartPosition];
				Line += (*src)[StartPosition];
				if ((int) (*src)[StartPosition] == 13)
				{
					if (StartPosition < (*src).size() - 1)
					{
						if ((int) (*src)[StartPosition + 1] == 10)
						{
							Line += '\n';
							StartPosition++;
						}
					}
				}
				StartPosition++;
			}
		}
		return Line;
	}

	std::string GetLines(const char* src, size_t StartPosition, int k)
	{
		std::string Line = "";
		for(int j = 0; j < k; j++)
		{
			if (StartPosition >= strlen(src))
			{
				if (j == 0)
					throw "Argument \"StartPosition\" resizes length of source";
				else
					throw "Argument \"k\" resizes amount of lines in source";
			}
			else
			{
				for(; ((int) src[StartPosition] != 10 && (int) src[StartPosition] != 13 && StartPosition < strlen(src)); StartPosition++)
					Line += src[StartPosition];
				Line += src[StartPosition];
				if ((int) src[StartPosition] == 13)
				{
					if (StartPosition < strlen(src) - 1)
					{
						if ((int) src[StartPosition + 1] == 10)
						{
							Line += '\n';
							StartPosition++;
						}
					}
				}
				StartPosition++;
			}
		}
		return Line;
	}

	std::string GetLines(char src[], size_t StartPosition, int k)
	{
		std::string Line = "";
		for(int j = 0; j < k; j++)
		{
			if (StartPosition >= strlen(src))
			{
				if (j == 0)
					throw "Argument \"StartPosition\" resizes length of source";
				else
					throw "Argument \"k\" resizes amount of lines in source";
			}
			else
			{
				for(; ((int) src[StartPosition] != 10 && (int) src[StartPosition] != 13 && StartPosition < strlen(src)); StartPosition++)
					Line += src[StartPosition];
				Line += src[StartPosition];
				if ((int) src[StartPosition] == 13)
				{
					if (StartPosition < strlen(src) - 1)
					{
						if ((int) src[StartPosition + 1] == 10)
						{
							Line += '\n';
							StartPosition++;
						}
					}
				}
				StartPosition++;
			}
		}
		return Line;
	}

	bool LinesSame(const char* Line1, const char* Line2)
	{
		std::string Str1 = "",
					Str2 = "";
		for(size_t i = 0; i < strlen(Line1) && (int) Line1[i] != 10 && (int) Line1[i] != 13; i++)
			Str1 += Line1[i];
		for(size_t i = 0; i < strlen(Line2) && (int) Line2[i] != 10 && (int) Line2[i] != 13; i++)
			Str2 += Line2[i];
		return (strcmp(Str1.c_str(), Str2.c_str()) == 0);
				
	}
}
