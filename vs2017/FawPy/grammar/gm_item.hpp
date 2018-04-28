#ifndef __GM_ITEM_HPP__
#define __GM_ITEM_HPP__



#include <string>
#include <vector>
#include <variant>
#include "../hstring.hpp"
#include "ParseTypes.hpp"
#include "gm_struct.hpp"
#include "gm_block.hpp"



// 描述任意类型一行 obj_expr_t    variant<obj_stat_t, obj_func_t, obj_class_t, nullptr_t>
struct obj_item_t : public std::variant<obj_stat_t, obj_func_t, obj_class_t, nullptr_t> {
	obj_item_t (obj_stat_t o)	: std::variant<obj_stat_t, obj_func_t, obj_class_t, nullptr_t> (o) {}
	obj_item_t (obj_func_t o)	: std::variant<obj_stat_t, obj_func_t, obj_class_t, nullptr_t> (o) {}
	obj_item_t (obj_class_t o)	: std::variant<obj_stat_t, obj_func_t, obj_class_t, nullptr_t> (o) {}
	obj_item_t (nullptr_t o)	: std::variant<obj_stat_t, obj_func_t, obj_class_t, nullptr_t> (o) {}
	obj_item_t () : std::variant<obj_stat_t, obj_func_t, obj_class_t, nullptr_t> (nullptr) {}
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

// 描述任意类型一段
struct obj_items_t : public std::variant<std::vector<obj_item_t>, nullptr_t> {
	obj_items_t (std::vector<obj_item_t> o)	: std::variant<std::vector<obj_item_t>, nullptr_t> (o) {}
	obj_items_t (nullptr_t o)				: std::variant<std::vector<obj_item_t>, nullptr_t> (o) {}
	std::string to_str (size_t padding) {
		if (index () == 1)
			return "parse error";
		ParseTypes pt;

		// 将所有命令代码放置在最后
		std::vector<obj_item_t> &items = std::get<0> (*this);
		for (auto iter = items.begin (), iterEnd = items.end (); iter != iterEnd; ++iter) {
			if (iter->index () == 0) {
				items.push_back (*iter);
				items.erase (iter);
				--iterEnd;
				--iter;
			} else if (iter->index () == 1) {
				pt.add_type (ParseType::ParseTypeFunction, std::get<1> (*iter).m_name, "");
			} else if (iter->index () == 2) {
				obj_class_t &cls = std::get<2> (*iter);
				pt.add_type (ParseType::ParseTypeFunction, cls.m_name, "");
				for (obj_stat_t &stat : cls.m_vStat) {
					//
				}
				for (obj_func_t &func : cls.m_vFunc) {
					//
				}
			}
		}

		// 解析代码
		std::string s;
		//ParseTypes pt;
		for (obj_item_t &item : items)
			s += item.to_str (padding);
		return s;
	}
};



#endif //__GM_ITEM_HPP__
