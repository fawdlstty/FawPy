////////////////////////////////////////////////////////////////////////////////
//
// Class Name:  hString
// Description: 用于字符串相关扩展操作
// Class URI:   
// Author:      Fawdlstty
// Author URI:  https://www.fawdlstty.com/
// Version:     0.1
// License:     MIT
// Last Update: Feb 22, 2018
//
////////////////////////////////////////////////////////////////////////////////



#ifndef __HSTRING_HPP__
#define __HSTRING_HPP__



#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cstdarg>
#include <ctime>
#include <chrono>

#ifdef _UNICODE
#define _T(x) L##x
#else //_UNICODE
#define _T(x) x
#endif //_UNICODE



template<typename charT>
class hString {
public:
	//清除字符串开始部分空格
	static void trimLeft (std::basic_string<charT> &str) {
		str.erase (0, str.find_first_not_of (' '));
	}

	//清除字符串结束部分空格
	static void trimRight (std::basic_string<charT> &str) {
		str.erase (str.find_last_not_of (' ') + 1);
	}

	//清楚两端空格
	static void trim (std::basic_string<charT> &str) {
		str.erase (0, str.find_first_not_of (' '));
		str.erase (str.find_last_not_of (' ') + 1);
	}

	//删除字符串中指定字符
	static void erase (std::basic_string<charT> &str, const charT &charactor) {
		str.erase (remove_if (str.begin (), str.end (), bind2nd (std::equal_to<charT> (), charactor)), str.end ());
	}

	// 根据index删除字符
	static void remove_at (std::basic_string<charT> &str, const int index) {
		if (str.length () <= index)
			return;
		str.erase (str.begin () + index);
	}

	//替换字符串中指定字符串
	static int replace (std::basic_string<charT> &str, const std::basic_string<charT> &strObj, const std::basic_string<charT> &strDest) {
		int ret = 0;
		size_t pos = str.find (strObj);
		while (pos != std::basic_string<charT>::npos) {
			ret++;
			str.replace (pos, strObj.size (), strDest);
			pos = str.find (strObj, pos + 2);
		}
		return ret;
	}

	//字符串截断
	static void split (std::basic_string<charT> s, std::vector<std::basic_string<charT> >& v, char ch = ' ') {
		size_t start = 0, p, len = s.length ();
		do {
			p = s.find (ch, start);
			if (p == std::string::npos) p = len;
			s[p] = '\0';
			if (s[start] != '\0') v.push_back (&s[start]);
			start = p + 1;
		} while (start < len);
	}

	//字符串格式化
	static std::basic_string<charT> format (std::basic_string<charT> fmt_str, ...) {
		try {
			std::basic_string<charT> str_result = "";
			if (fmt_str == "")
				return str_result;
			va_list ap;
#ifndef __GNUC__
			//来源：http://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf
			ptrdiff_t final_n, n = ((ptrdiff_t)fmt_str.size ()) * 2;
			std::unique_ptr<charT[]> formatted;
			while (true) {
				formatted.reset (new charT[n]);
				//strcpy_s (&formatted [0], fmt_str.size (), fmt_str.c_str ());
				va_start (ap, fmt_str);
				final_n = vsnprintf_s (&formatted[0], n, _TRUNCATE, fmt_str.c_str (), ap);
				va_end (ap);
				if (final_n < 0 || final_n >= n)
					n += abs (final_n - n + 1);
				else
					break;
			}
			str_result = formatted.get ();
#else //__GNUC__
			va_start (ap, fmt_str);
			char *buf = nullptr;
			int iresult = vasprintf (&buf, fmt_str.c_str (), ap);
			if (!buf) {
				va_end (ap);
				return str_result;
			}
			else if (iresult >= 0) {
				iresult = strlen (buf);
				str_result.append (buf, iresult);
			}
			free (buf);
			va_end (ap);
#endif //__GNUC__
			return str_result;
		}
		catch (...) {
			return "";
		}
	}

	// url编码
	static std::string url_encode (std::string str) {
		std::string str_ret = "";
		try {
			std::function<unsigned char (unsigned char)> _to_hex = [](unsigned char ch) { return (ch > 9 ? (ch + 'A' - 10) : ch + '0'); };
			str_ret.reserve (str.size ());
			for (char ch : str) {
				if (isalnum (ch) || ch == '-' || ch == '_' || ch == '.' || ch == '~')
					str_ret += ch;
				else if (ch == ' ')
					str_ret += '+';
				else {
					str_ret += '%';
					str_ret += _to_hex (((unsigned char)ch >> 4) & 0xf);
					str_ret += _to_hex ((unsigned char)ch & 0xf);
				}
			}
		}
		catch (...) {
		}
		return str_ret;
	}

	// url解码
	static std::string url_decode (std::string str) {
		std::string str_ret = "";
		try {
			std::function<unsigned char (unsigned char)> _from_hex = [](unsigned char ch) {
				if (ch >= 'A' && ch <= 'F')
					return ch - 'A' + 10;
				else if (ch >= 'a' && ch <= 'f')
					return ch - 'a' + 10;
				else if (ch >= '0' && ch <= '9')
					return ch - '0';
				return 0;
			};
			str_ret.reserve (str.size ());
			for (size_t i = 0; i < str.length (); ++i) {
				char ch = str[i];
				if (ch == '+')
					str_ret += ' ';
				else if (ch == '%') {
					if (i + 2 >= str.length ())
						return "";
					ch = _from_hex (str[++i]) << 4;
					ch |= _from_hex (str[++i]);
					str_ret += ch;
				}
				else
					str_ret += ch;
			}
		}
		catch (...) {
		}
		return str_ret;
	}

