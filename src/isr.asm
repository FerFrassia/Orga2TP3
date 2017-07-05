; ** por compatibilidad se omiten tildes **
; ==============================================================================
; TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
; ==============================================================================
; definicion de rutinas de atencion de interrupciones

%include "imprimir.mac"

BITS 32

sched_tarea_offset:     dd 0x00
sched_tarea_selector:   dw 0x00

;; PIC
extern fin_intr_pic1

;; Sched
extern sched_proximo_indice

extern catch_exception
extern catch_y_press
extern interrupcion_teclado
extern game_lanzar_zombi
extern game_jugador_mover
extern game_change_zombie
extern game_move_current_zombi

extern girar_reloj_actual

extern debug
extern isIdle
extern inDebugMode 
extern debugScreenOpen
;;
;; Definición de MACROS
;; -------------------------------------------------------------------------- ;;

%macro ISR 1
global _isr%1

_isr%1:
    mov dword [debug + 00], eax
    mov dword [debug + 04], ebx
    mov dword [debug + 08], ecx
    mov dword [debug + 12], edx
    mov dword [debug + 16], esi
    mov dword [debug + 20], edi
    mov dword [debug + 24], ebp

    mov word [debug + 88], %1 ;exception number
    mov dword [debug + 90], 0 ;empiezo con errcode en 0
    ;los que tienen error code lo tienen arriba en la pila
    ;lo sacamos asi queda la pila igual para toda excepcion
    mov eax, 0x8
    cmp eax, %1
    je .haserrorcode
    mov eax, 0xA
    cmp eax, %1 
    je .haserrorcode
    mov eax, 0xB
    cmp eax, %1 
    je .haserrorcode
    mov eax, 0xC
    cmp eax, %1 
    je .haserrorcode
    mov eax, 0xD
    cmp eax, %1 
    je .haserrorcode
    mov eax, 0xE
    cmp eax, %1 
    je .haserrorcode
    mov eax, 0x11
    cmp eax, %1 
    je .haserrorcode
    mov eax, 0x1E
    cmp eax, %1 
    je .haserrorcode

    .otros:  
    ; SS <-esp +16
    ; ESP <-esp +12
    ; EFLAGS <-esp +8
    ; CS  <-esp +4
    ; EIP <- esp

    mov eax, [esp+12]
    mov dword [debug + 28], eax ;esp
    
    mov eax, [esp]
    mov dword [debug + 32], eax ;eip
        
    mov ax, [esp + 4]
    mov word [debug + 36], ax ;cs
    mov ax, ds
    mov word [debug + 38], ax
    mov ax, es
    mov word [debug + 40], ax
    mov ax, fs
    mov word [debug + 42], ax
    mov ax, gs
    mov word [debug + 44], ax
    mov ax, [esp+16]
    mov word [debug + 46], ax ;ss
    
    xor eax, eax
    mov ax, [esp+8]  
    mov dword [debug + 48], eax ; eflags
    
    mov eax, cr0
    mov dword [debug + 52], eax
    mov eax, cr2
    mov dword [debug + 56], eax
    mov eax, cr3
    mov dword [debug + 60], eax
    mov eax, cr4
    mov dword [debug + 64], eax
    
    mov eax, [esp+12] ;el esp viejo
    ;xchg bx,bx
    mov esi, [eax]
    mov dword [debug + 68], esi
    mov esi, [eax+4]
    mov dword [debug + 72], esi
    mov esi, [eax+8]
    mov dword [debug + 76], esi
    mov esi, [eax+12]
    mov dword [debug + 80], esi
    mov esi, [eax+16]
    mov dword [debug + 84], esi

    push %1
    call catch_exception
    pop edi
    jmp 0x70:0xFAFAFA
    ;iret
    .haserrorcode:
        pop eax
        mov dword [debug + 90], eax
        jmp .otros

%endmacro

;;
;; Datos
;; -------------------------------------------------------------------------- ;;
; Scheduler
isrnumero:           dd 0x00000000
isrClock:            db '|/-\'
char_a:              db 'a'
char_s:              db 's'
char_d:              db 'd'            
char_w:              db 'w' 
char_shl:            db '>' 
char_i:              db 'i' 
char_j:              db 'j' 
char_k:              db 'k' 
char_l:              db 'l' 
char_shr:            db '<' 
offset:              dd 0
selector:            dw 0

;a: 1e s:1f d:20 w:11    shift: 2a  i:17 j:24  k:25 l:26  right_shift:36
;;
;; Rutina de atención de las EXCEPCIONES
;; -------------------------------------------------------------------------- ;;
ISR 0
ISR 1
ISR 2
ISR 3
ISR 4
ISR 5
ISR 6
ISR 7
ISR 8
ISR 9
ISR 10
ISR 11
ISR 12
ISR 13
ISR 14
ISR 15
ISR 16
ISR 17
ISR 18
ISR 19


