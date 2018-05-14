#encoding: utf-8
import ast



# 获取ast变量类型
base_type = lambda o:str(type(o))[8:-2].replace ('__main__.', '')



# 类型系统
class VarTypes ():
	# 前期只有std::any类型
	#types = {}
	#vars = {}

	# 获取变量类型
	def get_var_range (self, _var_name):
		return self.vars.get (_var_name)

	# 新增变量
	def add_var (self, _var_name, _var_range):
		self.vars[_var_name] = _var_range
	pass



# 值对象
class AST_Value ():
	#val = None
	#type = ''

	# 定义可接受的类型
	@staticmethod
	def accept (_type):
		return _type == '_ast.Num' or _type == '_ast.Str' or _type == '_ast.NameConstant' or _type == '_ast.Name' or _type == 'AST_Expr' or _type == '_ast.Tuple' or _type == 'str' or AST_Expr.accept (_type)

	# 构造函数
	def __init__ (self, _val, *_args):
		if (len (_args) > 0):
			self.type = 'AST_Expr'
			self.val = AST_Expr (_val, 'user_def', *_args)
		else:
			self.type = base_type (_val)
			if self.type == '_ast.Tuple':
				self.val = []
				for val_item in self.val.elts:
					self.val.append (AST_Value (val_item))
			elif AST_Expr.accept (self.type):
				self.val = AST_Expr (_val, self.type)
				self.type = 'AST_Expr'
				while self.val.oper == 'transfer':
					self.val = self.val.val
			else:
				self.val = _val

	# 获取值类型
	def get_type (self):
		if self.type == '_ast.Num':
			return 'int'
		elif self.type == '_ast.Str':
			return 'std::string'
		elif self.type == '_ast.NameConstant':
			return None
		elif self.type == '_ast.Tuple':
			vals = self.val.elts
			ret = 'std::tuple<'
			for i in vals:
				if (i > 0):
					ret += ', '
				ret += AST_Value (i).get_type ()
			ret += '>'
			return ret
		else:
			return self.type

	# 导出
	def export (self, _this, _is_left = False, _no_quot = False):
		if self.type == '_ast.Num':
			return str (self.val.n)
		elif self.type == '_ast.Str':
			return '"' + self.val.s + '"'
		elif self.type == 'str':
			return self.val
		elif self.type == '_ast.Name':
			if self.val.id == _this:
				return 'this'
			return self.val.id
		elif self.type == '_ast.NameConstant':
			return ['true', 'false'][[True, False].index (self.val.value)]
		elif self.type == 'AST_Expr':
			if _no_quot or self.val.oper == 'call':
				return self.val.export (_this)
			return '(' + self.val.export (_this) + ')'
		elif self.type == '_ast.Tuple':
			ret = ['std::make_tuple (', 'std::tie ('][_is_left]
			for _val in self.val:
				ret += [', ', ''][_val == self.val[0]] + _val.export (_this, _is_left)
			return ret + ')'
		else:
			return '?unknown?'
	pass



