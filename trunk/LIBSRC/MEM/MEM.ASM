;/LIBSRC/MEM/MEM.ASM
[bits 32]

[global _enable_paging]
[global _disable_paging]
[global _pmmngr_get_PDBR]


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