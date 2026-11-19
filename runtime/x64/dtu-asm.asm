;; Undermountain Enhanced - glue between C and the recompiled DTUGAME.EXE (x64)
;;
;; Execution model (from M-HT/SR, MIT):
;;   r11d       = emulated 32-bit x86 stack pointer (esp)
;;   _stack ptr = pointer to dtu_stack, saved by every C->asm entry (PROLOG):
;;                at [rsp] on Linux, in the home slot of rcx on Windows, so
;;                the runtime can save/restore esp around C calls
;;   CALL/RET   = macros that keep return addresses on the x86 stack

%include "x64inc.inc"

extern dtu_cstart
extern dtu_run_timer
extern dtu_frame_hook
extern dtu_seg_read
extern dtu_data_base
extern X86_InterruptFlag

global dtu_c_start
global dtu_c_call_irq
global dtu_c_call_proc
global dtu_c_call_mouse
global SR_CheckTimer
global SR_FrameHook
global SR_dtu_fpu_init
global SR_dtu_efg_format
global SR_dtu_sos_fill
global dtu_c_call_sos
global dtu_c_call_proc3
global dtu_efg_ds
global dtu_sprite_culls
global dtu_sprite_cull_on
global dtu_obj_affine
global dtu_far_detail
global dtu_weapon_light
global dtu_anim_obj
global dtu_smooth_anim
global dtu_menu_mouse
global dtu_dialog_keys
global dtu_double_lines
global dtu_phys_scale
global dtu_creature_floor
global dtu_step_gap
global dtu_sound_fallback
global dtu_sound_fb_now
global dtu_menu_ver_str
global dtu_menu_ver_y
extern x86_int
global x86_mov_reg_mem_32
global x86_mov_reg_mem_16
global dtu_timer_tick
global dtu_timer_run

%ifidn __OUTPUT_FORMAT__, elf64
section .note.GNU-stack noalloc noexec nowrite progbits
section .text progbits alloc exec nowrite align=16
%else
section .text code align=16
%endif

;; ---- entry from C: save non-volatile registers and the _stack pointer ----
%ifidn __OUTPUT_FORMAT__, win64
;; Windows x64: the _stack pointer goes into the caller's home slot of rcx
;; (FIRST_PARAMETER_OFFSET), exactly as in M-HT's Albion port.
%macro PROLOG 0
        SECTION_PROLOG
        mov [rsp+FIRST_PARAMETER_OFFSET], rcx
        mov r11d, [rcx]
%endm
%macro EPILOG 0
        mov rcx, [rsp+FIRST_PARAMETER_OFFSET]
        mov [rcx], r11d
        SECTION_EPILOG
    .end_function:
%endm
%define ARG1 rcx
%define ARG2d edx
%define ARG3d r8d
%define ARG4d r9d
%else
%macro PROLOG 0
        push rbp
        push rbx
        push rdi                ; _stack pointer; also aligns rsp to 16
        mov r11d, [rdi]
%endm
%macro EPILOG 0
        pop rdi
        mov [rdi], r11d
        pop rbx
        pop rbp
        ret
    .end_function:
%endm
%define ARG1 rdi
%define ARG2d esi
%define ARG3d edx
%define ARG4d ecx
%endif

;; int dtu_c_start(dtu_stack *s) - run the Watcom startup (cstart_) of the game
align 16
dtu_c_start:
        PROLOG
        CALL dtu_cstart
        EPILOG

;; void dtu_c_call_irq(dtu_stack *s, uint32_t handler)
;; Calls a protected-mode interrupt handler. Handlers end with iretd, which the
;; recompiler turns into "RET 8", so we push eflags and cs before the call.
align 16
dtu_c_call_irq:
        PROLOG
        mov r10d, ARG2d
        pushfq
        pop r8
        PUSH32 r8d              ; eflags
        PUSH32 dword 0x18       ; cs
        CALL r10
        EPILOG

;; void dtu_c_call_mouse(dtu_stack *s, uint32_t handler, uint32_t ax_bx, uint32_t cx_dx)
;; Calls the game's mouse event handler (int 33h function 0Ch). It is a far
;; procedure ending in retf, which the recompiler turns into "RET 4": push cs.
align 16
dtu_c_call_mouse:
        PROLOG
        mov r10d, ARG2d
        mov eax, ARG3d
        mov r9d, ARG4d
        mov ebx, eax
        shr ebx, 16             ; buttons
        movzx eax, ax           ; condition mask
        movzx ecx, r9w          ; x
        mov edx, r9d
        shr edx, 16             ; y
        xor esi, esi
        xor edi, edi
        PUSH32 dword 0x18       ; cs
        CALL r10
        EPILOG

