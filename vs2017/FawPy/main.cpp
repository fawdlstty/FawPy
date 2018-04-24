#include <iostream>
#include <fstream>
#include <string>
#include <variant>
#include <vector>
#include <cassert>

struct DisasmItem {
	size_t																level = 0;
	size_t																m_num = 0;
	std::string															m_cmd = "";
	size_t																m_val = 0;
	std::variant<std::vector<DisasmItem>, std::string, int, nullptr_t>	m_val2;

	DisasmItem (std::string s) {
		while (s[0] == ' ') {
			++level;
			s.erase (0, 4);
		}
		m_num = atoi (&s[0]);
		size_t p = 6;
		while (s[p] != ' ')
			m_cmd += s[p++];
		if (s.length () > 27) {
			m_val = atoi (&s[27]);
			if (s.length () > 33) {
				char ch = s[33];
				if ((ch >= '0' && ch <= '9') || ch == '-') {
					m_val2 = atoi (&s[33]);
				} else if (ch == '<') {
					m_val2 = std::vector<DisasmItem> ();
				} else {
					m_val2 = &s[33];
				}
			} else {
				m_val2 = nullptr;
			}
		}
	}

	static std::vector<DisasmItem> parse (std::string s) {
		std::vector<DisasmItem> v;
		size_t start = 0, p = 0;
		p = s.find ('\n');
		size_t level = 0;
		while (p != std::string::npos) {
			std::string str = s.substr (start, p);
			if (str.empty ())
				break;
			DisasmItem item (str);
#define _(x) std::get<std::vector<DisasmItem>> ((x).rbegin ()->m_val2)
			if (level == 0)
				v.push_back (item);
			else if (level == 1)
				_ (v).push_back (item);
			else if (level == 2)
				_ (_ (v)).push_back (item);
			else if (level == 3)
				_ (_ (_ (v))).push_back (item);
			else if (level == 4)
				_ (_ (_ (_ (v)))).push_back (item);
			else if (level == 5)
				_ (_ (_ (_ (_ (v))))).push_back (item);
			else if (level == 6)
				_ (_ (_ (_ (_ (_ (v)))))).push_back (item);
			else if (level == 7)
				_ (_ (_ (_ (_ (_ (_ (v))))))).push_back (item);
			else if (level == 8)
				_ (_ (_ (_ (_ (_ (_ (_ (v)))))))).push_back (item);
			else if (level == 9)
				_ (_ (_ (_ (_ (_ (_ (_ (_ (v))))))))).push_back (item);
#undef _
			start = p + 1;
			p = s.find ('\n', start);
		}
		return v;
	}
};

struct Obj_Stat {
	enum StatType {

	} m_type;
	size_t m_level;
	std::vector<std::string> m_voper;
};

struct Obj_Func {
	size_t m_level;
	std::string m_name;
	std::vector<std::string> m_pvaram;
	std::vector<Obj_Stat> m_vcode;
};

struct Obj_Class {
	size_t m_level;
	std::string m_name;
	std::vector<Obj_Func> m_vfunc;
	std::vector<Obj_Stat> m_param;
};

std::variant<Obj_Stat, Obj_Func, Obj_Class> Obj_Item;

void parse_code (std::vector<DisasmItem> &v, size_t level = 0) {
	for (size_t i = 0; i < v.size (); ++i) {
		if (v[i].m_cmd == "LOAD_BUILD_CLASS") {
			assert (v[i + 1].m_cmd == "LOAD_CONST");
			assert (v[i + 2].m_cmd == "LOAD_CONST");
			assert (v[i + 3].m_cmd == "MAKE_FUNCTION");
			Obj_Class cls;
			cls.m_level = level;
			cls.m_name = std::get<std::string> (v[i + 2].m_val2);
			cls.m_name = cls.m_name.substr (1, cls.m_name.length () - 2);
			std::vector<DisasmItem> &v_child = std::get<std::vector<DisasmItem>> (v[i + 1].m_val2);
			//if ()
		}
	}
}

int main (int argc, char* argv[]) {
	// 读取文件
	std::ifstream ifs ("D:/fawdlstty/Desktop/py_disasm/a.txt");
	std::string s ((std::istreambuf_iterator<char> (ifs)), std::istreambuf_iterator<char> ());
	std::vector<DisasmItem> v = DisasmItem::parse (s);

	// 翻译指令
	parse_code (v);

	std::cin >> s;
	return 0;
}
