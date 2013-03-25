/*
 * string_helper.cpp
 *
 *  Created on: 2011-3-14
 *      Author: qiyingwang
 */
#include "util/string_helper.hpp"
#include <ctype.h>
#include <iconv.h>
#include <errno.h>
#include <string.h>
#include <stdexcept>

namespace arch
{
	namespace util
	{

		std::string get_basename(const std::string& filename)
		{
#if defined(_WIN32)
			char dir_sep('\\');
#else
			char dir_sep('/');
#endif

			std::string::size_type pos = filename.rfind(dir_sep);
			if (pos != std::string::npos)
				return filename.substr(pos + 1);
			else
				return filename;
		}

		char* trim_str(char* s, const char* cset)
		{
			RETURN_NULL_IF_NULL(s);
			if (NULL == cset)
			{
				return s;
			}
			char *start, *end, *sp, *ep;
			size_t len;

			sp = start = s;
			ep = end = s + strlen(s) - 1;
			while (*sp && strchr(cset, *sp))
				sp++;
			while (ep > sp && strchr(cset, *ep))
				ep--;
			len = (sp > ep) ? 0 : ((ep - sp) + 1);
			if (start != sp)
				memmove(start, sp, len);
			start[len] = '\0';
			return start;
		}

		std::string trim_string(const std::string& str, const std::string& cset)
		{
			if (str.empty())
			{
				return str;
			}
			size_t start = 0;
			size_t str_len = str.size();
			size_t end = str_len - 1;
			while (start < str_len
					&& cset.find(str.at(start)) != std::string::npos)
			{
				start++;
			}
			while (end > start && end < str_len
					&& cset.find(str.at(end)) != std::string::npos)
			{
				end--;
			}
			return str.substr(start, end - start + 1);
		}

		std::vector<char*> split_str(char* str, const char* sep)
		{
			std::vector<char*> ret;
			char* start = str;
			char* found = NULL;
			uint32 sep_len = strlen(sep);
			while (*start && (found = strstr(start, sep)) != NULL)
			{
				*found = 0;
				if (*start)
				{
					ret.push_back(start);
				}
				start = (found + sep_len);
			}
			if (*start)
			{
				ret.push_back(start);
			}
			return ret;
		}

		std::vector<std::string> split_string(const std::string& str,
				const std::string& sep)
		{
			std::vector<std::string> ret;
			size_t start = 0;
			size_t str_len = str.size();
			size_t found = std::string::npos;
			while (start < str_len
					&& (found = str.find(sep, start)) != std::string::npos)
			{
				if (found > start)
				{
					ret.push_back(str.substr(start, found - start));
				}
				start = (found + sep.size());
			}
			if (start < str_len)
			{
				ret.push_back(str.substr(start));
			}
			return ret;
		}

		void split_string(const std::string& strs, const std::string& sp,
				std::vector<std::string>& res)
		{
			std::string::size_type pos1, pos2;

			pos2 = strs.find(sp);
			pos1 = 0;

			while (std::string::npos != pos2)
			{
				res.push_back(strs.substr(pos1, pos2 - pos1));
				pos1 = pos2 + sp.length();
				pos2 = strs.find(sp, pos1);
			}

			res.push_back(strs.substr(pos1));
		}

		int string_replace(std::string& str, const std::string& pattern,
				const std::string& newpat) {
			int count = 0;
			const size_t nsize = newpat.size();
			const size_t psize = pattern.size();

			for (size_t pos = str.find(pattern, 0); pos != std::string::npos;
					pos = str.find(pattern, pos + nsize)) {
				str.replace(pos, psize, newpat);
				count++;
			}

			return count;
		}

		char* str_tolower(char* str)
		{
			char* start = str;
			while (*start)
			{
				*start = tolower(*start);
				start++;
			}
			return str;
		}
		char* str_toupper(char* str)
		{
			char* start = str;
			while (*start)
			{
				*start = toupper(*start);
				start++;
			}
			return str;
		}

		std::string string_tolower(const std::string& str)
		{
			uint32 i = 0;
			std::string ret;
			while (i < str.size())
			{
				char ch = tolower(str.at(i));
				ret.push_back(ch);
				i++;
			}
			return ret;
		}
		std::string string_toupper(const std::string& str)
		{
			uint32 i = 0;
			std::string ret;
			while (i < str.size())
			{
				char ch = toupper(str.at(i));
				ret.push_back(ch);
				i++;
			}
			return ret;
		}

		bool str_toint64(const char* str, int64& value)
		{
			RETURN_FALSE_IF_NULL(str);
			char *endptr = NULL;
			long long int val = strtoll(str, &endptr, 10);
			if (NULL == endptr)
			{
				return false;
			}
			value = val;
			return true;
		}

		bool str_touint64(const char* str, uint64& value)
		{
			int64 temp;
			if (!str_toint64(str, temp))
			{
				return false;
			}
			value = (uint64) temp;
			return true;
		}

		bool str_tofloat(const char* str, float& value)
		{
			RETURN_FALSE_IF_NULL(str);
			char *endptr = NULL;
			float val = strtof(str, &endptr);
			if (NULL == endptr)
			{
				return false;
			}
			value = val;
			return true;
		}

