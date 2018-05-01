#ifndef __DISASMER_HPP__
#define __DISASMER_HPP__



#include <vector>
#include "DisasmItem.hpp"
#include "gm_item.hpp"



// �ֽ��������
struct Disasmer {
	obj_items_t m_items;
	Disasmer () : m_items (std::vector<obj_item_t> ()) {}

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
				Disasmer _child_bc;
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
				Disasmer _child_bc;
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
				else if (v[i].m_val2.index () == 3)
					_expr.m_vOper2.push_back (std::get<3> (v[i].m_val2));
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
				Disasmer _child_bc;
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
				Disasmer _child_bc;
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

	// �������
	std::string translate_code () {
		auto padding = [](size_t padding) { std::string s = ""; while (padding-- > 0) s += "    "; return s; };
		std::string s = "#include <iostream>\n#include <string>\n\n";
		s += m_items.to_str (0);
		size_t p = s.find ("if (__name__ == \"__main__\") {");
		if (p != std::string::npos && s[p - 1] == '\n') {
			hStringA::replace (s, std::string ("if (__name__ == \"__main__\") {"), std::string ("int main (int argc, char* argv[]) {"));
		}
		return s;
	}
};



#endif //__DISASMER_HPP__
