#ifndef __DISASMITEM_HPP__
#define __DISASMITEM_HPP__



#include <iostream>
#include <string>
#include <vector>
#include <variant>



// 字节码指令结构
struct DisasmItem {
	size_t																		m_level = 0;
	size_t																		m_offset = 0;
	std::string																	m_cmd = "";
	size_t																		m_val = 0;
	std::variant<std::vector<DisasmItem>, std::string, double, int, nullptr_t>	m_val2;

	// 生成指令结构
	DisasmItem (std::string s) {
		while (s[0] == ' ') {
			++m_level;
			s.erase (0, 4);
		}
		m_offset = atoi (&s[0]);
		size_t p = 6;
		while (s[p] != ' ')
			m_cmd += s[p++];
		if (s.length () > 27) {
			m_val = atoi (&s[27]);
			if (s.length () > 33) {
				char ch = s[33];
				if ((ch >= '0' && ch <= '9') || ch == '-' || ch == '.') {
					char *pch = &s[33];
					while ((*pch >= '0' && *pch <= '9') || *pch == '-') pch++;
					if (*pch == '.')
						m_val2 = atof (&s[33]);
					else
						m_val2 = atoi (&s[33]);
				} else if (ch == '<' && s.length () > 40) {
					m_val2 = std::vector<DisasmItem> ();
				} else {
					m_val2 = &s[33];
				}
			} else {
				m_val2 = nullptr;
			}
		}
	}

	// 生成指令结构数组
	static std::vector<DisasmItem> parse (std::string s) {
		std::vector<DisasmItem> v;
		size_t start = 0, p = 0;
		p = s.find ('\n');
		while (p != std::string::npos) {
			std::string str = s.substr (start, p - start);
			if (str.empty ())
				break;
			DisasmItem item (str);
#define _(x) std::get<std::vector<DisasmItem>> ((x).rbegin ()->m_val2)
			if (item.m_level == 0)
				v.push_back (item);
			else if (item.m_level == 1)
				_ (v).push_back (item);
			else if (item.m_level == 2)
				_ (_ (v)).push_back (item);
			else if (item.m_level == 3)
				_ (_ (_ (v))).push_back (item);
			else if (item.m_level == 4)
				_ (_ (_ (_ (v)))).push_back (item);
			else if (item.m_level == 5)
				_ (_ (_ (_ (_ (v))))).push_back (item);
			else if (item.m_level == 6)
				_ (_ (_ (_ (_ (_ (v)))))).push_back (item);
			else if (item.m_level == 7)
				_ (_ (_ (_ (_ (_ (_ (v))))))).push_back (item);
			else if (item.m_level == 8)
				_ (_ (_ (_ (_ (_ (_ (_ (v)))))))).push_back (item);
			else if (item.m_level == 9)
				_ (_ (_ (_ (_ (_ (_ (_ (_ (v))))))))).push_back (item);
#undef _
			start = p + 1;
			p = s.find ('\n', start);
		}
		return v;
	}
};



#endif //__DISASMITEM_HPP__