;; uint32_t dtu_c_call_proc(dtu_stack *s, uint32_t proc, uint32_t eax_, uint32_t edx_)
;; Calls a near procedure (watcall: eax, edx) and returns eax.
align 16
dtu_c_call_proc:
        PROLOG
        mov r10d, ARG2d
        mov eax, ARG3d
        mov edx, ARG4d
        CALL r10
        EPILOG

;; uint32_t dtu_c_call_proc3(dtu_stack *s, uint32_t proc, uint32_t eax_, uint32_t edx_, uint32_t ebx_)
align 16
dtu_c_call_proc3:
        PROLOG
        mov r10d, ARG2d
%ifidn __OUTPUT_FORMAT__, win64
        mov ebx, [rsp+FIRST_PARAMETER_OFFSET+4*8]
        mov eax, ARG3d
        mov edx, ARG4d
%else
        mov ebx, r8d
        mov eax, ARG3d
        mov edx, ARG4d
%endif
        CALL r10
        EPILOG

;; called with a native "call" from loops and calls in the recompiled code
align 16
SR_CheckTimer:
%ifidn __OUTPUT_FORMAT__, win64
        push r13
    .prolog_1:
        mov r13, rsp
    .end_prolog:
%endif
        pushfq
        mov r8d, [rel dtu_timer_run]
        cmp r8d, [rel dtu_timer_tick]
        jne .run
        popfq
%ifidn __OUTPUT_FORMAT__, win64
        pop r13
%endif
        ret
    .run:
%ifidn __OUTPUT_FORMAT__, win64
        ; [rsp] flags, [rsp+8] r13, [rsp+16] return address: 3 slots above the frame
        mov r10, [rsp+FIRST_PARAMETER_OFFSET+3*8]
        mov [rsp+FIRST_PARAMETER_OFFSET+4*8], eax      ; spare home slots of the frame
        mov [rsp+FIRST_PARAMETER_OFFSET+4*8+4], ecx
        mov [rsp+FIRST_PARAMETER_OFFSET+4*8+2*4], edx
        mov [r10], r11d
        sub rsp, 5*8
        mov rcx, r10
        call dtu_run_timer
        add rsp, 5*8
        mov r10, [rsp+FIRST_PARAMETER_OFFSET+3*8]
        mov eax, [rsp+FIRST_PARAMETER_OFFSET+4*8]
        mov ecx, [rsp+FIRST_PARAMETER_OFFSET+4*8+4]
        mov edx, [rsp+FIRST_PARAMETER_OFFSET+4*8+2*4]
        mov r11d, [r10]
        popfq
        pop r13
        ret
%else
        PUSH32 eax, ecx, edx, esi, edi
        mov r10, [rsp+2*8]      ; _stack pointer (above flags and return address)
        mov [r10], r11d
        mov rdi, r10
        call dtu_run_timer      ; rsp is 16-byte aligned here
        mov r10, [rsp+2*8]
        mov r11d, [r10]
        POP32 eax, ecx, edx, esi, edi
        popfq
        ret
%endif
    .end_function:

;; called with a native "call" once per game frame (before the frame time is
;; measured, 0x40EAF): frame limiter, fps counter, then pending timer ticks
align 16
SR_FrameHook:
%ifidn __OUTPUT_FORMAT__, win64
        push r13
    .prolog_1:
        mov r13, rsp
    .end_prolog:
%endif
        pushfq
%ifidn __OUTPUT_FORMAT__, win64
        ; [rsp] flags, [rsp+8] r13, [rsp+16] return address: 3 slots above the frame
        mov r10, [rsp+FIRST_PARAMETER_OFFSET+3*8]
        mov [rsp+FIRST_PARAMETER_OFFSET+4*8], eax      ; spare home slots of the frame
        mov [rsp+FIRST_PARAMETER_OFFSET+4*8+4], ecx
        mov [rsp+FIRST_PARAMETER_OFFSET+4*8+2*4], edx
        mov [r10], r11d
        sub rsp, 5*8
        mov rcx, r10
        call dtu_frame_hook
        add rsp, 5*8
        mov r10, [rsp+FIRST_PARAMETER_OFFSET+3*8]
        mov eax, [rsp+FIRST_PARAMETER_OFFSET+4*8]
        mov ecx, [rsp+FIRST_PARAMETER_OFFSET+4*8+4]
        mov edx, [rsp+FIRST_PARAMETER_OFFSET+4*8+2*4]
        mov r11d, [r10]
        popfq
        pop r13
        ret
%else
        PUSH32 eax, ecx, edx, esi, edi
        mov r10, [rsp+2*8]      ; _stack pointer (above flags and return address)
        mov [r10], r11d
        mov rdi, r10
        call dtu_frame_hook      ; rsp is 16-byte aligned here
        mov r10, [rsp+2*8]
        mov r11d, [r10]
        POP32 eax, ecx, edx, esi, edi
        popfq
        ret
