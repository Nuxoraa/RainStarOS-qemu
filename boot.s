; Константы для Multiboot
MBALIGN  equ  1 << 0
MEMINFO  equ  1 << 1
FLAGS    equ  MBALIGN | MEMINFO
MAGIC    equ  0x1BADB002
CHECKSUM equ -(MAGIC + FLAGS)

section .multiboot
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

section .bss
align 16
stack_bottom:
resb 16384 ; 16 КБ для стека
stack_top:

section .text
global _start
_start:
    mov esp, stack_top    ; Устанавливаем стек
    extern kernel_main
    call kernel_main      ; Переходим в C
    cli
.hang:  hlt               ; Если вышли из ядра — зависаем
    jmp .hang
