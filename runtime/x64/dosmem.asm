;; Undermountain Enhanced - DOS memory area for the Windows build.
;; The exe is linked at image base 0x20000 and this is its first custom
;; section, so it covers the fixed DOS addresses A0000-BFFFF (VGA window,
;; PSP, real-mode buffers). The image is mapped before any heap or stack
;; exists, so nothing else can occupy them. core.c checks the placement.
global dtu_dos_area
global dtu_dos_area_end
section .dosmem bss align=4096
dtu_dos_area: resb 0x100000
dtu_dos_area_end:
