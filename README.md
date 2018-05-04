# FawPy

将Python源码翻译为C艹源码的项目
Python最被大众诟病的主要因素就是速度慢，毕竟虚拟机解释执行，这就从根本上限制了Python无法开发超大型架构的项目，因为项目源码越多，项目就越慢。所以我在此发起这个项目，用于将Python翻译为C艹，给Python提速，使Python也可以自由自在的开发超大型项目而不用考虑速度因素。
原始调用方式比较麻烦，在此简化为web端在线翻译。链接如下：

<a href="http://111.230.39.240:4545/tools/fawpy" target="_blank">http://111.230.39.240:4545/tools/fawpy</a>

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

转换后的效果如下所示：

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

struct cls_a {
    auto fun_cls_d () {
        return this;
    }
};

int main (int argc, char* argv[]) {
    auto a = fun_a ();
    std::cout << a << std::endl;
    auto b = fun_b (2);
    std::cout << b << std::endl;
    auto c = fun_c (4, 5);
    std::cout << c << std::endl;
}
```

经测试，生成的代码编译成功。目前这个项目是测试版，翻译的不够完美，能翻译的结构有限，问题也较多，也有明确的弱点，比如生成器，截止C艹17还没有这玩意，所以从实现上就比较蛋疼了；另外还有模块的`__name=='__main__'`，etc...

各位大佬如果有什么好的建议及遇到什么问题欢迎提issue，同时也欢迎对这个项目感兴趣的大佬参与进来一起开发。

项目引用的第三方库：
* <a href="https://github.com/boostorg/boost" target="_blank">boost</a>
* <a href="https://github.com/qicosmos/cinatra" target="_blank">cinatra</a>

个人博客地址：<a href="https://www.fawdlstty.com" target="_blank">https://www.fawdlstty.com</a>，欢迎到访。
