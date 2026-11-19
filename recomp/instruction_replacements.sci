loc_DDA77,2,test ax, 0cf89h|jmp skp_DDA77
loc_DDA79,2,mov edi, ecx|skp_DDA77:
loc_DDAAC,2,test ax,0xcf89|jmp skp_DDAAC
loc_DDAAE,2,mov edi,ecx|skp_DDAAC:
loc_F31F9,2,test ax,0xcf89|jmp skp_F31F9
loc_F31FB,2,mov edi,ecx|skp_F31F9:
loc_1006EC,3,mov eax, 0FFFFFFFFh ; lsl: DS limit = 4 GB
loc_100903,3,mov eax, 0FFFFFFFFh ; lsl: segment limit 4 GB
loc_FE88E,3,mov ecx, 0x10 ; cr0 read (FPU present, ET=1)
loc_FE897,3,nop ; cr0 write ignored
loc_FE95C,3,mov ecx, 0x10 ; cr0 read
loc_FE96C,3,nop ; cr0 write ignored
loc_118626,3,mov eax, 0x10 ; cr0 read
loc_11862D,3,nop ; cr0 write ignored
loc_11868D,3,mov eax, 0x10 ; cr0 read
loc_118692,3,nop ; cr0 write ignored
loc_102C7A,3,mov ecx, 0x00C0F300 ; lar: access rights of allocated selector
loc_102D1F,3,mov ecx, 0x00C0F300 ; lar
loc_102D0E,3,mov edx, 0xFFFFFFFF ; lsl edx, ebx
loc_103421,3,mov ecx, 0x00C0F300 ; lar ecx, ecx
loc_F2C71,7,mov esi, [dtu_env_base] ; DOS4GW: env far pointer -> flat env block
loc_F2C78,8,xor ecx, ecx ; env selector from PSP:2Ch not needed
loc_F2CA6,8,nop ; store ds into code (cs:[F2DE1])
loc_F2CD1,4,add edi, [dtu_psp_base]|mov cl, [edi-0x1] ; command tail from fake PSP
loc_F2CF6,8,nop ; mov ds, env selector
loc_102E6E,3,mov r9d, 0x1102E6E|CALL x86_int ; far call into runtime-loaded driver (HMI SOS)
loc_102F80,3,mov r9d, 0x1102F80|CALL x86_int ; far call into runtime-loaded driver (HMI SOS)
loc_102FB4,3,mov r9d, 0x1102FB4|CALL x86_int ; far call into runtime-loaded driver (HMI SOS)
loc_10300C,3,mov r9d, 0x110300C|CALL x86_int ; far call into runtime-loaded driver (HMI SOS)
loc_103037,3,mov r9d, 0x1103037|CALL x86_int ; far call into runtime-loaded driver (HMI SOS)
loc_10305F,3,mov r9d, 0x110305F|CALL x86_int ; far call into runtime-loaded driver (HMI SOS)
loc_103088,3,mov r9d, 0x1103088|CALL x86_int ; far call into runtime-loaded driver (HMI SOS)
loc_10321C,3,mov r9d, 0x110321C|CALL x86_int ; far call into runtime-loaded driver (HMI SOS)
loc_10320D,3,mov r9d, 0x110320D|CALL x86_int ; far call into runtime-loaded driver (HMI SOS)
loc_DFB97,4,mov ax, 0 ; mov ax, DGROUP (selector fixup)
loc_F6D9C,4,mov ax, 0 ; mov ax, DGROUP (selector fixup)
loc_10A1E7,7,mov [dtu_int_num], esi|lea eax, [dtu_int_dispatch] ; int386x: stub table "int N; ret" -> runtime dispatcher
loc_10A25E,7,mov [dtu_int_num], eax|lea eax, [dtu_int_dispatch] ; intr: same
loc_102DE7,3,mov r9d, 0x1102DE7|CALL x86_int ; far call into runtime-loaded driver (HMI SOS)
loc_102E1E,3,mov r9d, 0x1102E1E|CALL x86_int ; far call into runtime-loaded driver (HMI SOS)
loc_102E93,3,mov r9d, 0x1102E93|CALL x86_int ; far call into runtime-loaded driver (HMI SOS)
loc_102EB8,3,mov r9d, 0x1102EB8|CALL x86_int ; far call into runtime-loaded driver (HMI SOS)
loc_102EFF,3,mov r9d, 0x1102EFF|CALL x86_int ; far call into runtime-loaded driver (HMI SOS)
loc_102F5B,3,mov r9d, 0x1102F5B|CALL x86_int ; far call into runtime-loaded driver (HMI SOS)
loc_103147,3,mov r9d, 0x1103147|CALL x86_int ; far call into runtime-loaded driver (HMI SOS)
loc_78A24,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_78A25,2,mov r10d, [dtu_fs_sel]|PUSH32 r10d ; fsgs
loc_78A27,2,mov r10d, [dtu_gs_sel]|PUSH32 r10d ; fsgs
loc_78AEA,2,movzx r10d, word [r11d]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_78AEC,2,movzx r10d, word [r11d]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_78AEE,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_D7CDB,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_D7CDE,2,movzx r10d, ax|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_D7CF0,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_D81E6,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_D81E9,2,movzx r10d, ax|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_D81FB,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_D8356,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_D8359,2,movzx r10d, ax|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_D836B,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_D84E8,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_D84EA,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_D850C,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_D8510,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_D8511,2,mov r10d, [dtu_fs_sel]|PUSH32 r10d ; fsgs
loc_D8513,2,mov r10d, [dtu_gs_sel]|PUSH32 r10d ; fsgs
loc_D855E,2,movzx r10d, word [r11d]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_D8560,2,movzx r10d, word [r11d]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_D8562,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_D8568,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_D8569,2,mov r10d, [dtu_fs_sel]|PUSH32 r10d ; fsgs
loc_D856B,2,mov r10d, [dtu_gs_sel]|PUSH32 r10d ; fsgs
loc_D88AC,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_D88AE,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_D88D0,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_D88F4,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_D88F6,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_D8918,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_D8A10,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_D8A13,2,movzx r10d, ax|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_D8A1F,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_D8A30,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_D8A33,2,movzx r10d, ax|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_D8A45,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_DA6AA,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_DA6AB,2,mov r10d, [dtu_fs_sel]|PUSH32 r10d ; fsgs
loc_DA6AD,2,mov r10d, [dtu_gs_sel]|PUSH32 r10d ; fsgs
loc_DA934,2,movzx r10d, word [r11d]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_DA936,2,movzx r10d, word [r11d]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_DA938,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_DAE40,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_DAE41,2,mov r10d, [dtu_fs_sel]|PUSH32 r10d ; fsgs
loc_DAE43,2,mov r10d, [dtu_gs_sel]|PUSH32 r10d ; fsgs
loc_DAF13,2,movzx r10d, word [r11d]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_DAF15,2,movzx r10d, word [r11d]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_DAF17,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_DDA6B,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_DDA70,2,movzx r10d, dx|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_DDA7B,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_DDA81,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_DDA84,2,movzx r10d, ax|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_DDA90,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_DDAA0,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_DDAA5,2,movzx r10d, dx|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_DDAB0,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_DDABD,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_DDAC0,2,movzx r10d, ax|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_DDACD,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_DDBD1,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_DDBD4,2,movzx r10d, ax|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_DDBE0,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_DFB95,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_DFB9E,3,movzx r10d, ax|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_DFBF1,2,mov r10d, [dtu_fs_sel]|PUSH32 r10d ; fsgs
loc_DFBF3,2,mov r10d, [dtu_gs_sel]|PUSH32 r10d ; fsgs
loc_DFC16,2,movzx r10d, word [r11d]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_DFC18,2,movzx r10d, word [r11d]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_DFC53,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_DFC58,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_DFD1D,7,mov r10d, [dtu_es_sel]|mov word [loc_149C2D], r10w ; fsgs
loc_DFD65,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_E36D7,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_E36D9,3,movzx r10d, dx|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_E36E5,3,mov r10d, [dtu_es_base]|mov byte [r10d + edi], al ; fsgs
loc_E36F6,4,mov r10d, [dtu_es_base]|mov byte [r10d + edi + 0x1], al ; fsgs
loc_E3702,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_E373F,4,movzx r10d, word [r11d + 0x70]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E3743,5,mov r10d, [dtu_gs_base]|mov ax, word [r10d + edi + 0x8] ; fsgs
loc_E374C,4,mov r10d, [dtu_gs_base]|mov eax, dword [r10d + edi + 0x4] ; fsgs
loc_E3754,4,mov r10d, [dtu_gs_base]|mov eax, dword [r10d + edi + 0xc] ; fsgs
loc_E375C,4,mov r10d, [dtu_gs_base]|mov eax, dword [r10d + edi + 0x10] ; fsgs
loc_E3769,4,mov r10d, [dtu_gs_base]|mov eax, dword [r10d + edi + 0x14] ; fsgs
loc_E3773,4,mov r10d, [dtu_gs_base]|mov eax, dword [r10d + edi + 0x18] ; fsgs
loc_E3777,3,mov r10d, [dtu_gs_base]|mov esi, dword [r10d + edi] ; fsgs
loc_E37C5,4,movzx r10d, word [r11d + 0x70]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E37D0,4,mov r10d, [dtu_gs_base]|mov ebx, dword [r10d + edi + 0x24] ; fsgs
loc_E37D8,5,mov r10d, [dtu_gs_base]|mov dx, word [r10d + edi + 0x28] ; fsgs
loc_E37E6,5,mov r10d, [dtu_gs_base]|mov ax, word [r10d + edi + 0x28] ; fsgs
loc_E37F0,4,mov r10d, [dtu_gs_base]|mov eax, dword [r10d + edi + 0x24] ; fsgs
loc_E37F8,5,mov r10d, [dtu_gs_base]|mov ax, word [r10d + edi + 0x30] ; fsgs
loc_E3802,4,mov r10d, [dtu_gs_base]|mov eax, dword [r10d + edi + 0x2c] ; fsgs
loc_E384C,4,movzx r10d, word [r11d + 0x70]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E3854,5,mov r10d, [dtu_gs_base]|mov word [r10d + edi + 0x28], ax ; fsgs
loc_E385D,4,mov r10d, [dtu_gs_base]|mov dword [r10d + edi + 0x24], eax ; fsgs
loc_E3865,5,mov r10d, [dtu_gs_base]|mov word [r10d + edi + 0x30], ax ; fsgs
loc_E386D,4,mov r10d, [dtu_gs_base]|mov dword [r10d + edi + 0x2c], eax ; fsgs
loc_E39A9,4,movzx r10d, word [r11d + 0x50]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E39B0,4,mov r10d, [dtu_gs_base]|mov byte [r10d + edx], 0x0 ; fsgs
loc_E39C1,4,movzx r10d, word [r11d + 0x50]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E39C8,4,mov r10d, [dtu_gs_base]|mov byte [r10d + eax], 0x80 ; fsgs
loc_E39D9,4,movzx r10d, word [r11d + 0x50]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E39E0,4,mov r10d, [dtu_gs_base]|mov byte [r10d + edx], 0x0 ; fsgs
loc_E3A00,4,movzx r10d, word [r11d + 0x70]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E3A04,4,mov r10d, [dtu_gs_base]|mov eax, dword [r10d + edi + 0x48] ; fsgs
loc_E3A0C,4,movzx r10d, word [r11d + 0x70]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E3A14,5,mov r10d, [dtu_gs_base]|mov word [r10d + edi + 0x8], ax ; fsgs
loc_E3A1D,4,mov r10d, [dtu_gs_base]|mov dword [r10d + edi + 0x4], eax ; fsgs
loc_E3A21,4,mov r10d, [dtu_gs_base]|mov ecx, dword [r10d + edi + 0x34] ; fsgs
loc_E3A29,5,mov r10d, [dtu_gs_base]|mov dx, word [r10d + edi + 0x38] ; fsgs
loc_E3A2E,4,mov r10d, [dtu_gs_base]|mov dword [r10d + edi + 0x48], eax ; fsgs
loc_E3A54,4,mov r10d, [dtu_gs_base]|mov edx, dword [r10d + edi + 0x44] ; fsgs
loc_E3A7B,4,mov r10d, [dtu_gs_base]|mov eax, dword [r10d + edi + 0x44] ; fsgs
loc_E3AD1,5,mov r10d, [dtu_gs_base]|mov ax, word [r10d + edi + 0x38] ; fsgs
loc_E3ADB,4,mov r10d, [dtu_gs_base]|mov eax, dword [r10d + edi + 0x34] ; fsgs
loc_E3AE3,5,mov r10d, [dtu_gs_base]|mov ax, word [r10d + edi + 0x40] ; fsgs
loc_E3AED,4,mov r10d, [dtu_gs_base]|mov eax, dword [r10d + edi + 0x3c] ; fsgs
loc_E3B49,4,movzx r10d, word [r11d + 0x70]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E3B51,5,mov r10d, [dtu_gs_base]|mov word [r10d + edi + 0x38], ax ; fsgs
loc_E3B5A,4,mov r10d, [dtu_gs_base]|mov dword [r10d + edi + 0x34], eax ; fsgs
loc_E3B62,5,mov r10d, [dtu_gs_base]|mov word [r10d + edi + 0x40], ax ; fsgs
loc_E3B74,4,mov r10d, [dtu_gs_base]|mov dword [r10d + edi + 0x3c], eax ; fsgs
loc_E3BB0,4,movzx r10d, word [r11d + 0x54]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E3BBD,4,mov r10d, [dtu_gs_base]|mov eax, dword [r10d + eax + 0x8] ; fsgs
loc_E3C8C,4,movzx r10d, word [r11d + 0x70]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E3C90,5,mov r10d, [dtu_gs_base]|mov word [r10d + edi + 0x20], dx ; fsgs
loc_E3C95,4,mov r10d, [dtu_gs_base]|mov dword [r10d + edi + 0x1c], eax ; fsgs
loc_E3C99,4,movzx r10d, word [r11d + 0x54]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E3CA1,4,mov r10d, [dtu_gs_base]|mov ebx, dword [r10d + eax + 0x8] ; fsgs
loc_E3CCB,4,movzx r10d, word [r11d + 0x54]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E3CCF,4,mov r10d, [dtu_gs_base]|mov ebx, dword [r10d + ebx + 0xc] ; fsgs
loc_E3CE6,4,mov r10d, [dtu_gs_base]|mov eax, dword [r10d + eax + 0x8] ; fsgs
loc_E3CEF,4,mov r10d, [dtu_gs_base]|mov ecx, dword [r10d + ecx + 0x4] ; fsgs
loc_E3CF3,3,mov r10d, [dtu_gs_base]|mov edx, dword [r10d + edx] ; fsgs
loc_E3DBC,4,movzx r10d, word [r11d + 0x70]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E3DC2,3,mov r10d, [dtu_gs_base]|mov edx, dword [r10d + edi] ; fsgs
loc_E3DCC,5,mov r10d, [dtu_gs_base]|mov bx, word [r10d + edi + 0x8] ; fsgs
loc_E3DD1,4,mov r10d, [dtu_gs_base]|mov edx, dword [r10d + edi + 0x4] ; fsgs
loc_E3DE4,4,mov r10d, [dtu_gs_base]|mov edx, dword [r10d + edi + 0xc] ; fsgs
loc_E3DEF,4,mov r10d, [dtu_gs_base]|mov edx, dword [r10d + edi + 0x10] ; fsgs
loc_E3E05,4,mov r10d, [dtu_gs_base]|mov edx, dword [r10d + edi + 0x18] ; fsgs
loc_E3E10,5,mov r10d, [dtu_gs_base]|mov bx, word [r10d + edi + 0x20] ; fsgs
loc_E3E15,4,mov r10d, [dtu_gs_base]|mov edx, dword [r10d + edi + 0x1c] ; fsgs
loc_E3E28,5,mov r10d, [dtu_gs_base]|mov bx, word [r10d + edi + 0x28] ; fsgs
loc_E3E2D,4,mov r10d, [dtu_gs_base]|mov edx, dword [r10d + edi + 0x24] ; fsgs
loc_E3E40,5,mov r10d, [dtu_gs_base]|mov bx, word [r10d + edi + 0x38] ; fsgs
loc_E3E45,4,mov r10d, [dtu_gs_base]|mov edx, dword [r10d + edi + 0x34] ; fsgs
loc_E3E58,5,mov r10d, [dtu_gs_base]|mov bx, word [r10d + edi + 0x30] ; fsgs
loc_E3E5D,4,mov r10d, [dtu_gs_base]|mov edx, dword [r10d + edi + 0x2c] ; fsgs
loc_E3E70,5,mov r10d, [dtu_gs_base]|mov bx, word [r10d + edi + 0x40] ; fsgs
loc_E3E75,4,mov r10d, [dtu_gs_base]|mov edx, dword [r10d + edi + 0x3c] ; fsgs
loc_E3E88,4,mov r10d, [dtu_gs_base]|mov edx, dword [r10d + edi + 0x44] ; fsgs
loc_E3E93,5,mov eax, dword [r11d + 0x74]|movzx r10d, word [r11d + 0x74 + 4]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E3E98,3,mov r10d, [dtu_gs_base]|mov dword [r10d + eax], ebp ; fsgs
loc_E4152,2,movzx r10d, ax|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4154,7,mov r10d, [dtu_gs_base]|mov dword [r10d + esi], 0x1000 ; fsgs
loc_E4180,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_E4185,2,movzx r10d, dx|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_E4196,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_E4239,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_E423E,2,movzx r10d, dx|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_E424F,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_E4505,4,movzx r10d, word [r11d + 0x30]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E450D,3,mov r10d, [dtu_gs_base]|mov dword [r10d + ebx], eax ; fsgs
loc_E489C,4,movzx r10d, word [r11d + 0xc]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E48A0,4,mov r10d, [dtu_gs_base]|mov ecx, dword [r10d + ebx + 0x2e] ; fsgs
loc_E48B1,4,movzx r10d, word [r11d + 0x8]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E48B5,5,mov r10d, [dtu_gs_base]|mov di, word [r10d + esi + 0x4] ; fsgs
loc_E48BA,3,mov r10d, [dtu_gs_base]|mov ecx, dword [r10d + esi] ; fsgs
loc_E48BD,4,movzx r10d, word [r11d + 0xc]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E48C1,5,mov r10d, [dtu_gs_base]|mov word [r10d + ebx + 0x4], di ; fsgs
loc_E48C6,3,mov r10d, [dtu_gs_base]|mov dword [r10d + ebx], ecx ; fsgs
loc_E48DA,4,movzx r10d, word [r11d + 0x8]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E48DE,5,mov r10d, [dtu_gs_base]|mov di, word [r10d + esi + 0x4] ; fsgs
loc_E48E3,3,mov r10d, [dtu_gs_base]|mov ecx, dword [r10d + esi] ; fsgs
loc_E48E6,4,movzx r10d, word [r11d + 0x4]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E48EA,5,mov r10d, [dtu_gs_base]|mov word [r10d + ebx + 0xc], di ; fsgs
loc_E48EF,4,mov r10d, [dtu_gs_base]|mov dword [r10d + ebx + 0x8], ecx ; fsgs
loc_E48F3,4,movzx r10d, word [r11d + 0x8]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E48F7,5,mov r10d, [dtu_gs_base]|mov di, word [r10d + esi + 0x4] ; fsgs
loc_E48FC,3,mov r10d, [dtu_gs_base]|mov ebx, dword [r10d + esi] ; fsgs
loc_E48FF,4,mov r10d, [dtu_gs_base]|mov ecx, dword [r10d + esi + 0x30] ; fsgs
loc_E4903,6,movzx r10d, word [eax + loc_15046E]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4911,5,mov r10d, [dtu_gs_base]|mov word [r10d + ecx + 0x14], di ; fsgs
loc_E4916,4,mov r10d, [dtu_gs_base]|mov dword [r10d + ecx + 0x10], ebx ; fsgs
loc_E491A,4,movzx r10d, word [r11d + 0x8]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E491E,5,mov r10d, [dtu_gs_base]|test byte [r10d + esi + 0x28], 0x40 ; fsgs
loc_E4932,4,mov r10d, [dtu_gs_base]|mov ecx, dword [r10d + esi + 0x30] ; fsgs
loc_E4936,2,movzx r10d, di|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4938,4,mov r10d, [dtu_gs_base]|mov dword [r10d + ebx + 0x18], ecx ; fsgs
loc_E4949,4,movzx r10d, word [r11d + 0x8]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E494D,4,mov r10d, [dtu_gs_base]|mov ebx, dword [r10d + esi + 0x30] ; fsgs
loc_E4951,2,movzx r10d, di|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4953,4,mov r10d, [dtu_gs_base]|mov dword [r10d + ecx + 0x1c], ebx ; fsgs
loc_E4964,4,movzx r10d, word [r11d + 0x8]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4968,4,mov r10d, [dtu_gs_base]|mov ecx, dword [r10d + esi + 0x34] ; fsgs
loc_E496C,2,movzx r10d, di|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E496E,4,mov r10d, [dtu_gs_base]|mov dword [r10d + ebx + 0x20], ecx ; fsgs
loc_E4972,4,movzx r10d, word [r11d + 0x8]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4976,4,mov r10d, [dtu_gs_base]|mov ebx, dword [r10d + esi + 0x30] ; fsgs
loc_E497A,4,mov r10d, [dtu_gs_base]|mov edi, dword [r10d + esi + 0x34] ; fsgs
loc_E497E,4,mov r10d, [dtu_gs_base]|mov ecx, dword [r10d + esi + 0x2c] ; fsgs
loc_E4982,6,movzx r10d, word [eax + loc_15046E]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4992,4,mov r10d, [dtu_gs_base]|mov dword [r10d + eax + 0x2c], ecx ; fsgs
loc_E49A5,4,mov r10d, [dtu_gs_base]|mov ecx, dword [r10d + esi + 0x8] ; fsgs
loc_E49A9,2,movzx r10d, di|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E49AB,4,mov r10d, [dtu_gs_base]|mov dword [r10d + ebx + 0x18], ecx ; fsgs
loc_E49BC,4,movzx r10d, word [r11d + 0x8]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E49C0,4,mov r10d, [dtu_gs_base]|mov ebx, dword [r10d + esi + 0x8] ; fsgs
loc_E49C4,2,movzx r10d, cx|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E49C6,4,mov r10d, [dtu_gs_base]|mov dword [r10d + eax + 0x1c], ebx ; fsgs
loc_E49F2,4,movzx r10d, word [r11d + 0x8]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E49F6,5,mov r10d, [dtu_gs_base]|mov cx, word [r10d + esi + 0x14] ; fsgs
loc_E49FB,2,movzx r10d, di|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E49FD,5,mov r10d, [dtu_gs_base]|mov word [r10d + ebx + 0x32], cx ; fsgs
loc_E4A0F,4,movzx r10d, word [r11d + 0x8]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4A13,5,mov r10d, [dtu_gs_base]|mov cx, word [r10d + esi + 0x18] ; fsgs
loc_E4A18,2,movzx r10d, di|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4A1A,5,mov r10d, [dtu_gs_base]|mov word [r10d + ebx + 0x34], cx ; fsgs
loc_E4A1F,4,movzx r10d, word [r11d + 0x8]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4A23,5,mov r10d, [dtu_gs_base]|mov cx, word [r10d + esi + 0x28] ; fsgs
loc_E4A28,7,mov ebx, dword [eax + loc_15046A]|movzx r10d, word [eax + loc_15046A + 4]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4A32,5,mov r10d, [dtu_gs_base]|mov word [r10d + ebx + 0x30], cx ; fsgs
loc_E4A44,4,movzx r10d, word [r11d + 0x8]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4A48,5,mov r10d, [dtu_gs_base]|mov cx, word [r10d + esi + 0x10] ; fsgs
loc_E4A4D,2,movzx r10d, di|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4A4F,5,mov r10d, [dtu_gs_base]|mov word [r10d + ebx + 0x36], cx ; fsgs
loc_E4A65,4,movzx r10d, word [r11d + 0x8]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4A69,5,mov r10d, [dtu_gs_base]|mov di, word [r10d + esi + 0x20] ; fsgs
loc_E4A6E,4,mov r10d, [dtu_gs_base]|mov ecx, dword [r10d + esi + 0x1c] ; fsgs
loc_E4A72,4,movzx r10d, word [r11d + 0x4]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4A76,5,mov r10d, [dtu_gs_base]|mov word [r10d + ebx + 0x40], di ; fsgs
loc_E4A7B,4,mov r10d, [dtu_gs_base]|mov dword [r10d + ebx + 0x3c], ecx ; fsgs
loc_E4A8C,4,movzx r10d, word [r11d + 0x8]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4A90,5,mov r10d, [dtu_gs_base]|mov cx, word [r10d + esi + 0xc] ; fsgs
loc_E4A95,2,movzx r10d, di|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4A97,5,mov r10d, [dtu_gs_base]|mov word [r10d + ebx + 0x38], cx ; fsgs
loc_E4AA9,4,movzx r10d, word [r11d + 0x8]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4AAD,5,mov r10d, [dtu_gs_base]|mov cx, word [r10d + esi + 0x24] ; fsgs
loc_E4AB2,2,movzx r10d, di|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4AB4,5,mov r10d, [dtu_gs_base]|mov word [r10d + ebx + 0x4a], cx ; fsgs
loc_E4AB9,7,mov ebx, dword [eax + loc_15046A]|movzx r10d, word [eax + loc_15046A + 4]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4AC0,7,mov r10d, [dtu_gs_base]|mov word [r10d + ebx + 0x3a], 0x0 ; fsgs
loc_E4AD4,4,movzx r10d, word [r11d + 0x8]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4AD8,4,mov r10d, [dtu_gs_base]|mov ebx, dword [r10d + esi + 0x38] ; fsgs
loc_E4ADC,2,movzx r10d, di|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4ADE,4,mov r10d, [dtu_gs_base]|mov dword [r10d + ecx + 0x44], ebx ; fsgs
loc_E4AE2,7,mov ebx, dword [eax + loc_15046A]|movzx r10d, word [eax + loc_15046A + 4]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4AE9,7,mov r10d, [dtu_gs_base]|mov word [r10d + ebx + 0x48], 0x0 ; fsgs
loc_E4AF0,7,mov ebx, dword [eax + loc_15046A]|movzx r10d, word [eax + loc_15046A + 4]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4AF7,8,mov r10d, [dtu_gs_base]|mov dword [r10d + ebx + 0x4c], 0x0 ; fsgs
loc_E4B0C,4,movzx r10d, word [r11d + 0x8]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4B10,4,mov r10d, [dtu_gs_base]|mov ecx, dword [r10d + esi + 0x8] ; fsgs
loc_E4B14,2,movzx r10d, di|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4B16,4,mov r10d, [dtu_gs_base]|mov dword [r10d + ebx + 0x50], ecx ; fsgs
loc_E4B27,4,movzx r10d, word [r11d + 0x8]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4B2B,5,mov r10d, [dtu_gs_base]|mov cx, word [r10d + esi + 0x40] ; fsgs
loc_E4B30,2,movzx r10d, di|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4B32,5,mov r10d, [dtu_gs_base]|mov word [r10d + ebx + 0x54], cx ; fsgs
loc_E4B44,4,movzx r10d, word [r11d + 0x8]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4B48,5,mov r10d, [dtu_gs_base]|mov cx, word [r10d + esi + 0x44] ; fsgs
loc_E4B4D,2,movzx r10d, di|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4B4F,5,mov r10d, [dtu_gs_base]|mov word [r10d + ebx + 0x56], cx ; fsgs
loc_E4B61,4,movzx r10d, word [r11d + 0x8]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4B65,5,mov r10d, [dtu_gs_base]|mov cx, word [r10d + esi + 0x48] ; fsgs
loc_E4B6A,2,movzx r10d, di|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4B6C,5,mov r10d, [dtu_gs_base]|mov word [r10d + ebx + 0x58], cx ; fsgs
loc_E4B7E,4,movzx r10d, word [r11d + 0x8]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4B82,5,mov r10d, [dtu_gs_base]|mov cx, word [r10d + esi + 0x4c] ; fsgs
loc_E4B87,2,movzx r10d, di|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4B89,5,mov r10d, [dtu_gs_base]|mov word [r10d + ebx + 0x5a], cx ; fsgs
loc_E4B9B,4,movzx r10d, word [r11d + 0x8]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4B9F,5,mov r10d, [dtu_gs_base]|mov cx, word [r10d + esi + 0x50] ; fsgs
loc_E4BA4,2,movzx r10d, di|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4BA6,5,mov r10d, [dtu_gs_base]|mov word [r10d + ebx + 0x5c], cx ; fsgs
loc_E4BB8,4,movzx r10d, word [r11d + 0x8]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4BBC,5,mov r10d, [dtu_gs_base]|mov cx, word [r10d + esi + 0x54] ; fsgs
loc_E4BC1,2,movzx r10d, di|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4BC3,5,mov r10d, [dtu_gs_base]|mov word [r10d + ebx + 0x5e], cx ; fsgs
loc_E4BD5,4,movzx r10d, word [r11d + 0x8]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4BD9,5,mov r10d, [dtu_gs_base]|mov cx, word [r10d + esi + 0x58] ; fsgs
loc_E4BDE,2,movzx r10d, di|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4BE0,5,mov r10d, [dtu_gs_base]|mov word [r10d + ebx + 0x60], cx ; fsgs
loc_E4BE5,7,mov ebx, dword [eax + loc_15046A]|movzx r10d, word [eax + loc_15046A + 4]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4BEC,8,mov r10d, [dtu_gs_base]|mov dword [r10d + ebx + 0x64], 0x0 ; fsgs
loc_E4BF4,7,mov r9d, dword [eax + loc_15046A]|movzx r10d, word [eax + loc_15046A + 4]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d|mov eax, r9d ; fsgs
loc_E4BFB,7,mov r10d, [dtu_gs_base]|mov word [r10d + eax + 0x68], 0x0 ; fsgs
loc_E4C4C,7,mov eax, dword [edx + loc_15046A]|movzx r10d, word [edx + loc_15046A + 4]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4C53,4,mov r10d, [dtu_gs_base]|mov ebx, dword [r10d + eax + 0x2e] ; fsgs
loc_E4C60,4,mov r10d, [dtu_gs_base]|mov bl, byte [r10d + eax + 0x31] ; fsgs
loc_E4C6E,4,mov r9, rsi|mov esi, [dtu_gs_base]|lea esi, [esi + eax + 0x31]|mov byte [esi], bh|mov rsi, r9 ; fsgs
loc_E4C72,7,mov eax, dword [edx + loc_15046A]|movzx r10d, word [edx + loc_15046A + 4]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4C79,7,mov r10d, [dtu_gs_base]|mov word [r10d + eax + 0x34], 0x0 ; fsgs
loc_E4CD4,2,movzx r10d, cx|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4CD6,5,mov r10d, [dtu_gs_base]|mov cx, word [r10d + ebx + 0x4] ; fsgs
loc_E4CDB,3,mov r10d, [dtu_gs_base]|mov ebp, dword [r10d + ebx] ; fsgs
loc_E4CDE,4,movzx r10d, word [r11d + 0x4]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4CE2,5,mov r10d, [dtu_gs_base]|mov word [r10d + edx + 0x4], cx ; fsgs
loc_E4CE7,3,mov r10d, [dtu_gs_base]|mov dword [r10d + edx], ebp ; fsgs
loc_E4CFB,3,movzx r10d, word [r11d]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4CFE,5,mov r10d, [dtu_gs_base]|mov cx, word [r10d + ebx + 0x4] ; fsgs
loc_E4D03,3,mov r10d, [dtu_gs_base]|mov ebp, dword [r10d + ebx] ; fsgs
loc_E4D06,4,movzx r10d, word [r11d + 0x4]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4D0A,5,mov r10d, [dtu_gs_base]|mov word [r10d + edx + 0xc], cx ; fsgs
loc_E4D0F,4,mov r10d, [dtu_gs_base]|mov dword [r10d + edx + 0x8], ebp ; fsgs
loc_E4D13,3,movzx r10d, word [r11d]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4D16,5,mov r10d, [dtu_gs_base]|mov dx, word [r10d + ebx + 0x4] ; fsgs
loc_E4D1B,4,mov r10d, [dtu_gs_base]|mov ecx, dword [r10d + ebx + 0x30] ; fsgs
loc_E4D23,3,mov r10d, [dtu_gs_base]|mov edx, dword [r10d + ebx] ; fsgs
loc_E4D26,7,mov ebp, dword [eax + loc_15046A]|movzx r10d, word [eax + loc_15046A + 4]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4D33,5,mov r10d, [dtu_gs_base]|mov word [r10d + ebp + 0x14], cx ; fsgs
loc_E4D38,4,mov r10d, [dtu_gs_base]|mov dword [r10d + ebp + 0x10], edx ; fsgs
loc_E4D49,3,movzx r10d, word [r11d]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4D4C,4,mov r10d, [dtu_gs_base]|mov edx, dword [r10d + ebx + 0x8] ; fsgs
loc_E4D50,2,movzx r10d, cx|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4D52,4,mov r10d, [dtu_gs_base]|mov dword [r10d + ebp + 0x50], edx ; fsgs
loc_E4D56,3,movzx r10d, word [r11d]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4D59,5,mov r10d, [dtu_gs_base]|test byte [r10d + ebx + 0x28], 0x40 ; fsgs
loc_E4D6D,4,mov r10d, [dtu_gs_base]|mov ebp, dword [r10d + ebx + 0x30] ; fsgs
loc_E4D71,2,movzx r10d, cx|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4D73,4,mov r10d, [dtu_gs_base]|mov dword [r10d + edx + 0x18], ebp ; fsgs
loc_E4D84,3,movzx r10d, word [r11d]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4D87,4,mov r10d, [dtu_gs_base]|mov edx, dword [r10d + ebx + 0x30] ; fsgs
loc_E4D8B,2,movzx r10d, cx|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4D8D,4,mov r10d, [dtu_gs_base]|mov dword [r10d + ebp + 0x1c], edx ; fsgs
loc_E4D9E,3,movzx r10d, word [r11d]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4DA1,4,mov r10d, [dtu_gs_base]|mov edx, dword [r10d + ebx + 0x34] ; fsgs
loc_E4DA5,2,movzx r10d, cx|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4DA7,4,mov r10d, [dtu_gs_base]|mov dword [r10d + ebp + 0x20], edx ; fsgs
loc_E4DAB,3,movzx r10d, word [r11d]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4DAE,4,mov r10d, [dtu_gs_base]|mov edx, dword [r10d + ebx + 0x30] ; fsgs
loc_E4DB2,4,mov r10d, [dtu_gs_base]|add edx, dword [r10d + ebx + 0x34] ; fsgs
loc_E4DB6,4,mov r10d, [dtu_gs_base]|mov ebp, dword [r10d + ebx + 0x2c] ; fsgs
loc_E4DBA,7,mov r9d, dword [eax + loc_15046A]|movzx r10d, word [eax + loc_15046A + 4]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d|mov eax, r9d ; fsgs
loc_E4DC3,4,mov r10d, [dtu_gs_base]|mov dword [r10d + eax + 0x2c], ebp ; fsgs
loc_E4DD6,4,mov r10d, [dtu_gs_base]|mov edx, dword [r10d + ebx + 0x8] ; fsgs
loc_E4DDA,2,movzx r10d, cx|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4DDC,4,mov r10d, [dtu_gs_base]|mov dword [r10d + ebp + 0x18], edx ; fsgs
loc_E4DED,3,movzx r10d, word [r11d]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4DF0,4,mov r10d, [dtu_gs_base]|mov eax, dword [r10d + ebx + 0x8] ; fsgs
loc_E4DF4,2,movzx r10d, cx|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4DF6,4,mov r10d, [dtu_gs_base]|mov dword [r10d + edx + 0x1c], eax ; fsgs
loc_E4E20,3,movzx r10d, word [r11d]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4E23,5,mov r10d, [dtu_gs_base]|mov cx, word [r10d + ebx + 0x18] ; fsgs
loc_E4E28,2,movzx r10d, si|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4E2A,5,mov r10d, [dtu_gs_base]|mov word [r10d + edx + 0x34], cx ; fsgs
loc_E4E2F,3,movzx r10d, word [r11d]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4E32,5,mov r10d, [dtu_gs_base]|mov cx, word [r10d + ebx + 0x28] ; fsgs
loc_E4E37,7,mov edx, dword [eax + loc_15046A]|movzx r10d, word [eax + loc_15046A + 4]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4E41,5,mov r10d, [dtu_gs_base]|mov word [r10d + edx + 0x30], cx ; fsgs
loc_E4E53,3,movzx r10d, word [r11d]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4E56,5,mov r10d, [dtu_gs_base]|mov cx, word [r10d + ebx + 0x20] ; fsgs
loc_E4E5B,4,mov r10d, [dtu_gs_base]|mov esi, dword [r10d + ebx + 0x1c] ; fsgs
loc_E4E5F,2,movzx r10d, di|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4E61,5,mov r10d, [dtu_gs_base]|mov word [r10d + edx + 0x40], cx ; fsgs
loc_E4E66,4,mov r10d, [dtu_gs_base]|mov dword [r10d + edx + 0x3c], esi ; fsgs
loc_E4E77,3,movzx r10d, word [r11d]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4E7A,5,mov r10d, [dtu_gs_base]|mov cx, word [r10d + ebx + 0xc] ; fsgs
loc_E4E7F,2,movzx r10d, si|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4E81,5,mov r10d, [dtu_gs_base]|mov word [r10d + edx + 0x38], cx ; fsgs
loc_E4E93,3,movzx r10d, word [r11d]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4E96,5,mov r10d, [dtu_gs_base]|mov dx, word [r10d + ebx + 0x24] ; fsgs
loc_E4E9B,2,movzx r10d, cx|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E4E9D,5,mov r10d, [dtu_gs_base]|mov word [r10d + eax + 0x4a], dx ; fsgs
loc_E4EE4,8,mov r9d, dword [edx + eax*2 + loc_15046A]|movzx r10d, word [edx + eax*2 + loc_15046A + 4]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d|mov eax, r9d ; fsgs
loc_E4EEC,4,mov r10d, [dtu_gs_base]|mov edx, dword [r10d + eax + 0x2e] ; fsgs
loc_E4EFD,4,mov r10d, [dtu_gs_base]|mov edx, dword [r10d + eax + 0x30] ; fsgs
loc_E4F05,5,mov r10d, [dtu_gs_base]|mov word [r10d + eax + 0x32], bx ; fsgs
loc_E5058,8,mov r9d, dword [edx + eax*2 + loc_15046A]|movzx r10d, word [edx + eax*2 + loc_15046A + 4]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d|mov eax, r9d ; fsgs
loc_E5060,4,mov r10d, [dtu_gs_base]|mov edx, dword [r10d + eax + 0x2e] ; fsgs
loc_E5071,4,mov r10d, [dtu_gs_base]|mov edx, dword [r10d + eax + 0x52] ; fsgs
loc_E5079,5,mov r10d, [dtu_gs_base]|mov word [r10d + eax + 0x54], bx ; fsgs
loc_E51FF,7,mov ebx, dword [eax + loc_15046A]|movzx r10d, word [eax + loc_15046A + 4]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_E5206,4,mov r10d, [dtu_gs_base]|mov ebx, dword [r10d + ebx + 0x32] ; fsgs
loc_E5271,8,mov r9d, dword [edx + eax*2 + loc_15046A]|movzx r10d, word [edx + eax*2 + loc_15046A + 4]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d|mov eax, r9d ; fsgs
loc_E5279,4,mov r10d, [dtu_gs_base]|mov eax, dword [r10d + eax + 0x2e] ; fsgs
loc_EAC45,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_EAC47,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_EAC6A,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_EC414,4,mov r10d, [dtu_es_sel]|mov word [eax + 0x20], r10w ; fsgs
loc_EC420,4,mov r10d, [dtu_fs_sel]|mov word [eax + 0x26], r10w ; fsgs
loc_EC424,4,mov r10d, [dtu_gs_sel]|mov word [eax + 0x28], r10w ; fsgs
loc_EC46C,2,movzx r10d, dx|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_EC479,2,movzx r10d, dx|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d ; fsgs
loc_EC486,2,movzx r10d, dx|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_EC53C,2,mov r10d, [dtu_fs_sel]|PUSH32 r10d ; fsgs
loc_EC644,7,movzx r10d, word [loc_14C4E5]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d ; fsgs
loc_EC64B,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_EC69A,2,movzx r10d, word [r11d]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_EC69C,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + eax] ; fsgs
loc_EC69F,2,mov r10d, [dtu_fs_sel]|PUSH32 r10d ; fsgs
loc_EC6B6,2,movzx r10d, word [r11d]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_EC6BC,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_EC6BD,2,mov r10d, [dtu_fs_sel]|PUSH32 r10d ; fsgs
loc_EC79D,7,movzx r10d, word [loc_46C698]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_EC7A4,7,movzx r10d, word [loc_466AB6]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d ; fsgs
loc_EC930,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_EC95E,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_EC991,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_EC9B1,2,movzx r10d, word [r11d]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_EC9B3,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_EC9D4,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_ECA06,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_ECA3D,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_ECA61,2,movzx r10d, word [r11d]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_ECA63,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_ECB14,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_ECB17,2,movzx r10d, ax|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_ECB23,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_EDC5A,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_EDC5D,2,movzx r10d, ax|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_EDC6A,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_EEDF4,5,mov edx, dword [r11d + 0x40]|movzx r10d, word [r11d + 0x40 + 4]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_EEE11,4,mov r10d, [dtu_gs_base]|movzx esi, byte [r10d + esi] ; fsgs
loc_EEE9C,4,movzx r10d, word [r11d + 0x40]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_EEEAD,3,mov r10d, [dtu_gs_base]|mov al, byte [r10d + eax] ; fsgs
loc_F11E2,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_F11E3,2,movzx r10d, cx|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_F11ED,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_F173E,7,movzx r10d, word [loc_466AB6]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d ; fsgs
loc_F1745,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1755,7,movzx r10d, word [loc_466AB4]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d ; fsgs
loc_F175C,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F176A,7,movzx r10d, word [loc_466AB2]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d ; fsgs
loc_F1771,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F177F,7,movzx r10d, word [loc_466AB0]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d ; fsgs
loc_F1786,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F179D,2,mov r10d, [dtu_fs_sel]|PUSH32 r10d ; fsgs
loc_F17A0,7,movzx r10d, word [loc_466AB6]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d ; fsgs
loc_F18B4,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F18BA,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F18C0,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F18C6,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F18CC,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F18D2,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F18D8,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F18DE,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F18E4,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F18EA,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F18F0,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F18F6,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F18FC,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1902,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1908,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F190E,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1914,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F191A,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1920,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1926,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F192C,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1932,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1938,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F193E,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1944,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F194A,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1950,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1956,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F195C,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1962,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1968,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F196E,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1974,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F197A,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1980,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1986,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F198C,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1992,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1998,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F199E,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F19A4,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F19AA,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F19B0,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F19B6,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F19BC,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F19C2,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F19C8,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F19CE,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F19D4,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F19DA,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F19E0,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F19E6,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F19EC,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F19F2,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F19F8,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F19FE,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1A04,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1A0A,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1A10,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1A16,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1A1C,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1A22,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1A28,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1A2E,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1A34,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1A3A,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1A40,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1A46,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1A4C,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1A52,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1A58,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1A5E,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1A64,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1A6A,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1A70,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1A76,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1A7C,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1A82,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1A88,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1A8E,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1A94,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1A9A,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1AA0,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1AA6,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1AAC,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1AB2,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1AB8,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1ABE,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1AC4,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1ACA,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1AD0,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1AD6,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1ADC,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1AE2,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1AE8,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1AEE,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1AF4,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1AFA,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1B00,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1B06,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1B0C,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1B12,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1B18,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1B1E,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1B24,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1B2A,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1B30,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1B36,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1B3C,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1B42,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1B48,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1B4E,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1B54,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1B5A,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1B60,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1B66,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1B6C,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1B72,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1B78,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1B7E,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1B84,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1B8A,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1B90,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1B96,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1B9C,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1BA2,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1BA8,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1BAE,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1BB4,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1BBA,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1BC0,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1BC6,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1BCC,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1BD2,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1BD8,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1BDE,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1BE4,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1BEA,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1BF0,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1BF6,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1BFC,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1C02,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1C08,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1C0E,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1C14,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1C1A,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1C20,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1C26,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1C2C,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1C32,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1C38,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1C3E,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1C44,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1C4A,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1C50,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1C56,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1C5C,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1C62,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1C68,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1C6E,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1C74,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1C7A,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1C80,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1C86,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1C8C,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1C92,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1C98,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1C9E,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1CA4,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1CAA,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1CB0,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1CB6,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1CBC,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1CC2,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1CC8,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1CCE,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1CD4,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1CDA,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1CE0,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1CE6,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1CEC,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1CF2,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1CF8,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1CFE,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1D04,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1D0A,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1D10,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1D16,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1D1C,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1D22,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1D28,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1D2E,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1D34,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1D3A,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1D40,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1D46,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1D4C,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1D52,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1D58,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1D5E,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1D64,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1D6A,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1D70,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1D76,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1D7C,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1D82,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1D88,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1D8E,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1D94,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1D9A,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1DA0,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1DA6,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1DAC,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1DB2,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1DB8,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1DBE,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1DC4,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1DCA,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1DD0,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1DD6,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1DDC,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1DE2,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1DE8,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1DEE,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1DF4,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1DFA,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1E00,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1E06,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1E0C,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1E12,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1E18,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1E1E,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1E24,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1E2A,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1E30,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1E36,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1E3C,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1E42,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1E48,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1E4E,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1E54,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1E5A,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1E60,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1E66,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1E6C,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1E72,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1E78,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1E7E,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1E84,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1E8A,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1E90,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1E96,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1E9C,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1EA2,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1EA8,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1EAE,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1EB4,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1EBA,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1EC0,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1EC6,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1ECC,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1ED2,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1ED8,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1EDE,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1EE4,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1EEA,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1EF0,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1EF6,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1EFC,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1F02,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1F08,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1F0E,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1F14,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1F1A,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1F20,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1F26,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1F2C,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1F32,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1F38,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1F3E,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1F44,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1F4A,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1F50,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1F56,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1F5C,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1F62,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1F68,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1F6E,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1F74,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1F7A,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1F80,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1F86,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1F8C,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1F92,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1F98,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1F9E,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1FA4,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1FAA,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1FB0,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1FB6,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1FBC,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1FC2,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1FC8,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1FCE,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1FD4,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1FDA,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1FE0,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1FE6,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1FEC,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1FF2,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1FF8,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F1FFE,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F2004,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F200A,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F2010,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F2016,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F201C,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F2022,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F2028,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F202E,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_F2035,2,movzx r10d, word [r11d]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_F2A89,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_F2A8C,2,movzx r10d, ax|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_F2A98,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_F2ADD,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_F2AE0,2,movzx r10d, ax|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_F2AEC,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_F2BD0,7,movzx r10d, word [loc_14D344]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_F2BD7,7,mov r10d, [dtu_es_base]|mov ebx, dword [r10d + 0x5c] ; fsgs
loc_F2BF6,2,movzx r10d, ax|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_F2C4E,3,movzx r10d, word [dtu_gs_sel]|mov ax, r10w ; fsgs
loc_F2CA4,2,movzx r10d, bx|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_F2CBC,7,movzx r10d, word [loc_14D344]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_F2CDF,3,movzx r10d, word [dtu_es_sel]|mov bx, r10w ; fsgs
loc_F2CE7,2,movzx r10d, dx|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_F2E53,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_F2E56,2,movzx edx, word [dtu_es_sel] ; fsgs
loc_F2E58,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_F31ED,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_F31F2,2,movzx r10d, dx|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_F31FD,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_F37FA,7,mov eax, dword [edx + loc_15046A]|movzx r10d, word [edx + loc_15046A + 4]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_F3801,5,mov r10d, [dtu_gs_base]|mov ax, word [r10d + eax + 0x30] ; fsgs
loc_F3840,6,movzx r10d, word [edx + loc_15046E]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_F384F,4,mov r10d, [dtu_gs_base]|mov edx, dword [r10d + eax + 0x4c] ; fsgs
loc_F3859,5,mov r10d, [dtu_gs_base]|movsx eax, word [r10d + eax + 0x3a] ; fsgs
loc_F38C0,8,mov r9d, dword [ebx + edx*2 + loc_15046A]|movzx r10d, word [ebx + edx*2 + loc_15046A + 4]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d|mov edx, r9d ; fsgs
loc_F38C8,5,mov r10d, [dtu_gs_base]|imul eax, dword [r10d + edx + 0x44] ; fsgs
loc_F390F,8,mov r9d, dword [edx + esi*2 + loc_15046A]|movzx r10d, word [edx + esi*2 + loc_15046A + 4]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d|mov edx, r9d ; fsgs
loc_F3917,6,mov r10d, [dtu_gs_base]|cmp word [r10d + edx + 0x36], 0x3 ; fsgs
loc_F6D95,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_F6E20,5,movzx r10d, word [r11d + 0x1c]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_F6E5E,5,movzx r10d, word [r11d + 0x30]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_F6E66,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_F6E69,5,movzx r10d, word [r11d + 0x18]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_F6E6E,2,mov r10d, [dtu_fs_sel]|PUSH32 r10d ; fsgs
loc_F6E70,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_F6E73,2,mov r10d, [dtu_gs_sel]|PUSH32 r10d ; fsgs
loc_F6E75,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_F6E78,5,movzx r10d, word [r11d + 0x3c]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_F6E94,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_F6F16,5,movzx r10d, word [r11d + 0x3c]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_F7117,4,mov r10d, [dtu_es_base]|movsx edx, byte [r10d + esi] ; fsgs
loc_F7120,4,mov r10d, [dtu_es_base]|movsx edx, word [r10d + esi] ; fsgs
loc_F7129,3,mov r10d, [dtu_es_base]|mov edx, dword [r10d + esi] ; fsgs
loc_F7180,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_F718D,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_F719E,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_F71B1,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_F71BE,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_F71CF,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_F71E2,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_F71EF,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_F7200,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_F7213,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_F7374,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_F7387,2,movzx r10d, ax|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_F7413,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_FE736,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_FE764,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_FF952,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_FF972,3,movzx r10d, ax|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_FFA21,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_FFD70,2,movzx r10d, word [r11d]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_FFD72,2,movzx r10d, word [r11d]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_FFD74,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_10007E,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_100081,2,movzx r10d, ax|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_10008D,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_1000BD,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_1000C0,2,movzx r10d, ax|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_1000D2,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_1001AA,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_1001B9,2,movzx r10d, dx|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_1001BB,5,mov r10d, [dtu_es_base]|cmp word [r10d + eax], 0x0 ; fsgs
loc_1001C2,6,mov r10d, [dtu_es_base]|cmp word [r10d + eax + 0x2], 0x0 ; fsgs
loc_1001D4,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_1002AA,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_1002AB,2,movzx r10d, dx|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_1002B3,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_100531,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_100532,2,movzx r10d, ax|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_10053C,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_10079F,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_1007A0,2,movzx r10d, ax|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_1007AA,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_100A56,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_100AFE,3,mov edx, dword [ecx + -0x8]|movzx r10d, word [ecx + -0x8 + 4]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_100B08,3,mov r10d, [dtu_es_base]|mov dword [r10d + edx], eax ; fsgs
loc_100B7E,3,mov edx, dword [eax + -0x8]|movzx r10d, word [eax + -0x8 + 4]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_100B88,4,mov r10d, [dtu_es_base]|mov word [r10d + edx], ax ; fsgs
loc_100BC5,3,mov r9d, dword [edx + -0x8]|movzx r10d, word [edx + -0x8 + 4]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|mov edx, r9d ; fsgs
loc_100C10,2,movzx r10d, dx|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_100D2C,3,mov r10d, [dtu_es_base]|mov dl, byte [r10d + ebx] ; fsgs
loc_100D59,3,mov r10d, [dtu_es_base]|mov dl, byte [r10d + ebx] ; fsgs
loc_100D84,3,mov r10d, [dtu_es_base]|mov dl, byte [r10d + ebx] ; fsgs
loc_100DEF,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_100F93,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_100F9A,2,movzx r10d, cx|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_100FA0,3,mov r10d, [dtu_es_base]|mov bl, byte [r10d + esi] ; fsgs
loc_100FAF,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_100FB7,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_100FBE,2,movzx r10d, dx|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_100FC4,4,mov r10d, [dtu_es_base]|mov di, word [r10d + ecx] ; fsgs
loc_100FEF,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_100FF2,2,movzx r10d, ax|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_100FFE,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_101133,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_10114D,2,movzx r10d, dx|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_1012BA,2,movzx r10d, ax|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_1012D2,2,movzx r10d, dx|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_101305,4,mov r10d, [dtu_es_base]|mov byte [r10d + ecx], 0x0 ; fsgs
loc_10130E,2,movzx edx, word [dtu_es_sel] ; fsgs
loc_101392,2,movzx r10d, dx|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_1013D2,2,movzx r10d, ax|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_1013F9,4,mov r10d, [dtu_es_base]|mov ax, word [r10d + ecx] ; fsgs
loc_101405,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + ecx] ; fsgs
loc_101419,2,movzx edx, word [dtu_es_sel] ; fsgs
loc_10142A,2,movzx edx, word [dtu_es_sel] ; fsgs
loc_101570,3,mov r10d, [dtu_es_base]|mov dl, byte [r10d + ecx] ; fsgs
loc_101609,2,movzx edx, word [dtu_es_sel] ; fsgs
loc_101616,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_1022CE,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_1022E9,3,mov ecx, dword [edi + -0x8]|movzx r10d, word [edi + -0x8 + 4]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_1022FC,2,movzx r10d, ax|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_10230C,2,movzx r10d, ax|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_102342,4,mov r10d, [dtu_es_base]|mov word [r10d + ecx], ax ; fsgs
loc_10234E,3,mov r10d, [dtu_es_base]|mov byte [r10d + ecx], al ; fsgs
loc_102359,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_102363,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_102391,3,mov ecx, dword [edi + -0x8]|movzx r10d, word [edi + -0x8 + 4]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_1023A3,2,movzx r10d, ax|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_1023B3,2,movzx r10d, ax|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_102411,3,mov r10d, [dtu_es_base]|mov byte [r10d + ecx], al ; fsgs
loc_102419,4,mov r10d, [dtu_es_base]|mov word [r10d + ecx], ax ; fsgs
loc_10246C,4,mov r10d, [dtu_es_base]|mov byte [r10d + ecx], 0x0 ; fsgs
loc_102472,6,mov r10d, [dtu_es_base]|mov word [r10d + ecx], 0x0 ; fsgs
loc_102485,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_10249A,3,mov edx, dword [edi + -0x8]|movzx r10d, word [edi + -0x8 + 4]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_1024AD,2,movzx r10d, cx|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_1024BF,2,movzx r10d, cx|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_1024CC,4,mov r10d, [dtu_es_base]|mov word [r10d + edx], bx ; fsgs
loc_1024D0,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_1024D8,3,mov r10d, [dtu_es_base]|mov dword [r10d + edx], ebx ; fsgs
loc_1024DB,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_102526,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_102569,3,mov r9d, dword [esi + -0x8]|movzx r10d, word [esi + -0x8 + 4]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|mov esi, r9d ; fsgs
loc_10257C,2,movzx r10d, ax|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_10258C,2,movzx r10d, ax|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_1025FE,3,mov r10d, [dtu_es_base]|mov byte [r10d + esi], al ; fsgs
loc_10260E,4,mov r10d, [dtu_es_base]|mov byte [r10d + esi], 0x0 ; fsgs
loc_102618,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_102621,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_1028C0,3,mov ebx, dword [ecx + -0x8]|movzx r10d, word [ecx + -0x8 + 4]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_1028D7,2,movzx r10d, ax|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_1028EB,2,movzx r10d, ax|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_10290C,3,mov r10d, [dtu_es_base]|mov dword [r10d + ebx], eax ; fsgs
loc_102913,4,mov r10d, [dtu_es_base]|mov dword [r10d + ebx + 0x4], eax ; fsgs
loc_102926,3,mov r10d, [dtu_es_base]|mov dword [r10d + ebx], eax ; fsgs
loc_102936,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_102AD6,3,mov eax, dword [edx + -0x8]|movzx r10d, word [edx + -0x8 + 4]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_102AED,2,movzx r10d, dx|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_102B01,2,movzx r10d, dx|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_102B0E,4,mov r10d, [dtu_es_base]|mov word [r10d + eax], di ; fsgs
loc_102B17,3,mov r10d, [dtu_es_base]|mov dword [r10d + eax], edi ; fsgs
loc_102B20,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_102B99,2,mov r10d, [dtu_fs_sel]|PUSH32 r10d ; fsgs
loc_102B9B,2,mov r10d, [dtu_gs_sel]|PUSH32 r10d ; fsgs
loc_102B9D,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_102BC4,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_102BC5,2,movzx r10d, word [r11d]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_102BC7,2,movzx r10d, word [r11d]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_102BD8,2,mov r10d, [dtu_fs_sel]|PUSH32 r10d ; fsgs
loc_102BDA,2,mov r10d, [dtu_gs_sel]|PUSH32 r10d ; fsgs
loc_102BDC,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_102C03,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_102C04,2,movzx r10d, word [r11d]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_102C06,2,movzx r10d, word [r11d]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_102C16,2,mov r10d, [dtu_fs_sel]|PUSH32 r10d ; fsgs
loc_102C18,2,mov r10d, [dtu_gs_sel]|PUSH32 r10d ; fsgs
loc_102C1A,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_102C9A,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_102C9B,2,movzx r10d, word [r11d]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_102C9D,2,movzx r10d, word [r11d]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_102CA9,2,movzx r10d, ax|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_102CB4,2,mov r10d, [dtu_fs_sel]|PUSH32 r10d ; fsgs
loc_102CB6,2,mov r10d, [dtu_gs_sel]|PUSH32 r10d ; fsgs
loc_102CB8,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_102CD1,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_102CD2,2,movzx r10d, word [r11d]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_102CD4,2,movzx r10d, word [r11d]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_102CE4,2,mov r10d, [dtu_fs_sel]|PUSH32 r10d ; fsgs
loc_102CE6,2,mov r10d, [dtu_gs_sel]|PUSH32 r10d ; fsgs
loc_102CE8,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_102D2F,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_102D30,2,movzx r10d, word [r11d]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_102D32,2,movzx r10d, word [r11d]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_102D42,2,mov r10d, [dtu_fs_sel]|PUSH32 r10d ; fsgs
loc_102D44,2,mov r10d, [dtu_gs_sel]|PUSH32 r10d ; fsgs
loc_102D46,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_102D4A,3,mov edi, dword [ebp + 0xc]|movzx r10d, word [ebp + 0xc + 4]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_102D54,4,mov r10d, [dtu_es_sel]|mov word [esi + 0x4], r10w ; fsgs
loc_102D60,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_102D61,2,movzx r10d, word [r11d]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_102D63,2,movzx r10d, word [r11d]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_102D74,2,mov r10d, [dtu_fs_sel]|PUSH32 r10d ; fsgs
loc_102D76,2,mov r10d, [dtu_gs_sel]|PUSH32 r10d ; fsgs
loc_102D78,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_102D98,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_102D99,2,movzx r10d, word [r11d]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_102D9B,2,movzx r10d, word [r11d]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_102DA7,2,movzx r10d, ax|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_102DA9,2,movzx r10d, ax|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_102DAB,2,movzx r10d, ax|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d ; fsgs
loc_102DB5,2,mov r10d, [dtu_fs_sel]|PUSH32 r10d ; fsgs
loc_102DB7,2,mov r10d, [dtu_gs_sel]|PUSH32 r10d ; fsgs
loc_102DB9,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_102DC5,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_102DC6,2,movzx r10d, word [r11d]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_102DC8,2,movzx r10d, word [r11d]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_102E10,2,mov r10d, [dtu_fs_sel]|PUSH32 r10d ; fsgs
loc_102E12,2,mov r10d, [dtu_gs_sel]|PUSH32 r10d ; fsgs
loc_102E14,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_102E1A,4,mov edi, dword [ebp + 0x10]|movzx r10d, word [ebp + 0x10 + 4]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d ; fsgs
loc_102E22,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_102E23,3,mov edi, dword [ebp + 0x18]|movzx r10d, word [ebp + 0x18 + 4]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_102E33,3,mov edi, dword [ebp + 0x18]|movzx r10d, word [ebp + 0x18 + 4]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_102E36,5,mov r10d, [dtu_es_base]|mov word [r10d + edi + 0x44], ds ; fsgs
loc_102E3B,5,mov r10d, [dtu_es_base]|mov word [r10d + edi + 0x4c], ds ; fsgs
loc_102E40,5,mov r10d, [dtu_es_base]|mov word [r10d + edi + 0x54], ds ; fsgs
loc_102E45,5,mov r10d, [dtu_es_base]|mov word [r10d + edi + 0x5c], ds ; fsgs
loc_102E4A,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_102E4C,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_102E4D,2,movzx r10d, word [r11d]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_102E4F,2,movzx r10d, word [r11d]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_102E60,2,mov r10d, [dtu_fs_sel]|PUSH32 r10d ; fsgs
loc_102E62,2,mov r10d, [dtu_gs_sel]|PUSH32 r10d ; fsgs
loc_102E64,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_102E6A,4,mov edi, dword [ebp + 0x10]|movzx r10d, word [ebp + 0x10 + 4]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d ; fsgs
loc_102E71,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_102E72,2,movzx r10d, word [r11d]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_102E74,2,movzx r10d, word [r11d]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_102F72,2,mov r10d, [dtu_fs_sel]|PUSH32 r10d ; fsgs
loc_102F74,2,mov r10d, [dtu_gs_sel]|PUSH32 r10d ; fsgs
loc_102F76,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_102F7C,4,mov edi, dword [ebp + 0x10]|movzx r10d, word [ebp + 0x10 + 4]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d ; fsgs
loc_102F83,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_102F84,2,movzx r10d, word [r11d]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_102F86,2,movzx r10d, word [r11d]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_102F97,2,mov r10d, [dtu_fs_sel]|PUSH32 r10d ; fsgs
loc_102F99,2,mov r10d, [dtu_gs_sel]|PUSH32 r10d ; fsgs
loc_102F9B,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_102F9C,4,mov edi, dword [ebp + 0x10]|movzx r10d, word [ebp + 0x10 + 4]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d ; fsgs
loc_102FA8,3,mov edi, dword [ebp + 0x18]|movzx r10d, word [ebp + 0x18 + 4]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_102FB2,2,movzx r10d, word [r11d]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_102FD8,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_102FD9,2,movzx r10d, word [r11d]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_102FDB,2,movzx r10d, word [r11d]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_102FEC,2,mov r10d, [dtu_fs_sel]|PUSH32 r10d ; fsgs
loc_102FEE,2,mov r10d, [dtu_gs_sel]|PUSH32 r10d ; fsgs
loc_102FF0,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_102FF1,4,mov edi, dword [ebp + 0x10]|movzx r10d, word [ebp + 0x10 + 4]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d ; fsgs
loc_10300F,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_103010,2,movzx r10d, word [r11d]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_103012,2,movzx r10d, word [r11d]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_103023,2,mov r10d, [dtu_fs_sel]|PUSH32 r10d ; fsgs
loc_103025,2,mov r10d, [dtu_gs_sel]|PUSH32 r10d ; fsgs
loc_103027,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_103028,4,mov edi, dword [ebp + 0x10]|movzx r10d, word [ebp + 0x10 + 4]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d ; fsgs
loc_10303A,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_10303B,2,movzx r10d, word [r11d]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_10303D,2,movzx r10d, word [r11d]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_10304E,2,mov r10d, [dtu_fs_sel]|PUSH32 r10d ; fsgs
loc_103050,2,mov r10d, [dtu_gs_sel]|PUSH32 r10d ; fsgs
loc_103052,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_103053,4,mov edi, dword [ebp + 0x10]|movzx r10d, word [ebp + 0x10 + 4]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d ; fsgs
loc_103062,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_103063,2,movzx r10d, word [r11d]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_103065,2,movzx r10d, word [r11d]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_103076,2,mov r10d, [dtu_fs_sel]|PUSH32 r10d ; fsgs
loc_103078,2,mov r10d, [dtu_gs_sel]|PUSH32 r10d ; fsgs
loc_10307A,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_10307B,4,mov edi, dword [ebp + 0x10]|movzx r10d, word [ebp + 0x10 + 4]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d ; fsgs
loc_10308B,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_10308C,2,movzx r10d, word [r11d]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_10308E,2,movzx r10d, word [r11d]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_1030CB,2,mov r10d, [dtu_fs_sel]|PUSH32 r10d ; fsgs
loc_1030CD,2,mov r10d, [dtu_gs_sel]|PUSH32 r10d ; fsgs
loc_1030CF,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_1030EE,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_1030EF,2,movzx r10d, word [r11d]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_1030F1,2,movzx r10d, word [r11d]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_103102,2,mov r10d, [dtu_fs_sel]|PUSH32 r10d ; fsgs
loc_103104,2,mov r10d, [dtu_gs_sel]|PUSH32 r10d ; fsgs
loc_103106,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_103125,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_103126,2,movzx r10d, word [r11d]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_103128,2,movzx r10d, word [r11d]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_1031FE,2,mov r10d, [dtu_fs_sel]|PUSH32 r10d ; fsgs
loc_103200,2,mov r10d, [dtu_gs_sel]|PUSH32 r10d ; fsgs
loc_103202,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_103204,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_103209,4,mov edi, dword [ebp + 0x20]|movzx r10d, word [ebp + 0x20 + 4]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d ; fsgs
loc_103212,2,movzx r10d, dx|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_103218,4,mov edi, dword [ebp + 0x10]|movzx r10d, word [ebp + 0x10 + 4]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d ; fsgs
loc_10321F,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_103221,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_103222,2,movzx r10d, word [r11d]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_103224,2,movzx r10d, word [r11d]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_103262,2,mov r10d, [dtu_fs_sel]|PUSH32 r10d ; fsgs
loc_103264,2,mov r10d, [dtu_gs_sel]|PUSH32 r10d ; fsgs
loc_103266,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_10326D,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_10326E,2,movzx r10d, word [r11d]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_103270,2,movzx r10d, word [r11d]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_103281,2,mov r10d, [dtu_fs_sel]|PUSH32 r10d ; fsgs
loc_103283,2,mov r10d, [dtu_gs_sel]|PUSH32 r10d ; fsgs
loc_103285,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_1032B0,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_1032B1,2,movzx r10d, word [r11d]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_1032B3,2,movzx r10d, word [r11d]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_1032BF,2,movzx r10d, ax|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_1032C1,2,movzx r10d, ax|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d ; fsgs
loc_1032C3,2,movzx r10d, ax|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_1032CD,2,mov r10d, [dtu_fs_sel]|PUSH32 r10d ; fsgs
loc_1032CF,2,mov r10d, [dtu_gs_sel]|PUSH32 r10d ; fsgs
loc_1032D1,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_1032E0,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_1032E1,2,movzx r10d, word [r11d]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_1032E3,2,movzx r10d, word [r11d]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_1032F4,2,mov r10d, [dtu_fs_sel]|PUSH32 r10d ; fsgs
loc_1032F6,2,mov r10d, [dtu_gs_sel]|PUSH32 r10d ; fsgs
loc_1032F8,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_10332C,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_10334E,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_10334F,2,movzx r10d, word [r11d]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_103351,2,movzx r10d, word [r11d]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_103362,2,mov r10d, [dtu_fs_sel]|PUSH32 r10d ; fsgs
loc_103364,2,mov r10d, [dtu_gs_sel]|PUSH32 r10d ; fsgs
loc_103366,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_10339A,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_10339D,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_10339E,2,movzx r10d, word [r11d]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_1033A0,2,movzx r10d, word [r11d]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_1033B1,2,mov r10d, [dtu_fs_sel]|PUSH32 r10d ; fsgs
loc_1033B3,2,mov r10d, [dtu_gs_sel]|PUSH32 r10d ; fsgs
loc_1033B5,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_103436,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_103437,2,movzx r10d, word [r11d]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_103439,2,movzx r10d, word [r11d]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_103445,2,movzx r10d, ax|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_103447,2,movzx r10d, ax|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d ; fsgs
loc_103449,2,movzx r10d, ax|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_103453,2,mov r10d, [dtu_fs_sel]|PUSH32 r10d ; fsgs
loc_103455,2,mov r10d, [dtu_gs_sel]|PUSH32 r10d ; fsgs
loc_103457,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_103462,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_103463,2,movzx r10d, word [r11d]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_103465,2,movzx r10d, word [r11d]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_103695,4,movzx r10d, word [r11d + 0x20]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_1036A1,5,mov r10d, [dtu_gs_base]|mov word [r10d + edx + 0x4], ax ; fsgs
loc_1036AA,3,mov r10d, [dtu_gs_base]|mov dword [r10d + edx], eax ; fsgs
loc_1036AD,5,mov edx, dword [r11d + 0x3c]|movzx r10d, word [r11d + 0x3c + 4]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_1036B6,5,mov r10d, [dtu_gs_base]|mov word [r10d + edx + 0x4], ax ; fsgs
loc_1036BB,3,mov r10d, [dtu_gs_base]|mov dword [r10d + edx], edi ; fsgs
loc_10394F,4,movzx r10d, word [r11d + 0x1c]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_10395B,5,mov r10d, [dtu_gs_base]|mov word [r10d + edx + 0x4], ax ; fsgs
loc_103964,3,mov r10d, [dtu_gs_base]|mov dword [r10d + edx], eax ; fsgs
loc_103967,5,mov edx, dword [r11d + 0x3c]|movzx r10d, word [r11d + 0x3c + 4]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_103970,3,mov r10d, [dtu_gs_base]|mov dword [r10d + edx], edi ; fsgs
loc_103977,5,mov r10d, [dtu_gs_base]|mov word [r10d + edx + 0x4], ax ; fsgs
loc_10466B,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_10466C,2,mov r10d, [dtu_fs_sel]|PUSH32 r10d ; fsgs
loc_10466E,2,mov r10d, [dtu_gs_sel]|PUSH32 r10d ; fsgs
loc_104671,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_10467E,2,movzx r10d, word [r11d]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_104680,2,movzx r10d, word [r11d]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_104682,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_1046BD,2,mov r10d, [dtu_fs_sel]|PUSH32 r10d ; fsgs
loc_1046BF,2,mov r10d, [dtu_gs_sel]|PUSH32 r10d ; fsgs
loc_1046C1,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_1046F1,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_1046F2,2,movzx r10d, word [r11d]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_1046F4,2,movzx r10d, word [r11d]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_104705,2,mov r10d, [dtu_fs_sel]|PUSH32 r10d ; fsgs
loc_104707,2,mov r10d, [dtu_gs_sel]|PUSH32 r10d ; fsgs
loc_104709,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_10470A,3,mov edi, dword [ebp + 0xc]|movzx r10d, word [ebp + 0xc + 4]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_104713,7,mov r10d, [dtu_es_sel]|mov word [loc_14F260], r10w ; fsgs
loc_104780,7,mov r10d, [dtu_es_sel]|mov word [loc_14F254], r10w ; fsgs
loc_10478B,2,movzx r10d, ax|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_1047B5,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_1047B6,2,movzx r10d, word [r11d]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_1047B8,2,movzx r10d, word [r11d]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_1047C9,2,mov r10d, [dtu_fs_sel]|PUSH32 r10d ; fsgs
loc_1047CB,2,mov r10d, [dtu_gs_sel]|PUSH32 r10d ; fsgs
loc_1047CD,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_1047E3,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_1047E4,2,movzx r10d, word [r11d]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_1047E6,2,movzx r10d, word [r11d]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_1047F7,2,mov r10d, [dtu_fs_sel]|PUSH32 r10d ; fsgs
loc_1047F9,2,mov r10d, [dtu_gs_sel]|PUSH32 r10d ; fsgs
loc_1047FB,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_104811,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_104812,2,movzx r10d, word [r11d]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_104814,2,movzx r10d, word [r11d]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_104825,2,mov r10d, [dtu_fs_sel]|PUSH32 r10d ; fsgs
loc_104827,2,mov r10d, [dtu_gs_sel]|PUSH32 r10d ; fsgs
loc_104829,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_10488F,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_104890,2,movzx r10d, word [r11d]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_104892,2,movzx r10d, word [r11d]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_1048B7,2,movzx r10d, word [r11d]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_1048B9,2,movzx r10d, word [r11d]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_1048BB,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_105AE6,2,mov r10d, [dtu_fs_sel]|PUSH32 r10d ; fsgs
loc_105BDF,7,movzx r10d, word [loc_1512D8]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d ; fsgs
loc_105BE6,3,mov r10d, [dtu_fs_base]|mov al, byte [r10d + ecx] ; fsgs
loc_105C50,2,movzx r10d, word [r11d]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_105C60,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_105D93,7,movzx r10d, word [loc_46C698]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_105E92,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + ebx] ; fsgs
loc_105EE7,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_105F18,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + ebx] ; fsgs
loc_10607D,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_1060AF,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_1060E1,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_106113,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_106145,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_106177,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_1061A9,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_1061DB,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_10620D,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_10623F,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_106271,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_1062A3,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_1062D5,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_106307,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_106339,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_10636B,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_1063A2,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_1063BC,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_1063D9,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_1063F3,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_106428,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_106442,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_10645F,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_106479,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_1064AE,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_1064C8,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_1064E5,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_1064FF,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_106534,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_10654E,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_10656B,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_106585,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_1066C6,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_1066FE,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_10673C,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_106776,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_1067B4,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_1067EE,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_10682C,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_106866,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_1068A4,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_1068DE,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_10691C,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_106956,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_106994,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_1069CE,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_106A08,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_106A42,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_106AA0,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + ebx] ; fsgs
loc_106B26,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + ebx] ; fsgs
loc_106C51,3,mov r10d, [dtu_es_base]|mov cl, byte [r10d + eax] ; fsgs
loc_106C5E,3,mov r9, rsi|mov esi, [dtu_es_base]|lea esi, [esi + eax]|mov ch, byte [esi]|mov rsi, r9 ; fsgs
loc_106C70,3,mov r10d, [dtu_es_base]|mov cl, byte [r10d + eax] ; fsgs
loc_106C7F,3,mov r9, rsi|mov esi, [dtu_es_base]|lea esi, [esi + eax]|mov ch, byte [esi]|mov rsi, r9 ; fsgs
loc_106CCD,3,mov r10d, [dtu_es_base]|mov cl, byte [r10d + eax] ; fsgs
loc_106CDA,3,mov r9, rsi|mov esi, [dtu_es_base]|lea esi, [esi + eax]|mov ch, byte [esi]|mov rsi, r9 ; fsgs
loc_106CEC,3,mov r10d, [dtu_es_base]|mov cl, byte [r10d + eax] ; fsgs
loc_106CFB,3,mov r9, rsi|mov esi, [dtu_es_base]|lea esi, [esi + eax]|mov ch, byte [esi]|mov rsi, r9 ; fsgs
loc_106D49,3,mov r10d, [dtu_es_base]|mov cl, byte [r10d + eax] ; fsgs
loc_106D56,3,mov r9, rsi|mov esi, [dtu_es_base]|lea esi, [esi + eax]|mov ch, byte [esi]|mov rsi, r9 ; fsgs
loc_106D68,3,mov r10d, [dtu_es_base]|mov cl, byte [r10d + eax] ; fsgs
loc_106D77,3,mov r9, rsi|mov esi, [dtu_es_base]|lea esi, [esi + eax]|mov ch, byte [esi]|mov rsi, r9 ; fsgs
loc_106DC5,3,mov r10d, [dtu_es_base]|mov cl, byte [r10d + eax] ; fsgs
loc_106DD2,3,mov r9, rsi|mov esi, [dtu_es_base]|lea esi, [esi + eax]|mov ch, byte [esi]|mov rsi, r9 ; fsgs
loc_106DE4,3,mov r10d, [dtu_es_base]|mov cl, byte [r10d + eax] ; fsgs
loc_106DF3,3,mov r9, rsi|mov esi, [dtu_es_base]|lea esi, [esi + eax]|mov ch, byte [esi]|mov rsi, r9 ; fsgs
loc_106ECA,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_106EE6,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_106F03,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_106F20,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_106F3D,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_106F5A,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_106F77,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_106F94,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_106FB1,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_106FCE,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_106FEB,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_107008,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_107025,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_107042,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_10705F,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_10707C,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_107284,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_107285,2,mov r10d, [dtu_fs_sel]|PUSH32 r10d ; fsgs
loc_107288,7,movzx r10d, word [loc_46C698]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_10728F,7,movzx r10d, word [loc_466AB6]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d ; fsgs
loc_10732C,4,mov r10d, [dtu_es_base]|movzx eax, byte [r10d + ebx] ; fsgs
loc_10736B,2,movzx r10d, word [r11d]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_10736D,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_107392,4,mov r10d, [dtu_es_base]|movzx eax, byte [r10d + ebx] ; fsgs
loc_1074E1,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_10750E,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_10753B,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_107568,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_107595,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_1075C2,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_1075EF,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_10761C,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_10764E,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_10765F,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_107673,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_107684,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_1076B1,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_1076C2,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_1076D6,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_1076E7,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_1077B6,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_1077EB,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_107826,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_10785D,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_107898,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_1078CF,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_10790A,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_107941,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_10799A,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + ebx] ; fsgs
loc_107AD3,3,mov r10d, [dtu_es_base]|mov cl, byte [r10d + eax] ; fsgs
loc_107AE5,3,mov r10d, [dtu_es_base]|mov cl, byte [r10d + eax] ; fsgs
loc_107AF7,3,mov r10d, [dtu_es_base]|mov cl, byte [r10d + eax] ; fsgs
loc_107B09,3,mov r10d, [dtu_es_base]|mov cl, byte [r10d + eax] ; fsgs
loc_107BD1,3,mov r10d, [dtu_es_base]|mov cl, byte [r10d + eax] ; fsgs
loc_107BE3,3,mov r10d, [dtu_es_base]|mov cl, byte [r10d + eax] ; fsgs
loc_107BF5,3,mov r10d, [dtu_es_base]|mov cl, byte [r10d + eax] ; fsgs
loc_107C07,3,mov r10d, [dtu_es_base]|mov cl, byte [r10d + eax] ; fsgs
loc_107D57,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_107D88,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_107DBB,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_107DEE,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_107E21,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_107E54,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_107E87,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_107EBA,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + eax] ; fsgs
loc_10A192,2,movzx eax, word [dtu_es_sel] ; fsgs
loc_10A19D,2,movzx eax, word [dtu_fs_sel] ; fsgs
loc_10A1A3,2,movzx eax, word [dtu_gs_sel] ; fsgs
loc_10A1AD,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_10A1DE,3,mov r10d, [dtu_es_sel]|mov word [ebx], r10w ; fsgs
loc_10A1E1,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_10A1EF,2,movzx r10d, word [ebx]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_10A20A,2,mov r10d, [dtu_gs_sel]|PUSH32 r10d ; fsgs
loc_10A20C,2,mov r10d, [dtu_fs_sel]|PUSH32 r10d ; fsgs
loc_10A20E,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_10A240,4,mov r10d, [dtu_es_sel]|mov word [ebx + 0x1e], r10w ; fsgs
loc_10A244,4,mov r10d, [dtu_fs_sel]|mov word [ebx + 0x20], r10w ; fsgs
loc_10A248,4,mov r10d, [dtu_gs_sel]|mov word [ebx + 0x22], r10w ; fsgs
loc_10A24F,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_10A250,2,movzx r10d, word [r11d]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_10A252,2,movzx r10d, word [r11d]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_10A27C,3,movzx r10d, word [ebx + 0x1e]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_10A27F,3,movzx r10d, word [ebx + 0x20]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d ; fsgs
loc_10A282,3,movzx r10d, word [ebx + 0x22]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_10A5AB,4,movzx r10d, word [r11d + 0x14]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_10A5BD,3,mov r10d, [dtu_gs_base]|mov byte [r10d + edx], bl ; fsgs
loc_10A5D7,4,movzx r10d, word [r11d + 0x14]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_10A5E8,3,mov r10d, [dtu_gs_base]|mov byte [r10d + ebx], cl ; fsgs
loc_10A5FF,4,movzx r10d, word [r11d + 0x14]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d ; fsgs
loc_10A614,3,mov r10d, [dtu_gs_base]|mov bl, byte [r10d + ebx] ; fsgs
loc_10BCEF,3,mov r10d, [dtu_es_base]|mov byte [r10d + edi], al ; fsgs
loc_10BD02,3,mov r10d, [dtu_es_base]|mov byte [r10d + edi], al ; fsgs
loc_10BD51,3,mov r10d, [dtu_es_base]|mov al, byte [r10d + edi] ; fsgs
loc_10BD57,3,mov r10d, [dtu_es_base]|mov byte [r10d + edi], al ; fsgs
loc_10BD6A,3,mov r10d, [dtu_es_base]|mov byte [r10d + edi], al ; fsgs
loc_10BDC8,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_10BDFF,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_10BE09,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_10BE12,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_10BE4F,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_10BE59,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_10C16F,3,movzx r10d, ax|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_10C394,3,movzx r10d, ax|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_10C62D,3,movzx r10d, ax|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_10C84D,3,movzx r10d, ax|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_10CACE,3,movzx r10d, ax|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_10E564,3,movzx r10d, ax|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_10F6D4,3,movzx r10d, ax|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_11165D,3,movzx r10d, ax|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_1118D9,3,movzx r10d, ax|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_111AEB,3,movzx r10d, ax|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_111F3E,3,movzx r10d, ax|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_1123CE,3,movzx r10d, ax|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_113570,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_113573,2,movzx r10d, ax|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_113585,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_113823,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_113826,2,movzx r10d, ax|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_113838,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_1156E2,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_115745,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_115751,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_1157AA,3,movzx r10d, cx|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_115831,3,movzx r10d, word [dtu_fs_sel]|mov cx, r10w ; fsgs
loc_115839,3,movzx r10d, word [dtu_gs_sel]|mov cx, r10w ; fsgs
loc_115975,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_11597D,3,movzx r10d, cx|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_115998,3,movzx r10d, cx|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_1159B1,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_115AB5,3,mov r10d, [dtu_es_base]|mov eax, dword [r10d + esi] ; fsgs
loc_115AD4,3,mov r10d, [dtu_es_base]|mov eax, dword [r10d + esi] ; fsgs
loc_115AF3,3,mov r10d, [dtu_es_base]|mov eax, dword [r10d + esi] ; fsgs
loc_115B0F,3,mov r10d, [dtu_es_base]|mov eax, dword [r10d + esi] ; fsgs
loc_115B2B,3,mov r10d, [dtu_es_base]|mov eax, dword [r10d + esi] ; fsgs
loc_115B4A,3,mov r10d, [dtu_es_base]|mov eax, dword [r10d + esi] ; fsgs
loc_115B69,3,mov r10d, [dtu_es_base]|mov eax, dword [r10d + esi] ; fsgs
loc_115B88,3,mov r10d, [dtu_es_base]|mov eax, dword [r10d + esi] ; fsgs
loc_115BD0,3,mov r10d, [dtu_es_base]|mov eax, dword [r10d + esi] ; fsgs
loc_115BEA,3,mov r10d, [dtu_es_base]|mov dword [r10d + esi], eax ; fsgs
loc_115BFB,3,mov r10d, [dtu_es_base]|mov dword [r10d + esi], eax ; fsgs
loc_115C09,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_115C0A,3,movzx r10d, word [dtu_es_sel]|mov ax, r10w ; fsgs
loc_115C10,3,movzx r10d, dx|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_115C1A,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_115C21,4,mov r10d, [dtu_es_base]|mov ax, word [r10d + esi] ; fsgs
loc_115C7A,4,mov r10d, [dtu_es_base]|mov word [r10d + esi], ax ; fsgs
loc_115C83,3,mov r10d, [dtu_es_base]|mov eax, dword [r10d + esi] ; fsgs
loc_115CA2,3,mov r10d, [dtu_es_base]|mov eax, dword [r10d + esi] ; fsgs
loc_115CC1,3,mov r10d, [dtu_es_base]|mov eax, dword [r10d + esi] ; fsgs
loc_115CDD,3,mov r10d, [dtu_es_base]|mov eax, dword [r10d + esi] ; fsgs
loc_115CF9,3,mov r10d, [dtu_es_base]|mov eax, dword [r10d + esi] ; fsgs
loc_115D18,3,mov r10d, [dtu_es_base]|mov eax, dword [r10d + esi] ; fsgs
loc_115D37,3,mov r10d, [dtu_es_base]|mov eax, dword [r10d + esi] ; fsgs
loc_115D56,3,mov r10d, [dtu_es_base]|mov eax, dword [r10d + esi] ; fsgs
loc_115D9E,3,mov r10d, [dtu_es_base]|mov eax, dword [r10d + esi] ; fsgs
loc_115DD7,3,mov r10d, [dtu_es_base]|mov dword [r10d + esi], eax ; fsgs
loc_115E07,3,mov r10d, [dtu_es_base]|mov dword [r10d + esi], eax ; fsgs
loc_115E0F,3,mov r10d, [dtu_es_base]|mov eax, dword [r10d + esi] ; fsgs
loc_115E12,4,mov r10d, [dtu_es_base]|mov edx, dword [r10d + esi + 0x4] ; fsgs
loc_115E32,3,mov r10d, [dtu_es_base]|mov eax, dword [r10d + esi] ; fsgs
loc_115E35,4,mov r10d, [dtu_es_base]|mov edx, dword [r10d + esi + 0x4] ; fsgs
loc_115E55,3,mov r10d, [dtu_es_base]|mov eax, dword [r10d + esi] ; fsgs
loc_115E58,4,mov r10d, [dtu_es_base]|mov edx, dword [r10d + esi + 0x4] ; fsgs
loc_115E75,3,mov r10d, [dtu_es_base]|mov eax, dword [r10d + esi] ; fsgs
loc_115E78,4,mov r10d, [dtu_es_base]|mov edx, dword [r10d + esi + 0x4] ; fsgs
loc_115E95,3,mov r10d, [dtu_es_base]|mov eax, dword [r10d + esi] ; fsgs
loc_115E98,4,mov r10d, [dtu_es_base]|mov edx, dword [r10d + esi + 0x4] ; fsgs
loc_115EB8,3,mov r10d, [dtu_es_base]|mov eax, dword [r10d + esi] ; fsgs
loc_115EBB,4,mov r10d, [dtu_es_base]|mov edx, dword [r10d + esi + 0x4] ; fsgs
loc_115EDB,3,mov r10d, [dtu_es_base]|mov eax, dword [r10d + esi] ; fsgs
loc_115EDE,4,mov r10d, [dtu_es_base]|mov edx, dword [r10d + esi + 0x4] ; fsgs
loc_115EFE,3,mov r10d, [dtu_es_base]|mov eax, dword [r10d + esi] ; fsgs
loc_115F01,4,mov r10d, [dtu_es_base]|mov edx, dword [r10d + esi + 0x4] ; fsgs
loc_115F4A,3,mov r10d, [dtu_es_base]|mov eax, dword [r10d + esi] ; fsgs
loc_115F51,4,mov r10d, [dtu_es_base]|mov eax, dword [r10d + esi + 0x4] ; fsgs
loc_115F59,5,mov r10d, [dtu_es_base]|mov ax, word [r10d + esi + 0x8] ; fsgs
loc_115F91,3,mov r10d, [dtu_es_base]|mov eax, dword [r10d + esi] ; fsgs
loc_115F94,4,mov r10d, [dtu_es_base]|mov edx, dword [r10d + esi + 0x4] ; fsgs
loc_115FAF,3,mov r10d, [dtu_es_base]|mov dword [r10d + esi], eax ; fsgs
loc_115FB2,4,mov r10d, [dtu_es_base]|mov dword [r10d + esi + 0x4], edx ; fsgs
loc_115FBF,3,mov r10d, [dtu_es_base]|mov dword [r10d + esi], eax ; fsgs
loc_115FC6,4,mov r10d, [dtu_es_base]|mov dword [r10d + esi + 0x4], eax ; fsgs
loc_115FCF,5,mov r10d, [dtu_es_base]|mov word [r10d + esi + 0x8], ax ; fsgs
loc_115FE2,3,mov r10d, [dtu_es_base]|mov dword [r10d + esi], eax ; fsgs
loc_115FE5,4,mov r10d, [dtu_es_base]|mov dword [r10d + esi + 0x4], edx ; fsgs
loc_115FF4,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_115FF5,3,movzx r10d, word [dtu_es_sel]|mov ax, r10w ; fsgs
loc_115FFB,3,movzx r10d, dx|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_116005,4,mov r10d, [dtu_es_base]|mov ebx, dword [r10d + ebp + 0x4] ; fsgs
loc_11602E,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_116065,4,mov r10d, [dtu_es_base]|mov word [r10d + esi], ax ; fsgs
loc_11606E,4,mov r10d, [dtu_es_base]|mov ax, word [r10d + esi] ; fsgs
loc_11608F,4,mov r10d, [dtu_es_base]|mov ax, word [r10d + esi] ; fsgs
loc_1160B0,4,mov r10d, [dtu_es_base]|mov ax, word [r10d + esi] ; fsgs
loc_1160CE,4,mov r10d, [dtu_es_base]|mov ax, word [r10d + esi] ; fsgs
loc_1160EC,4,mov r10d, [dtu_es_base]|mov ax, word [r10d + esi] ; fsgs
loc_11610D,4,mov r10d, [dtu_es_base]|mov ax, word [r10d + esi] ; fsgs
loc_11612E,4,mov r10d, [dtu_es_base]|mov ax, word [r10d + esi] ; fsgs
loc_11614F,4,mov r10d, [dtu_es_base]|mov ax, word [r10d + esi] ; fsgs
loc_116199,4,mov r10d, [dtu_es_base]|mov ax, word [r10d + esi] ; fsgs
loc_1161DF,4,mov r10d, [dtu_es_base]|mov word [r10d + esi], ax ; fsgs
loc_11621B,4,mov r10d, [dtu_es_base]|mov word [r10d + esi], ax ; fsgs
loc_116252,3,mov r10d, [dtu_es_base]|mov eax, dword [r10d + esi] ; fsgs
loc_116284,3,mov r10d, [dtu_es_base]|mov dword [r10d + esi], eax ; fsgs
loc_118648,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_118649,2,movzx r10d, dx|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_118671,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_11867E,4,mov r10d, [dtu_es_base]|mov bl, byte [r10d + eax + 0x40] ; fsgs
loc_118682,4,mov r10d, [dtu_es_base]|mov byte [r10d + eax + 0x3e], dl ; fsgs
loc_118689,4,mov r10d, [dtu_es_base]|mov byte [r10d + eax + 0x40], bl ; fsgs
loc_11869B,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_1188C5,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_11890A,2,movzx r10d, di|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_11891D,2,movzx r10d, ax|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_118984,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_119C1B,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_119C1C,2,mov r10d, [dtu_fs_sel]|PUSH32 r10d ; fsgs
loc_119C2B,6,movzx r10d, word [loc_14D36D]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d ; fsgs
loc_119C35,2,movzx eax, word [dtu_fs_sel] ; fsgs
loc_119C3D,2,movzx r10d, ax|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_119C41,3,mov r10d, [dtu_es_base]|cmp dl, byte [r10d + eax] ; fsgs
loc_119C46,3,mov r9, rsi|mov esi, [dtu_es_base]|lea esi, [esi + eax]|mov dh, byte [esi]|mov rsi, r9 ; fsgs
loc_119C8D,2,movzx eax, word [dtu_fs_sel] ; fsgs
loc_119C91,2,movzx r10d, ax|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_119C95,4,mov r10d, [dtu_es_base]|cmp byte [r10d + eax], 0x0 ; fsgs
loc_119CA4,3,mov r10d, [dtu_es_base]|mov dl, byte [r10d + eax] ; fsgs
loc_119CE1,2,movzx r10d, word [r11d]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_119CE3,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_11A25C,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_11A25D,2,mov r10d, [dtu_fs_sel]|PUSH32 r10d ; fsgs
loc_11A25F,2,mov r10d, [dtu_gs_sel]|PUSH32 r10d ; fsgs
loc_11A2E7,2,movzx r10d, word [r11d]|mov [dtu_gs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_gs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_11A2E9,2,movzx r10d, word [r11d]|mov [dtu_fs_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_fs_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_11A2EB,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_11A2F1,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_11A2F2,2,mov r10d, [dtu_fs_sel]|PUSH32 r10d ; fsgs
loc_11A2F4,2,mov r10d, [dtu_gs_sel]|PUSH32 r10d ; fsgs
loc_11B948,1,mov r10d, [dtu_es_sel]|PUSH32 r10d ; fsgs
loc_11B94E,6,mov esi, dword [loc_153718]|movzx r10d, word [loc_153718 + 4]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d ; fsgs
loc_11B95E,4,mov r10d, [dtu_es_base]|inc dword [r10d + esi + 0x36] ; fsgs
loc_11B962,5,mov r10d, [dtu_es_base]|mov ds, word [r10d + esi + 0x18] ; fsgs
loc_11B967,4,mov r10d, [dtu_es_base]|mov eax, dword [r10d + esi + 0x14] ; fsgs
loc_11B986,1,movzx r10d, word [r11d]|mov [dtu_es_sel], r10d|mov r10d, [dtu_sel_base + r10*4]|mov [dtu_es_base], r10d|lea r11d, [r11d+4] ; fsgs
loc_1BBCE,3,mov r10d, dtu_zero_page|test eax, eax|cmovnz r10d, eax|cmp byte [r10d], 0x40 ; strtok result may be NULL (read IVT under DOS): read an empty string instead
loc_1BBF7,5,mov eax, [loc_1B5648]|mov r10d, dtu_zero_page|test eax, eax|cmovz eax, r10d ; strcmp(NULL, ...) every frame: compare an empty string instead
loc_DA3F1,3,xor ebx, ebx ; keyboard flush: BIOS buffer tail at 0040:001C is unused here (head/tail both stay 0); avoids a NULL-page fault per call
loc_DA3F6,3,nop ; keyboard flush: head = tail (BIOS buffer unused)
loc_1017B,3,lea r10d, [edi + 0x2]|mov r9d, dtu_zero_page|cmp r10d, 0xA0000|cmovb r10d, r9d|mov al, [r10d] ; called with a NULL object every frame: read the zero byte without faulting (flags are set again by the next cmp)
loc_40EAF,5,call SR_FrameHook|CALL loc_DFAE0 ; once per frame, before the frame time is measured: frame limiter, fps counter, pending timer ticks
loc_2C03E,2,imul dword [dtu_friction]|mov r10d, edx|sar r10d, 31|and r10d, 0xFFFF|add eax, r10d|adc edx, 0|shrd eax, edx, 16 ; velocity damping without forward input (strafing, stopping): v/3 per frame made strafing depend on the frame rate; now v*dtu_friction/65536 (rounded toward 0), set per frame from the frame time
loc_2C051,2,imul dword [dtu_friction]|mov r10d, edx|sar r10d, 31|and r10d, 0xFFFF|add eax, r10d|adc edx, 0|shrd eax, edx, 16 ; velocity damping without forward input (strafing, stopping): v/3 per frame made strafing depend on the frame rate; now v*dtu_friction/65536 (rounded toward 0), set per frame from the frame time
loc_2C064,2,imul dword [dtu_friction]|mov r10d, edx|sar r10d, 31|and r10d, 0xFFFF|add eax, r10d|adc edx, 0|shrd eax, edx, 16 ; velocity damping without forward input (strafing, stopping): v/3 per frame made strafing depend on the frame rate; now v*dtu_friction/65536 (rounded toward 0), set per frame from the frame time
loc_2C077,2,imul dword [dtu_friction]|mov r10d, edx|sar r10d, 31|and r10d, 0xFFFF|add eax, r10d|adc edx, 0|shrd eax, edx, 16 ; velocity damping without forward input (strafing, stopping): v/3 per frame made strafing depend on the frame rate; now v*dtu_friction/65536 (rounded toward 0), set per frame from the frame time
loc_9F17E,5,lea r10d, [eax + 0x7e]|mov r9d, dtu_zero_page|cmp r10d, 0xA0000|cmovb r10d, r9d|cmp word [r10d], 1 ; reads through an uninitialised local ([esp+4]) that is 0 here (bug in the original, harmless under DOS): read the zero byte without a fault (thousands of times per second in combat)
loc_FE71C,5,mov edx, SR_dtu_efg_format ; printf %e/%f/%g in C (crt_efg_format)
loc_96439,1,mov r9d, 0xEF0002|CALL x86_int|jmp loc_96405 ; movie player (MPLAY32) replaced by mve.c; then the original cleanup (0x96405)
loc_E36AC,1,mov r9d, 0xEF0101|CALL x86_int|RET ; sosDIGIInitSystem -> sos.c
loc_E3714,5,mov r9d, 0xEF0102|CALL x86_int|RET ; sosDIGIUnInitSystem -> sos.c
loc_E3728,1,mov r9d, 0xEF0103|CALL x86_int|RET 16 ; sosDIGIInitDriver -> sos.c (4 stack arguments)
loc_E3EA8,1,mov r9d, 0xEF0104|CALL x86_int|RET ; sosDIGIUnInitDriver -> sos.c
loc_E4854,1,mov r9d, 0xEF0105|CALL x86_int|RET ; sosDIGIStartSample -> sos.c
loc_E4C30,1,mov r9d, 0xEF0106|CALL x86_int|RET ; sosDIGIStopSample -> sos.c
loc_E4C90,1,mov r9d, 0xEF0107|CALL x86_int|RET ; sosDIGIContinueSample -> sos.c
loc_E4ECC,1,mov r9d, 0xEF0108|CALL x86_int|RET ; sosDIGISetSampleVolume -> sos.c
loc_E5040,1,mov r9d, 0xEF0109|CALL x86_int|RET ; sosDIGISetPanLocation -> sos.c
loc_E51E4,1,mov r9d, 0xEF010A|CALL x86_int|RET ; sosDIGIGetSampleHandle -> sos.c
loc_E5254,1,mov r9d, 0xEF010B|CALL x86_int|RET ; sosDIGISampleDone -> sos.c
loc_2D42C,2,mov edx, 0x18 ; cs for the SOS timer hook (the recompiler turns mov reg, cs into 0, and the game's timer interrupt skips a hook with selector 0)
loc_104968,1,mov r9d, 0xEF0020|CALL x86_int|RET ; scaled bitmap row with self-modifying unrolled loop -> render.c
loc_1059F4,1,mov r9d, 0xEF0021|CALL x86_int|RET ; its setup (patches the loop) -> render.c
loc_EAA01,2,cmp dword [dtu_sprite_cull_on], 0|je short loc_EAA01_draw|mov r9d, 0xEF0028|CALL x86_int|test eax, eax|jz short loc_EAA01_draw|jmp loc_EAA86|loc_EAA01_draw:|mov eax, ebx ; g3_draw_bitmap: a sprite right at the camera (water splashes at your feet) is drawn a little further away -> render.c; drawn as it was, one texel filled the whole view (in DOS as well)
loc_675D4,7,cmp dword [dtu_obj_affine], 0 ; DTU forces linear (affine) texture mapping for all polygon objects ([0x13D3E3] = 1, never changed), so textures on beds and tables swim with the viewing angle. Now chosen by the runtime: 0 = the normal choice (perspective nearby, as for walls)
loc_585FB,5,cmp dword [dtu_far_detail], 0|je short loc_585FB_keep|mov r9d, [dtu_far_detail]|mov [loc_46C68C], r9d|mov dword [loc_46C688], 99|mov dword [loc_1F3AF8], 99|loc_585FB_keep:|mov eax, [loc_13CD18] ; set_detail_level: perspective mapping (smooth lighting) and no flat shading up to the view distance, for walls and objects; the original switched to linear and then flat polygons a few segments away, visible as jumps in lighting
loc_58815,5,cmp dword [dtu_far_detail], 0|je short loc_58815_keep|mov r9d, [dtu_far_detail]|mov [loc_46C68C], r9d|mov dword [loc_46C688], 99|mov dword [loc_1F3AF8], 99|loc_58815_keep:|mov eax, [loc_13CD18] ; set_detail_level: perspective mapping (smooth lighting) and no flat shading up to the view distance, for walls and objects; the original switched to linear and then flat polygons a few segments away, visible as jumps in lighting
loc_675AB,3,cmp dword [dtu_weapon_light], 0|je short loc_675AB_orig|movsx eax, word [esi + 0x1a6]|loc_675AB_orig:|cmp eax, 0xf ; while you attack, the light of your own model (the weapon) doubles every frame as long as light/4096 < 15, so it overflows the 16-bit light value and the weapon is drawn black for a frame (about 3 times per swing at 60 fps, in DOS as well). Now the doubling stops at light 15 (dtu_weapon_light = 0: original)
loc_6747E,2,mov esi, eax|mov [dtu_anim_obj], eax ; object renderer: remember the object for the smooth animation in draw_polygon_model (render.c)
loc_67A00,3,mov dword [dtu_anim_obj], 0|mov eax, [ebp + 0x6a] ; end of the object renderer
loc_72A51,4,mov eax, ebp|mov edx, edi|mov r9d, 0xEF0022|CALL x86_int|mov [r11d + 0x58], eax ; draw_polygon_model: submodel angles of the frame -> render.c (between two key frames)
loc_729F3,2,mov edx, eax|mov eax, esi|mov r9d, 0xEF0023|CALL x86_int|mov esi, eax ; draw_polygon_model: root offset of the frame -> render.c
loc_64AC9,5,cmp dword [dtu_menu_mouse], 0|jne short loc_64AC9_ptr|CALL loc_66CCF|jmp short loc_64AC9_done|loc_64AC9_ptr:|mov r9d, 0xEF0024|CALL x86_int|loc_64AC9_done: ; text menus (newmenu): mouse pointer instead of mouse motion as arrow keys -> menu.c (dtu_menu_mouse = 0: original)
loc_B2901,3,cmp dword [dtu_dialog_keys], 0|je short loc_B2901_orig|cmp eax, 0x2|jb short loc_B2901_orig|cmp eax, 0xb|ja short loc_B2901_orig|lea r9d, [eax - 2]|cmp r9d, [loc_1465AC]|jge short loc_B2901_orig|mov [loc_3D4990], r9d|jmp loc_B2A5E|loc_B2901_orig:|cmp eax, 0x39 ; dialog answers (dlgmenu, key handler 0xB28F0): keys 1..9 and 0 choose answer 1..10 directly, as Enter does on the selected one (dtu_dialog_keys = 0: original)
loc_E7D4A,7,cmp dword [loc_13A94B], 0|je short loc_E7D4A_z|cmp dword [dtu_double_lines], 0|loc_E7D4A_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_E92F0,7,cmp dword [loc_13A94B], 0|je short loc_E92F0_z|cmp dword [dtu_double_lines], 0|loc_E92F0_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_EC7C3,7,cmp dword [loc_13A94B], 0|je short loc_EC7C3_z|cmp dword [dtu_double_lines], 0|loc_EC7C3_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_EC93E,7,cmp dword [loc_13A94B], 0|je short loc_EC93E_z|cmp dword [dtu_double_lines], 0|loc_EC93E_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_EC96C,7,cmp dword [loc_13A94B], 0|je short loc_EC96C_z|cmp dword [dtu_double_lines], 0|loc_EC96C_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_EC99D,7,cmp dword [loc_13A94B], 0|je short loc_EC99D_z|cmp dword [dtu_double_lines], 0|loc_EC99D_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_EC9E6,7,cmp dword [loc_13A94B], 0|je short loc_EC9E6_z|cmp dword [dtu_double_lines], 0|loc_EC9E6_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_ECA18,7,cmp dword [loc_13A94B], 0|je short loc_ECA18_z|cmp dword [dtu_double_lines], 0|loc_ECA18_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_ECA4D,7,cmp dword [loc_13A94B], 0|je short loc_ECA4D_z|cmp dword [dtu_double_lines], 0|loc_ECA4D_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_ECA98,7,cmp dword [loc_13A94B], 0|je short loc_ECA98_z|cmp dword [dtu_double_lines], 0|loc_ECA98_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_105D9A,7,cmp dword [loc_13A94B], 0|je short loc_105D9A_z|cmp dword [dtu_double_lines], 0|loc_105D9A_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_105EB6,7,cmp dword [loc_13A94B], 0|je short loc_105EB6_z|cmp dword [dtu_double_lines], 0|loc_105EB6_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_105F3A,7,cmp dword [loc_13A94B], 0|je short loc_105F3A_z|cmp dword [dtu_double_lines], 0|loc_105F3A_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_10608F,7,cmp dword [loc_13A94B], 0|je short loc_10608F_z|cmp dword [dtu_double_lines], 0|loc_10608F_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_1060C1,7,cmp dword [loc_13A94B], 0|je short loc_1060C1_z|cmp dword [dtu_double_lines], 0|loc_1060C1_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_1060F3,7,cmp dword [loc_13A94B], 0|je short loc_1060F3_z|cmp dword [dtu_double_lines], 0|loc_1060F3_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_106125,7,cmp dword [loc_13A94B], 0|je short loc_106125_z|cmp dword [dtu_double_lines], 0|loc_106125_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_106157,7,cmp dword [loc_13A94B], 0|je short loc_106157_z|cmp dword [dtu_double_lines], 0|loc_106157_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_106189,7,cmp dword [loc_13A94B], 0|je short loc_106189_z|cmp dword [dtu_double_lines], 0|loc_106189_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_1061BB,7,cmp dword [loc_13A94B], 0|je short loc_1061BB_z|cmp dword [dtu_double_lines], 0|loc_1061BB_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_1061ED,7,cmp dword [loc_13A94B], 0|je short loc_1061ED_z|cmp dword [dtu_double_lines], 0|loc_1061ED_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_10621F,7,cmp dword [loc_13A94B], 0|je short loc_10621F_z|cmp dword [dtu_double_lines], 0|loc_10621F_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_106251,7,cmp dword [loc_13A94B], 0|je short loc_106251_z|cmp dword [dtu_double_lines], 0|loc_106251_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_106283,7,cmp dword [loc_13A94B], 0|je short loc_106283_z|cmp dword [dtu_double_lines], 0|loc_106283_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_1062B5,7,cmp dword [loc_13A94B], 0|je short loc_1062B5_z|cmp dword [dtu_double_lines], 0|loc_1062B5_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_1062E7,7,cmp dword [loc_13A94B], 0|je short loc_1062E7_z|cmp dword [dtu_double_lines], 0|loc_1062E7_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_106319,7,cmp dword [loc_13A94B], 0|je short loc_106319_z|cmp dword [dtu_double_lines], 0|loc_106319_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_10634B,7,cmp dword [loc_13A94B], 0|je short loc_10634B_z|cmp dword [dtu_double_lines], 0|loc_10634B_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_10637D,7,cmp dword [loc_13A94B], 0|je short loc_10637D_z|cmp dword [dtu_double_lines], 0|loc_10637D_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_106406,7,cmp dword [loc_13A94B], 0|je short loc_106406_z|cmp dword [dtu_double_lines], 0|loc_106406_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_10648C,7,cmp dword [loc_13A94B], 0|je short loc_10648C_z|cmp dword [dtu_double_lines], 0|loc_10648C_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_106512,7,cmp dword [loc_13A94B], 0|je short loc_106512_z|cmp dword [dtu_double_lines], 0|loc_106512_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_106598,7,cmp dword [loc_13A94B], 0|je short loc_106598_z|cmp dword [dtu_double_lines], 0|loc_106598_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_1066DA,7,cmp dword [loc_13A94B], 0|je short loc_1066DA_z|cmp dword [dtu_double_lines], 0|loc_1066DA_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_106717,7,cmp dword [loc_13A94B], 0|je short loc_106717_z|cmp dword [dtu_double_lines], 0|loc_106717_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_106751,7,cmp dword [loc_13A94B], 0|je short loc_106751_z|cmp dword [dtu_double_lines], 0|loc_106751_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_10678F,7,cmp dword [loc_13A94B], 0|je short loc_10678F_z|cmp dword [dtu_double_lines], 0|loc_10678F_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_1067C9,7,cmp dword [loc_13A94B], 0|je short loc_1067C9_z|cmp dword [dtu_double_lines], 0|loc_1067C9_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_106807,7,cmp dword [loc_13A94B], 0|je short loc_106807_z|cmp dword [dtu_double_lines], 0|loc_106807_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_106841,7,cmp dword [loc_13A94B], 0|je short loc_106841_z|cmp dword [dtu_double_lines], 0|loc_106841_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_10687F,7,cmp dword [loc_13A94B], 0|je short loc_10687F_z|cmp dword [dtu_double_lines], 0|loc_10687F_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_1068B9,7,cmp dword [loc_13A94B], 0|je short loc_1068B9_z|cmp dword [dtu_double_lines], 0|loc_1068B9_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_1068F7,7,cmp dword [loc_13A94B], 0|je short loc_1068F7_z|cmp dword [dtu_double_lines], 0|loc_1068F7_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_106931,7,cmp dword [loc_13A94B], 0|je short loc_106931_z|cmp dword [dtu_double_lines], 0|loc_106931_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_10696F,7,cmp dword [loc_13A94B], 0|je short loc_10696F_z|cmp dword [dtu_double_lines], 0|loc_10696F_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_1069A9,7,cmp dword [loc_13A94B], 0|je short loc_1069A9_z|cmp dword [dtu_double_lines], 0|loc_1069A9_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_1069E7,7,cmp dword [loc_13A94B], 0|je short loc_1069E7_z|cmp dword [dtu_double_lines], 0|loc_1069E7_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_106A1D,7,cmp dword [loc_13A94B], 0|je short loc_106A1D_z|cmp dword [dtu_double_lines], 0|loc_106A1D_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_106A5B,7,cmp dword [loc_13A94B], 0|je short loc_106A5B_z|cmp dword [dtu_double_lines], 0|loc_106A5B_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_106AC2,7,cmp dword [loc_13A94B], 0|je short loc_106AC2_z|cmp dword [dtu_double_lines], 0|loc_106AC2_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_10729C,7,cmp dword [loc_13A94B], 0|je short loc_10729C_z|cmp dword [dtu_double_lines], 0|loc_10729C_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_107337,7,cmp dword [loc_13A94B], 0|je short loc_107337_z|cmp dword [dtu_double_lines], 0|loc_107337_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_107399,7,cmp dword [loc_13A94B], 0|je short loc_107399_z|cmp dword [dtu_double_lines], 0|loc_107399_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_1074ED,7,cmp dword [loc_13A94B], 0|je short loc_1074ED_z|cmp dword [dtu_double_lines], 0|loc_1074ED_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_10751A,7,cmp dword [loc_13A94B], 0|je short loc_10751A_z|cmp dword [dtu_double_lines], 0|loc_10751A_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_107547,7,cmp dword [loc_13A94B], 0|je short loc_107547_z|cmp dword [dtu_double_lines], 0|loc_107547_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_107574,7,cmp dword [loc_13A94B], 0|je short loc_107574_z|cmp dword [dtu_double_lines], 0|loc_107574_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_1075A1,7,cmp dword [loc_13A94B], 0|je short loc_1075A1_z|cmp dword [dtu_double_lines], 0|loc_1075A1_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_1075CE,7,cmp dword [loc_13A94B], 0|je short loc_1075CE_z|cmp dword [dtu_double_lines], 0|loc_1075CE_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_1075FB,7,cmp dword [loc_13A94B], 0|je short loc_1075FB_z|cmp dword [dtu_double_lines], 0|loc_1075FB_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_107628,7,cmp dword [loc_13A94B], 0|je short loc_107628_z|cmp dword [dtu_double_lines], 0|loc_107628_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_1077C6,7,cmp dword [loc_13A94B], 0|je short loc_1077C6_z|cmp dword [dtu_double_lines], 0|loc_1077C6_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_107800,7,cmp dword [loc_13A94B], 0|je short loc_107800_z|cmp dword [dtu_double_lines], 0|loc_107800_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_107837,7,cmp dword [loc_13A94B], 0|je short loc_107837_z|cmp dword [dtu_double_lines], 0|loc_107837_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_107872,7,cmp dword [loc_13A94B], 0|je short loc_107872_z|cmp dword [dtu_double_lines], 0|loc_107872_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_1078A9,7,cmp dword [loc_13A94B], 0|je short loc_1078A9_z|cmp dword [dtu_double_lines], 0|loc_1078A9_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_1078E4,7,cmp dword [loc_13A94B], 0|je short loc_1078E4_z|cmp dword [dtu_double_lines], 0|loc_1078E4_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_10791B,7,cmp dword [loc_13A94B], 0|je short loc_10791B_z|cmp dword [dtu_double_lines], 0|loc_10791B_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_107956,7,cmp dword [loc_13A94B], 0|je short loc_107956_z|cmp dword [dtu_double_lines], 0|loc_107956_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_1079A4,7,cmp dword [loc_13A94B], 0|je short loc_1079A4_z|cmp dword [dtu_double_lines], 0|loc_1079A4_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_107B32,7,cmp dword [loc_13A94B], 0|je short loc_107B32_z|cmp dword [dtu_double_lines], 0|loc_107B32_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_107B52,7,cmp dword [loc_13A94B], 0|je short loc_107B52_z|cmp dword [dtu_double_lines], 0|loc_107B52_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_107B73,7,cmp dword [loc_13A94B], 0|je short loc_107B73_z|cmp dword [dtu_double_lines], 0|loc_107B73_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_107B94,7,cmp dword [loc_13A94B], 0|je short loc_107B94_z|cmp dword [dtu_double_lines], 0|loc_107B94_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_107C30,7,cmp dword [loc_13A94B], 0|je short loc_107C30_z|cmp dword [dtu_double_lines], 0|loc_107C30_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_107C50,7,cmp dword [loc_13A94B], 0|je short loc_107C50_z|cmp dword [dtu_double_lines], 0|loc_107C50_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_107C71,7,cmp dword [loc_13A94B], 0|je short loc_107C71_z|cmp dword [dtu_double_lines], 0|loc_107C71_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_107C92,7,cmp dword [loc_13A94B], 0|je short loc_107C92_z|cmp dword [dtu_double_lines], 0|loc_107C92_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_107D63,7,cmp dword [loc_13A94B], 0|je short loc_107D63_z|cmp dword [dtu_double_lines], 0|loc_107D63_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_107D95,7,cmp dword [loc_13A94B], 0|je short loc_107D95_z|cmp dword [dtu_double_lines], 0|loc_107D95_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_107DC8,7,cmp dword [loc_13A94B], 0|je short loc_107DC8_z|cmp dword [dtu_double_lines], 0|loc_107DC8_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_107DFB,7,cmp dword [loc_13A94B], 0|je short loc_107DFB_z|cmp dword [dtu_double_lines], 0|loc_107DFB_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_107E2E,7,cmp dword [loc_13A94B], 0|je short loc_107E2E_z|cmp dword [dtu_double_lines], 0|loc_107E2E_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_107E61,7,cmp dword [loc_13A94B], 0|je short loc_107E61_z|cmp dword [dtu_double_lines], 0|loc_107E61_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_107E94,7,cmp dword [loc_13A94B], 0|je short loc_107E94_z|cmp dword [dtu_double_lines], 0|loc_107E94_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_107EC7,7,cmp dword [loc_13A94B], 0|je short loc_107EC7_z|cmp dword [dtu_double_lines], 0|loc_107EC7_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_10BC72,7,cmp dword [loc_13A94B], 0|je short loc_10BC72_z|cmp dword [dtu_double_lines], 0|loc_10BC72_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_10BCF2,7,cmp dword [loc_13A94B], 0|je short loc_10BCF2_z|cmp dword [dtu_double_lines], 0|loc_10BCF2_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_10BD10,7,cmp dword [loc_13A94B], 0|je short loc_10BD10_z|cmp dword [dtu_double_lines], 0|loc_10BD10_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_10BD2B,7,cmp dword [loc_13A94B], 0|je short loc_10BD2B_z|cmp dword [dtu_double_lines], 0|loc_10BD2B_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_10BD5A,7,cmp dword [loc_13A94B], 0|je short loc_10BD5A_z|cmp dword [dtu_double_lines], 0|loc_10BD5A_z: ; full-height 3D: line doubling only if dtu_double_lines
loc_406E1,3,lea ebp, [eax + 1]|cmp dword [loc_13A94B], 0|je short loc_406E1_z|cmp dword [dtu_double_lines], 0|jne short loc_406E1_z|add ebp, ebp|loc_406E1_z: ; full-height 3D: render canvas at full height
loc_40729,7,cmp dword [loc_13A94B], 0|je short loc_40729_z|cmp dword [dtu_double_lines], 0|loc_40729_z: ; full-height 3D: height already doubled
loc_D9419,3,mov eax, [esi + 0x26]|cmp dword [loc_13A94B], 0|je short loc_D9419_z|cmp dword [dtu_double_lines], 0|jne short loc_D9419_z|cmp dword [loc_453928], loc_1C7EA8|jne short loc_D9419_z|sar eax, 1|loc_D9419_z: ; g3_start_frame: full-height 3D view (render canvas at double height) -> half the aspect
loc_9DD0F,5,mov eax, [loc_30F42C]|imul dword [dtu_phys_scale]|shrd eax, edx, 0x10 ; object physics: number of 1/64 s steps from the scaled frame time (behaves as at io_physics_fps; ladder exit, jumps)
loc_9DD26,6,mov eax, [loc_30F42C]|imul dword [dtu_phys_scale]|shrd eax, edx, 0x10|mov edx, eax ; object physics: remainder of the scaled frame time
loc_6C742,2,mov ecx, eax|cmp dword [dtu_creature_floor], 0|je short loc_6C742_z|mov r9d, 0xEF0026|CALL x86_int|loc_6C742_z: ; object gravity: height above the floor from 0x6C30E -> render.c (where the floor is under a creature)
loc_72A0F,4,cmp dword [dtu_creature_floor], 0|je short loc_72A0F_z|mov r9d, 0xEF0027|CALL x86_int|loc_72A0F_z:|lea esi, [r11d + 0x30] ; draw_polygon_model: a creature is drawn with its lowest point on the floor -> render.c
loc_F080C,5,CALL loc_D8B54|cmp dword [dtu_creature_floor], 0|je short loc_F080C_z|mov r9d, 0xEF0025|CALL x86_int|loc_F080C_z: ; model interpreter, OP_DEFPOINTS: lowest rotated point of a creature -> render.c
loc_9FD68,4,cmp dword [dtu_step_gap], 0|je short loc_9FD68_play|mov r9d, 0xEF0029|CALL x86_int|test eax, eax|jnz loc_9FE00|loc_9FD68_play:|mov eax, [r11d + 4] ; footstep of a walking/running creature: started every game frame while the animation is on its sound frame (four times at 60 fps) -> render.c keeps dtu_step_gap ms between two of one creature
loc_2E631,1,cmp dword [dtu_sound_fallback], 0|je short loc_2E631_link|mov r9d, 0xEF002A|CALL x86_int|cmp dword [dtu_sound_fb_now], 0|je short loc_2E631_link|cmp dword [dtu_sound_fb_now], 2|je short loc_2E631_drop|movsx edx, dx|imul edx, edx, 0x1DF|add edx, loc_2B0614|mov ebx, [r11d + 8]|PUSH32 ebx|mov ebx, [r11d + 8]|PUSH32 ebx|PUSH32 ecx|xor ebx, ebx|PUSH32 ebx|lea ecx, [edx + 0x14]|movsx edx, word [edx + 0xE]|CALL loc_2E957|RET 8|loc_2E631_drop:|mov eax, -1|RET 8|loc_2E631_link:|PUSH32 esi ; digi_link_sound_to_object: a one-shot sound for which all 16 sound objects are in use (ambient sounds) was dropped -> render.c says so, then it is played at the object's position (segment +0xE, position +0x14) through 0x2E957 like digi_link_sound_to_pos (stack: once 0, volume, distance, flag); 2 = the same sound was just started at a position (melee death sound), so it is not doubled
loc_2E957,1,cmp dword [dtu_sound_fallback], 0|je short loc_2E957_go|mov r9d, 0xEF002B|CALL x86_int|loc_2E957_go:|PUSH32 esi ; play a sound at a position: render.c notes which sound and when (see loc_2E631)
loc_58158,5,mov r9d, 0xEF002C|CALL x86_int|CALL loc_80620|cmp dword [dtu_menu_ver_str], 0|je short loc_58158_done|mov eax, 0x8000|mov edx, [dtu_menu_ver_y]|mov ebx, 2|mov ecx, [dtu_menu_ver_str]|CALL loc_80620|loc_58158_done: ; main menu: the copyright (0x80620: x 0x8000 = centred, y, colour 2, text) one line higher, and "Undermountain Enhanced <version>" under it (menu.c)
