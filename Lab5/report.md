# 编译原理 Lab5 实验报告
211220028 党任飞

# 一、实现功能
根据给定的框架代码，实现基于数据流分析的中间代码优化，对输入的input.ir进行公共子表达式消除、无用代码消除、常量折叠等优化，并输出为output.ir。

# 二、执行方法
进入`Lab5/Code/`目录，执行：
```
> make
> ./parser input.ir output.ir
```
即可执行并将input.ir进行中间代码优化，得到输出output.ir。

# 三、核心实现细节
实现路径：
+ 核心函数为`IR_optimize()`，在这里面框架代码已经实现了基于活跃变量、常量传播和公共子表达式的优化路线。
+ 实现`solver.c`中的迭代求解器。主要是反向的求解器，可以参考框架内正向求解器的实现。
+ 实现`available_expressions_analysis.c`、`constant_propagation.c`、`copy_propagation.c`、`live_variable_analysis.c`。
  
由于之前修过软件分析，因此对这个数据流分析框架结构比较熟悉。重点是搞清楚每个分析的方向、初始化、边界条件、may/must条件。
