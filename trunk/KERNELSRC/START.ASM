[bits		32]

[global		start]							; LINKER CALLS START
[global _sys_stack]
start:										; Start point of KERNEL
	mov		esp, _sys_stack     			; This points the stack to our new stack area
	jmp		startsub						; Only have 4 bytes lets move to sub function
	
;Lets make this GRUB compatible:
ALIGN 4
mboot:
    MULTIBOOT_PAGE_ALIGN	equ 1<<0
    MULTIBOOT_MEMORY_INFO	equ 1<<1
    MULTIBOOT_AOUT_KLUDGE	equ 1<<16
    MULTIBOOT_HEADER_MAGIC	equ 0x1BADB002
    MULTIBOOT_HEADER_FLAGS	equ MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO | MULTIBOOT_AOUT_KLUDGE
    MULTIBOOT_CHECKSUM	equ -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)
    EXTERN code, bss, end
	dd MULTIBOOT_HEADER_MAGIC
    dd MULTIBOOT_HEADER_FLAGS
    dd MULTIBOOT_CHECKSUM
    dd mboot
    dd code
    dd bss
    dd end
    dd start
;GRUB DONE.

;All external functions available from C code
[extern 	_main]
[extern		_gp]
[extern		_idtp]

;All local functions available to C code
[global		_gdt_flush]
[global		_idt_load]

;
;	MMU/CPU Functions
;
 
[global _enable_paging]
[global _disable_paging]
[global _pmmngr_get_PDBR]
[global _pmmngr_get_CR0]
[global _enter_usermode]


_enable_paging:
	mov eax, cr0
	or eax, 0x80000000
	mov cr0, eax
ret

_disable_paging:
	mov eax, cr0
	and eax, 0x7FFFFFFF
	mov cr0, eax
ret

_pmmngr_get_PDBR:
	mov eax, cr3
ret

_pmmngr_get_CR0:
	mov eax, cr0
ret

_enter_usermode:
	cli
	mov ax, 0x23	; user mode data selector is 0x20 (GDT entry 3). Also sets RPL to 3
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	push 0x23		; SS, notice it uses same selector as above
	push esp		; ESP
	pushfd			; EFLAGS

	pop eax
	or eax, 0x200	; enable IF in EFLAGS
	push eax

	push 0x1b		; CS, user mode code selector is 0x18. With RPL 3 this is 0x1b
	lea eax, [a]	; EIP first
	push eax
	iretd
	a:
		add esp, 4 ; fix stack
ret

;
;	Landing point of kernel sub jump
;
startsub:									; Sub function from start
	call	_main							; Call main() from C code

	cli										; Clear Interrupts
	hlt										; Halt System
	jmp $									; Infinite Loop
	

_gdt_flush:
    lgdt [_gp]        						; Load the GDT with our '_gp' which is a special pointer
    mov ax, 0x10      						; 0x10 is the offset in the GDT to our data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 0x08:flush2   						; 0x08 is the offset to our code segment: Far jump!
flush2:
    ret               						; Returns back to the C code!
	
	
; Loads the IDT defined in '_idtp' into the processor.
; This is declared in C as 'extern void idt_load();'
_idt_load:
    lidt [_idtp]
    ret

	
[extern 	_ISR_HANDLER]

[global 	_ISR0]
[global 	_ISR1]
[global 	_ISR2]
[global 	_ISR3]
[global 	_ISR4]
[global 	_ISR5]
[global 	_ISR6]
[global 	_ISR7]
[global 	_ISR8]
[global 	_ISR9]
[global 	_ISR10]
[global 	_ISR11]
[global 	_ISR12]
[global 	_ISR13]
[global 	_ISR14]
[global 	_ISR15]
[global 	_ISR16]
[global 	_ISR17]
[global 	_ISR18]
[global 	_ISR19]
[global 	_ISR20]
[global 	_ISR21]
[global 	_ISR22]
[global 	_ISR23]
[global 	_ISR24]
[global 	_ISR25]
[global 	_ISR26]
[global 	_ISR27]
[global 	_ISR28]
[global 	_ISR29]
[global 	_ISR30]
[global 	_ISR31]

