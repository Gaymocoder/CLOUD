#include "dictionaries.h"
#include "std_extra.h"
#include <iostream>
#include <string>
#include <sstream>

std::string RusToUTF_decode(std::string Str)
{
	std::string Out = "";
	char Buf[Str.length()];
	for(size_t i = 0; i < Str.length(); i++)
	{
		if (i + 5 < Str.length())
		{
			if (Str[i] == '%')
			{
				Str.copy(Buf, 3, i);
				if (strcmp(Buf, "%20") == 0)
				{
					Out += RusUTFToChar[KeyByValue(std::string(Buf), RusUTFToURL)];
					i += 2;
				}
				else if (strcmp(Buf, "%2F") == 0)
				{
					Out += RusUTFToChar[KeyByValue(std::string(Buf), RusUTFToURL)];
					i += 2;
				}
				else if (strcmp(Buf, "%28") == 0)
				{
					Out += RusUTFToChar[KeyByValue(std::string(Buf), RusUTFToURL)];
					i += 2;
				}
				else if (strcmp(Buf, "%29") == 0)
				{
					Out += RusUTFToChar[KeyByValue(std::string(Buf), RusUTFToURL)];
					i += 2;
				}
				else if (Str[i+3] == '%')
				{
					Str.copy(Buf, 6, i);
					i += 5;
					Out += RusUTFToChar[KeyByValue(std::string(Buf), RusUTFToURL)];
				}
				else
				{
					Out += Str[i];
				}
			}
			else
			{
				Out += Str[i];
			}
		}
		else if (i + 2 < Str.length())
		{
			Str.copy(Buf, 3, i);
			if (strcmp(Buf, "%20") == 0)
			{
				Out += RusUTFToChar[KeyByValue(std::string(Buf), RusUTFToURL)];
				i += 2;
			}
			else if (strcmp(Buf, "%2F") == 0)
			{
				Out += RusUTFToChar[KeyByValue(std::string(Buf), RusUTFToURL)];
				i += 2;
			}
			else if (strcmp(Buf, "%28") == 0)
			{
				Out += RusUTFToChar[KeyByValue(std::string(Buf), RusUTFToURL)];
				i += 2;
			}
			else if (strcmp(Buf, "%29") == 0)
			{
				Out += RusUTFToChar[KeyByValue(std::string(Buf), RusUTFToURL)];
				i += 2;
			}
			else
			{
				Out += Str[i];
			}
		}
		else
		{
			Out += Str[i];
		}
		for(size_t i = 0; i < Str.length(); i++)
			Buf[i] = 0;
	}
	return Out;
}

std::string RusToUTF2_decode(std::string Str)
{
	std::string Out = "";
	char Buf[Str.length()] = "";
	for(size_t i = 0; i < Str.length(); i++)
	{
		if (i + 2 < Str.length())
		{
			Str.copy(Buf, 3, i);
			if (Str[i] == '%')
			{
				Out += RusUTFToChar[KeyByValue(std::string(Buf), RusUTFToURL2)];
				i += 2;
			}
			else
			{
				Out += Str[i];
			}
		}
		else
		{
			Out += Str[i];
		}
		for(size_t i = 0; i < Str.length(); i++)
			Buf[i] = 0;
	}
	return Out;
}

std::string RusToHtml_encode(std::string src)
{
	std::string Encoded;
	for(size_t i = 0; i < src.length(); i++)
	{
		if (src.length() - i > 1)
		{
			std::stringstream Char;
			Char << src[i] << src[i+1];
			try
			{
				std::string HtmlCode = KeyByValue(Char.str(), RusUTFToChar);
				Encoded += HtmlCode;
				i++;
			}
			catch (char const* err)
			{
				if (strcmp(err, "Key was not found") == 0)
					Encoded += src[i];
			}
		}
		else
			Encoded += src[i];
	}
	return Encoded;
}
