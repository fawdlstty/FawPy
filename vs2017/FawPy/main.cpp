#include <iostream>
#include <fstream>
#include <string>
#include <variant>
#include <vector>
#include "hstring.hpp"



// �ֽ���ָ��ṹ
struct DisasmItem {
	size_t																m_level = 0;
	size_t																m_offset = 0;
	std::string															m_cmd = "";
	size_t																m_val = 0;
	std::variant<std::vector<DisasmItem>, std::string, int, nullptr_t>	m_val2;

	// ����ָ��ṹ
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
				}
				else if (ch == '<') {
					m_val2 = std::vector<DisasmItem> ();
				}
				else {
					m_val2 = &s[33];
				}
			}
			else {
				m_val2 = nullptr;
			}
		}
	}

	// ����ָ��ṹ����
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



// obj_expr_t  obj_value_t  obj_stat_t  obj_if_t  obj_for_t  obj_while_t  obj_func_t  obj_class_t  obj_item_t  obj_items_t
#pragma region �﷨�ṹ�嶨��
struct obj_value_t;

// ���ṹ    m_vOper1  m_operator  m_vOper2
struct obj_expr_t {
	std::vector<obj_value_t>	m_vOper1;
	std::string					m_operator;
	std::vector<obj_value_t>	m_vOper2;
	void init () { m_vOper1.clear (); m_operator.clear (); m_vOper2.clear (); }
	bool empty () { return (!(m_vOper1.size () || m_vOper2.size ())); }
	void xchg (obj_expr_t &o) { m_operator.swap (o.m_operator); m_vOper1.swap (o.m_vOper1); m_vOper2.swap (o.m_vOper2); }
	std::string to_str (std::string str_self);
};

// ֵ�ṹ
struct obj_value_t : public std::variant<obj_expr_t, std::string, int, nullptr_t> {
	obj_value_t (obj_expr_t o)	: std::variant<obj_expr_t, std::string, int, nullptr_t> (o) {}
	obj_value_t (std::string o)	: std::variant<obj_expr_t, std::string, int, nullptr_t> (o) {}
	obj_value_t (int o)			: std::variant<obj_expr_t, std::string, int, nullptr_t> (o) {}
	obj_value_t (nullptr_t o)	: std::variant<obj_expr_t, std::string, int, nullptr_t> (o) {}
	std::string to_str (std::string str_self) {
		if (index () == 1) {
			std::string s = std::get<1> (*this);
			if (s == str_self)
				s = "this";
			else if (!s.empty () && s[0] == '\'')
				s[0] = s[s.size () - 1] = '"';
			return s;
		} else if (index () == 2) {
			return hStringA::format ("%d", std::get<2> (*this));
		} else {
			obj_expr_t &_expr = std::get<0> (*this);
			if (_expr.m_operator == "call" || _expr.m_operator == "->")
				return _expr.to_str (str_self);
			return hStringA::format ("(%s)", _expr.to_str (str_self).c_str ());
		}
	}
};

std::string obj_expr_t::to_str (std::string str_self) {
	std::string s;
	if (m_operator == "call" && m_vOper1.size () == 1 && m_vOper1[0].index () == 1 && std::get<1> (m_vOper1[0]) == "print") {
		s += "std::cout << ";
		for (obj_value_t &val : m_vOper2) {
			s += val.to_str (str_self);
			s += " << ";
		}
		s += "std::endl";
		return s;
	}
	if (m_vOper1.size () > 1) s += "(";
	for (size_t i = 0; i < m_vOper1.size (); ++i) {
		if (i > 0) s += ", ";
		s += m_vOper1[i].to_str (str_self);
	}
	if (m_vOper1.size () > 1) s += ")";
	//
	if (m_operator == "+" || m_operator == "-" || m_operator == "*" || m_operator == "/") {
		s += m_operator;
	} else if (m_operator == "==" || m_operator == ">" || m_operator == "<" || m_operator == ">=" || m_operator == "<=" || m_operator == "!=" || m_operator == "=") {
		s += ' ';
		s += m_operator;
		s += ' ';
	} else if (m_operator == "return") {
		s += m_operator;
		s += ' ';
	} else if (m_operator == "call") {
		s += ' ';
	} else if (m_operator == "->") {
		s += '.';
	}
	//
	if (m_vOper2.size () > 1 || m_operator == "call") s += "(";
	for (size_t i = 0; i < m_vOper2.size (); ++i) {
		if (i > 0) s += ", ";
		s += m_vOper2[i].to_str (str_self);
	}
	if (m_vOper2.size () > 1 || m_operator == "call") s += ")";
	return s;
}