	// 解包消息
	static std::map<std::string, std::string> unpack_map (std::string data, std::string content_type) {
		std::map<std::string, std::string> ret;
		try {
			std::string str_key = "", str_value = "";
			if (data.length () == 0 || data[0] == '<')
				return ret;
			else if (content_type.size () > 20 && content_type.substr (0, 20) == "multipart/form-data;") {
				std::string boundary = content_type.substr (content_type.find ("boundary=") + 5);
				boundary[0] = '\r';
				boundary[1] = '\n';
				boundary[2] = '-';
				boundary[3] = '-';
				int p = boundary.length (), p2;
				while (data.find (boundary, p) != -1) {
					p = data.find ("name=\"", p) + 6;
					p2 = data.find ("\"", p);
					str_key = data.substr (p, p2 - p);
					p = data.find ("\r\n\r\n", p) + 4;
					p2 = data.find (boundary, p);
					str_value = data.substr (p, p2 - p);
					ret[str_key] = str_value;
					p = p2 + boundary.length ();
				}
			}
			else {
				char *p = (data[0] == '{' ? &data[1] : &data[0]);
				int state = 0, state2 = 0;
				while (*p != '\0' && (*p != '}' || (state == 1 || state == 3))) {
					if (state & 1 && *p == '\\') {
						++p;
						if (state == 1 || (state == 0 && state2 == 0))
							str_key += *p;
						else if (state == 2 && state2 == 1)
							str_value += *p;
						else if (state == 3 || (state == 0 && state2 == 1))
							str_value += *p;
						++p;
						continue;
					}
					if (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n')
						;
					else if (*p == '\"') {
						if (state == 3) {
							ret[str_key] = str_value;
							str_key = str_value = "";
						}
						state = (state + 1) % 4;
					}
					else if ((*p == '&' || *p == ',') && state != 3) {
						state2 = 0;
						if (state == 2)
							state = 0;
						if (str_key != "" && state == 0) {
							ret[str_key] = str_value;
							str_key = str_value = "";
						}
					}
					else if ((*p == '=' || *p == ':') && state2 == 0) {
						state2 = 1;
					}
					else {
						if (state == 1 || (state == 0 && state2 == 0))
							str_key += *p;
						else if (state == 2 && state2 == 1)
							str_value += *p;
						else if (state == 3 || (state == 0 && state2 == 1))
							str_value += *p;
					}
					++p;
				}
				if (str_key != "" && state == 0) {
					ret[str_key] = str_value;
					str_key = str_value = "";
				}
			}
		}
		catch (...) {
		}
		return ret;
	}

	// 打包消息
	// type 0: a=b&c=d
	// type 1: { "a": "b", "c": "d" }
	static std::string pack_map (std::map<std::string, std::string> m, int type = 0)
	{
		std::string s = "";
		try {
			if (type == 1) s += "{";
			for (auto i = m.begin (); i != m.end (); ++i) {
				if (i != m.begin ()) {
					if (type == 1)			s += ", ";
					else if (type == 0)		s += "&";
				}
				if (type == 1)				s += "\"";
				s += i->first; //hString<char>::url_encode (i->first);
				if (type == 1)				s += "\": \"";
				else if (type == 0)			s += "=";
				s += i->second; //hString<char>::url_encode (i->second);
				if (type == 1)				s += "\"";
			}
			if (type == 1)					s += "}";
		}
		catch (...) {
		}
		return s;
	}

	// 生成长度为n的空字符串
	static std::string make_space (int n) {
		std::string s;
		if (n > 0)
			s.resize (n, ' ');
		return s;
	}

	// 格式化日期
	static std::string format_date () {
		try {
			char buf_time[32], buf_time2[32];
			buf_time[0] = buf_time2[0] = '\0';
			auto time_now = std::chrono::system_clock::now ();
			auto duration_in_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_now.time_since_epoch ());
			auto ms_part = duration_in_ms - std::chrono::duration_cast<std::chrono::seconds>(duration_in_ms);
			time_t raw_time = std::chrono::system_clock::to_time_t (time_now);
			tm *local_time_now = localtime (&raw_time);//_localtime64_s
			strftime (buf_time2, sizeof (buf_time2), "%Y%m%d-%H%M%S", local_time_now);
			//char *xx = std::put_time (local_time_now, "%Y-%m-%d %H:%M:%S");
			sprintf (buf_time, "%s-%03d", buf_time2, ms_part.count ());
			return std::string (buf_time);
		}
		catch (...) {
		}
		return "";
	}

	//
	static std::string get_padding (size_t padding) {
		std::string s = "";
		while (padding-- > 0)
			s += "    ";
		return s;
	}
};



typedef hString<char> hStringA;
typedef hString<wchar_t> hStringW;
#ifdef _UNICODE
typedef hStringW hString_t;
typedef std::wstring string_t;
#else //_UNICODE
typedef hStringA hString_t;
typedef std::string string_t;
#endif //_UNICODE



#endif //__HSTRING_HPP__
