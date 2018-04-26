#include <iostream>
#include <fstream>
#include <string>
#include <variant>
#include <vector>



// 字节码指令结构
struct DisasmItem {
	size_t																m_level = 0;
	size_t																m_offset = 0;
	std::string															m_cmd = "";
	size_t																m_val = 0;
	std::variant<std::vector<DisasmItem>, std::string, int, nullptr_t>	m_val2;

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

	// 生成指令结构数组
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



// obj_expr_t  obj_value_t  obj_stat_t  obj_if_t  obj_for_t  obj_while_t  obj_func_t  obj_class_t  obj_item_t  obj_items_t
#pragma region 语法结构体定义
struct obj_value_t;

// 语句结构    m_level  m_vOper1  m_operator  m_vOper2
struct obj_expr_t {
	std::vector<obj_value_t>	m_vOper1;
	std::string					m_operator;
	std::vector<obj_value_t>	m_vOper2;
	void init () { m_vOper1.clear (); m_operator.clear (); m_vOper2.clear (); }
	bool empty () { return (!(m_vOper1.size () || m_vOper2.size ())); }
	void xchg (obj_expr_t &o) { m_operator.swap (o.m_operator); m_vOper1.swap (o.m_vOper1); m_vOper2.swap (o.m_vOper2); }
	std::string to_str ();
};

// 值结构
struct obj_value_t : public std::variant<obj_expr_t, std::string> {
	obj_value_t (obj_expr_t o)	: std::variant<obj_expr_t, std::string> (o) {}
	obj_value_t (std::string o)	: std::variant<obj_expr_t, std::string> (o) {}
	std::string to_str () {
		if (index () == 1) return std::get<1> (*this);
		std::string s = "(";
		s += std::get<0> (*this).to_str ();
		s += ")";
		return s;
	}
};

std::string obj_expr_t::to_str () {
	std::string str;
	if (m_operator == "call") {
		if (m_vOper1.size () > 1) str += "(";
		for (size_t i = 0; i < m_vOper1.size (); ++i) {
			if (i > 0) str += ", ";
			str += m_vOper1[i].to_str ();
		}
		if (m_vOper1.size () > 1) str += ")";
		str += " (";
		for (size_t i = 0; i < m_vOper2.size (); ++i) {
			if (i > 0) str += ", ";
			str += m_vOper2[i].to_str ();
		}
		str += ");";
		return str;
	}
}

struct obj_if_t;
struct obj_for_t;
struct obj_while_t;
struct obj_stat_t;

// if结构    m_level  m_exprs
struct obj_if_t {
	size_t															m_level;
	std::vector<std::pair<obj_value_t, std::vector<obj_stat_t>>>	m_exprs;
};

// for结构    m_level  m_name  m_begin  m_end  m_step  m_vStat
struct obj_for_t {
	size_t						m_level;
	std::string					m_name;
	obj_value_t					m_begin;
	obj_value_t					m_end;
	obj_value_t					m_step;
	std::vector<obj_stat_t>		m_vStat;
};

// while结构    m_level  m_expr  m_vStat
struct obj_while_t {
	size_t						m_level;
	obj_value_t					m_expr;
	std::vector<obj_stat_t>		m_vStat;
};

// 语句结构
struct obj_stat_t : public std::variant<obj_expr_t, obj_if_t, obj_for_t, obj_while_t> {
	obj_stat_t (obj_expr_t o)	: std::variant<obj_expr_t, obj_if_t, obj_for_t, obj_while_t> (o) {}
	obj_stat_t (obj_if_t o)		: std::variant<obj_expr_t, obj_if_t, obj_for_t, obj_while_t> (o) {}
	obj_stat_t (obj_for_t o)	: std::variant<obj_expr_t, obj_if_t, obj_for_t, obj_while_t> (o) {}
	obj_stat_t (obj_while_t o)	: std::variant<obj_expr_t, obj_if_t, obj_for_t, obj_while_t> (o) {}
	std::string to_str (size_t padding) {

	}
};

// 函数结构    m_level  m_name  m_vParam  m_vStat
struct obj_func_t {
	size_t						m_level;
	std::string					m_name;
	std::vector<std::string>	m_vParam;
	std::vector<obj_stat_t>		m_vStat;
};

// 类结构    m_level  m_name  m_vStat  m_vFunc
struct obj_class_t {
	size_t						m_level;
	std::string					m_name;
	std::vector<obj_stat_t>		m_vStat;
	std::vector<obj_func_t>		m_vFunc;
};

// 描述任意类型一行 obj_expr_t
struct obj_item_t : public std::variant<obj_stat_t, obj_func_t, obj_class_t, nullptr_t> {
	obj_item_t (obj_stat_t o)	: std::variant<obj_stat_t, obj_func_t, obj_class_t, nullptr_t> (o) {}
	obj_item_t (obj_func_t o)	: std::variant<obj_stat_t, obj_func_t, obj_class_t, nullptr_t> (o) {}
	obj_item_t (obj_class_t o)	: std::variant<obj_stat_t, obj_func_t, obj_class_t, nullptr_t> (o) {}
	obj_item_t (nullptr_t o)	: std::variant<obj_stat_t, obj_func_t, obj_class_t, nullptr_t> (o) {}
};

// 描述任意类型一段
typedef std::variant<std::vector<obj_item_t>, nullptr_t>				obj_items_t;
#pragma endregion



// 字节码解析器
struct BitConverter {
	obj_items_t m_items;
	BitConverter () : m_items (std::vector<obj_item_t> ()) {}

