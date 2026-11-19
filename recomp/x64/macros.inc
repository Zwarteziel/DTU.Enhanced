;;
;;  Copyright (C) 2016-2026 Roman Pauer
;;
;;  Permission is hereby granted, free of charge, to any person obtaining a copy of
;;  this software and associated documentation files (the "Software"), to deal in
;;  the Software without restriction, including without limitation the rights to
;;  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
;;  of the Software, and to permit persons to whom the Software is furnished to do
;;  so, subject to the following conditions:
;;
;;  The above copyright notice and this permission notice shall be included in all
;;  copies or substantial portions of the Software.
;;
;;  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
;;  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
;;  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
;;  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
;;  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
;;  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
;;  SOFTWARE.
;;

%macro ACTION_UNCONDITIONAL_SHORT_JUMP_BACKWARD 0
    call SR_CheckTimer
%endmacro

%macro ACTION_UNCONDITIONAL_SHORT_JUMP_FORWARD 0
    ; do nothing
%endmacro

%macro ACTION_UNCONDITIONAL_JUMP 0
    call SR_CheckTimer
%endmacro

%macro ACTION_LOOP_BACKWARD 0
    call SR_CheckTimer
%endmacro

%macro ACTION_LOOP_FORWARD 0
    ; do nothing
%endmacro

%macro ACTION_CONDITIONAL_JUMP_BACKWARD 0
    call SR_CheckTimer
%endmacro

%macro ACTION_CONDITIONAL_JUMP_FORWARD 0
    ; do nothing
%endmacro

%macro ACTION_CALL 0
    call SR_CheckTimer
%endmacro

; ------------------------------------------------------------------------------
; Descent to Undermountain additions

%macro JECXZ_FAR 1
        db 0x67, 0xE3, 0x05             ; jecxz +5 (to the far jump)
        jmp strict near %%next
        jmp near %1
    %%next:
%endmacro

%macro JCXZ_FAR 1
        push rcx
        movzx ecx, cx
        jrcxz %%take
        pop rcx
        jmp %%next
    %%take:
        pop rcx
        jmp %1
    %%next:
%endmacro

%macro OUTSB_1 0
        ; keeps the native stack untouched: the runtime reads the _stack pointer at [rsp]
        PUSH32 eax
        mov al, [esi]
        lea esi, [esi+1]
        CALL x86_out_dx_al
        POP32 eax
%endmacro

%macro REP_OUTSB 0
        PUSHFD
        PUSH32 eax
    %%loop:
        test ecx, ecx
        jz near %%done
        mov al, [esi]
        lea esi, [esi+1]
        CALL x86_out_dx_al
        lea ecx, [ecx-1]
        jmp near %%loop
    %%done:
        POP32 eax
        POPFD
%endmacro

%macro LOOP_FAR 1
        db 0x67, 0xE2, 0x05             ; a32 loop +5 (to the far jump)
        jmp strict near %%next
        jmp near %1
    %%next:
%endmacro
