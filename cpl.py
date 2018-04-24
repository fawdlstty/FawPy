import dis
import sys

def _get_instructions_bytes(code, _file, _line_offset=0):
	for offset, op, arg in dis._unpack_opargs(code.co_code):
		argval = None
		argrepr = ''
		if arg is not None:
			argval = arg
			if op in dis.hasconst:
				argval, argrepr = dis._get_const_info(arg, code.co_consts)
			elif op in dis.hasname:
				argval, argrepr = dis._get_name_info(arg, code.co_names)
			elif op in dis.hasjrel:
				argval = offset + 2 + arg
				argrepr = "to " + repr(argval)
			elif op in dis.haslocal:
				argval, argrepr = dis._get_name_info(arg, code.co_varnames)
			elif op in dis.hascompare:
				argval = dis.cmp_op[arg]
				argrepr = argval
			elif op in dis.hasfree:
				argval, argrepr = dis._get_name_info(arg, code.co_cellvars + code.co_freevars)
			elif op == dis.FORMAT_VALUE:
				argval = ((None, str, repr, ascii)[arg & 0x3], bool(arg & 0x4))
				argrepr = ('', 'str', 'repr', 'ascii')[arg & 0x3]
				if argval[1]:
					if argrepr:
						argrepr += ', '
					argrepr += 'with format'
		disasm_line = ' ' * _line_offset
		disasm_line += repr (offset).ljust (6)
		disasm_line += dis.opname[op].ljust (21)
		if arg is not None:
			disasm_line += repr (arg).ljust (6)
			if argrepr:
				if (repr (type (argval))[8:-2] == 'code'):
				else:
					disasm_line += argrepr
		print (disasm_line, file=_file)
		if (repr (type (argval))[8:-2] == 'code'):
			_get_instructions_bytes (argval, _file, _line_offset + 4)

if (__name__ == '__main__'):
	f = open (sys.argv[1], 'rU')
	src = f.read()
	f.close()
	if not src or src[-1] != '\n':
		src += '\n'
	code = compile(src, '', 'exec')
	f = open ('disasm.txt', 'w')
	_get_instructions_bytes (code, f)
	f.close ()

