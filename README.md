# FawPy

将Python源码翻译为C++源码的项目
Python最被大众诟病的主要因素就是速度慢，毕竟虚拟机解释执行，这就从根本上限制了Python无法开发超大型架构的项目，因为项目源码越多，项目就越慢。所以我在此发起这个项目，用于将Python翻译为C艹，给Python提速，使Python也可以自由自在的开发超大型项目而不用考虑速度因素。

示例Python代码如下（test1.py）：

```python
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
```

代码首先通过cpl.py转为字节码反汇编形式（disasm.txt）：

```assembly
0     LOAD_CONST           0     <code object fun_a at 0x000002514DE1AE40, file "", line 1>
    0     LOAD_CONST           1     'hello world'
    2     RETURN_VALUE         
2     LOAD_CONST           1     'fun_a'
4     MAKE_FUNCTION        0     
6     STORE_NAME           0     fun_a
8     LOAD_CONST           2     <code object fun_b at 0x000002514DE2EF60, file "", line 4>
    0     LOAD_FAST            0     b
    2     LOAD_CONST           1     1
    4     BINARY_ADD           
    6     RETURN_VALUE         
10    LOAD_CONST           3     'fun_b'
12    MAKE_FUNCTION        0     
14    STORE_NAME           1     fun_b
16    LOAD_CONST           4     <code object fun_c at 0x000002514DE530C0, file "", line 7>
    0     LOAD_FAST            0     a
    2     LOAD_CONST           1     2
    4     BINARY_MULTIPLY      
    6     LOAD_FAST            1     b
    8     LOAD_CONST           2     3
    10    BINARY_TRUE_DIVIDE   
    12    BINARY_SUBTRACT      
    14    RETURN_VALUE         
18    LOAD_CONST           5     'fun_c'
20    MAKE_FUNCTION        0     
22    STORE_NAME           2     fun_c
24    LOAD_BUILD_CLASS     
26    LOAD_CONST           6     <code object cls_a at 0x000002514DE53540, file "", line 10>
    0     LOAD_NAME            0     __name__
    2     STORE_NAME           1     __module__
    4     LOAD_CONST           0     'cls_a'
    6     STORE_NAME           2     __qualname__
    8     LOAD_CONST           1     <code object fun_cls_d at 0x000002514DE535D0, file "", line 11>
        0     LOAD_FAST            0     self
        2     RETURN_VALUE         
    10    LOAD_CONST           2     'cls_a.fun_cls_d'
    12    MAKE_FUNCTION        0     
    14    STORE_NAME           3     fun_cls_d
    16    LOAD_CONST           3     None
    18    RETURN_VALUE         
28    LOAD_CONST           7     'cls_a'
30    MAKE_FUNCTION        0     
32    LOAD_CONST           7     'cls_a'
34    CALL_FUNCTION        2     
36    STORE_NAME           3     cls_a
38    LOAD_NAME            4     __name__
40    LOAD_CONST           8     '__main__'
42    COMPARE_OP           2     ==
44    POP_JUMP_IF_FALSE    94    
46    LOAD_NAME            0     fun_a
48    CALL_FUNCTION        0     
50    STORE_NAME           5     a
52    LOAD_NAME            6     print
54    LOAD_NAME            5     a
56    CALL_FUNCTION        1     
58    POP_TOP              
60    LOAD_NAME            1     fun_b
62    LOAD_CONST           9     2
64    CALL_FUNCTION        1     
66    STORE_NAME           7     b
68    LOAD_NAME            6     print
70    LOAD_NAME            7     b
72    CALL_FUNCTION        1     
74    POP_TOP              
76    LOAD_NAME            2     fun_c
78    LOAD_CONST           10    4
80    LOAD_CONST           11    5
82    CALL_FUNCTION        2     
84    STORE_NAME           8     c
86    LOAD_NAME            6     print
88    LOAD_NAME            8     c
90    CALL_FUNCTION        1     
92    POP_TOP              
94    LOAD_CONST           12    None
96    RETURN_VALUE         
```

Python字节码几乎与源码是一一对应的，所以翻译起来比较方便。接下来项目代码主要读入字节码反汇编文件，也就是disasm1.txt，将字节码转为C++源码。转换后的效果如下所示：

```cpp
#include <iostream>
#include <string>

auto fun_a () {
    return "hello world";
}

auto fun_b (int b) {
    return (b+1);
}

auto fun_c (int a, int b) {
    return ((a*2)-(b/3));
}

class cls_a {
    auto fun_cls_d () {
        return this;
    }
};

int main (int argc, char* argv[]) {
    a = fun_a ();
    std::cout << a << std::endl;
    b = fun_b (2);
    std::cout << b << std::endl;
    c = fun_c (4, 5);
    std::cout << c << std::endl;
}
```

目前这个项目是测试版，翻译的不够完美，声明变量这儿都还没有定义变量类型，能翻译的结构有限，问题也较多，也有明确的弱点，比如迭代器，截止C艹17还没有这玩意，所以从实现上就比较蛋疼了；另外还有模块的`__name=='__main__'`，etc...

各位大佬如果有什么好的建议及遇到什么问题欢迎提issue。


