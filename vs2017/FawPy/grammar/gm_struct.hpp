#ifndef __GM_STRUCT_HPP__
#define __GM_STRUCT_HPP__



#include <vector>
#include <variant>
#include <utility>
#include "../hstring.hpp"
#include "ParseTypes.hpp"
#include "gm_expr.hpp"



struct obj_stat_t;

// if结构    vector<pair<obj_value_t, vector<obj_stat_t>>>
struct obj_if_t : std::vector<std::pair<obj_value_t, std::vector<obj_stat_t>>> {
	obj_if_t () : std::vector<std::pair<obj_value_t, std::vector<obj_stat_t>>> (std::vector<std::pair<obj_value_t, std::vector<obj_stat_t>>> ()) {}
	std::string to_str (size_t padding, std::string str_self);
};

// for结构    m_name  m_begin  m_end  m_step  m_vStat
struct obj_for_t {
	obj_for_t () : m_begin (nullptr), m_end (nullptr), m_step (nullptr) {}
	std::string					m_name;
	obj_value_t					m_begin;
	obj_value_t					m_end;
	obj_value_t					m_step;
	std::vector<obj_stat_t>		m_vStat;
	std::string to_str (size_t padding, std::string str_self);
};

// while结构    m_expr  m_vStat
struct obj_while_t {
	obj_value_t					m_expr;
	std::vector<obj_stat_t>		m_vStat;
	std::string to_str (size_t padding, std::string str_self);
};

// 语句结构    variant<obj_expr_t, obj_if_t, obj_for_t, obj_while_t>
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



#endif //__GM_STRUCT_HPP__
