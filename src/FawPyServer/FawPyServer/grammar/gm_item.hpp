#ifndef __GM_ITEM_HPP__
#define __GM_ITEM_HPP__



#include <string>
#include <vector>
#include <variant>
#include "hstring.hpp"
#include "ParseTypes.hpp"
#include "gm_struct.hpp"
#include "gm_block.hpp"



// ������������һ�� obj_expr_t    variant<obj_stat_t, obj_func_t, obj_class_t, nullptr_t>
struct obj_item_t : public std::variant<obj_stat_t, obj_func_t, obj_class_t, nullptr_t> {
	obj_item_t (obj_stat_t o)	: std::variant<obj_stat_t, obj_func_t, obj_class_t, nullptr_t> (o) {}
	obj_item_t (obj_func_t o)	: std::variant<obj_stat_t, obj_func_t, obj_class_t, nullptr_t> (o) {}
	obj_item_t (obj_class_t o)	: std::variant<obj_stat_t, obj_func_t, obj_class_t, nullptr_t> (o) {}
	obj_item_t (nullptr_t o)	: std::variant<obj_stat_t, obj_func_t, obj_class_t, nullptr_t> (o) {}
	obj_item_t () : std::variant<obj_stat_t, obj_func_t, obj_class_t, nullptr_t> (nullptr) {}

	// �������룬������ࡢ������ȫ�ֱ����������Է����������ParseTypes
	void parse_type (ParseTypes &pt) {
		if (index () == 0) {
			obj_stat_t &stat = std::get<0> (*this);
			pt.add_type (ParseType::ParseTypeGlobal, stat.get_setval_varname (pt), "");
		} else if (index () == 1) {
			obj_func_t &func = std::get<1> (*this);
			pt.add_type (ParseType::ParseTypeFunction, func.m_name, "");
		} else if (index () == 2) {
			obj_class_t &cls = std::get<2> (*this);
			pt.add_type (ParseType::ParseTypeClass, cls.m_name, "");
			for (obj_stat_t &stat : cls.m_vStat)
				pt.add_type (ParseType::ParseTypeClassAttr, stat.get_setval_varname (pt), "");
			for (obj_func_t &func : cls.m_vFunc)
				pt.add_type (ParseType::ParseTypeClassMethod, func.m_name, "");
		}
	}
	std::string to_str (ParseTypes &pt, size_t padding) {
		if (index () == 0) {
			return std::get<0> (*this).to_str (pt, padding, "");
		} else if (index () == 1) {
			return std::get<1> (*this).to_str (pt, padding);
		} else if (index () == 2) {
			return std::get<2> (*this).to_str (pt, padding);
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
		ParseTypes pt;

		// ���������������������
		std::vector<obj_item_t> &items = std::get<0> (*this);
		for (auto iter = items.begin (), iterEnd = items.end (); iter != iterEnd; ++iter) {
			iter->parse_type (pt);
			//if (iter->index () == 0) {
			//	items.push_back (*iter);
			//	items.erase (iter);
			//	--iterEnd;
			//	--iter;
			//}
		}

		// ��������
		std::string s;
		//ParseTypes pt;
		for (obj_item_t &item : items)
			s += item.to_str (pt, padding);
		return s;
	}
};



#endif //__GM_ITEM_HPP__
