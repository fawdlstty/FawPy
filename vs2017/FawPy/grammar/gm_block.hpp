#ifndef __GM_BLOCK_HPP__
#define __GM_BLOCK_HPP__



#include <string>
#include <vector>
#include "../hstring.hpp"
#include "gm_struct.hpp"
#include "ParseTypes.hpp"



// 函数结构    m_name  m_vParam  m_vStat
struct obj_func_t {
	std::string					m_name;
	std::vector<std::string>	m_vParam;
	std::vector<obj_stat_t>		m_vStat;
	std::string to_str (ParseTypes &pt, size_t padding, bool rm_self = false) {
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
			s += stat.to_str (pt, padding + 1, str_self);
		s += hStringA::format ("%s}\n\n", hStringA::get_padding (padding).c_str ());
		pt.clear_var ();
		return s;
	}
};

// 类结构    m_name  m_vStat  m_vFunc
struct obj_class_t {
	std::string					m_name;
	std::vector<obj_stat_t>		m_vStat;
	std::vector<obj_func_t>		m_vFunc;
	std::string to_str (ParseTypes &pt, size_t padding) {
		std::string s = hStringA::format ("%sstruct %s {\n", hStringA::get_padding (padding).c_str (), m_name.c_str ());
		for (obj_stat_t &stat : m_vStat)
			s += stat.to_str (pt, padding + 1, "");
		for (obj_func_t &func : m_vFunc)
			s += func.to_str (pt, padding + 1, true);
		if (m_vFunc.size () > 0)
			s.erase (s.end () - 1);
		s += hStringA::format ("%s};\n\n", hStringA::get_padding (padding).c_str ());
		return s;
	}
};



#endif //__GM_BLOCK_HPP__