; In just a few pages in this tutorial, we will add our Interrupt
; Service Routines (ISRs) right here!
;  0: Divide By Zero Exception
_ISR0:
    cli
    push byte 0
    push byte 0
    jmp ISR_COMMON

;  1: Debug Exception
_ISR1:
    cli
    push byte 0
    push byte 1
    jmp ISR_COMMON

;  2: Non Maskable Interrupt Exception
_ISR2:
    cli
    push byte 0
    push byte 2
    jmp ISR_COMMON

;  3: Int 3 Exception
_ISR3:
    cli
    push byte 0
    push byte 3
    jmp ISR_COMMON

;  4: INTO Exception
_ISR4:
    cli
    push byte 0
    push byte 4
    jmp ISR_COMMON

;  5: Out of Bounds Exception
_ISR5:
    cli
    push byte 0
    push byte 5
    jmp ISR_COMMON

;  6: Invalid Opcode Exception
_ISR6:
    cli
    push byte 0
    push byte 6
    jmp ISR_COMMON

;  7: Coprocessor Not Available Exception
_ISR7:
    cli
    push byte 0
    push byte 7
    jmp ISR_COMMON

;  8: Double Fault Exception (With Error Code!)
_ISR8:
    cli
    push byte 8
    jmp ISR_COMMON

;  9: Coprocessor Segment Overrun Exception
_ISR9:
    cli
    push byte 0
    push byte 9
    jmp ISR_COMMON

; 10: Bad TSS Exception (With Error Code!)
_ISR10:
    cli
    push byte 10
    jmp ISR_COMMON

; 11: Segment Not Present Exception (With Error Code!)
_ISR11:
    cli
    push byte 11
    jmp ISR_COMMON

; 12: Stack Fault Exception (With Error Code!)
_ISR12:
    cli
    push byte 12
    jmp ISR_COMMON

; 13: General Protection Fault Exception (With Error Code!)
_ISR13:
    cli
    push byte 13
    jmp ISR_COMMON

; 14: Page Fault Exception (With Error Code!)
_ISR14:
    cli
    push byte 14
    jmp ISR_COMMON

; 15: Reserved Exception
_ISR15:
    cli
    push byte 0
    push byte 15
    jmp ISR_COMMON

; 16: Floating Point Exception
_ISR16:
    cli
    push byte 0
    push byte 16
    jmp ISR_COMMON

; 17: Alignment Check Exception
_ISR17:
    cli
    push byte 0
    push byte 17
    jmp ISR_COMMON

; 18: Machine Check Exception
_ISR18:
    cli
    push byte 0
    push byte 18
    jmp ISR_COMMON

; 19: Reserved
_ISR19:
    cli
    push byte 0
    push byte 19
    jmp ISR_COMMON

; 20: Reserved
_ISR20:
    cli
    push byte 0
    push byte 20
    jmp ISR_COMMON

; 21: Reserved
_ISR21:
    cli
    push byte 0
    push byte 21
    jmp ISR_COMMON

; 22: Reserved
_ISR22:
    cli
    push byte 0
    push byte 22
    jmp ISR_COMMON

; 23: Reserved
_ISR23:
    cli
    push byte 0
    push byte 23
    jmp ISR_COMMON

; 24: Reserved
_ISR24:
    cli
    push byte 0
    push byte 24
    jmp ISR_COMMON

; 25: Reserved
_ISR25:
    cli
    push byte 0
    push byte 25
    jmp ISR_COMMON

; 26: Reserved
_ISR26:
    cli
    push byte 0
    push byte 26
    jmp ISR_COMMON

; 27: Reserved
_ISR27:
    cli
    push byte 0
    push byte 27
    jmp ISR_COMMON

; 28: Reserved
_ISR28:
    cli
    push byte 0
    push byte 28
    jmp ISR_COMMON

; 29: Reserved
_ISR29:
    cli
    push byte 0
    push byte 29
    jmp ISR_COMMON

