#ifndef __PARSETYPES_HPP__
#define __PARSETYPES_HPP__



#include <string>
#include <vector>
#include <tuple>




struct ParseType {
	enum VarRegionEnum {
		VarRegionNone			= 0,
		VarRegionGlobal			= 0,
		VarRegionClass			= 1,
		VarRegionInFunction		= 2,
	};
	enum VarTypeEnum {
		VarTypeNone				= 0,
		VarTypeFunction			= 1,
		VarTypeClass			= 2,
		VarTypeClassAttr		= 3,
		VarTypeClassMethod		= 4,
		VarTypePredefine		= 5,
		VarTypeUserDefine		= 6,
		VarTypeAny				= 7,
	};

	VarTypeEnum		m_type;
	std::string		m_type_name;
	ParseType (VarTypeEnum type, std::string type_name) : m_type (type), m_type_name (type_name) {}
};

struct ParseTypes : public std::vector<ParseType> {
	std::vector<std::tuple<ParseType::VarRegionEnum, std::vector<ParseType>::iterator, std::string>> m_vars;

	// 添加一个类型
	void add_type (ParseType::VarTypeEnum type, std::string type_name) {
		if (type_name.empty ())
			return;
		push_back (ParseType (type, type_name));
	}

	// 添加一个变量
	void add_var (ParseType::VarRegionEnum region, ParseType::VarTypeEnum type, std::string type_name, std::string var_name) {
		std::vector<ParseType>::iterator iter = begin ();
		for (; iter != end (); ++iter) {
			if (iter->m_type == type && iter->m_type_name == type_name)
				break;
		}
		if (iter == end ()) {
			push_back (ParseType (type, type_name));
			iter = end () - 1;
		}
		m_vars.push_back (std::make_tuple (region, iter, var_name));
	}

	// 获取一个变量
	std::tuple<ParseType::VarRegionEnum, ParseType::VarTypeEnum, std::string> get_var (std::string var_name) {
		for (auto[region, iter, _var_name] : m_vars) {
			if (_var_name == var_name)
				return std::make_tuple (region, iter->m_type, iter->m_type_name);
		}
		return std::make_tuple (ParseType::VarRegionNone, ParseType::VarTypeNone, "");
	}

	// 清空变量
	void clear_var () {
		m_vars.clear ();
	}
};



#endif //__PARSETYPES_HPP__
