#ifndef UPDATE_H
	#define UPDATE_H
	#include <map>

	typedef std::map <std::string, std::string> StrDict;

	StrDict RusUTFToURL =
	{
		// Capital russian chars
		{"&#1040;", "%D0%90"}, {"&#1041;", "%D0%91"}, {"&#1042;", "%D0%92"}, {"&#1043;", "%D0%93"}, {"&#1044;", "%D0%94"}, {"&#1045;", "%D0%95"}, {"&#1025;", "%D0%81"}, {"&#1046;", "%D0%96"},
		{"&#1047;", "%D0%97"}, {"&#1048;", "%D0%98"}, {"&#1049;", "%D0%99"}, {"&#1050;", "%D0%9A"}, {"&#1051;", "%D0%9B"}, {"&#1052;", "%D0%9C"}, {"&#1053;", "%D0%9D"}, {"&#1054;", "%D0%9E"},
		{"&#1055;", "%D0%9F"}, {"&#1056;", "%D0%A0"}, {"&#1057;", "%D0%A1"}, {"&#1058;", "%D0%A2"}, {"&#1059;", "%D0%A3"}, {"&#1060;", "%D0%A4"}, {"&#1061;", "%D0%A5"}, {"&#1062;", "%D0%A6"},
		{"&#1063;", "%D0%A7"}, {"&#1064;", "%D0%A8"}, {"&#1065;", "%D0%A9"}, {"&#1066;", "%D0%AA"}, {"&#1067;", "%D0%AB"}, {"&#1068;", "%D0%AC"}, {"&#1069;", "%D0%AD"}, {"&#1070;", "%D0%AE"},
		{"&#1071;", "%D0%AF"},

		
		// Lowercase russian chars
		{"&#1072;", "%D0%B0"}, {"&#1073;", "%D0%B1"}, {"&#1074;", "%D0%B2"}, {"&#1075;", "%D0%B3"}, {"&#1076;", "%D0%B4"}, {"&#1077;", "%D0%B5"}, {"&#1105;", "%D1%91"}, {"&#1078;", "%D0%B6"},
		{"&#1079;", "%D0%B7"}, {"&#1080;", "%D0%B8"}, {"&#1081;", "%D0%B9"}, {"&#1082;", "%D0%BA"}, {"&#1083;", "%D0%BB"}, {"&#1084;", "%D0%BC"}, {"&#1085;", "%D0%BD"}, {"&#1086;", "%D0%BE"},
		{"&#1087;", "%D0%BF"}, {"&#1088;", "%D1%80"}, {"&#1089;", "%D1%81"}, {"&#1090;", "%D1%82"}, {"&#1091;", "%D1%83"}, {"&#1092;", "%D1%84"}, {"&#1093;", "%D1%85"}, {"&#1094;", "%D1%86"},
		{"&#1095;", "%D1%87"}, {"&#1096;", "%D1%88"}, {"&#1097;", "%D1%89"}, {"&#1098;", "%D1%8A"}, {"&#1099;", "%D1%8B"}, {"&#1100;", "%D1%8C"}, {"&#1101;", "%D1%8D"}, {"&#1102;", "%D1%8E"},
		{"&#1103;", "%D1%8F"},
		
		// Space
		{"&#32;", "%20"},
		
		// Slash
		{"&#92;", "%2F"},
		
		// ()
		{"&#40;", "%28"},
		{"&#41;", "%29"}
	};

	StrDict RusUTFToURL2 =
	{
		
		// Capital russian chars
		{"&#1040;", "%C0"}, {"&#1041;", "%C1"}, {"&#1042;", "%C2"}, {"&#1043;", "%C3"}, {"&#1044;", "%C4"}, {"&#1045;", "%C5"}, {"&#1025;", "%A8"}, {"&#1046;", "%C6"},
		{"&#1047;", "%C7"}, {"&#1048;", "%C8"}, {"&#1049;", "%C9"}, {"&#1050;", "%CA"}, {"&#1051;", "%CB"}, {"&#1052;", "%CC"}, {"&#1053;", "%CD"}, {"&#1054;", "%CE"},
		{"&#1055;", "%CF"}, {"&#1056;", "%D0"}, {"&#1057;", "%D1"}, {"&#1058;", "%D2"}, {"&#1059;", "%D3"}, {"&#1060;", "%D4"}, {"&#1061;", "%D5"}, {"&#1062;", "%D6"},
		{"&#1063;", "%D7"}, {"&#1064;", "%D8"}, {"&#1065;", "%D9"}, {"&#1066;", "%DA"}, {"&#1067;", "%DB"}, {"&#1068;", "%DC"}, {"&#1069;", "%DD"}, {"&#1070;", "%DE"},
		{"&#1071;", "%DF"},

		
		// Lowercase russian chars
		{"&#1072;", "%E0"}, {"&#1073;", "%E1"}, {"&#1074;", "%E2"}, {"&#1075;", "%E3"}, {"&#1076;", "%E4"}, {"&#1077;", "%E5"}, {"&#1105;", "%B8"}, {"&#1078;", "%E6"},
		{"&#1079;", "%E7"}, {"&#1080;", "%E8"}, {"&#1081;", "%E9"}, {"&#1082;", "%EA"}, {"&#1083;", "%EB"}, {"&#1084;", "%EC"}, {"&#1085;", "%ED"}, {"&#1086;", "%EE"},
		{"&#1087;", "%EF"}, {"&#1088;", "%F0"}, {"&#1089;", "%F1"}, {"&#1090;", "%F2"}, {"&#1091;", "%F3"}, {"&#1092;", "%F4"}, {"&#1093;", "%F5"}, {"&#1094;", "%F6"},
		{"&#1095;", "%F7"}, {"&#1096;", "%F8"}, {"&#1097;", "%F9"}, {"&#1098;", "%FA"}, {"&#1099;", "%FB"}, {"&#1100;", "%FC"}, {"&#1101;", "%FD"}, {"&#1102;", "%FE"},
		{"&#1103;", "%FF"},
		
		// Space
		{"&#32;", "%20"},
		
		// Slash
		{"&#92;", "%2F"},
		
		// ()
		{"&#40;", "%28"},
		{"&#41;", "%29"}
	};

	StrDict RusUTFToChar = 
	{
		// Capital russian chars
		{"&#1040;", "А"}, {"&#1041;", "Б"}, {"&#1042;", "В"}, {"&#1043;", "Г"}, {"&#1044;", "Д"}, {"&#1045;", "Е"}, {"&#1025;", "Ё"}, {"&#1046;", "Ж"}, {"&#1047;", "З"}, {"&#1048;", "И"}, {"&#1049;", "Й"},
		{"&#1050;", "К"}, {"&#1051;", "Л"}, {"&#1052;", "М"}, {"&#1053;", "Н"}, {"&#1054;", "О"}, {"&#1055;", "П"}, {"&#1056;", "Р"}, {"&#1057;", "С"}, {"&#1058;", "Т"}, {"&#1059;", "У"}, {"&#1060;", "Ф"},
		{"&#1061;", "Х"}, {"&#1062;", "Ц"},	{"&#1063;", "Ч"}, {"&#1064;", "Ш"},	{"&#1065;", "Щ"}, {"&#1066;", "Ъ"}, {"&#1067;", "Ы"}, {"&#1068;", "Ь"}, {"&#1069;", "Э"}, {"&#1070;", "Ю"}, {"&#1071;", "Я"},
		
		// Lowercase russian chars
		{"&#1072;", "а"}, {"&#1073;", "б"}, {"&#1074;", "в"}, {"&#1075;", "г"}, {"&#1076;", "д"}, {"&#1077;", "е"}, {"&#1105;", "ё"}, {"&#1078;", "ж"}, {"&#1079;", "з"}, {"&#1080;", "и"}, {"&#1081;", "й"},
		{"&#1082;", "к"}, {"&#1083;", "л"}, {"&#1084;", "м"}, {"&#1085;", "н"}, {"&#1086;", "о"}, {"&#1087;", "п"}, {"&#1088;", "р"}, {"&#1089;", "с"}, {"&#1090;", "т"}, {"&#1091;", "у"}, {"&#1092;", "ф"},
		{"&#1093;", "х"}, {"&#1094;", "ц"}, {"&#1095;", "ч"}, {"&#1096;", "ш"}, {"&#1097;", "щ"}, {"&#1098;", "ъ"}, {"&#1099;", "ы"}, {"&#1100;", "ь"}, {"&#1101;", "э"}, {"&#1102;", "ю"}, {"&#1103;", "я"},
		
		// Space
		{"&#32;", " "},
		
		//Slash
		{"&#92;", "/"},
		
		// ()
		{"&#40;", "("},
		{"&#41;", ")"}
	};

#endif