#include <iostream>
#include <fstream>
#include <string>
#include <variant>
#include <vector>
#include "hstring.hpp"
#include "grammar/DisasmItem.hpp"
#include "grammar/ParseTypes.hpp"
#include "grammar/gm_expr.hpp"
#include "grammar/gm_struct.hpp"
#include "grammar/gm_block.hpp"
#include "grammar/gm_item.hpp"
#include "grammar/Disasmer.hpp"



int main (int argc, char* argv[]) {
	// 读取文件
	std::ifstream ifs ("D:/GitHub/FawPy/disasm1.txt");
	std::string s ((std::istreambuf_iterator<char> (ifs)), std::istreambuf_iterator<char> ());
	std::vector<DisasmItem> v = DisasmItem::parse (s);

	// 翻译指令
	if (v.size () < 2) {
		std::cout << ("未找到文件 disasm.txt 或文件为空") << std::endl;
	} else if (v[v.size () - 2].m_cmd != "LOAD_CONST" || v[v.size () - 1].m_cmd != "RETURN_VALUE") {
		std::cout << (v[v.size () - 2].m_cmd != "LOAD_CONST" ? "待翻译字节码倒数第二条指令不是 LOAD_CONST" : "待翻译字节码倒数第一条指令不是 RETURN_VALUE") << std::endl;
	} else {
		v.erase (v.end () - 2, v.end ());
		Disasmer parser;
		parser.parse_items (v);
		parser.print_code ();
	}
	system ("pause");
	return 0;
}