%endif
    .end_function:

;; Watcom 387 initialisation (original at 0xFE766): report a 387, no emulator
align 16
SR_dtu_fpu_init:
        fninit
        mov byte [dtu_data_base + (0x1494D4 - 0x120000)], 3    ; __8087
        mov byte [dtu_data_base + (0x14D330 - 0x120000)], 1    ; __real87
        RET

;; SOS DIGI fill handler (sos.c): called by the SOS timer service as a far
;; procedure ("push 0; call near"), returns with ret 4
align 16
SR_dtu_sos_fill:
        mov r9d, 0xEF0110
        CALL x86_int
        RET 4

;; void dtu_c_call_sos(dtu_stack *s, uint32_t handler, uint32_t drv, uint32_t action, uint32_t sample)
;; Calls a sample callback: far cdecl (WORD driver, WORD action, WORD sample), ends in retf.
align 16
dtu_c_call_sos:
        PROLOG
        mov r10d, ARG2d
        mov eax, ARG3d
        mov edx, ARG4d
%ifidn __OUTPUT_FORMAT__, win64
        mov ecx, [rsp+FIRST_PARAMETER_OFFSET+4*8]
%else
        mov ecx, r8d
%endif
        PUSH32 ecx
        PUSH32 edx
        PUSH32 eax
        PUSH32 dword 0x18       ; cs
        CALL r10
        add r11d, 12
        EPILOG

;; Watcom _EFG_Format (original at 0x1151E0, called through [0x1532D4] by printf for
;; %e/%f/%g): eax = buffer, edx = &va_list, ebx = format specs. Done in C (dpmi.c,
;; crt_efg_format) via x86_int; returns eax = string, edx = ds.
align 16
SR_dtu_efg_format:
        mov r10d, ds
        mov [rel dtu_efg_ds], r10d
        mov r9d, 0xEF0000
        jmp x86_int

;; segment-prefixed memory read with a constant offset (r10d = offset) -> r9d
%macro SEG_READ 1
        pushfq
        push rax
        push rcx
        push rdx
        push rsi
        push rdi
        push r11
%ifidn __OUTPUT_FORMAT__, win64
        sub rsp, 5*8            ; home slots + alignment
        mov ecx, r10d
        mov edx, %1
        call dtu_seg_read
        add rsp, 5*8
%else
        sub rsp, 8
        mov edi, r10d
        mov esi, %1
        call dtu_seg_read
        add rsp, 8
%endif
        mov r9d, eax
        pop r11
        pop rdi
        pop rsi
        pop rdx
        pop rcx
        pop rax
        popfq
        RET
%endm

align 16
x86_mov_reg_mem_32:
        SEG_READ 4
align 16
x86_mov_reg_mem_16:
        SEG_READ 2

%ifidn __OUTPUT_FORMAT__, win64
section .pdata rdata align=4
        P_UNWIND_INFO dtu_c_start, dtu_c_start.end_function, x_common
        P_UNWIND_INFO dtu_c_call_irq, dtu_c_call_irq.end_function, x_common
        P_UNWIND_INFO dtu_c_call_proc, dtu_c_call_proc.end_function, x_common
        P_UNWIND_INFO SR_CheckTimer, SR_CheckTimer.end_function, x_SR_CheckTimer
        P_UNWIND_INFO SR_FrameHook, SR_FrameHook.end_function, x_SR_FrameHook
        P_UNWIND_INFO dtu_c_call_mouse, dtu_c_call_mouse.end_function, x_common
        P_UNWIND_INFO dtu_c_call_sos, dtu_c_call_sos.end_function, x_common
        P_UNWIND_INFO dtu_c_call_proc3, dtu_c_call_proc3.end_function, x_common
section .xdata rdata align=8
align 8
x_common:
        X_UNWIND_INFO dtu_c_start
align 8
x_SR_CheckTimer:
        db 1                                            ; version 1, no flags
        db SR_CheckTimer.end_prolog - SR_CheckTimer     ; size of prolog
        db 2                                            ; count of unwind codes
        db 0x0d                                         ; frame register r13, offset 0
        db SR_CheckTimer.end_prolog - SR_CheckTimer
        db 0x03                                         ; UWOP_SET_FPREG
        db SR_CheckTimer.prolog_1 - SR_CheckTimer
        db 0xd0                                         ; UWOP_PUSH_NONVOL r13
align 8
x_SR_FrameHook:
        db 1
        db SR_FrameHook.end_prolog - SR_FrameHook
        db 2
        db 0x0d
        db SR_FrameHook.end_prolog - SR_FrameHook
        db 0x03
        db SR_FrameHook.prolog_1 - SR_FrameHook
        db 0xd0
