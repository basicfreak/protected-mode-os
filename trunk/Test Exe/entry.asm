[bits 32]

[global entry]
[extern _main]

entry:
	call _main
	jmp $