struct obj_if_t;
struct obj_for_t;
struct obj_while_t;
struct obj_stat_t;

// if�ṹ    vector<pair<obj_value_t, vector<obj_stat_t>>>
struct obj_if_t : std::vector<std::pair<obj_value_t, std::vector<obj_stat_t>>> {
	obj_if_t () : std::vector<std::pair<obj_value_t, std::vector<obj_stat_t>>> (std::vector<std::pair<obj_value_t, std::vector<obj_stat_t>>> ()) {}
	std::string to_str (size_t padding, std::string str_self);
};

// for�ṹ    m_name  m_begin  m_end  m_step  m_vStat
struct obj_for_t {
	obj_for_t () : m_begin (nullptr), m_end (nullptr), m_step (nullptr) {}
	std::string					m_name;
	obj_value_t					m_begin;
	obj_value_t					m_end;
	obj_value_t					m_step;
	std::vector<obj_stat_t>		m_vStat;
	std::string to_str (size_t padding, std::string str_self);
};

// while�ṹ    m_expr  m_vStat
struct obj_while_t {
	obj_value_t					m_expr;
	std::vector<obj_stat_t>		m_vStat;
	std::string to_str (size_t padding, std::string str_self);
};

// ���ṹ    variant<obj_expr_t, obj_if_t, obj_for_t, obj_while_t>
struct obj_stat_t : public std::variant<obj_expr_t, obj_if_t, obj_for_t, obj_while_t, nullptr_t> {
	obj_stat_t (obj_expr_t o)	: std::variant<obj_expr_t, obj_if_t, obj_for_t, obj_while_t, nullptr_t> (o) {}
	obj_stat_t (obj_if_t o)		: std::variant<obj_expr_t, obj_if_t, obj_for_t, obj_while_t, nullptr_t> (o) {}
	obj_stat_t (obj_for_t o)	: std::variant<obj_expr_t, obj_if_t, obj_for_t, obj_while_t, nullptr_t> (o) {}
	obj_stat_t (obj_while_t o)	: std::variant<obj_expr_t, obj_if_t, obj_for_t, obj_while_t, nullptr_t> (o) {}
	obj_stat_t (nullptr_t o)	: std::variant<obj_expr_t, obj_if_t, obj_for_t, obj_while_t, nullptr_t> (o) {}
	obj_stat_t ()				: std::variant<obj_expr_t, obj_if_t, obj_for_t, obj_while_t, nullptr_t> (nullptr) {}
	std::string to_str (size_t padding, std::string str_self) {
		if (index () == 0) {
			return hStringA::format ("%s%s;\n", hStringA::get_padding (padding).c_str (), std::get<0> (*this).to_str (str_self).c_str ());
		} else if (index () == 1) {
			return std::get<1> (*this).to_str (padding, str_self);
		} else if (index () == 2) {
			return std::get<2> (*this).to_str (padding, str_self);
		} else if (index () == 3) {
			return std::get<3> (*this).to_str (padding, str_self);
		}
		return "";
	}
};

