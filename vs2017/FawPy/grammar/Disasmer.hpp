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
#define RETURN_ERROR(x) { std::cout << "pos " << v[i].m_offset << ": " << (x) << std::endl; return 0; }
		std::vector<obj_item_t> &m_v = std::get<0> (m_items);
		obj_expr_t _expr;
		// ���ڿ���һ�δ����ɢ�ڶ��У����Բ���һ��������ֱ��������һ����������ʱ����
		size_t i = index;
		for (; i < v.size (); ++i) {
			if (v[i].m_cmd == "LOAD_BUILD_CLASS") {
				// �ഴ��ָ��
				if (_expr.m_vOper1.size () || _expr.m_vOper2.size ())	{ RETURN_ERROR ("�ഴ��ʱ����ջ�����д���������"); }
				else if (v.size () <= i + 6)							{ RETURN_ERROR ("�ഴ��ָ����ָ������� 6 ��"); }
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
			}
			else if (v[i].m_cmd == "LOAD_CONST" && v[i].m_val2.index () == 0) {
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
			}
			else if (v[i].m_cmd == "LOAD_CONST" || v[i].m_cmd == "LOAD_FAST" || v[i].m_cmd == "LOAD_NAME") {
				// ���س��������������
				if (v[i].m_val2.index () == 1)
					_expr.m_vOper2.push_back (std::get<1> (v[i].m_val2));
				else if (v[i].m_val2.index () == 2)
					_expr.m_vOper2.push_back (std::get<2> (v[i].m_val2));
				else if (v[i].m_val2.index () == 3)
					_expr.m_vOper2.push_back (std::get<3> (v[i].m_val2));
			}
			else if (v[i].m_cmd == "RETURN_VALUE") {
				// ���ؽ��
				if (_expr.m_vOper2.size () == 0) { RETURN_ERROR ("����ջ��û�����ݣ��޷�ִ�� RETURN_VALUE"); }
				_expr.m_operator = "return";
				obj_stat_t tmp_stat;
				tmp_stat = _expr;
				obj_item_t tmp_item;
				tmp_item = tmp_stat;
				m_v.push_back (tmp_item);
				return i - index + 1;
			}
			else if (v[i].m_cmd == "BINARY_ADD" || v[i].m_cmd == "BINARY_SUBTRACT" || v[i].m_cmd == "BINARY_MULTIPLY" || v[i].m_cmd == "BINARY_TRUE_DIVIDE" || v[i].m_cmd == "COMPARE_OP") {
				// ��������ָ��
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
			}
			else if (v[i].m_cmd == "LOAD_ATTR") {
				// ���س�Աָ��
				if (_expr.m_vOper2.size () < 1) { RETURN_ERROR ("����ջ����������һ�����޷�ִ�� CALL_FUNCTION"); }
				_expr.m_vOper1.push_back (*_expr.m_vOper2.begin ());
				_expr.m_vOper2.erase (_expr.m_vOper2.begin ());
				_expr.m_vOper2.push_back (std::get<1> (v[i].m_val2));
				_expr.m_operator = "->";
				obj_expr_t _expr2;
				_expr2.swap (_expr);
				_expr.m_vOper2.push_back (_expr2);
			}
			else if (v[i].m_cmd == "CALL_FUNCTION") {
				// ���ú���ָ��
				if (_expr.m_vOper2.size () < 1) { RETURN_ERROR ("����ջ����������һ�����޷�ִ�� CALL_FUNCTION"); }
				_expr.m_vOper1.push_back (*_expr.m_vOper2.begin ());
				_expr.m_vOper2.erase (_expr.m_vOper2.begin ());
				_expr.m_operator = "call";
				obj_expr_t _expr2;
				_expr2.swap (_expr);
				_expr.m_vOper2.push_back (_expr2);
			}
			else if (v[i].m_cmd == "STORE_NAME") {
				// �洢����ָ��
				_expr.m_vOper1.push_back (std::get<1> (v[i].m_val2));
				_expr.m_operator = "=";
				m_v.push_back (obj_stat_t (_expr));
				return i - index + 1;
			}
			else if (v[i].m_cmd == "POP_TOP") {
				// ����ջ��Ԫ��ָ��
				m_v.push_back (obj_stat_t (_expr));
				return i - index + 1;
			}
			else if (v[i].m_cmd == "POP_JUMP_IF_TRUE" || v[i].m_cmd == "POP_JUMP_IF_FALSE") {
				// ���� if ָ��
				if (v[i].m_cmd == "POP_JUMP_IF_TRUE") {
					obj_expr_t _expr2;
					_expr2.swap (_expr);
					_expr.m_operator = "!";
					_expr.m_vOper2.push_back (_expr2);
				}
				obj_if_t _if;
				std::vector<DisasmItem> vchild;
				size_t sz_end = v[i].m_val;
				size_t j = i + 1;
				for (; v.size () > j && v[j].m_offset < sz_end;) {
					vchild.push_back (v[j]);
					v.erase (v.begin () + j);
				}
				size_t sz_else = 0;
				if (vchild.rbegin ()->m_cmd == "JUMP_FORWARD") {
					sz_else = atoi (&(std::get<1> (vchild.rbegin ()->m_val2))[2]);
					vchild.pop_back ();
				}
				Disasmer _child;
				if (!_child.parse_items (vchild))
					return 0;
				_if.push_back (std::make_pair<obj_value_t, std::vector<obj_stat_t>> (std::move (*_expr.m_vOper2.rbegin ()), std::vector<obj_stat_t> ()));
				_expr.m_vOper2.erase (_expr.m_vOper2.end () - 1);
				_child.m_items.addto_vstat (_if.rbegin ()->second);
				if (sz_else) {
					vchild.clear ();
					for (j = i + 1; v.size () > j && v[j].m_offset < sz_else;) {
						vchild.push_back (v[j]);
						v.erase (v.begin () + j);
					}
					std::get<0> (_child.m_items).clear ();
					if (!_child.parse_items (vchild))
						return 0;
					_if.push_back (std::make_pair<obj_value_t, std::vector<obj_stat_t>> (nullptr, std::vector<obj_stat_t> ()));
					_child.m_items.addto_vstat (_if.rbegin ()->second);
				}
				m_v.push_back (obj_stat_t (_if));
				return i - index + 1;
			}
			else if (v[i].m_cmd == "DUP_TOP") {
				// ����ջ��Ԫ��ָ����ֻ���Ǹ��� if ָ��
				if (_expr.m_vOper2.size () != 2)						{ RETURN_ERROR ("���� if ָ���ʱ����ջԪ�ظ�������Ϊ 2 ��"); }
				obj_expr_t _exprX;
				_exprX.m_operator = "&&";
				while (v[i].m_cmd == "DUP_TOP") {
					if (v.size () <= i + 9)								{ RETURN_ERROR ("���� if ָ��1���ָ������� 9 ��"); }
					else if (v[i + 1].m_cmd != "ROT_THREE")				{ RETURN_ERROR ("���� if ָ��1����� 1 ��ָ�Ϊ ROT_THREE"); }
					else if (v[i + 2].m_cmd != "COMPARE_OP")			{ RETURN_ERROR ("���� if ָ��1����� 2 ��ָ�Ϊ COMPARE_OP"); }
					else if (v[i + 3].m_cmd != "JUMP_IF_FALSE_OR_POP")	{ RETURN_ERROR ("���� if ָ��1����� 3 ��ָ�Ϊ JUMP_IF_FALSE_OR_POP"); }
					else if (v[i + 4].m_cmd != "LOAD_CONST")			{ RETURN_ERROR ("���� if ָ��1����� 4 ��ָ�Ϊ LOAD_CONST"); }
					//
					obj_expr_t _expr2;
					_expr2.m_vOper1.push_back (*(_expr.m_vOper2.rbegin () + 1));
					_expr2.m_operator = std::get<1> (v[i + 2].m_val2);
					_expr2.m_vOper2.push_back (*_expr.m_vOper2.rbegin ());
					_expr.m_vOper2.erase (_expr.m_vOper2.begin ());
					_exprX.m_vOper2.push_back (_expr2);
					//
					if (v[i + 4].m_val2.index () == 1)
						_expr.m_vOper2.push_back (std::get<1> (v[i + 4].m_val2));
					else if (v[i + 4].m_val2.index () == 2)
						_expr.m_vOper2.push_back (std::get<2> (v[i + 4].m_val2));
					else if (v[i + 4].m_val2.index () == 3)
						_expr.m_vOper2.push_back (std::get<3> (v[i + 4].m_val2));
					i += 5;
				}
				if (v.size () <= i + 3)									{ RETURN_ERROR ("���� if ָ��2���ָ������� 5 ��"); }
				else if (v[i].m_cmd != "COMPARE_OP")					{ RETURN_ERROR ("���� if ָ��2����� 1 ��ָ�Ϊ COMPARE_OP"); }
				else if (v[i + 1].m_cmd != "JUMP_FORWARD")				{ RETURN_ERROR ("���� if ָ��2����� 2 ��ָ�Ϊ JUMP_FORWARD"); }
				else if (v[i + 2].m_cmd != "ROT_TWO")					{ RETURN_ERROR ("���� if ָ��2����� 3 ��ָ�Ϊ ROT_TWO"); }
				else if (v[i + 3].m_cmd != "POP_TOP")					{ RETURN_ERROR ("���� if ָ��2����� 4 ��ָ�Ϊ POP_TOP"); }
				//
				obj_expr_t _expr2;
				_expr2.m_vOper1.push_back (*(_expr.m_vOper2.rbegin () + 1));
				_expr2.m_operator = std::get<1> (v[i].m_val2);
				_expr2.m_vOper2.push_back (*_expr.m_vOper2.rbegin ());
				_exprX.m_vOper2.push_back (_expr2);
				_expr.m_vOper2.clear ();
				_expr.m_vOper2.push_back (_exprX);
				// ���潻��ifָ��ִ��
				i += 3;
			}
			else if (v[i].m_cmd == "SETUP_LOOP") {
				// ѭ������ָ����ֻ���� for ѭ��
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
					else { RETURN_ERROR ("�ݲ�֧��for���Ƕ�������"); }
				}
				m_v.push_back (obj_stat_t (_for));
				if		(v[j].m_cmd != "JUMP_ABSOLUTE")					{ RETURN_ERROR ("ѭ������������ 2 ��ָ�Ϊ JUMP_ABSOLUTE"); }
				else if	(v[j + 1].m_cmd != "POP_BLOCK")					{ RETURN_ERROR ("ѭ������������ 1 ��ָ�Ϊ POP_BLOCK"); }
				return j - index + 2;
			}
			else {
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
			//"std::string operator* (std::string s, int n) {\n"
			//"    std::string ret;\n"
			//"    while (n-- > 0) ret += s;\n"
			//"    return ret;\n"
			//"}\n\n";
		s += m_items.to_str (0);
		size_t p = s.find ("if (__name__ == \"__main__\") {");
		if (p != std::string::npos && s[p - 1] == '\n') {
			hStringA::replace (s, std::string ("if (__name__ == \"__main__\") {"), std::string ("int main (int argc, char* argv[]) {"));
		}
		return s;
	}
};



#endif //__DISASMER_HPP__
