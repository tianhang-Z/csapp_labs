gcc -E hello.c -o hello.i           #预编译
gcc -S hello.i -o hello.s       #编译
gcc -c hello.s -o hello.o      #汇编
gcc [目标文件] -o [可执行程序] -l[动态库名]
gcc hello.o -o hello             #链接
