.section .text, "ax"
.global	_start
_start:
  mov     $0x1, %eax
  mov     $0x42, %ebx
  int     $0x80
