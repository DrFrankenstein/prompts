;Multiples of 3 and 5 <https://projecteuler.net/problem=1>
;If we list all the natural numbers below 10 that are multiples of 3 or 5, we get 3, 5, 6 and 9. The sum of these multiples is 23.
;Find the sum of all the multiples of 3 or 5 below 1000.

;Assemble with Microsoft Macro Assembler (tested with Version 10.00.40219.01 (Visual Studio 2012)) and run on any 32-bit Windows.

.model flat, stdcall
option casemap:none

STD_OUTPUT_HANDLE   equ -11
GetStdHandle        proto stdcall, nStdHandle:dword
WriteFile           proto stdcall, hFile:dword, lpBuffer:ptr byte, nNumberOfBytesToWrite:dword, lpumberOfBytesWritten:ptr dword, lpOverlapped: ptr dword
FORMAT_MESSAGE_ALLOCATE_BUFFER equ 100h
FORMAT_MESSAGE_FROM_STRING equ 400h
FORMAT_MESSAGE_ARGUMENT_ARRAY equ 2000h
FormatMessageA      proto stdcall, dwFlags:dword, lpSource:ptr byte, dwMessageId:dword, dwLanguageId:dword, lpBuffer:ptr byte, nSize:dword, varargs:dword
LocalFree           proto stdcall, hMem:dword

LIMIT               equ 1000

.const
outputMsg           db 'Result: %1!u!%n', 0

.data?
stdout              dd ?

.code

printf proc, format:ptr byte;,...
local buffer:ptr byte
    invoke  FormatMessageA, \
            FORMAT_MESSAGE_ALLOCATE_BUFFER or FORMAT_MESSAGE_FROM_STRING or FORMAT_MESSAGE_ARGUMENT_ARRAY, \
            format, 0, 0, addr buffer, 0, addr format + 4
    invoke  WriteFile, stdout, buffer, eax, 0, 0
    
    invoke  LocalFree, buffer
    ret
printf endp

main proc
;sum = eax
;number = ecx
    invoke  GetStdHandle, STD_OUTPUT_HANDLE
    mov     stdout, eax

    xor     eax, eax        ;sum = 0
    
    mov     ecx, (LIMIT - 1) / 3 * 3        ;highest multiple of 3
    .repeat
        add     eax, ecx
        sub     ecx, 3
    .until zero?
    
    mov     ecx, (LIMIT - 1) / 5 * 5        ;highest multiple of 5
    .repeat
        add     eax, ecx
        sub     ecx, 5
    .until zero?
    
    mov     ecx, (LIMIT - 1) / 15 * 15      ;highest multiple of 15
    .repeat
        sub     eax, ecx
        sub     ecx, 15
    .until zero?
    
    push    eax
    invoke  printf, offset outputMsg
    add     esp, 4

    ret
main endp

end