std::string obj_if_t::to_str (size_t padding, std::string str_self) {
	std::string s = "";
	for (std::pair<obj_value_t, std::vector<obj_stat_t>> &p : *this) {
		s += hStringA::get_padding (padding);
		if (!s.empty ())
			s += "else ";
		std::string tmp = p.first.to_str (str_self);
		if (!tmp.empty ()) {
			s += "if (";
			if (!tmp.empty () && tmp[0] == '(')
				tmp = tmp.substr (1, tmp.size () - 2);
			s += tmp;
			s += ") {\n";
		} else {
			s += "{";
		}
		for (obj_stat_t &stat : p.second)
			s += stat.to_str (padding + 1, str_self);
		s += hStringA::format ("%s}\n", hStringA::get_padding (padding).c_str ());
	}
	return s;
}
std::string obj_for_t::to_str (size_t padding, std::string str_self) {
	std::string s = hStringA::format ("%sfor (auto %s = %s, %s != %s; %s += %s) {\n", hStringA::get_padding (padding).c_str (),
		m_name.c_str (), m_begin.to_str (str_self).c_str (),
		m_name.c_str (), m_end.to_str (str_self).c_str (),
		m_name.c_str (), m_step.to_str (str_self).c_str ());
	for (obj_stat_t &stat : m_vStat)
		s += stat.to_str (padding + 1, str_self);
	s += hStringA::format ("%s}\n", hStringA::get_padding (padding).c_str ());
	return s;
}
std::string obj_while_t::to_str (size_t padding, std::string str_self) {
	std::string s = hStringA::format ("%swhile %s {\n", hStringA::get_padding (padding).c_str (), m_expr.to_str (str_self).c_str ());
	for (obj_stat_t &stat : m_vStat)
		s += stat.to_str (padding + 1, str_self);
	s += hStringA::format ("%s}\n", hStringA::get_padding (padding).c_str ());
	return s;
}

// �����ṹ    m_name  m_vParam  m_vStat
struct obj_func_t {
	std::string					m_name;
	std::vector<std::string>	m_vParam;
	std::vector<obj_stat_t>		m_vStat;
	std::string to_str (size_t padding, bool rm_self = false) {
		std::string s = hStringA::format ("%sauto %s (", hStringA::get_padding (padding).c_str (), m_name.c_str ());
		std::string str_self;
		if (rm_self) {
			str_self = m_vParam[0];
			m_vParam.erase (m_vParam.begin ());
		}
		for (std::string &sp : m_vParam) {
			if ('(' != *s.crbegin ())
				s += ", ";
			s += "int ";
			s += sp;
		}
		s += ") {\n";
		for (obj_stat_t &stat : m_vStat)
			s += stat.to_str (padding + 1, str_self);
		s += hStringA::format ("%s}\n\n", hStringA::get_padding (padding).c_str ());
		return s;
	}
};

// ��ṹ    m_name  m_vStat  m_vFunc
struct obj_class_t {
	std::string					m_name;
	std::vector<obj_stat_t>		m_vStat;
	std::vector<obj_func_t>		m_vFunc;
	std::string to_str (size_t padding) {
		std::string s = hStringA::format ("%sclass %s {\n", hStringA::get_padding (padding).c_str (), m_name.c_str ());
		for (obj_stat_t &stat : m_vStat)
			s += stat.to_str (padding + 1, "");
		for (obj_func_t &func : m_vFunc)
			s += func.to_str (padding + 1, true);
		if (m_vFunc.size () > 0)
			s.erase (s.end () - 1);
		s += hStringA::format ("%s};\n\n", hStringA::get_padding (padding).c_str ());
		return s;
	}
};

// ������������һ�� obj_expr_t    variant<obj_stat_t, obj_func_t, obj_class_t, nullptr_t>
struct obj_item_t : public std::variant<obj_stat_t, obj_func_t, obj_class_t, nullptr_t> {
	obj_item_t (obj_stat_t o)	: std::variant<obj_stat_t, obj_func_t, obj_class_t, nullptr_t> (o) {}
	obj_item_t (obj_func_t o)	: std::variant<obj_stat_t, obj_func_t, obj_class_t, nullptr_t> (o) {}
	obj_item_t (obj_class_t o)	: std::variant<obj_stat_t, obj_func_t, obj_class_t, nullptr_t> (o) {}
	obj_item_t (nullptr_t o)	: std::variant<obj_stat_t, obj_func_t, obj_class_t, nullptr_t> (o) {}
	obj_item_t ()				: std::variant<obj_stat_t, obj_func_t, obj_class_t, nullptr_t> (nullptr) {}
	std::string to_str (size_t padding) {
		if (index () == 0) {
			return std::get<0> (*this).to_str (padding, "");
		} else if (index () == 1) {
			return std::get<1> (*this).to_str (padding);
		} else if (index () == 2) {
			return std::get<2> (*this).to_str (padding);
		}
		return "";
	}
};

