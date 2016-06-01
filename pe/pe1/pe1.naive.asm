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
ExitProcess         proto stdcall, uExitCode:dword

.const
outputMsg           db 'Result: %1!d!%n', 0

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

main proc uses ebx esi edi
    invoke  GetStdHandle, STD_OUTPUT_HANDLE
    mov     stdout, eax

    mov     ebx, 3
    mov     esi, 5
    mov     ecx, 999
    xor     edx, edx
    xor     edi, edi
    
    by3:
        mov     eax, ecx
        xor     edx, edx
        div     ebx
        test    edx, edx
        jnz     by5
        
        add     edi, ecx
        jmp     next
        
    by5:
        mov     eax, ecx
        xor     edx, edx
        div     esi
        test    edx, edx
        jnz     next
        
        add     edi, ecx
        
    next:
    loop by3
    
    push    edi
    invoke  printf, offset outputMsg
    add     esp, 4

    ret
main endp

end