		bool str_todouble(const char* str, double& value)
		{
			RETURN_FALSE_IF_NULL(str);
			char *endptr = NULL;
			double val = strtod(str, &endptr);
			if (NULL == endptr)
			{
				return false;
			}
			value = val;
			return true;
		}

		static inline char toHex(char x)
		{
			return x > 9 ? x - 10 + 'A' : x + '0';
		}

		static inline bool fromHex(char x, char& val)
		{
			x = toupper(x);
			if (isdigit(x) || (x >= 'A' && x <= 'F'))
			{
				val = isdigit(x) ? x - '0' : x - 'A' + 10;
				return true;
			}
			return false;
		}

		bool url_encode(const std::string& sIn, std::string& sOut)
		{
			//char hexchars[] = "0123456789ABCDEF";
			for (size_t ix = 0; ix < sIn.size(); ix++)
			{
				char buf[4];
				memset(buf, 0, 4);
				if (isalnum((char) sIn[ix]))
				{
					buf[0] = sIn[ix];
				} else if (isspace((char) sIn[ix]))
				{
					buf[0] = '+';
				} else
				{
					buf[0] = '%';
					int tmp = (sIn[ix]) & 0xFF;
					sprintf(buf + 1, "%02X", tmp);
				}
				sOut.append(buf);
			}
			return true;
		}

		bool url_decode(const std::string& sIn, std::string& sb)
		{
			sb.clear();
			size_t numChars = sIn.size();

			for (size_t ix = 0; ix < numChars; ix++)
			{
				char ch = sIn.at(ix);
				switch (ch)
				{
					case '%':
					{
						if (ix + 2 < numChars)
						{
							char temp;
							if (fromHex(sIn[ix + 1], temp))
							{
								ch = (temp << 4);
								if (fromHex(sIn[ix + 2], temp))
								{
									ch |= temp;
									ix += 2;
								} else //fromHex(sIn[ix + 2]) failed
								{
									return false;
								}
							} else //fromHex(sIn[ix + 1]) failed
							{
								return false;
							}

						} else //ix + 2 >= numChars
						{
							return false;
						}
						break;
					}
					case '+':
					{
						ch = ' ';
						break;
					}
					default:
					{
						break;
					}
				}
				sb += ch;
			}
			return true;
		}

		int codec_string_to_wstring(const std::string &input, std::wstring& out,
				const char* codec)
		{
			size_t in_size = input.size();
			char * in_buf = const_cast<char*>(input.c_str());
			size_t buf_size = input.length() * 6; // pessimistic: max UTF-8 char size
			wchar_t buf[buf_size];
			memset(buf, 0, sizeof(wchar_t) * buf_size);
			char * out_buf = (char*) buf;
			size_t out_size(buf_size);
			iconv_t conv_desc = iconv_open("wchar_t", codec);
			if (conv_desc == iconv_t(-1))
			{
				//throw std::runtime_error(std::string("Could not open iconv: ") + strerror(errno));
				return -1;
			}
			size_t iconv_value = iconv(conv_desc, &in_buf, &in_size, &out_buf,
					&out_size);
			if (iconv_value == (size_t) -1)
			{
				//throw std::runtime_error(std::string("When converting: ") + strerror(errno));
				return -1;
			}
			int ret = iconv_close(conv_desc);
			if (ret != 0)
			{
				//throw std::runtime_error(std::string("Could not close iconv: ") + strerror(errno));
				return -1;
			}
			out.append(buf, iconv_value);
			//out.append(buf);
			//std::string s(buf);
			//delete[] buf;
			return out.size();
		}

		int wstring_to_codec_string(const std::wstring &input, std::string& out,
				const char* codec)
		{
			size_t in_size = input.length() * sizeof(wchar_t);
			char * in_buf = (char*) input.data();
			size_t buf_size = input.length() * 6; // pessimistic: max UTF-8 char size
			char buf[buf_size];
			memset(buf, 0, buf_size);
			char * out_buf(buf);
			size_t out_size(buf_size);
			iconv_t conv_desc = iconv_open(codec, "wchar_t");
			if (conv_desc == iconv_t(-1))
			{
				//throw std::runtime_error(std::string("Could not open iconv: ") + strerror(errno));
				return -1;
			}
			size_t iconv_value = iconv(conv_desc, &in_buf, &in_size, &out_buf,
					&out_size);
			if (iconv_value == -1)
			{
				//throw std::runtime_error(std::string("When converting: ") + strerror(errno));
				return -1;
			}
			int ret = iconv_close(conv_desc);
			if (ret != 0)
			{
				//throw std::runtime_error(std::string("Could not close iconv: ") + strerror(errno));
				return -1;
			}
			out.append(buf);
			//std::string s(buf);
			//delete[] buf;
			return out.size();
		}

		int gbk_string_to_utf8_string(const std::string &input,
				std::string& out)
		{
			return string_codec_convert(input, out, "GBK", "UTF-8");
		}