// ������������һ��
struct obj_items_t : public std::variant<std::vector<obj_item_t>, nullptr_t> {
	obj_items_t (std::vector<obj_item_t> o)	: std::variant<std::vector<obj_item_t>, nullptr_t> (o) {}
	obj_items_t (nullptr_t o)				: std::variant<std::vector<obj_item_t>, nullptr_t> (o) {}
	std::string to_str (size_t padding) {
		if (index () == 1)
			return "parse error";
		std::string s;
		for (obj_item_t &item : std::get<0> (*this))
			s += item.to_str (padding);
		return s;
	}
};
#pragma endregion



// �ֽ��������
struct BitConverter {
	obj_items_t m_items;
	BitConverter () : m_items (std::vector<obj_item_t> ()) {}

	// ������һ�δ���
	size_t parse_item (std::vector<DisasmItem> &v, size_t index) {
#define RETURN_ERROR(x) { std::cout << "line " << i << ": " << (x) << std::endl; return 0; }
		std::vector<obj_item_t> &m_v = std::get<0> (m_items);
		obj_expr_t _expr;
		// ���ڿ���һ�δ����ɢ�ڶ��У����Բ���һ��������ֱ��������һ����������ʱ����
		size_t i = index;
		for (; i < v.size (); ++i) {
			if (v[i].m_cmd == "LOAD_BUILD_CLASS") {
				// �ഴ��ָ��
				if (_expr.m_vOper1.size () || _expr.m_vOper2.size ())	{ RETURN_ERROR ("�ഴ��ʱ����ջ�����д���������"); }
				else if (v.size () <= i + 6)							{ RETURN_ERROR ("�ഴ��ָ����ָ������� 3 ��"); }
				else if (v[i + 1].m_cmd != "LOAD_CONST")				{ RETURN_ERROR ("�ഴ��ָ������ 1 ��ָ�Ϊ LOAD_CONST"); }
				else if (v[i + 1].m_val2.index () != 0)					{ RETURN_ERROR ("�ഴ��ָ������ 1 ��ָ�Ϊ LOAD_CONST �������"); }
				else if (v[i + 2].m_cmd != "LOAD_CONST")				{ RETURN_ERROR ("�ഴ��ָ������ 2 ��ָ�Ϊ LOAD_CONST"); }
				else if (v[i + 3].m_cmd != "MAKE_FUNCTION")				{ RETURN_ERROR ("�ഴ��ָ������ 3 ��ָ�Ϊ MAKE_FUNCTION"); }
				else if (v[i + 4].m_cmd != "LOAD_CONST")				{ RETURN_ERROR ("�ഴ��ָ������ 4 ��ָ�Ϊ LOAD_CONST"); }
				else if (v[i + 5].m_cmd != "CALL_FUNCTION")				{ RETURN_ERROR ("�ഴ��ָ������ 5 ��ָ�Ϊ CALL_FUNCTION"); }
				else if (v[i + 6].m_cmd != "STORE_NAME")				{ RETURN_ERROR ("�ഴ��ָ������ 6 ��ָ�Ϊ STORE_NAME"); }
				obj_class_t cls;
				cls.m_name = std::get<std::string> (v[i + 2].m_val2);
				cls.m_name = cls.m_name.substr (1, cls.m_name.length () - 2);
				//
				std::vector<DisasmItem> &v_child = std::get<0> (v[i + 1].m_val2);
				if (v_child.size () < 6)										{ RETURN_ERROR ("���ڲ�ָ������� 6 ��"); }
				else if (v_child[0].m_cmd != "LOAD_NAME")						{ RETURN_ERROR ("���ڲ�ָ��� 1 ����Ϊ LOAD_NAME"); }
				else if (v_child[1].m_cmd != "STORE_NAME")						{ RETURN_ERROR ("���ڲ�ָ��� 2 ����Ϊ STORE_NAME"); }
				else if (v_child[2].m_cmd != "LOAD_CONST")						{ RETURN_ERROR ("���ڲ�ָ��� 3 ����Ϊ LOAD_CONST"); }
				else if (v_child[3].m_cmd != "STORE_NAME")						{ RETURN_ERROR ("���ڲ�ָ��� 4 ����Ϊ STORE_NAME"); }
				else if (v_child[v_child.size () - 2].m_cmd != "LOAD_CONST")	{ RETURN_ERROR ("���ڲ�ָ����� 2 ����Ϊ LOAD_CONST"); }
				else if (v_child[v_child.size () - 1].m_cmd != "RETURN_VALUE")	{ RETURN_ERROR ("���ڲ�ָ����� 1 ����Ϊ RETURN_VALUE"); }
				v_child.erase (v_child.begin (), v_child.begin () + 4);
				v_child.erase (v_child.end () - 2, v_child.end ());
				//
				BitConverter _child_bc;
				if (!_child_bc.parse_items (v_child)) { RETURN_ERROR (""); }
				std::vector<obj_item_t> child_items = std::get<0> (_child_bc.m_items);
				for (obj_item_t &child_item : child_items) {
					if (child_item.index () == 0) { cls.m_vStat.push_back (std::get<0> (child_item)); }
					else if (child_item.index () == 1) { cls.m_vFunc.push_back (std::get<1> (child_item)); }
					else { RETURN_ERROR ("�ݲ�֧����Ƕ����"); }
				}
				m_v.push_back (cls);
				return 7;
			} else if (v[i].m_cmd == "LOAD_CONST" && v[i].m_val2.index () == 0) {
				// ��������ָ��
				if (_expr.m_vOper1.size () || _expr.m_vOper2.size ())	{ RETURN_ERROR ("��������ʱ����ջ�����д���������"); }
				else if (v.size () <= i + 3)							{ RETURN_ERROR ("��������ָ����ָ������� 3 ��"); }
				else if (v[i + 1].m_cmd != "LOAD_CONST")				{ RETURN_ERROR ("��������ָ������ 1 ��ָ�Ϊ LOAD_CONST"); }
				else if (v[i + 2].m_cmd != "MAKE_FUNCTION")				{ RETURN_ERROR ("��������ָ������ 2 ��ָ�Ϊ MAKE_FUNCTION"); }
				else if (v[i + 3].m_cmd != "STORE_NAME")				{ RETURN_ERROR ("��������ָ������ 3 ��ָ�Ϊ STORE_NAME"); }
				obj_func_t func;
				func.m_name = std::get<1> (v[i + 3].m_val2);
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
				if (!_child_bc.parse_items (v_child)) { RETURN_ERROR (""); }
				std::vector<obj_item_t> child_items = std::get<0> (_child_bc.m_items);
				for (obj_item_t &child_item : child_items) {
					if (child_item.index () == 0) { func.m_vStat.push_back (std::get<0> (child_item)); }
					else { RETURN_ERROR ("�ݲ�֧�ֺ���Ƕ�������"); }
				}
				m_v.push_back (func);
				return 4;
			} else if (v[i].m_cmd == "LOAD_CONST" || v[i].m_cmd == "LOAD_FAST" || v[i].m_cmd == "LOAD_NAME") {
				if (v[i].m_val2.index () == 1)
					_expr.m_vOper2.push_back (std::get<1> (v[i].m_val2));
				else if (v[i].m_val2.index () == 2)
					_expr.m_vOper2.push_back (std::get<2> (v[i].m_val2));
			} else if (v[i].m_cmd == "RETURN_VALUE") {
				if (_expr.m_vOper2.size () == 0) { RETURN_ERROR ("����ջ��û�����ݣ��޷�ִ�� RETURN_VALUE"); }
				_expr.m_operator = "return";
				obj_stat_t tmp_stat;
				tmp_stat = _expr;
				obj_item_t tmp_item;
				tmp_item = tmp_stat;
				m_v.push_back (tmp_item);
				return i - index + 1;
			} else if (v[i].m_cmd == "BINARY_ADD" || v[i].m_cmd == "BINARY_SUBTRACT" || v[i].m_cmd == "BINARY_MULTIPLY" || v[i].m_cmd == "BINARY_TRUE_DIVIDE" || v[i].m_cmd == "COMPARE_OP") {
				if (_expr.m_vOper2.size () < 2) { RETURN_ERROR ("����ջ�����������������޷�ִ����������"); }
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
			} else if (v[i].m_cmd == "LOAD_ATTR") {
				if (_expr.m_vOper2.size () < 1) { RETURN_ERROR ("����ջ����������һ�����޷�ִ�� CALL_FUNCTION"); }
				_expr.m_vOper1.push_back (*_expr.m_vOper2.begin ());
				_expr.m_vOper2.erase (_expr.m_vOper2.begin ());
				_expr.m_vOper2.push_back (std::get<1> (v[i].m_val2));
				_expr.m_operator = "->";
				obj_expr_t _expr2;
				_expr2.xchg (_expr);
				_expr.m_vOper2.push_back (_expr2);
			} else if (v[i].m_cmd == "CALL_FUNCTION") {
				if (_expr.m_vOper2.size () < 1) { RETURN_ERROR ("����ջ����������һ�����޷�ִ�� CALL_FUNCTION"); }
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
				_if.push_back (std::make_pair<obj_value_t, std::vector<obj_stat_t>> (std::move (_expr.m_vOper2[0]), std::vector<obj_stat_t> ()));
				_expr.m_vOper2.erase (_expr.m_vOper2.begin ());
				size_t j = i + 1, k = 0;
				BitConverter _child_bc;
				size_t sz_end = v[i].m_val;
				for (; j < v.size () && v[j].m_offset < sz_end; j += k) {
					k = _child_bc.parse_item (v, j);
					if (!k) { RETURN_ERROR (""); }
				}
				std::vector<obj_item_t> child_items = std::get<0> (_child_bc.m_items);
				for (obj_item_t &child_item : child_items) {
					if (child_item.index () == 0) { _if.rbegin ()->second.push_back (std::get<0> (child_item)); }
					else { RETURN_ERROR ("�ݲ�֧��if���Ƕ�������"); }
				}
				m_v.push_back (obj_stat_t (_if));
				return j - index;
			} else if (v[i].m_cmd == "SETUP_LOOP") {
				// ѭ������ָ��
				// ���ֻ���� for ѭ��
				if (_expr.m_vOper1.size () || _expr.m_vOper2.size ())	{ RETURN_ERROR ("ѭ������ʱ����ջ�����д���������"); }
				else if (v.size () <= i + 9)							{ RETURN_ERROR ("ѭ������ָ����ָ������� 7 ��"); }
				else if (v[i + 1].m_cmd != "LOAD_NAME")					{ RETURN_ERROR ("ѭ������ָ������ 1 ��ָ�Ϊ LOAD_NAME"); }
				else if (std::get<1> (v[i + 1].m_val2) != "range")		{ RETURN_ERROR ("ѭ������ָ������ 1 ��ָ�� LOAD_NAME ��Ϊ range"); }
				else if (v[i + 2].m_cmd != "LOAD_CONST")				{ RETURN_ERROR ("ѭ������ָ������ 2 ��ָ�Ϊ LOAD_CONST"); }
				else if (v[i + 2].m_val2.index () != 2)					{ RETURN_ERROR ("ѭ������ָ������ 2 ��ָ�� LOAD_CONST ������"); }
				else if (v[i + 3].m_cmd != "LOAD_CONST")				{ RETURN_ERROR ("ѭ������ָ������ 3 ��ָ�Ϊ LOAD_CONST"); }
				else if (v[i + 3].m_val2.index () != 2)					{ RETURN_ERROR ("ѭ������ָ������ 3 ��ָ�� LOAD_CONST ������"); }
				else if (v[i + 4].m_cmd != "CALL_FUNCTION")				{ RETURN_ERROR ("ѭ������ָ������ 4 ��ָ�Ϊ CALL_FUNCTION"); }
				else if (v[i + 5].m_cmd != "GET_ITER")					{ RETURN_ERROR ("ѭ������ָ������ 5 ��ָ�Ϊ GET_ITER"); }
				else if (v[i + 6].m_cmd != "FOR_ITER")					{ RETURN_ERROR ("ѭ������ָ������ 6 ��ָ�Ϊ FOR_ITER"); }
				else if (v[i + 7].m_cmd != "STORE_NAME")				{ RETURN_ERROR ("ѭ������ָ������ 7 ��ָ�Ϊ STORE_NAME"); }
				obj_for_t _for;
				_for.m_name = std::get<1> (v[i + 7].m_val2);
				_for.m_begin = std::get<2> (v[i + 2].m_val2);
				_for.m_end = std::get<2> (v[i + 3].m_val2);
				_for.m_step = 1;
				size_t j = i + 8, k = 0;
				BitConverter _child_bc;
				size_t sz_end = atoi (&std::get<1> (v[i].m_val2).c_str ()[3]) - 4;
				for (; j < v.size () && v[j].m_offset < sz_end; j += k) {
					k = _child_bc.parse_item (v, j);
					if (!k) { RETURN_ERROR (""); }
				}
				std::vector<obj_item_t> child_items = std::get<0> (_child_bc.m_items);
				for (obj_item_t &child_item : child_items) {
					if (child_item.index () == 0) { _for.m_vStat.push_back (std::get<0> (child_item)); }
					else { RETURN_ERROR ("�ݲ�֧��if���Ƕ�������"); }
				}
				m_v.push_back (obj_stat_t (_for));
				if		(v[j].m_cmd != "JUMP_ABSOLUTE")					{ RETURN_ERROR ("ѭ������������ 2 ��ָ�Ϊ JUMP_ABSOLUTE"); }
				else if	(v[j + 1].m_cmd != "POP_BLOCK")					{ RETURN_ERROR ("ѭ������������ 1 ��ָ�Ϊ POP_BLOCK"); }
				return j - index + 2;
			} else {
				RETURN_ERROR ("�ݲ�֧�ֵ�ָ��: " + v[i].m_cmd);
			}
		}
		RETURN_ERROR ("��ǰָ��δ�γ�������һ�����");
#undef RETURN_ERROR
	}

