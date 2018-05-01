#define _SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING
#define _SILENCE_CXX17_RESULT_OF_DEPRECATION_WARNING
#define _CRT_SECURE_NO_WARNINGS
#pragma execution_character_set("utf-8")
#include <iostream>
#include <string>
#include <fstream>
#include <thread>
#include <chrono>
#include "cinatra/http_server.hpp"
using namespace cinatra;
#include "grammar/fawpy.hpp"

std::string gen_file_name (size_t len) {
	std::string name;
	for (size_t i = 0; i < len; ++i)
		name += char (rand () % 26 + 'a');
	return name;
}

int main () {
	srand ((unsigned int) time (nullptr));
	http_server_<io_service_inplace> server;
	server.listen ("4545");
	server.set_http_handler<GET, POST> ("/tools/fawpy", [](const request& req, response& res) {
		res.set_status_and_content (status_type::ok, ""
			"<!doctype html>"
			"<html lang=\"zh-CN\">"
			"	<head>"
			"		<meta charset=\"utf-8\">"
			"		<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">"
			"		<meta name=\"viewport\" content=\"width=device-width, initial-scale=1, shrink-to-fit=no\">"
			"		<link type=\"text/css\" rel=\"stylesheet\" href=\"https://cdn.bootcss.com/bootstrap/4.0.0/css/bootstrap.min.css\">"
			"		<title>Python源码转C艹源码</title>"
			"		<style type=\"text/css\">"
			"			.row { margin-bottom: 20px; }"
			"			.form-control { min-height: 200px; }"
			"		</style>"
			"	</head>"
			"	<body style=\"background-color: #999;\">"
			"		<div class=\"container\">"
			"			<p>&nbsp;</p>"
			"			<h1 style=\"text-align: center;\">Python源码转C艹源码</h1>"
			"			<div class=\"row\">"
			"				<p class=\"col\">Python源码粘贴处：</p>"
			"				<div class=\"w-100\"></div>"
			"				<div id=\"text1\" class=\"col form-control\" contenteditable=\"true\">def fun_a ():<br />&nbsp;&nbsp;&nbsp;&nbsp;return 'hello world'<br />def fun_b (b):<br />&nbsp;&nbsp;&nbsp;&nbsp;return b + 1<br />def fun_c (a, b):<br />&nbsp;&nbsp;&nbsp;&nbsp;return a * 2 - b / 3<br />class cls_a ():<br />&nbsp;&nbsp;&nbsp;&nbsp;def fun_cls_d (self):<br />&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;return self<br />if (__name__ == '__main__'):<br />&nbsp;&nbsp;&nbsp;&nbsp;a = fun_a ()<br />&nbsp;&nbsp;&nbsp;&nbsp;print (a)<br />&nbsp;&nbsp;&nbsp;&nbsp;b = fun_b (2)<br />&nbsp;&nbsp;&nbsp;&nbsp;print (b)<br />&nbsp;&nbsp;&nbsp;&nbsp;c = fun_c (4, 5)<br />&nbsp;&nbsp;&nbsp;&nbsp;print (c)</div>"
			"			</div>"
			"			<div class=\"row\"><div class=\"col\"><button class=\"btn btn-primary\" onclick=\"my_convert()\">转换</button></div></div>"
			"			<div class=\"row\">"
			"				<p class=\"col\">生成的C艹源码：</p>"
			"				<div class=\"w-100\"></div>"
			"				<div id=\"text2\" class=\"col form-control\" contenteditable=\"true\"></div>"
			"			</div>"
			"		</div>"
			"		<script type=\"text/javascript\" src=\"https://cdn.bootcss.com/jquery/3.3.1/jquery.min.js\"></script>"
			"		<script type=\"text/javascript\" src=\"https://cdn.bootcss.com/popper.js/1.12.9/umd/popper.min.js\"></script>"
			"		<script type=\"text/javascript\" src=\"https://cdn.bootcss.com/bootstrap/4.0.0/js/bootstrap.min.js\"></script>"
			"		<script type=\"text/javascript\">"
			"			function my_convert () {"
			"				$.post ('/tools/fawpy_py2c', {"
			"					'text': $('#text1').html ().replace (/&nbsp;/g, ' ').replace (/<br[ \\/]*>/g, '\\n')"
			"				}, function (data, status) {"
			"					$('#text2').html (decodeURIComponent (data).toString ().replace (/\\+/g, '&nbsp;').replace (/</g, '&lt;').replace (/>/g, '&gt;').replace (/\\n/g, '<br />'));"
			"				});"
			"			}"
			"		</script>"
			"	</body>"
			"</html>");
	});
	server.set_http_handler<GET, POST> ("/tools/fawpy_py2c", [](const request& req, response& res) {
		std::string text = req.get_query_value ("text").data ();
		text = hStringA::url_decode (text);
		std::string str_file = gen_file_name (10);
		std::string str_file2 = gen_file_name (10);
		[](std::string &str_file, std::string &text) {
			std::ofstream ofs (str_file);
			ofs.write (text.data (), text.length ());
			ofs.close ();
		} (str_file, text);
		std::string str_cmd = "python cpl.py " + str_file + " " + str_file2;
		system (str_cmd.c_str ());
		std::this_thread::sleep_for (std::chrono::seconds (2));
		std::string s = [](std::string &str_file2, std::string &text) -> std::string {
			std::ifstream ifs (str_file2);
			std::string s ((std::istreambuf_iterator<char> (ifs)), std::istreambuf_iterator<char> ());
			ifs.close ();
			return s;
		} (str_file2, text);
		std::vector<DisasmItem> v = DisasmItem::parse (s);
		if (v.size () < 2) {
			s = "未找到源码文件或文件为空";
		} else if (v[v.size () - 2].m_cmd != "LOAD_CONST" || v[v.size () - 1].m_cmd != "RETURN_VALUE") {
			s =  (v[v.size () - 2].m_cmd != "LOAD_CONST" ? "待翻译字节码倒数第二条指令不是 LOAD_CONST" : "待翻译字节码倒数第一条指令不是 RETURN_VALUE");
		} else {
			v.erase (v.end () - 2, v.end ());
			Disasmer parser;
			parser.parse_items (v);
			s = parser.translate_code ();
		}
#ifndef __GNUC__
		system (hStringA::format ("del %s -f", str_file.c_str ()).c_str ());
		system (hStringA::format ("del %s -f", str_file2.c_str ()).c_str ());
#else
		system (hStringA::format ("rm %s -f", str_file.c_str ()).c_str ());
		system (hStringA::format ("rm %s -f", str_file2.c_str ()).c_str ());
#endif
		s = hStringA::url_encode (s);
		res.set_status_and_content (status_type::ok, std::move (s));
	});
	server.run ();
	return 0;
}
