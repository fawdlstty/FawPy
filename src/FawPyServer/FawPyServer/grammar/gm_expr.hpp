#ifndef __GM_EXPR_HPP__
#define __GM_EXPR_HPP__



#include <string>
#include <vector>
#include <variant>
#include "hstring.hpp"
#include "ParseTypes.hpp"



struct obj_value_t;

// 语句结构    m_vOper1  m_operator  m_vOper2
struct obj_expr_t {
	std::vector<obj_value_t>	m_vOper1;
	std::string					m_operator;
	std::vector<obj_value_t>	m_vOper2;
	void init () { m_vOper1.clear (); m_operator.clear (); m_vOper2.clear (); }
	bool empty () { return (!(m_vOper1.size () || m_vOper2.size ())); }
	void xchg (obj_expr_t &o) { m_operator.swap (o.m_operator); m_vOper1.swap (o.m_vOper1); m_vOper2.swap (o.m_vOper2); }
	std::string to_str (ParseTypes &pt, std::string str_self);
};

// 值结构
struct obj_value_t : public std::variant<obj_expr_t, std::string, double, int, nullptr_t> {
	obj_value_t (obj_expr_t o)	: std::variant<obj_expr_t, std::string, double, int, nullptr_t> (o) {}
	obj_value_t (std::string o)	: std::variant<obj_expr_t, std::string, double, int, nullptr_t> (o) {}
	obj_value_t (double o)		: std::variant<obj_expr_t, std::string, double, int, nullptr_t> (o) {}
	obj_value_t (int o)			: std::variant<obj_expr_t, std::string, double, int, nullptr_t> (o) {}
	obj_value_t (nullptr_t o)	: std::variant<obj_expr_t, std::string, double, int, nullptr_t> (o) {}
	std::string to_str (ParseTypes &pt, std::string str_self) {
		if (index () == 0) {
			obj_expr_t &_expr = std::get<0> (*this);
			if (_expr.m_operator == "call" || _expr.m_operator == "->")
				return _expr.to_str (pt, str_self);
			return hStringA::format ("(%s)", _expr.to_str (pt, str_self).c_str ());
		} else if (index () == 1) {
			std::string s = std::get<1> (*this);
			if (s == str_self)
				s = "this";
			else if (!s.empty () && s[0] == '\'')
				s[0] = s[s.size () - 1] = '"';
			return s;
		} else if (index () == 2) {
			return hStringA::format ("%lf", std::get<2> (*this));
		} else if (index () == 3) {
			return hStringA::format ("%d", std::get<3> (*this));
		}
		return "";
	}
};

std::string obj_expr_t::to_str (ParseTypes &pt, std::string str_self) {
	std::string s;
	// 特殊情况1：print 转 std::cout
	if (m_operator == "call" && m_vOper1.size () == 1 && m_vOper1[0].index () == 1 && std::get<1> (m_vOper1[0]) == "print") {
		s += "std::cout << ";
		for (auto iter = m_vOper2.begin (); iter != m_vOper2.end (); ++iter) {
			if (iter != m_vOper2.begin ())
				s += "' ' << ";
			s += iter->to_str (pt, str_self);
			s += " << ";
		}
		s += "std::endl";
		return s;
	}
	// 特殊情况2：赋值时先确定是否有这个变量
	if (m_operator == "=" && m_vOper1.size () == 1 && m_vOper1[0].index () == 1) {
		std::string varName = std::get<1> (m_vOper1[0]);
		// 没有这个变量
		if (pt.get_var (varName) == pt.end ()) {
			s = "auto ";
		}
	}

	if (m_vOper1.size () > 1) s += "(";
	for (size_t i = 0; i < m_vOper1.size (); ++i) {
		if (i > 0) s += ", ";
		s += m_vOper1[i].to_str (pt, str_self);
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
		s += m_vOper2[i].to_str (pt, str_self);
	}
	if (m_vOper2.size () > 1 || m_operator == "call") s += ")";
	return s;
}



#endif //__GM_EXPR_HPP__