; 30: Reserved
_ISR30:
    cli
    push byte 0
    push byte 30
    jmp ISR_COMMON

; 31: Reserved
_ISR31:
    cli
    push byte 0
    push byte 31
    jmp ISR_COMMON

; This is our common ISR stub. It saves the processor state, sets
; up for kernel mode segments, calls the C-level fault handler,
; and finally restores the stack frame.
ISR_COMMON:
    pusha
    push ds
    push es
    push fs
    push gs
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov eax, esp
    push eax
    mov eax, _ISR_HANDLER
    call eax
    pop eax
    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8
    iret

[extern _IRQ_HANDLER]

[global _IRQ0]
[global _IRQ1]
[global _IRQ2]
[global _IRQ3]
[global _IRQ4]
[global _IRQ5]
[global _IRQ6]
[global _IRQ7]
[global _IRQ8]
[global _IRQ9]
[global _IRQ10]
[global _IRQ11]
[global _IRQ12]
[global _IRQ13]
[global _IRQ14]
[global _IRQ15]

[extern _PIT_TASK_IR]

[global __SYSTEM_IDLE_THREAD]
__SYSTEM_IDLE_THREAD:
	hlt
	jmp __SYSTEM_IDLE_THREAD

_IRQ0:		; PIT This will be different than all the others for my scheduler
	cli
	; Save Registers
	pusha
	push ds
	push es
	push fs
	push gs
	mov eax, esp
	; Call Int Routine
	push eax
	mov eax, _PIT_TASK_IR
	call eax
	; Send 0x20 to 0x20 EOI - Primary PIC
	mov al, 0x20
	out 0x20, al
	pop eax
	; Restore Registers
	; If task switch is done these values have been changed since push
	pop gs
	pop fs
	pop es
	pop ds
	popa
	; Return or Call Task
	iret
	
_IRQ1:
	cli
	push byte 0
	push byte 33
	jmp IRQ_COMMON
	
_IRQ2:
	cli
	push byte 0
	push byte 34
	jmp IRQ_COMMON
	
_IRQ3:
	cli
	push byte 0
	push byte 35
	jmp IRQ_COMMON
	
_IRQ4:
	cli
	push byte 0
	push byte 36
	jmp IRQ_COMMON
	
_IRQ5:
	cli
	push byte 0
	push byte 37
	jmp IRQ_COMMON
	
_IRQ6:
	cli
	push byte 0
	push byte 38
	jmp IRQ_COMMON
	
_IRQ7:
	cli
	push byte 0
	push byte 39
	jmp IRQ_COMMON
	
_IRQ8:
	cli
	push byte 0
	push byte 40
	jmp IRQ_COMMON
	
_IRQ9:
	cli
	push byte 0
	push byte 41
	jmp IRQ_COMMON
	
_IRQ10:
	cli
	push byte 0
	push byte 42
	jmp IRQ_COMMON
	
_IRQ11:
	cli
	push byte 0
	push byte 43
	jmp IRQ_COMMON
	
_IRQ12:
	cli
	push byte 0
	push byte 44
	jmp IRQ_COMMON
	
_IRQ13:
	cli
	push byte 0
	push byte 45
	jmp IRQ_COMMON
	
_IRQ14:
	cli
	push byte 0
	push byte 46
	jmp IRQ_COMMON
	
_IRQ15:
	cli
	push byte 0
	push byte 47
	jmp IRQ_COMMON
	
IRQ_COMMON:
	pusha
	push ds
	push es
	push fs
	push gs
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov eax, esp
	push eax
	mov eax, _IRQ_HANDLER
	call eax
	pop eax
	pop gs
	pop fs
	pop es
	pop ds
	popa
	add esp, 8				; Cleans up the pushed error code and pushed ISR number
	iret


[extern 	_INT_HANDLER]

