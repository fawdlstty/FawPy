def fun_a ():
	return 'hello world'

def fun_b (b):
	return b + 1

def fun_c (a, b):
	return a*2-b/3

class cls_a ():
	def fun_cls_d (self):
		return self

if (__name__ == '__main__'):
	a = fun_a ()
	print (a)
	b = fun_b (2)
	print (b)
	c = fun_c (4, 5)
	print (c)






