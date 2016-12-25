#Covariant Darwin Reference
##第一部分：概述
**Covariant Darwin**是一款使用C++编程语言编写的跨平台字符图形库，编译Darwin需要您的编译器支持C++11/14标准。
##第二部分：编译和运行
Darwin除C++ STL和必要的C API之外，核心组件不依赖其他程序库。Darwin能够自动选择适用于您平台的适配程序库和组件（编译期），但您可能需要家在一些为Darwin编写的扩展模块才能使程序正常运转。Darwin在GCC和LLVM Clang编译器上能够编译通过，其他编译器未作测试。
##第三部分：基础组件
###1：计时器（`headers/timer.hpp`）
Darwin计时器（`darwin::timer`）是一个类（`class`），但是其成员都是静态成员。使用类包装计时器主要目的是隐藏内部实现。  
####类型定义
`darwin::timer_t` 定义为无符号长整数类型（`unsigned long`），用于储存时间。  
`darwin::timer::time_unit` 定义为 `enum class`，包含了5个枚举量：
1. `nano_sec` 纳秒*
2. `micro_sec` 微秒*
3. `milli_sec` 毫秒
4. `second` 秒
5. `minute` 分
> *注意：带星号的内容在Microsoft Windows®操作系统中可能无法正常工作。*  

####静态方法
`static void reset()` 重置时钟，在程序加载时会自动重置一次。  
`static timer_t time(time_unit unit=time_unit::milli_sec)` 获取从上次重置时钟到函数执行的时间，时间单位默认为毫秒。  
`static void delay(timer_t time,time_unit unit=time_unit::milli_sec)` 使当前线程阻塞一段时间，时间单位默认为毫秒。  
`static timer_t measure(const std::function<void()>& func,time_unit unit=time_unit::milli_sec)` 计算一个函数消耗的时间，这个函数必须为void()形式的函数。时间单位默认为毫秒。  
###2：调试器（`headers/debugger.hpp`）
####宏定义
`Darwin_log(msg)` 向日志中输出一条消息。  
`Darwin_Warning(msg)` 报告一条警告消息，可以通过定义`DARWIN_IGNORE_WARNING`宏来忽略所有警告，可以通过定义`DARWIN_STRICT_CHECK`宏让Darwin收到警告后立刻停止运行。
`Darwin_Error(msg)` 报告一条错误消息，Darwin会立刻停止运行。  
Darwin会默认将所有日志输出到可执行文件同一目录中的`darwin_runtime.log`文件中。可以通过`Darwin_Set_Log_Path(path)`宏来重新设置日志文件的路径。
####输出文件流类（`darwin::outfs`）
特点：**不可继承，非线程安全，不可复制，RAII**  
#####构造函数
`outfs::outfs()` 构建一个空的实例。  
`outfs::outfs(const char*)`和`outfs::outfs(const std::string&)` 打开一个文件，如文件不存在将会新建一个文件。  
#####成员函数
`bool usable() const noexcept` 获取可用状态。  
`void open(const char*)`和`void open(const std::string&)` 打开一个文件，如已经打开另一个文件则操作无效。  
`void close()` 关闭已经打开的文件。  
`template<typename...> void printf(const char* format,...)` 格式化输出。  
`void flush()` 刷新文件流。
#####示例
```c++
#include "./headers/debugger.hpp"
int main()
{
	darwin::outfs out("./abc.txt");
	if(!out) return -1;
	out.printf("%s\n","Hello,World!");
	return 0;
}
```
##第四部分：核心组件
###1：`enum class`定义
**`darwin::status`（状态）：**
1. `null`（空）
2. `ready`（就绪）
3. `busy`（忙碌）
4. `leisure`（空闲）
5. `error`（错误）

**`darwin::results`（结果）：**
1. `null`（空）
2. `success`（成功）
3. `failure`（失败）

**`darwin::colors`（颜色）：**
1. `white`（白色）
2. `black`（黑色）
3. `red`（红色）
4. `green`（绿色）
5. `blue`（蓝色）
6. `pink`（粉色）
7. `yellow`（黄色）
8. `cyan`（青色）

**`darwin::attris`（属性）：**
1. `bright`（高亮）
2. `underline`（下划线）

###2：像素类