[global 	_INT0]
[global 	_INT1]
[global 	_INT2]
[global 	_INT3]
[global 	_INT4]
[global 	_INT5]
[global 	_INT6]
[global 	_INT7]
[global 	_INT8]
[global 	_INT9]
[global 	_INT10]
[global 	_INT11]
[global 	_INT12]
[global 	_INT13]
[global 	_INT14]
[global 	_INT15]
[global 	_INT16]
[global 	_INT17]
[global 	_INT18]
[global 	_INT19]
[global 	_INT20]
[global 	_INT21]
[global 	_INT22]
[global 	_INT23]
[global 	_INT24]
[global 	_INT25]
[global 	_INT26]
[global 	_INT27]
[global 	_INT28]
[global 	_INT29]
[global 	_INT30]
[global 	_INT31]


_INT0:
    cli
    push byte 0
    push byte 0
    jmp INT_COMMON

_INT1:
    cli
    push byte 0
    push byte 1
    jmp INT_COMMON

_INT2:
    cli
    push byte 0
    push byte 2
    jmp INT_COMMON

_INT3:
    cli
    push byte 0
    push byte 3
    jmp INT_COMMON

_INT4:
    cli
    push byte 0
    push byte 4
    jmp INT_COMMON

_INT5:
    cli
    push byte 0
    push byte 5
    jmp INT_COMMON

_INT6:
    cli
    push byte 0
    push byte 6
    jmp INT_COMMON

_INT7:
    cli
    push byte 0
    push byte 7
    jmp INT_COMMON

_INT8:
    cli
	push byte 0
    push byte 8
    jmp INT_COMMON

_INT9:
    cli
    push byte 0
    push byte 9
    jmp INT_COMMON

_INT10:
    cli
	push byte 0
    push byte 10
    jmp INT_COMMON

_INT11:
    cli
	push byte 0
    push byte 11
    jmp INT_COMMON

_INT12:
    cli
	push byte 0
    push byte 12
    jmp INT_COMMON

_INT13:
    cli
	push byte 0
    push byte 13
    jmp INT_COMMON

_INT14:
    cli
	push byte 0
    push byte 14
    jmp INT_COMMON

_INT15:
    cli
    push byte 0
    push byte 15
    jmp INT_COMMON

_INT16:
    cli
    push byte 0
    push byte 16
    jmp INT_COMMON

_INT17:
    cli
    push byte 0
    push byte 17
    jmp INT_COMMON

_INT18:
    cli
    push byte 0
    push byte 18
    jmp INT_COMMON

_INT19:
    cli
    push byte 0
    push byte 19
    jmp INT_COMMON

_INT20:
    cli
    push byte 0
    push byte 20
    jmp INT_COMMON

_INT21:
    cli
    push byte 0
    push byte 21
    jmp INT_COMMON

_INT22:
    cli
    push byte 0
    push byte 22
    jmp INT_COMMON

_INT23:
    cli
    push byte 0
    push byte 23
    jmp INT_COMMON

_INT24:
    cli
    push byte 0
    push byte 24
    jmp INT_COMMON

_INT25:
    cli
    push byte 0
    push byte 25
    jmp INT_COMMON

_INT26:
    cli
    push byte 0
    push byte 26
    jmp INT_COMMON

_INT27:
    cli
    push byte 0
    push byte 27
    jmp INT_COMMON

_INT28:
    cli
    push byte 0
    push byte 28
    jmp INT_COMMON

_INT29:
    cli
    push byte 0
    push byte 29
    jmp INT_COMMON

_INT30:
    cli
    push byte 0
    push byte 30
    jmp INT_COMMON

_INT31:
    cli
    push byte 0
    push byte 31
    jmp INT_COMMON

INT_COMMON:
    pusha
    push ds
    push es
    push fs
    push gs
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov eax, esp
    push eax
    mov eax, _INT_HANDLER
    call eax
    pop eax
    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8
    iret

[global __INT_30_TEST]
__INT_30_TEST:
	push eax
	push ebx
	mov eax, 0x1EE77E57
	mov ebx, 0x12345678
	int 0x30
	pop ebx
	pop eax
	ret
	
SECTION .bss
    resb 8192               ; This reserves 8KBytes of memory here
_sys_stack:
