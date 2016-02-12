.model flat, stdcall
option casemap:none

STD_INPUT_HANDLE    equ -10
STD_OUTPUT_HANDLE   equ -11
GetStdHandle        proto stdcall, nStdHandle:dword
ReadFile            proto stdcall, hFile:dword, lpBuffer:ptr byte, nNumberOfBytesToRead:dword, lpNumberOfBytesRead:ptr dword, lpOverlapped:ptr dword
WriteFile           proto stdcall, hFile:dword, lpBuffer:ptr byte, nNumberOfBytesToWrite:dword, lpumberOfBytesWritten:ptr dword, lpOverlapped: ptr dword
FORMAT_MESSAGE_ALLOCATE_BUFFER equ 100h
FORMAT_MESSAGE_FROM_STRING equ 400h
FORMAT_MESSAGE_ARGUMENT_ARRAY equ 2000h
FormatMessageA      proto stdcall, dwFlags:dword, lpSource:ptr byte, dwMessageId:dword, dwLanguageId:dword, lpBuffer:ptr byte, nSize:dword, varargs:dword
LocalFree           proto stdcall, hMem:dword
ExitProcess         proto stdcall, uExitCode:dword

.const
errInvChar          db 'invalid character%n', 0
errReadErr          db 'cannot read input file%n', 0

msgFinalFloor       db 'final floor: %1!d!%n', 0

.data?
stdout              dd ?
stdin               dd ?

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

nextChar proc
local char:byte
local bytesRead:dword
    invoke  ReadFile, stdin, addr char, 1, addr bytesRead, 0
    
    .if     eax == 0
        invoke printf, offset errReadErr
        ret
    .endif
    
    .if bytesRead == 0
        xor     eax, eax
        ret
    .endif
    
    mov     al, char
    ret
nextChar endp

main proc uses ebx
;floor = ebx
    invoke  GetStdHandle, STD_INPUT_HANDLE
    mov     stdin, eax
    invoke  GetStdHandle, STD_OUTPUT_HANDLE
    mov     stdout, eax

    xor     ebx, ebx
    
    invoke  nextChar
    .while  al != 0
        .if     al == '('
            inc     ebx
        .elseif al == ')'
            dec     ebx
        .else
            invoke  printf, offset errInvChar
            invoke  ExitProcess, 1
        .endif
        
        invoke  nextChar
    .endw
    
    push    ebx
    invoke  printf, offset msgFinalFloor

    invoke  ExitProcess, 0
main endp

end