;;
;; Rutina de atención del RELOJ
;; -------------------------------------------------------------------------- ;;
global _isr32 
_isr32:
    pushad

    call proximo_reloj
    mov eax, [inDebugMode] 
    mov esi, [debugScreenOpen]
    and eax, esi
    cmp eax, 1
    je .nojump  ;si esta en modo debug con la pantalla de debug abierta no hago nada


    call sched_proximo_indice

    cmp ax, 0
    je .nojump
        push eax
        call girar_reloj_actual
        pop eax
        mov [selector], ax
        call fin_intr_pic1
        jmp far [offset]
        jmp .end

    .nojump:
    call fin_intr_pic1

    .end:
    popad
    iret

;;
;; Rutina de atención del TECLADO
;; -------------------------------------------------------------------------- ;;
global _isr33
_isr33:
    pushad
    xor eax, eax
    in al, 0x60
    
    mov edi, [inDebugMode] 
    mov esi, [debugScreenOpen]
    and edi, esi
    cmp edi, 1
    je .test_y  ;si esta en modo debug con la pantalla de debug abierta solo ejecuto para la tecla y



    cmp al, 0x1e
    je .a
    cmp al, 0x1f
    je .s
    cmp al, 0x20
    je .d
    cmp al, 0x11
    je .w
    cmp al, 0x2a
    je .sh_left
    cmp al, 0x17
    je .i
    cmp al, 0x24
    je .j
    cmp al, 0x25
    je .k
    cmp al, 0x26
    je .l
    cmp al, 0x36
    je .sh_right
    .test_y:

    cmp al, 0x15
    je .y
    
    .fin:
        call fin_intr_pic1
        popad
        iret

; INSTRUCCIONES DE MOVER AL JUGADOR

.w:  ; A hacia arriba
    mov ebx, -1
    push ebx
    mov ebx, 0
    push ebx
    call game_jugador_mover
    add esp, 8
    jmp .fin

.s:  ; A hacia abajo
    mov ebx, 1
    push ebx
    mov ebx, 0
    push ebx
    call game_jugador_mover
    add esp, 8
    jmp .fin

.i:   ; B hacia arriba
    mov ebx, -1
    push ebx
    mov ebx, 1
    push ebx
    call game_jugador_mover
    add esp, 8
    jmp .fin

.k:   ; B hacia abajo
    mov ebx, 1
    push ebx
    mov ebx, 1
    push ebx
    call game_jugador_mover
    add esp, 8
    jmp .fin

; INSTRUCCIONES PARA CAMBIAR DE ZOMBIE

.a: ; A hacia izq
    mov ebx, -1
    push ebx
    mov ebx, 0
    push ebx
    call game_change_zombie
    add esp, 8
    jmp .fin

.d: ; A hacia der
    mov ebx, 1
    push ebx
    mov ebx, 0
    push ebx
    call game_change_zombie
    add esp, 8
    jmp .fin

.j: ; B hacia izq
    mov ebx, -1
    push ebx
    mov ebx, 1
    push ebx
    call game_change_zombie
    add esp, 8
    jmp .fin

.l: ; B hacia der
    mov ebx, 1
    push ebx
    mov ebx, 1
    push ebx
    call game_change_zombie
    add esp, 8
    jmp .fin


; INSTRUCCIONES PARA LANZAR UNO ZOMBIE

.sh_left:
    mov ebx, 0
    push ebx    
    call game_lanzar_zombi
    pop ebx    
    jmp .fin

.sh_right:
    mov ebx, 1
    push ebx    
    call game_lanzar_zombi
    pop ebx
    jmp .fin

;Modo debug
.y:
    call catch_y_press
    jmp .fin

;;
;; Rutinas de atención de las SYSCALLS
;; -------------------------------------------------------------------------- ;;
global _isr102
_isr102:
    pushad
    push eax
    call game_move_current_zombi
    pop eax
    ;jump a la tarea idle
    jmp 0x70:0xFAFAFA    
    popad
    iret




%define IZQ 0xAAA
%define DER 0x441
%define ADE 0x83D
%define ATR 0x732


;; Funciones Auxiliares
;; -------------------------------------------------------------------------- ;;
proximo_reloj:
        pushad
        inc DWORD [isrnumero]
        mov ebx, [isrnumero]
        cmp ebx, 0x4
        jl .ok
                mov DWORD [isrnumero], 0x0
                mov ebx, 0
        .ok:
                add ebx, isrClock
                imprimir_texto_mp ebx, 1, 0x0f, 49, 79
                popad
        ret

global nuestro_proximo_reloj
nuestro_proximo_reloj:
        pop esi
            mov edi, [isrnumero]
            add edi, isrClock
            imprimir_texto_mp edi, 1, 0x0f, esi, 48
        sub esp,4
        ret

        
        
