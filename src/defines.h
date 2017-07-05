/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

    Definiciones globales del sistema.
*/

#ifndef __DEFINES_H__
#define __DEFINES_H__

/* Direcciones de memoria */
/* -------------------------------------------------------------------------- */
#define VIDEO                   0x000B8000 /* direccion fisica del buffer de video */

#define PAGE_SIZE 0x1000

#define INICIO_PAGINAS_LIBRES 0x100000

#define VIRTUAL_COD_ZOMBIE_1 0x8000000
#define VIRTUAL_COD_ZOMBIE_2 0x8001000
#define VIRTUAL_COD_ZOMBIE_3 0x8002000
#define VIRTUAL_COD_ZOMBIE_4 0x8003000
#define VIRTUAL_COD_ZOMBIE_5 0x8004000
#define VIRTUAL_COD_ZOMBIE_6 0x8005000
#define VIRTUAL_COD_ZOMBIE_7 0x8006000
#define VIRTUAL_COD_ZOMBIE_8 0x8007000
#define VIRTUAL_COD_ZOMBIE_9 0x8008000

#define CR3KERNEL 0x27000

typedef enum {A_MONK, A_SUICIDE_UNIT, A_DRUNK_DRIVER, B_MONK, B_SUICIDE_UNIT, B_DRUNK_DRIVER} zombie;

//MONK -----> GUERRERO
//SUICIDE_UNIT ----> CLERIGO
//DRUNK_DRIVER ----> MAGO

/* JUEGO */
/* -------------------------------------------------------------------------- */
#define START_Y_PLAYERS				22
#define START_SELECTED_ZOMBIE_A		A_MONK
#define START_SELECTED_ZOMBIE_B		B_MONK

#define POS_INIT_ZOMBI_A			2
#define POS_INIT_ZOMBI_B			77


/* Reloj */
/* -------------------------------------------------------------------------- */
#define RELOJ_SIZE              4


/* Modulo */
/* -------------------------------------------------------------------------- */
#define MOD(a,b) ((((a)%(b))+(b))%(b))


/* Bool */
/* -------------------------------------------------------------------------- */
#define TRUE                    0x00000001
#define FALSE                   0x00000000
#define ERROR                   1


/* Misc */
/* -------------------------------------------------------------------------- */
#define CANT_ZOMBIS             8

#define SIZE_W                  78
#define SIZE_H                  44


/* Indices en la gdt */
/* -------------------------------------------------------------------------- */
#define GDT_COUNT 31

#define GDT_IDX_NULL_DESC           0
#define GDT_IDX_COD_KERNEL          8
#define GDT_IDX_DAT_KERNEL          9
#define GDT_IDX_COD_USER            10
#define GDT_IDX_DAT_USER            11
#define GDT_IDX_VIDEO_KERNEL		12
#define GDT_IDX_TAREA_INICIAL		13
#define GDT_IDX_TAREA_IDLE			14
#define GDT_IDX_TAREA_A0			15
#define GDT_IDX_TAREA_A1			16
#define GDT_IDX_TAREA_A2			17
#define GDT_IDX_TAREA_A3			18
#define GDT_IDX_TAREA_A4			19
#define GDT_IDX_TAREA_A5			20
#define GDT_IDX_TAREA_A6			21
#define GDT_IDX_TAREA_A7			22
#define GDT_IDX_TAREA_B0			23
#define GDT_IDX_TAREA_B1			24
#define GDT_IDX_TAREA_B2			25
#define GDT_IDX_TAREA_B3			26
#define GDT_IDX_TAREA_B4			27
#define GDT_IDX_TAREA_B5			28
#define GDT_IDX_TAREA_B6			29
#define GDT_IDX_TAREA_B7			30

/* Offsets en la gdt */
/* -------------------------------------------------------------------------- */
#define GDT_OFF_NULL_DESC           (GDT_IDX_NULL_DESC      << 3)

#endif  /* !__DEFINES_H__ */
