%include "rw32-2022.inc"

section .data
	sMessage db "Hello World!",EOL,0

section .text

CMAIN:
        push ebp
        mov ebp,esp

	mov esi,sMessage
	call WriteString

        pop ebp	
	ret