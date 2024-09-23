包含8个实验的笔记，答案和源代码。

- *Data Lab* [data_lab_note](./csapp_note/1,data_lab.md)

- 对应第二章 信息的表示和处理

  涉及整型 浮点数的表示和运算 

  重点是理解补码 补码的范围 补码的加法 用补码加法表示做减法运算   利用取反~计算相反数 

  理解算数移位和逻辑移位   对整型做逻辑运算！（非） 

  除法正数向下取整   和负数向上取整  （舍弃小数部分）  利用移位及加减运算实现乘除法

  不使用减法 实现整数的大小比较

   浮点数的表示

  > Students implement simple logical, two's complement, and floating point functions, but using a highly restricted subset of C. For example, they might be asked to compute the absolute value of a number using only bit-level operations and straightline code. This lab helps students understand the bit-level representations of C data types and the bit-level behavior of the operations on data.

- *Bomb Lab*  [bomb_lab_note](./csapp_note/2,bomb_lab.md)

  对应第三章

  学习汇编  数据访问（寻址方式 出入栈） 算数和逻辑运算 控制（条件码 跳转 循环 switch[跳转表]） 程序运行时（栈帧 转移控制 参数的传递 局部变量的存储）  数组的分配和访问（指针运算） 数据结构（struct union 数据对齐）  

  c代码与汇编代码的对应关系

  GDB调试

  gcc从c到可执行文件的生成

  > A "binary bomb" is a program provided to students as an object code file. When run, it prompts the user to type in 6 different strings. If any of these is incorrect, the bomb "explodes," printing an error message and logging the event on a grading server. Students must "defuse" their own unique bomb by disassembling and reverse engineering the program to determine what the 6 strings should be. The lab teaches students to understand assembly language, and also forces them to learn how to use a debugger. It's also great fun. A legendary lab among the CMU undergrads.
  >
  > Here's a [Linux/x86-64 binary bomb](http://csapp.cs.cmu.edu/3e/bomb.tar) that you can try out for yourself. The feature that notifies the grading server has been disabled, so feel free to explode this bomb with impunity. If you're an instructor with a CS:APP account, then you can download the [solution](http://csapp.cs.cmu.edu/im/bomb-solution.txt).

- *Attack Lab  [attack_lab_note](./csapp_note/3,attack_lab(target).md)*

  对应第三章后面

  缓存区溢出 对抗缓存区溢出（栈随机化 栈破坏检测）

  学习CI和ROP代码攻击

  > Note: This is the 64-bit successor to the 32-bit Buffer Lab.
  >
  > Students are given a pair of unique custom-generated x86-64 binary executables, called targets, that have buffer overflow bugs. One target is vulnerable to code injection attacks. The other is vulnerable to return-oriented programming attacks. Students are asked to modify the behavior of the targets by developing exploits based on either code injection or return-oriented programming. This lab teaches the students about the stack discipline and teaches them about the danger of writing code that is vulnerable to buffer overflow attacks.
  >
  > If you're a self-study student, here are a pair of [Ubuntu 12.4 targets](http://csapp.cs.cmu.edu/3e/target1.tar) that you can try out for yourself. You'll need to run your targets using the **"-q"** option so that they don't try to contact a non-existent grading server. If you're an instructor with a CS:APP acount, you can download the solutions [here](https://csapp.cs.cmu.edu/im/labs/target1-sol.tar).

- *Architecture Lab* [arch_lab_note](./csapp_note/4,arch_lab.md)

  对应第四章

  自定义的Y86-64语言  hcl语言（描述处理器对指令的处理流程）

  实现seq处理器（指令逐条运行）和pipe处理器 （流水线化的指令处理，其中涉及指令执行的五个阶段 {取值 译码 执行 访存   写回  更新PC} 五个阶段划分，使得指令全部可以用相似的流程执行）

  pipe处理器流水线冒险，分为数据和控制冒险。数据冒险就是：下一条指令需要之前指令的计算结果；控制冒险就是：一条指令要确定下一指令的位置。了解冒险的触发条件 处理措施（暂停 bubble 转发）

  > Note: Updated to Y86-64 for CS:APP3e.
  >
  > Students are given a small default Y86-64 array copying function and a working pipelined Y86-64 processor design that runs the copy function in some nominal number of clock cycles per array element (CPE). The students attempt to minimize the CPE by modifying both the function and the processor design. This gives the students a deep appreciation for the interactions between hardware and software.
  >
  > Note: The lab materials include the master source distribution of the Y86-64 processor simulators and the *Y86-64 Guide to Simulators*.

- *Cache Lab*  [cache_lab_note](./csapp_note/5,cache_lab.md)

  对应第六章

  讲存储器体系层次，三级缓存，缓存的组织结构（S,E,B,m)。给定地址后如何在内存中寻找。缓存以块为单位进行数据交换。理解局部性。

  lab主要是模拟cache的存取，以及结合cache特点编写局部性良好的矩阵转置函数（减少miss次数）

  > At CMU we use this lab in place of the Performance Lab. Students write a general-purpose cache simulator, and then optimize a small matrix transpose kernel to minimize the number of misses on a simulated cache. This lab uses the Valgrind tool to generate address traces.
  >
  > Note: This lab must be run on a 64-bit x86-64 system.

- *Performance Lab*

  对应第五章 可略

  > Students optimize the performance of an application kernel function such as convolution or matrix transposition. This lab provides a clear demonstration of the properties of cache memories and gives them experience with low-level program optimization.

- *Shell Lab*   [shell_lab_note](./csapp_note/6,shell_lab.md)

  对应第八章

  主要讲异常控制流，异常可以分为四种。

  （1）中断  外部IO引起 （2）故障 如缺页  （3）终止 如除法除以2、访问未定义虚拟内存  （4）陷阱 主要用于系统调用 如文件读写 进程控制

  还讲了进程的概念，进程就是执行的程序，每个程序都运行在进程的上下文中，每个进程有自己的私有地址空间，包括只读代码段、变量段、堆、共享库映射区、用户栈和内核区域，进程有自己的逻辑控制流，也即是PC值的序列。重叠的逻辑控制流就是并发。

  进程控制：进程创建和终止，子进程回收等

  信号：用于通知进程发生了异常，信号包括很多种，可以人为设置阻塞，信号处理程序的编写需要注意很多事项，如与主程序共享的数据结构的访问保护，另外多进程之间的竞争是并发中常见的错误。

  本章实验是完成一个简单的shell，其可以运行一些内置命令和可执行文件，并分为前台运行和后台运行，可以通过键盘ctrl-z、ctrl-c发送信号控制进程运行。

  > Students implement their own simple Unix shell program with job control, including the ctrl-c and ctrl-z keystrokes, fg, bg, and jobs commands. This is the students' first introduction to application level concurrency, and gives them a clear idea of Unix process control, signals, and signal handling.

- *Malloc Lab*  [malloc_lab_note](./csapp_note/7,malloc_lab.md)

  对应第九章

  虚拟内存是对主存的抽象。虚拟地址经过MMU、快表TLB和页表，被翻译成物理地址，进而访问主存。

  虚拟内存的内容是在磁盘上的，需要时缓存在主存上面，并以页为单位进行交换，可以将其称之为虚拟页和物理页。

  使用页表记录虚拟页是否在主存上，页面是放在主存中的，由内核管理，内核为每个进程都提供了页表。页表条目简称为PTE，通过在PTE上设置许可位（如可读、可写、是否允许用户访问），可以保护内存。主存是全相联的，虚拟页可以缓存在任何物理页。

  仿照cache的三级结构，页表也被存储在了快表TLB中，用于加速地址翻译。另外，通过多级页表，可以缩小页表大小，少占用内存。

  OS利用虚拟内存的概念，为每个进程独立的虚拟地址空间，这样的好处是可以简化链接和简化加载，每个进程都有相似的虚拟地址空间区域结构，磁盘的ELF可执行文件的各个节直接映射到虚拟地址的区域。还可以简化共享，不同进程的可以共享同一物理页。还能简化内存分配，当进程申请新的堆空间时，直接为其分配虚拟地址空间，再映射到物理地址即可。

  内存映射指的是虚拟内存区域和磁盘上的对象相关联。

  堆用于动态内存分配，其是必须的，因为有些数据结构在运行后才知道大小。动态内存分配的主要问题是空闲块的记录和选择，空闲块的分割和合并。需要合理设计以提高请求和释放的处理速度，提高内存利用率。

  本章实现是完成一个自己的动态内存分配器。

  Students implement their own versions of malloc, free, and realloc. This lab gives students a clear understanding of data layout and organization, and requires them to evaluate different trade-offs between space and time efficiency. One of our favorite labs. When students finish this one, they really understand pointers!

- *Proxy Lab*  [proxy_lab_note](./csapp_note/8,proxy_lab.md)

  对应第10、11、12章

  实现一个支持并发和缓存的cache，用到了线程编程、生产者消费者模型、读者写者模型、LRU。

  > Students implement a concurrent caching Web proxy that sits between their browser and the rest of the World Wide Web. This lab exposes students to the interesting world of network programming, and ties together many of the concepts from the course, such as byte ordering, caching, process control, signals, signal handling, file I/O, concurrency, and synchronization.