	// ���������δ���
	bool parse_items (std::vector<DisasmItem> &v) {
		size_t k = 0;
		for (size_t i = 0; i < v.size (); i += k) {
			k = parse_item (v, i);
			if (!k) return false;
		}
		return true;
	}

	// ��ӡ�����Ĵ���
	void print_code () {
		auto padding = [](size_t padding) { std::string s = ""; while (padding-- > 0) s += "    "; return s; };
		std::cout << "#include <iostream>" << std::endl;
		std::cout << "#include <string>" << std::endl << std::endl;
		std::string s = m_items.to_str (0);
		size_t p = s.find ("if (__name__ == \"__main__\") {");
		if (p != std::string::npos && s[p - 1] == '\n') {
			hStringA::replace (s, std::string ("if (__name__ == \"__main__\") {"), std::string ("int main (int argc, char* argv[]) {"));
		}
		std::cout << s << std::endl;
	}
};



int main (int argc, char* argv[]) {
	// ��ȡ�ļ�
	std::ifstream ifs ("D:/GitHub/FawPy/disasm1.txt");
	std::string s ((std::istreambuf_iterator<char> (ifs)), std::istreambuf_iterator<char> ());
	std::vector<DisasmItem> v = DisasmItem::parse (s);

	// ����ָ��
	if (v.size () < 2) {
		std::cout << ("δ�ҵ��ļ� disasm.txt ���ļ�Ϊ��") << std::endl;
		system ("pause");
		return 0;
	}
	if (v[v.size () - 2].m_cmd != "LOAD_CONST" || v[v.size () - 1].m_cmd != "RETURN_VALUE") {
		if (v[v.size () - 2].m_cmd != "LOAD_CONST")
			std::cout << ("�������ֽ��뵹���ڶ���ָ��� LOAD_CONST") << std::endl;
		else
			std::cout << ("�������ֽ��뵹����һ��ָ��� RETURN_VALUE") << std::endl;
		system ("pause");
		return 0;
	}
	v.erase (v.end () - 2, v.end ());
	BitConverter parser;
	parser.parse_items (v);
	parser.print_code ();

	system ("pause");
	return 0;
}
