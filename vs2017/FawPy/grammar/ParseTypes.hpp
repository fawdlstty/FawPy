#ifndef __PARSETYPES_HPP__
#define __PARSETYPES_HPP__



#include <string>
#include <vector>
#include <tuple>




struct ParseType {
	enum ParseTypeEnum {
		ParseTypeNone				= 0,
		ParseTypeGlobal				= 1,
		ParseTypeFunction			= 2,
		ParseTypeClass				= 3,
		ParseTypeClassAttr			= 4,
		ParseTypeClassMethod		= 5,
		//ParseTypeLocalVariable	= 6,
	};

	ParseTypeEnum	m_type;
	std::string		m_strName;
	std::string		m_strName2;
	ParseType (ParseTypeEnum type, std::string strName, std::string strName2) : m_type (type), m_strName (strName), m_strName2 (strName2) {}
};

struct ParseTypes : public std::vector<ParseType> {
	std::vector<std::tuple<std::vector<ParseType>::iterator, std::string>> m_vars;

	// 添加一个变量名
	void add_type (ParseType::ParseTypeEnum type, std::string typeName, std::string typeName2) {
		auto iter = begin ();
		for (; iter != end () && iter->m_type <= type; ++iter);
		insert (iter, { type, typeName, typeName2 });
	}

	// 获取类型
	ParseType::ParseTypeEnum get_type (std::string strName, std::string strName2) {
		for (ParseType &pt : *this) {
			if (strName == pt.m_strName && strName2 == pt.m_strName2)
				return pt.m_type;
		}
		return ParseType::ParseTypeNone;
	}

	// 添加一个新变量
	void add_var (std::vector<ParseType>::iterator iter, std::string varName) {
		m_vars.push_back (std::make_tuple (iter, varName));
	}

	// 获取变量类型
	std::vector<ParseType>::iterator get_var (std::string varName) {
		for (auto[iter, name] : m_vars) {
			if (name == varName)
				return iter;
		}
		return end ();
	}
};



#endif //__PARSETYPES_HPP__
