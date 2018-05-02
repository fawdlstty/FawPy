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
	http_server server ((std::thread::hardware_concurrency () + 1) * 2);
	server.listen ("0.0.0.0", "4545");
	//http_server_<io_service_inplace> server;
	//server.listen ("4545");
	server.set_http_handler<GET, POST> ("/tools/fawpy", [](const request& req, response& res) {
		res.set_status_and_content (status_type::ok, "\n"
			"<!doctype html>\n"
			"<html lang=\"zh-CN\">\n"
			"	<head>\n"
			"		<meta charset=\"utf-8\">\n"
			"		<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">\n"
			"		<meta name=\"viewport\" content=\"width=device-width, initial-scale=1, shrink-to-fit=no\">\n"
			"		<link type=\"text/css\" rel=\"stylesheet\" href=\"https://cdn.bootcss.com/bootstrap/4.0.0/css/bootstrap.min.css\">\n"
			"		<title>Python源码转C艹源码</title>\n"
			"		<style type=\"text/css\">\n"
			"			.row { margin-bottom: 20px; }\n"
			"			.form-control { min-height: 200px; }\n"
			"		</style>\n"
			"	</head>\n"
			"	<body style=\"background-color: #cce8cf;\">\n"
			"	<a href=\"https://github.com/fawdlstty/FawPy\" target=\"_blank\" class=\"github-corner\"><svg width=\"80\" height=\"80\" viewBox=\"0 0 250 250\" style=\"fill:#151513; color:#fff; position: absolute; top: 0; border: 0; right: 0;\"><path d=\"M0,0 L115,115 L130,115 L142,142 L250,250 L250,0 Z\"></path><path d=\"M128.3,109.0 C113.8,99.7 119.0,89.6 119.0,89.6 C122.0,82.7 120.5,78.6 120.5,78.6 C119.2,72.0 123.4,76.3 123.4,76.3 C127.3,80.9 125.5,87.3 125.5,87.3 C122.9,97.6 130.6,101.9 134.4,103.2\" fill=\"currentColor\" style=\"transform-origin: 130px 106px;\" class=\"octo-arm\"></path><path d=\"M115.0,115.0 C114.9,115.1 118.7,116.5 119.8,115.4 L133.7,101.6 C136.9,99.2 139.9,98.4 142.2,98.6 C133.8,88.0 127.5,74.4 143.8,58.0 C148.5,53.4 154.0,51.2 159.7,51.0 C160.3,49.4 163.2,43.6 171.4,40.1 C171.4,40.1 176.1,42.5 178.8,56.2 C183.1,58.6 187.2,61.8 190.9,65.4 C194.5,69.0 197.7,73.2 200.1,77.6 C213.8,80.2 216.3,84.9 216.3,84.9 C212.7,93.1 206.9,96.0 205.4,96.6 C205.1,102.4 203.0,107.8 198.3,112.5 C181.9,128.9 168.3,122.5 157.7,114.1 C157.9,116.9 156.7,120.9 152.7,124.9 L141.0,136.5 C139.8,137.7 141.6,141.9 141.8,141.8 Z\" fill=\"currentColor\" class=\"octo-body\"></path></svg></a><style>.github-corner:hover .octo-arm{animation:octocat-wave 560ms ease-in-out}@keyframes octocat-wave{0%,100%{transform:rotate(0)}20%,60%{transform:rotate(-25deg)}40%,80%{transform:rotate(10deg)}}@media (max-width:500px){.github-corner:hover .octo-arm{animation:none}.github-corner .octo-arm{animation:octocat-wave 560ms ease-in-out}}</style>\n"
			"		<div class=\"container\">\n"
			"			<p>&nbsp;</p>\n"
			"			<h1 style=\"text-align: center;\">Python源码转C艹源码</h1>\n"
			"			<div class=\"row\">\n"
			"				<p class=\"col\">Python源码粘贴处：</p>\n"
			"				<div class=\"w-100\"></div>\n"
			"				<div id=\"text1\" class=\"col form-control\" contenteditable=\"true\">def fun_a ():<br />&nbsp;&nbsp;&nbsp;&nbsp;return 'hello world'<br />def fun_b (b):<br />&nbsp;&nbsp;&nbsp;&nbsp;return b + 1<br />def fun_c (a, b):<br />&nbsp;&nbsp;&nbsp;&nbsp;return a * 2 - b / 3<br />class cls_a ():<br />&nbsp;&nbsp;&nbsp;&nbsp;def fun_cls_d (self):<br />&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;return self<br />if (__name__ == '__main__'):<br />&nbsp;&nbsp;&nbsp;&nbsp;a = fun_a ()<br />&nbsp;&nbsp;&nbsp;&nbsp;print (a)<br />&nbsp;&nbsp;&nbsp;&nbsp;b = fun_b (2)<br />&nbsp;&nbsp;&nbsp;&nbsp;print (b)<br />&nbsp;&nbsp;&nbsp;&nbsp;c = fun_c (4, 5)<br />&nbsp;&nbsp;&nbsp;&nbsp;print (c)</div>\n"
			"			</div>\n"
			"			<div class=\"row\"><div class=\"col\"><button class=\"btn btn-primary\" onclick=\"my_convert()\">转换</button></div></div>\n"
			"			<div class=\"row\">\n"
			"				<p class=\"col\">生成的C艹源码：</p>\n"
			"				<div class=\"w-100\"></div>\n"
			"				<div id=\"text2\" class=\"col form-control\" contenteditable=\"true\"></div>\n"
			"			</div>\n"
			"			<p>&nbsp;</p>\n"
			"		</div>\n"
			"		<script type=\"text/javascript\" src=\"https://cdn.bootcss.com/jquery/3.3.1/jquery.min.js\"></script>\n"
			"		<script type=\"text/javascript\" src=\"https://cdn.bootcss.com/popper.js/1.12.9/umd/popper.min.js\"></script>\n"
			"		<script type=\"text/javascript\" src=\"https://cdn.bootcss.com/bootstrap/4.0.0/js/bootstrap.min.js\"></script>\n"
			"		<script type=\"text/javascript\">\n"
			"			function my_convert () {\n"
			"				$.post ('/tools/fawpy_py2c', {\n"
			"					'text': $('#text1').html ().replace (/&nbsp;/g, ' ').replace (/<br[ /]*>/g, '\\n')\n"
			"				}, function (data, status) {\n"
			"					$('#text2').html (decodeURIComponent (data).toString ().replace (/ /g, '&nbsp;').replace (/</g, '&lt;').replace (/>/g, '&gt;').replace (/\\n/g, '<br />'));\n"
			"				});\n"
			"			}\n"
			"		</script>\n"
			"	</body>\n"
			"</html>\n"
		);
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
		std::string str_cmd = "python3.6 cpl.py " + str_file + " " + str_file2;
		system (str_cmd.c_str ());
		std::this_thread::sleep_for (std::chrono::milliseconds (100));
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