	// 反编译一段代码
	size_t parse_item (std::vector<DisasmItem> &v, size_t index, size_t _level) {
#define RETURN_ERROR(x) { std::cout << "line " << i << ": " << (x) << std::endl; return 0; }
		std::vector<obj_item_t> &m_v = std::get<0> (m_items);
		obj_expr_t _expr;
		// 由于可能一段代码分散于多行，所以产生一个遍历，直到读到第一段完整代码时跳出
		size_t i = index;
		for (; i < v.size (); ++i) {
			if (v[i].m_cmd == "LOAD_BUILD_CLASS") {
				// 类创建指令
				if		(_expr.m_vOper1.size () || _expr.m_vOper2.size ())	 { RETURN_ERROR ("类创建指令后跟指令不可少于 3 个"); }
				else if	(v.size () <= i + 3)				{ RETURN_ERROR ("类创建指令后跟指令不可少于 3 个"); }
				else if	(v[i + 1].m_cmd != "LOAD_CONST")	{ RETURN_ERROR ("类创建指令后跟第 1 个指令不为 LOAD_CONST"); }
				else if	(v[i + 1].m_val2.index () != 0)		{ RETURN_ERROR ("类创建指令后跟第 1 个指令不为 LOAD_CONST 代码对象"); }
				else if	(v[i + 2].m_cmd != "LOAD_CONST")	{ RETURN_ERROR ("类创建指令后跟第 2 个指令不为 LOAD_CONST"); }
				else if	(v[i + 3].m_cmd != "MAKE_FUNCTION")	{ RETURN_ERROR ("类创建指令后跟第 3 个指令不为 MAKE_FUNCTION"); }
				obj_class_t cls;
				cls.m_level = _level;
				cls.m_name = std::get<std::string> (v[i + 2].m_val2);
				cls.m_name = cls.m_name.substr (1, cls.m_name.length () - 2);
				//
				std::vector<DisasmItem> &v_child = std::get<0> (v[i + 1].m_val2);
				if		(v_child.size () < 6)									{ RETURN_ERROR ("类内部指令不可少于 6 个"); }
				else if	(v_child[0].m_cmd != "LOAD_NAME")						{ RETURN_ERROR ("类内部指令第 1 个不为 LOAD_NAME"); }
				else if	(v_child[1].m_cmd != "STORE_NAME")						{ RETURN_ERROR ("类内部指令第 2 个不为 STORE_NAME"); }
				else if	(v_child[2].m_cmd != "LOAD_CONST")						{ RETURN_ERROR ("类内部指令第 3 个不为 LOAD_CONST"); }
				else if	(v_child[3].m_cmd != "STORE_NAME")						{ RETURN_ERROR ("类内部指令第 4 个不为 STORE_NAME"); }
				else if	(v_child[v_child.size () - 2].m_cmd != "LOAD_CONST")	{ RETURN_ERROR ("类内部指令倒数第 2 个不为 LOAD_CONST"); }
				else if	(v_child[v_child.size () - 1].m_cmd != "RETURN_VALUE")	{ RETURN_ERROR ("类内部指令倒数第 1 个不为 RETURN_VALUE"); }
				v_child.erase (v_child.begin (), v_child.begin () + 4);
				v_child.erase (v_child.end () - 2, v_child.end ());
				//
				BitConverter _child_bc;
				if (!_child_bc.parse_items (v_child, _level + 1)) { RETURN_ERROR (""); }
				std::vector<obj_item_t> child_items = std::get<0> (_child_bc.m_items);
				for (obj_item_t &child_item : child_items) {
					if		(child_item.index () == 0)	{ cls.m_vStat.push_back (std::get<0> (child_item)); }
					else if	(child_item.index () == 1)	{ cls.m_vFunc.push_back (std::get<1> (child_item)); }
					else								{ RETURN_ERROR ("暂不支持类嵌套类"); }
				}
				m_v.push_back (cls);
				return 4;
			} else if (v[i].m_cmd == "LOAD_CONST" && v[i].m_val2.index () == 0) {
				// 函数创建指令
				if		(_expr.m_vOper1.size () || _expr.m_vOper2.size ())	{ RETURN_ERROR ("类创建指令后跟指令不可少于 3 个"); }
				else if	(v.size () <= i + 3)				{ RETURN_ERROR ("函数创建指令后跟指令不可少于 3 个"); }
				else if	(v[i + 1].m_cmd != "LOAD_CONST")	{ RETURN_ERROR ("函数创建指令后跟第 1 个指令不为 LOAD_CONST"); }
				else if	(v[i + 2].m_cmd != "MAKE_FUNCTION")	{ RETURN_ERROR ("函数创建指令后跟第 2 个指令不为 MAKE_FUNCTION"); }
				else if	(v[i + 3].m_cmd != "STORE_NAME")	{ RETURN_ERROR ("函数创建指令后跟第 3 个指令不为 STORE_NAME"); }
				obj_func_t func;
				func.m_level = _level;
				func.m_name = std::get<std::string> (v[i + 2].m_val2);
				func.m_name = func.m_name.substr (1, func.m_name.length () - 2);
				//
				std::vector<DisasmItem> &v_child = std::get<0> (v[i].m_val2);
				for (DisasmItem &_child_item : v_child) {
					if (_child_item.m_cmd == "LOAD_FAST") {
						std::string _tmp = std::get<1> (_child_item.m_val2);
						size_t j = 0;
						for (j = 0; j < func.m_vParam.size (); ++j) {
							if (func.m_vParam[j] == _tmp)
								break;
						}
						if (j == func.m_vParam.size ())
							func.m_vParam.push_back (_tmp);
					}
				}
				//
				BitConverter _child_bc;
				if (!_child_bc.parse_items (v_child, _level + 1)) { RETURN_ERROR (""); }
				std::vector<obj_item_t> child_items = std::get<0> (_child_bc.m_items);
				for (obj_item_t &child_item : child_items) {
					if	(child_item.index () == 0)	{ func.m_vStat.push_back (std::get<0> (child_item)); }
					else							{ RETURN_ERROR ("暂不支持函数嵌套类或函数"); }
				}
				m_v.push_back (func);
				return 4;
			} else if (v[i].m_cmd == "LOAD_CONST" || v[i].m_cmd == "LOAD_FAST" || v[i].m_cmd == "LOAD_NAME") {
				_expr.m_vOper2.push_back (std::get<1> (v[i].m_val2));
			} else if (v[i].m_cmd == "RETURN_VALUE") {
				if (_expr.m_vOper2.size () == 0) { RETURN_ERROR ("数据栈中没有数据，无法执行 RETURN_VALUE"); }
				_expr.m_operator = "return";
				m_v.push_back (obj_stat_t (_expr));
				return i - index + 1;
			} else if (v[i].m_cmd == "BINARY_ADD" || v[i].m_cmd == "BINARY_SUBTRACT" || v[i].m_cmd == "BINARY_MULTIPLY" || v[i].m_cmd == "BINARY_TRUE_DIVIDE" || v[i].m_cmd == "COMPARE_OP") {
				if (_expr.m_vOper2.size () < 2) { RETURN_ERROR ("数据栈中数据少于两条，无法执行算数运算"); }
				obj_expr_t _expr2;
				_expr2.m_vOper2.push_back (*_expr.m_vOper2.rbegin ());
				_expr.m_vOper2.pop_back ();
				if (v[i].m_cmd == "BINARY_ADD")					_expr2.m_operator = "+";
				else if (v[i].m_cmd == "BINARY_SUBTRACT")		_expr2.m_operator = "-";
				else if (v[i].m_cmd == "BINARY_MULTIPLY")		_expr2.m_operator = "*";
				else if (v[i].m_cmd == "BINARY_TRUE_DIVIDE")	_expr2.m_operator = "/";
				else if (v[i].m_cmd == "COMPARE_OP")			_expr2.m_operator = std::get<1> (v[i].m_val2);
				_expr2.m_vOper1.push_back (*_expr.m_vOper2.rbegin ());
				_expr.m_vOper2.pop_back ();
				_expr.m_vOper2.push_back (_expr2);
			} else if (v[i].m_cmd == "CALL_FUNCTION") {
				if (_expr.m_vOper2.size () < 1) { RETURN_ERROR ("数据栈中数据少于一条，无法执行 CALL_FUNCTION"); }
				_expr.m_vOper1.push_back (*_expr.m_vOper2.begin ());
				_expr.m_vOper2.erase (_expr.m_vOper2.begin ());
				_expr.m_operator = "call";
				obj_expr_t _expr2;
				_expr2.xchg (_expr);
				_expr.m_vOper2.push_back (_expr2);
			} else if (v[i].m_cmd == "STORE_NAME") {
				_expr.m_vOper1.push_back (std::get<1> (v[i].m_val2));
				_expr.m_operator = "=";
				m_v.push_back (obj_stat_t (_expr));
				return i - index + 1;
			} else if (v[i].m_cmd == "POP_TOP") {
				m_v.push_back (obj_stat_t (_expr));
				return i - index + 1;
			} else if (v[i].m_cmd == "POP_JUMP_IF_TRUE" || v[i].m_cmd == "POP_JUMP_IF_FALSE") {
				if (v[i].m_cmd == "POP_JUMP_IF_TRUE") {
					obj_expr_t _expr2;
					_expr2.xchg (_expr);
					_expr.m_operator = "!";
					_expr.m_vOper2.push_back (_expr2);
				}
				obj_if_t _if;
				_if.m_level = _level;
				_if.m_exprs.push_back (std::make_pair<obj_value_t, std::vector<obj_stat_t>> (_expr, std::vector<obj_stat_t> ()));
				std::vector<obj_stat_t> &_stats = _if.m_exprs.rbegin ()->second;
				size_t j = i + 1, k = 0;
				BitConverter _child_bc;
				for (; j < v.size () && v[j].m_offset < v[i].m_val; j += k) {
					size_t k = _child_bc.parse_item (v, j, _level + 1);
					if (!k) { RETURN_ERROR (""); }
				}
				std::vector<obj_item_t> child_items = std::get<0> (_child_bc.m_items);
				for (obj_item_t &child_item : child_items) {
					if (child_item.index () == 0) { _stats.push_back (std::get<0> (child_item)); }
					else { RETURN_ERROR ("暂不支持if语句嵌套类或函数"); }
				}
				return j - index;
			} else {
				RETURN_ERROR ("暂不支持的指令");
				RETURN_ERROR (v[i].m_cmd);
			}
		}
		RETURN_ERROR ("当前指令未形成完整的一句代码");
#undef RETURN_ERROR
	}

