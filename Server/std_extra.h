#pragma once

#include "dictionaries.h"
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <vector>
#include <cmath>
#include <algorithm>

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
	
	std::string KeyByValue(std::string Value, StrDict Dict);
	std::string GetLine(const std::vector <char> &bytes, size_t pos);
}