%endif

%ifidn __OUTPUT_FORMAT__, elf64
section .data progbits alloc noexec write align=16
%else
section .data data align=16
%endif

dtu_timer_tick: dd 0
dtu_efg_ds: dd 0
dtu_sprite_culls: dd 0
dtu_sprite_cull_on: dd 1
dtu_obj_affine: dd 0
dtu_far_detail: dd 99
dtu_weapon_light: dd 1
dtu_anim_obj: dd 0
dtu_smooth_anim: dd 1
dtu_menu_mouse: dd 1
dtu_dialog_keys: dd 1
; 3D view: 1 = original (rendered at half height, every line written twice); 0 = all 480 lines
dtu_double_lines: dd 0
; physics integration time per frame (16.16 factor on the frame time), set by io.c
dtu_phys_scale: dd 0x10000
; 1 = creatures are drawn with their feet on the floor (render.c); 0 = original
dtu_creature_floor: dd 1
; minimum time between two footsteps of one creature in ms (render.c; 0 = original)
dtu_step_gap: dd 200
; 1 = a one-shot sound linked to an object that finds all 16 sound objects in use
;     is played at the object's position instead of not at all (render.c)
dtu_sound_fallback: dd 1
dtu_sound_fb_now: dd 0
; the version line under the copyright in the main menu (menu.c): text (0 = none) and y
dtu_menu_ver_str: dd 0
dtu_menu_ver_y: dd 0
dtu_timer_run:  dd 0

;; Targets of the self-modifying MVE block decoder (it patches ModRM bytes in
;; code we cannot run). Writes land here harmlessly; movies decode wrongly
;; until the MVE player is replaced.
%macro SMC 1
global loc_%1
loc_%1: times 256 db 0x90
%endm
align 16
SMC 10CD96
SMC 10CEE2
SMC 10D04E
SMC 10D256
SMC 10D426
SMC 10D68D
SMC 10D7F1
SMC 10D961
SMC 10DB21
SMC 10DD39
SMC 10DFD5
SMC 10E251
SMC 10E7C6
SMC 10E8AE
SMC 10E9B2
SMC 10EB0A
SMC 10EC1E
SMC 10ED79
SMC 10EE65
SMC 10EF61
SMC 10F0A1
SMC 10F1F1
SMC 10F369
SMC 10F4C5
SMC 10F9F9
SMC 10FAF9
SMC 10FC75
SMC 10FE31
SMC 10FFB5
SMC 11021A
SMC 110449
SMC 1105E5
SMC 11076A
SMC 110A52
SMC 110DFE
SMC 111176

%ifidn __OUTPUT_FORMAT__, elf64
section .text progbits alloc exec nowrite align=16
%else
section .text code align=16
%endif
;; default protected-mode interrupt handler: "iretd" (frame of eip, cs, eflags)
global dtu_iret_stub
align 16
dtu_iret_stub:
        RET 8

;; Watcom int386()/intr() jump into a table of "int N; ret" stubs at
;; 0x10A28A + 3*N. The recompiled callers store 3*N in dtu_int_num and
;; "return" here instead.
global dtu_int_dispatch
global dtu_int_num
align 16
dtu_int_dispatch:
        mov r9d, [rel dtu_int_num]
        imul r9d, r9d, 43691            ; /3 for values below 2^16
        shr r9d, 17
        CALL x86_int
        RET

%ifidn __OUTPUT_FORMAT__, elf64
section .data progbits alloc noexec write align=16
%else
section .data data align=16
%endif
dtu_int_num: dd 0

;; ES/FS/GS emulation (see tools/gen_fsgs.py): current selectors, their bases,
;; and the base of every selector (kept up to date by dpmi.c)
global dtu_es_sel
global dtu_es_base
global dtu_fs_sel
global dtu_gs_sel
global dtu_fs_base
global dtu_gs_base
global dtu_sel_base
%ifidn __OUTPUT_FORMAT__, elf64
section .data progbits alloc noexec write align=16
%else
section .data data align=16
%endif
global dtu_zero_page
global dtu_friction
dtu_friction: dd 21846          ; 65536/3: the game's velocity damping per frame (see io.c)
dtu_zero_page: times 16 db 0     ; stands in for NULL strings the game reads every frame
dtu_es_sel:  dd 0
dtu_es_base: dd 0
dtu_fs_sel:  dd 0
dtu_gs_sel:  dd 0
dtu_fs_base: dd 0
dtu_gs_base: dd 0
%ifidn __OUTPUT_FORMAT__, elf64
section .bss nobits alloc noexec write align=16
%else
;; own section: must come after .dosmem in the Windows image (see dosmem.asm)
section dtubss bss align=16
%endif
dtu_sel_base: resd 65536