		int string_codec_convert_with_desc(iconv_t cd, const std::string &input,
				std::string& out)
		{
			size_t in_size = input.size();
			char * in_buf = (char*) input.data();
			size_t buf_size = input.length() * 6; // pessimistic: max UTF-8 char size
			char buf[buf_size];
			memset(buf, 0, buf_size);
			char * out_buf = buf;
			size_t out_size(buf_size);
			iconv_t conv_desc = cd;
			if (conv_desc == iconv_t(-1))
			{
				return -1;
			}

			size_t iconv_value = iconv(conv_desc, &in_buf, &in_size, &out_buf,
					&out_size);
			if (iconv_value == (size_t) -1)
			{
				return -1;
			}
			out.append(buf);
			return out.size();
		}

		int string_codec_convert(const std::string &input, std::string& out,
				const char* fromcodec, const char* tocodec)
		{
			size_t in_size = input.size();
			char * in_buf = (char*) input.data();
			size_t buf_size = input.length() * 6; // pessimistic: max UTF-8 char size
			char buf[buf_size];
			memset(buf, 0, buf_size);
			char * out_buf = buf;
			size_t out_size(buf_size);
			iconv_t conv_desc = iconv_open(tocodec, fromcodec);
			if (conv_desc == iconv_t(-1))
			{
				//throw std::runtime_error(std::string("Could not open iconv: ") + strerror(errno));
				return -1;
			}

			size_t iconv_value = iconv(conv_desc, &in_buf, &in_size, &out_buf,
					&out_size);
			if (iconv_value == -1)
			{
				//throw std::runtime_error(std::string("When converting: ") + strerror(errno));
				return -1;
			}
			int ret = iconv_close(conv_desc);
			if (ret != 0)
			{
				//throw std::runtime_error(std::string("Could not close iconv: ") + strerror(errno));
				return -1;
			}
			out.append(buf);
			//std::string s(buf);
			//delete[] buf;
			return out.size();
		}

		bool is_gbk_string(const std::string& sIn)
		{
			uint32 len = sIn.size();
			uint32 i = 0;
			while (i < len)
			{
				uint8 ch = (uint8) sIn.at(i);
				if (ch <= 0x7F)
				{
					i++;
				} else
				{
					//2bytes
					if (i + 1 >= len)
					{
						return false;
					}
					i++;
					uint8 ch2 = (uint8) sIn.at(i);
					if (ch >= 0xA1 && ch <= 0xA9)
					{
						if (ch2 >= 0xA1 && ch2 <= 0xFE)
						{
							i++;
							continue;
						}
					}
					if (ch >= 0xB0 && ch <= 0xF7)
					{
						if (ch2 >= 0xA1 && ch2 <= 0xFE)
						{
							i++;
							continue;
						}
					}
					if (ch >= 0x81 && ch <= 0xA0)
					{
						if (ch2 >= 0x40 && ch2 <= 0xFE && ch2 != 0x7F)
						{
							i++;
							continue;
						}
					}
					if (ch >= 0xAA && ch <= 0xFE)
					{
						if (ch2 >= 0x40 && ch2 <= 0xA0 && ch2 != 0x7F)
						{
							i++;
							continue;
						}
					}
					if ((ch >= 0xA8 && ch <= 0xA9)
							|| (ch >= 0xA1 && ch <= 0xA7))
					{
						if (ch2 >= 0x40 && ch2 <= 0xA0 && ch2 != 0x7F)
						{
							i++;
							continue;
						}
					}
					if ((ch >= 0xAA && ch <= 0xAF)
							|| (ch >= 0xF8 && ch <= 0xFE))
					{
						if (ch2 >= 0xA1 && ch2 <= 0xFE)
						{
							i++;
							continue;
						}
					}
					return false;
				}
			}
			return true;
		}

		bool is_big5_string(const std::string& sIn)
		{
			uint32 len = sIn.size();
			uint32 i = 0;
			while (i < len)
			{
				uint8 ch = (uint8) sIn.at(i);
				if (ch <= 0x7F)
				{
					i++;
				} else
				{
					//2bytes
					if (i + 1 >= len)
					{
						return false;
					}
					i++;
					uint8 ch2 = (uint8) sIn.at(i);
					if (ch >= 0x81 && ch <= 0xFE)
					{
						if ((ch2 >= 0x40 && ch2 <= 0x7E)
								|| (ch2 >= 0xA1 && ch2 <= 0xFE))
						{
							i++;
						} else
						{
							return false;
						}
					} else
					{
						return false;
					}
				}
			}
			return true;
		}

		bool has_prefix(const std::string& str, const std::string& prefix)
		{
			if (prefix.empty())
			{
				return true;
			}
			if (str.size() < prefix.size())
			{
				return false;
			}
			return str.find(prefix) == 0;
		}
		bool has_suffix(const std::string& str, const std::string& suffix)
		{
			if (suffix.empty())
			{
				return true;
			}
			if (str.size() < suffix.size())
			{
				return false;
			}
			return str.rfind(suffix) == str.size() - suffix.size();
		}
	}
}