# 公式对象
class AST_Expr ():
	#oper = 'operator'
	#val_left = None
	#val_right = None
	########## or ##########
	#oper = 'return'
	#val
	########## or ##########
	#oper = 'call'
	#func = ''
	#params = []
	########## or ##########
	#oper = '->'
	#val_left = None
	#val_right = None

	# 定义可接受的类型
	@staticmethod
	def accept (_type):
		return _type == '_ast.Assign' or _type == '_ast.Return' or _type == '_ast.BinOp' or _type == '_ast.Compare' or _type == '_ast.Expr' or _type == '_ast.Call' or _type == '_ast.Attribute' or _type == '_ast.BoolOp' or _type == '_ast.UnaryOp'

	# 获取操作符类型的操作符
	__get_type_oper = lambda self, _type: ['+', '-', '*', '/', '==', '<', '<=', '>', '>=', '&&', '||', '!'][[
		'_ast.Add', '_ast.Sub', '_ast.Mult', '_ast.Div', '_ast.Eq', '_ast.Lt', '_ast.LtE', '_ast.Gt', '_ast.GtE', '_ast.And', '_ast.Or', '_ast.Not'].index (_type)]

	# 构造函数
	def __init__ (self, _val, _type, *_args):
		if (_type == 'user_def'):
			self.oper = self.__get_type_oper (base_type (_args[0]))
			self.val_left = AST_Value (_val)
			self.val_right = AST_Value (_args[1])
		elif _type == '_ast.Assign':
			self.oper = '='
			self.val_left = AST_Value (_val.targets[0])
			self.val_right = AST_Value (_val.value)
		elif _type == '_ast.Return':
			self.oper = 'return'
			_type = base_type (_val.value)
			if AST_Value.accept (_type):
				self.val = AST_Value (_val.value)
			else:
				print ('[AST_Expr] parse error: cannot return type', _type)
		elif _type == '_ast.BinOp':
			self.oper = self.__get_type_oper (base_type (_val.op))
			self.val_left = AST_Value (_val.left)
			self.val_right = AST_Value (_val.right)
		elif _type == '_ast.Compare':
			if len (_val.ops) == 1:
				self.oper = self.__get_type_oper (base_type (_val.ops[0]))
				self.val_left = AST_Value (_val.left)
				self.val_right = AST_Value (_val.comparators[0])
			else:
				self.oper = '&&'
				self.vals = []
				for i in range (0, len (_val.ops)):
					self.vals.append (AST_Value ([_val.comparators[i - 1], _val.left][i == 0], _val.ops[i], _val.comparators[i]))
		elif _type == '_ast.Expr':
			self.oper = 'transfer'
			self.val = AST_Expr (_val.value, base_type (_val.value))
		elif _type == '_ast.BoolOp':
			self.oper = self.__get_type_oper (base_type (_val.op))
			self.vals = []
			for _val in _val.values:
				self.vals.append (AST_Value (_val))
		elif _type == '_ast.UnaryOp':
			self.oper = self.__get_type_oper (base_type (_val.op))
			self.val = AST_Value (_val.operand)
		elif _type == '_ast.Call':
			self.oper = 'call'
			self.func = AST_Value (_val.func)
			self.params = []
			for _arg in _val.args:
				self.params.append (AST_Value (_arg))
			self.pairs = {}
			for _pair in _val.keywords:
				self.pairs[_pair.arg] = AST_Value (_pair.value)
		elif _type == '_ast.Attribute':
			self.oper = '->'
			self.val_left = AST_Value (_val.value)
			self.val_right = AST_Value (_val.attr)
		else:
			print ('[AST_Expr] parse error: unprocessed type', _type)

	# 导出
	def export (self, _this):
		if self.oper in ['=', '+', '-', '*', '/', '==', '<', '<=', '>', '>=']:
			return self.val_left.export (_this, self.oper[-1] == '=') + ' ' + self.oper + ' ' + self.val_right.export (_this)
		elif self.oper in ['&&', '||']:
			_ret = self.vals[0].export (_this)
			for i in range (1, len (self.vals)):
				_ret += ' ' + self.oper + ' ' + self.vals[i].export (_this)
			return _ret
		elif self.oper in ['!']:
			return self.oper + self.val.export (_this)
		elif self.oper == 'return':
			return 'return ' + self.val.export (_this)
		elif self.oper == 'call':
			_func = self.func.export (_this, False, True)
			_ret = ''
			if (_func == 'print'):
				if 'file' in self.pairs.keys () and self.pairs['file'] !=  'sys.stdout':
					_ret += self.pairs['file'].export (_this)
				else:
					_ret += 'std::cout'
				_join = '" "' if not 'sep' in self.pairs.keys () else self.pairs['sep'].export (_this)
				for i in range (0, len (self.params)):
					_ret += [' << ' + _join, ''][i == 0 or _join == '']
					_ret += ' << ' + self.params[i].export (_this)
				_ret += ' << ' + ('std::endl' if not 'end' in self.pairs.keys () else self.pairs['end'].export (_this))
				# 以下代码含义为，考虑转义字符后的 _ret = _ret.replace ('" << "', '')
				_start = 0
				_p = _ret.find ('" << "', _start)
				_transmean_counter = lambda s: 0 if s[-1:] != '\\' else 1 + _transmean_counter (s[:-1])
				while _p != -1:
					_left_tmp = _ret[:_p]
					if _transmean_counter (_left_tmp) % 2 == 0:
						_ret = _left_tmp + _ret[_p + 6:]
					_p = _ret.find ('" << "', _start)
			else:
				_ret += _func + ' ('
				_first = True
				for i in range (0, len (self.params)):
					_ret += [', ', ''][i == 0]
					_ret += self.params[i].export (_this)
				_ret += ')'
			return _ret
		elif self.oper == '->':
			return self.val_left.export (_this) + '.' + self.val_right.export (_this)
		else:
			return '?unknown?'
	pass



# if 对象
class AST_If ():
	#tests = [ AST_Value() ]
	#stmts = [ AST_Stmts() ]
	#else_stmt = None or AST_Stmts()

	# 定义可接受的类型
	@staticmethod
	def accept (_type):
		return _type == '_ast.If'

	# 构造函数
	def __init__ (self, _val):
		self.tests = []
		self.stmts = []
		self.else_stmt = None
		while True:
			self.tests.append (AST_Value (_val.test))
			self.stmts.append (AST_Stmts (_val.body))
			if len (_val.orelse) == 0:
				break
			if not AST_If.accept (base_type (_val.orelse[0])):
				self.else_stmt = AST_Stmts (_val.orelse)
				break
			_val = _val.orelse[0]

	# 导出
	def export (self, pd, _this):
		_ret = ''
		for i in range (0, len (self.stmts)):
			_ret += [' else if', pd + 'if'][i == 0] + ' (' + self.tests[i].export (_this, False, True) + ') {\n';
			_ret += self.stmts[i].export (pd + '    ', _this) + pd + '}'
		if self.else_stmt != None:
			_ret += ' else {\n' + self.else_stmt.export (pd + '    ', _this) + pd + '}'
		return _ret + '\n'
	pass



# while 对象
class AST_While ():
	#val = None

	# 定义可接受的类型
	@staticmethod
	def accept (_type):
		return _type == '_ast.While'

	# 构造函数
	def __init__ (self, _val):
		self.val = _val

	# 导出
	def export (self, pd, _this):
		return ''
	pass



# for 对象
class AST_For ():
	#val = None

	# 定义可接受的类型
	@staticmethod
	def accept (_type):
		return _type == '_ast.For'

	# 构造函数
	def __init__ (self, _val):
		self.val = _val

	# 导出
	def export (self, pd, _this):
		return ''
	pass



# 语句 对象
class AST_Stmt ():
	#val = None

	# 定义可接受的类型
	@staticmethod
	def accept (_type):
		return AST_Expr.accept (_type) or AST_If.accept (_type) or AST_While.accept (_type) or AST_For.accept (_type)

	# 构造函数
	def __init__ (self, _val):
		_type = base_type (_val)
		if _type == '_ast.Expr':
			_val = _val.value
			_type = base_type (_val)
		if AST_Expr.accept (_type):
			self.val = AST_Expr (_val, _type)
		elif AST_If.accept (_type):
			self.val = AST_If (_val)
		elif AST_While.accept (_type):
			self.val = AST_While (_val)
		elif AST_For.accept (_type):
			self.val = AST_For (_val)
		else:
			print ('[AST_Stmt] parse error: unprocessed type', _type)

	# 导出
	def export (self, pd, _this):
		_type = base_type (self.val)
		if (_type == 'AST_Expr'):
			return pd + self.val.export (_this) + ';\n'
		return self.val.export (pd, _this)
	pass



# 语句组 对象
class AST_Stmts ():
	#stmts = []

	# 构造函数
	def __init__ (self, _items):
		self.stmts = []
		for _item in _items:
			_type = base_type (_item)
			if not AST_Stmt.accept (_type):
				print ('[AST_Stmts] parse error: cannot accept type', _type)
			self.stmts.append (AST_Stmt (_item))

	# 导出
	def export (self, pd, _this):
		_ret = ''
		for _item in self.stmts:
			_ret += _item.export (pd, _this)
		return _ret
	pass



# 函数对象
class AST_Func ():
	#name = ''
	#args = []
	#stmts = []
	#this = ''

	# 定义可接受的类型
	@staticmethod
	def accept (_type):
		return _type == '_ast.FunctionDef'

	# 构造函数
	def __init__ (self, _val, _isDyMethod = False):
		self.name = _val.name
		self.args = []
		for _arg in _val.args.args:
			self.args.append (_arg.arg)
		self.stmts = AST_Stmts (_val.body)
		self.this = ''
		if _isDyMethod:
			self.this = self.args[0]
			del [self.args[0]]

	# 导出
	def export (self, pd, _this = ''):
		_ret = pd + 'std::any ' + self.name + ' ('
		for _arg in self.args:
			_ret += [', ', ''][_arg == self.args[0]]
			_ret += 'std::any ' + _arg
		_ret += ') {\n'
		_ret += self.stmts.export (pd + '    ', self.this)
		return _ret + pd + '}\n\n'
	pass



# 类对象
class AST_Class ():
	#name = ''
	#items = None

	# 定义可接受的类型
	@staticmethod
	def accept (_type):
		return _type == '_ast.ClassDef'

	# 构造函数
	def __init__ (self, _val):
		self.name = _val.name
		self.items = AST_Items (_val.body, True)

	# 导出
	def export (self, pd, _this = ''):
		_ret = pd + 'struct ' + self.name + ' {\n'
		_ret += self.items.export (pd + '    ')
		while _ret[-2:] == '\n\n':
			_ret = _ret[1:-1]
		return _ret + pd + '};\n\n'
	pass



# 代码块对象，可代表任一 AST_Expr、AST_If、AST_While、AST_For、AST_Func、AST_Class
class AST_Item ():
	#val = None

	# 构造函数
	def __init__ (self, _val, _isInClass = False):
		_type = base_type (_val)
		if (_type == '_ast.Expr'):
			_val = _val.value
			_type = base_type (_val)
		if AST_Expr.accept (_type):
			self.val = AST_Expr (_val, _type)
		elif AST_If.accept (_type):
			self.val = AST_If (_val)
		elif AST_While.accept (_type):
			self.val = AST_While (_val)
		elif AST_For.accept (_type):
			self.val = AST_For (_val)
		elif AST_Func.accept (_type):
			self.val = AST_Func (_val, _isInClass)
		elif AST_Class.accept (_type):
			self.val = AST_Class (_val)
		else:
			print ('[AST_Item] parse error: unprocessed type', _type)

	# 导出
	def export (self, pd, _this):
		if (self.val != None):
			return self.val.export (pd, _this)
		return '\n'
	pass



# 模块对象，包含一组代码块对象
class AST_Items ():
	#items = []

	# 构造函数
	def __init__ (self, _items, _isInClass = False):
		self.items = []
		for _item in _items:
			self.items.append (AST_Item (_item, _isInClass))

	# 导出
	def export (self, pd = '', _this = ''):
		_ret = ''
		for _item in self.items:
			_ret += _item.export (pd, _this)
		return _ret
	pass



# AST解析器
class ASTParser ():
	#items = None

	# 构造函数
	def __init__ (self, _file):
		with open (_file, 'rU') as f:
			_module = ast.parse (f.read (), '', 'exec')
			self.items = AST_Items (_module.body)

	# 导出语法树
	def export (self):
		return '#include <iostream>\n#include <string>\n\n' + self.items.export ()
	pass



# 程序入口点
if (__name__ == '__main__'):
	ap = ASTParser ('D:/Downloads/FawPy-master/test2.py')
	s = ap.export ()
	print (s)
	pass
