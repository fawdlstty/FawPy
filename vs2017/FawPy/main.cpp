#include <iostream>
#include <fstream>
#include <string>
#include <variant>
#include <vector>



struct DisasmItem {
	size_t																m_level = 0;
	size_t																m_num = 0;
	std::string															m_cmd = "";
	size_t																m_val = 0;
	std::variant<std::vector<DisasmItem>, std::string, int, nullptr_t>	m_val2;

	DisasmItem (std::string s) {
		while (s[0] == ' ') {
			++m_level;
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
		None = 0,
		Operation,
		Item1,
	}							m_type = None;
	size_t						m_level;
	std::vector<std::string>	m_voper;

	void init () {
		m_type = None;
		m_voper.clear ();
	}
};

struct Obj_Func {
	size_t						m_level;
	std::string					m_name;
	std::vector<std::string>	m_pvaram;
	std::vector<Obj_Stat>		m_vstat;
};

struct Obj_Class {
	size_t						m_level;
	std::string					m_name;
	std::vector<Obj_Stat>		m_vstat;
	std::vector<Obj_Func>		m_vfunc;
};

typedef std::variant<Obj_Stat, Obj_Func, Obj_Class>		Obj_Item;
typedef std::variant<std::vector<Obj_Item>, nullptr_t>	Obj_Items;



Obj_Items parse_code (std::vector<DisasmItem> &v) {
#define RETURN_ERROR(x) { std::cout << (x) << std::endl; return nullptr; }
	Obj_Items _item;
	_item = std::vector<Obj_Item> ();
	std::vector<Obj_Item> &_v = std::get<0> (_item);
	//
	Obj_Stat stat;
	stat.m_level = v[0].m_level;
	for (size_t i = 0; i < v.size (); ++i) {
		if (v [i].m_cmd == "LOAD_BUILD_CLASS") {
			// 类创建指令
			if (stat.m_type != Obj_Stat::None) {
				RETURN_ERROR ("正在处理其他操作时不可创建类");
			} else if (v.size () <= i + 3) {
				RETURN_ERROR ("类创建指令后跟指令不可少于 3 个");
			} else if (v [i + 1].m_cmd != "LOAD_CONST") {
				RETURN_ERROR ("类创建指令后跟第 1 个指令不为 LOAD_CONST");
			} else if (v [i + 1].m_val2.index () != 0) {
				RETURN_ERROR ("类创建指令后跟第 1 个指令不为 LOAD_CONST 代码对象");
			} else if (v [i + 2].m_cmd != "LOAD_CONST") {
				RETURN_ERROR ("类创建指令后跟第 2 个指令不为 LOAD_CONST");
			} else if (v [i + 3].m_cmd != "MAKE_FUNCTION") {
				RETURN_ERROR ("类创建指令后跟第 3 个指令不为 MAKE_FUNCTION");
			}
			Obj_Class cls;
			cls.m_level = stat.m_level;
			cls.m_name = std::get<std::string> (v [i + 2].m_val2);
			cls.m_name = cls.m_name.substr (1, cls.m_name.length () - 2);
			//
			std::vector<DisasmItem> &v_child = std::get<std::vector<DisasmItem>> (v [i + 1].m_val2);
			if (v_child.size () < 6) {
				RETURN_ERROR ("类内部指令不可少于 6 个");
			} else if (v_child [0].m_cmd != "LOAD_NAME") {
				RETURN_ERROR ("类内部指令第 1 个不为 LOAD_NAME");
			} else if (v_child [1].m_cmd != "STORE_NAME") {
				RETURN_ERROR ("类内部指令第 2 个不为 STORE_NAME");
			} else if (v_child [2].m_cmd != "LOAD_CONST") {
				RETURN_ERROR ("类内部指令第 3 个不为 LOAD_CONST");
			} else if (v_child [3].m_cmd != "STORE_NAME") {
				RETURN_ERROR ("类内部指令第 4 个不为 STORE_NAME");
			} else if (v_child [v_child.size () - 2].m_cmd != "LOAD_CONST") {
				RETURN_ERROR ("类内部指令倒数第 2 个不为 LOAD_CONST");
			} else if (v_child [v_child.size () - 1].m_cmd != "RETURN_VALUE") {
				RETURN_ERROR ("类内部指令倒数第 1 个不为 RETURN_VALUE");
			}
			v_child.erase (v_child.begin (), v_child.begin () + 4);
			v_child.erase (v_child.end () - 2, v_child.end ());
			//
			Obj_Items _child_items = parse_code (v_child);
			if (_child_items.index () == 1)
				return nullptr;
			std::vector<Obj_Item> child_items = std::get<0> (_child_items);
			for (Obj_Item &child_item : child_items) {
				if (child_item.index () == 0) {
					cls.m_vstat.push_back (std::get<0> (child_item));
				} else if (child_item.index () == 1) {
					cls.m_vfunc.push_back (std::get<1> (child_item));
				} else {
					RETURN_ERROR ("暂不支持类嵌套类");
				}
			}
			_v.push_back (cls);
			i += 3;
		} else if (v [i].m_cmd == "LOAD_CONST" && v [i].m_val2.index () == 0) {
			// 函数创建指令
			if (stat.m_type != Obj_Stat::None) {
				RETURN_ERROR ("正在处理其他操作时不可创建函数");
			} else if (v.size () <= i + 3) {
				RETURN_ERROR ("函数创建指令后跟指令不可少于 3 个");
			} else if (v [i + 1].m_cmd != "LOAD_CONST") {
				RETURN_ERROR ("函数创建指令后跟第 1 个指令不为 LOAD_CONST");
			} else if (v [i + 2].m_cmd != "MAKE_FUNCTION") {
				RETURN_ERROR ("函数创建指令后跟第 2 个指令不为 MAKE_FUNCTION");
			} else if (v [i + 3].m_cmd != "STORE_NAME") {
				RETURN_ERROR ("函数创建指令后跟第 3 个指令不为 STORE_NAME");
			}
			Obj_Func func;
			func.m_level = stat.m_level;
			func.m_name = std::get<std::string> (v [i + 2].m_val2);
			func.m_name = func.m_name.substr (1, func.m_name.length () - 2);
			//
			std::vector<DisasmItem> &v_child = std::get<std::vector<DisasmItem>> (v [i].m_val2);
			for (DisasmItem &_child_item : v_child) {
				if (_child_item.m_cmd == "LOAD_FAST") {
					std::string _tmp = std::get<1> (_child_item.m_val2);
					size_t j = 0;
					for (j = 0; j < func.m_pvaram.size (); ++j) {
						if (func.m_pvaram [j] == _tmp)
							break;
					}
					if (j == func.m_pvaram.size ())
						func.m_pvaram.push_back (_tmp);
				}
			}
			//
			Obj_Items _child_items = parse_code (v_child);
			if (_child_items.index () == 1)
				return nullptr;
			std::vector<Obj_Item> child_items = std::get<0> (_child_items);
			for (Obj_Item &child_item : child_items) {
				if (child_item.index () == 0) {
					func.m_vstat.push_back (std::get<0> (child_item));
				} else {
					RETURN_ERROR ("暂不支持函数嵌套类或函数");
				}
			}
			_v.push_back (func);
			i += 3;
		} else if (v[i].m_cmd == "LOAD_CONST" || v[i].m_cmd == "LOAD_FAST" || v[i].m_cmd == "LOAD_NAME") {
			stat.m_voper.push_back (std::get<1> (v[i].m_val2));
			stat.m_type = Obj_Stat::Item1;
		} else if (v[i].m_cmd == "RETURN_VALUE") {
			stat.m_voper.insert (stat.m_voper.begin (), "return");
			stat.m_type = Obj_Stat::Operation;
			_v.push_back (stat);
			stat.init ();
		} else if (v[i].m_cmd=="BINARY_ADD" || v[i].m_cmd=="BINARY_SUBTRACT" || v[i].m_cmd=="BINARY_MULTIPLY" || v[i].m_cmd=="BINARY_TRUE_DIVIDE") {
			std::string str_signal;
			if (v[i].m_cmd == "BINARY_ADD")
				str_signal = "+";
			else if (v[i].m_cmd == "BINARY_SUBTRACT")
				str_signal = "-";
			else if (v[i].m_cmd == "BINARY_MULTIPLY")
				str_signal = "*";
			else
				str_signal = "/";
		}
	}
	return _item;
#undef RETURN_ERROR
}



int main (int argc, char* argv []) {
	// 读取文件
	std::ifstream ifs ("D:/fawdlstty/Desktop/py_disasm/a.txt");
	std::string s ((std::istreambuf_iterator<char> (ifs)), std::istreambuf_iterator<char> ());
	std::vector<DisasmItem> v = DisasmItem::parse (s);

	// 翻译指令
	if (v [v.size () - 2].m_cmd == "LOAD_CONST") {
		std::cout << ("待翻译字节码倒数第二条指令不是 LOAD_CONST") << std::endl;
	} else if (v [v.size () - 1].m_cmd == "RETURN_VALUE") {
		std::cout << ("待翻译字节码倒数第一条指令不是 RETURN_VALUE") << std::endl;
	}
	v.erase (v.end () - 2, v.end ());
	parse_code (v);

	std::cin >> s;
	return 0;
}
