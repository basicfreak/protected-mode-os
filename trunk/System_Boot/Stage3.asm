[bits 32]
[org 0xC200]

[global start]

start:
jmp startsub

startsub:
cli
hlt