	// 反编译整段代码
	bool parse_items (std::vector<DisasmItem> &v, size_t _level) {
		size_t k = 0;
		for (size_t i = 0; i < v.size (); i += k) {
			k = parse_item (v, i, _level);
			if (!k) return false;
		}
		return true;
	}

	// 打印翻译后的代码
	void print_code () {
		auto padding = [](size_t padding) { std::string s = ""; while (padding-- > 0) s += "    "; return s; };
		std::vector<obj_item_t> &m_v = std::get<0> (m_items);
		for (obj_item_t &item : m_v) {
			if (item.index () == 0) {
				obj_stat_t &stat = std::get<0> (item);
				if (stat.index () == 0) {
					obj_expr_t &_expr = std::get<0> (stat);
				} else if (stat.index () == 1) {
					obj_if_t &_if = std::get<1> (stat);
				} else if (stat.index () == 2) {
					obj_for_t &_for = std::get<2> (stat);
				} else if (stat.index () == 3) {
					obj_while_t &_while = std::get<3> (stat);
				}
			} else if (item.index () == 1) {
				obj_func_t &func = std::get<1> (item);
			} else if (item.index () == 2) {
				obj_class_t &cls = std::get<2> (item);
				std::cout << padding (cls.m_level) << "class " << cls.m_name << '{' << std::endl;
				for (obj_stat_t &stat : cls.m_vStat) {
					//std::cout << padding (cls.m_level + 1) << 
				}
				std::cout << padding (cls.m_level) << "};" << std::endl;
			}
		}
	}
};



int main (int argc, char* argv[]) {
	// 读取文件
	std::ifstream ifs ("D:/fawdlstty/Desktop/py_disasm/a.txt");
	std::string s ((std::istreambuf_iterator<char> (ifs)), std::istreambuf_iterator<char> ());
	std::vector<DisasmItem> v = DisasmItem::parse (s);

	// 翻译指令
	if		(v[v.size () - 2].m_cmd == "LOAD_CONST")	{ std::cout << ("待翻译字节码倒数第二条指令不是 LOAD_CONST") << std::endl; }
	else if	(v[v.size () - 1].m_cmd == "RETURN_VALUE")	{ std::cout << ("待翻译字节码倒数第一条指令不是 RETURN_VALUE") << std::endl; }
	v.erase (v.end () - 2, v.end ());
	BitConverter parser;
	parser.parse_items (v, 0);
	parser.print_code ();

	std::cout << std::endl << std::endl;
	system ("pause");
	return 0;
}

