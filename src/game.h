/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
*/

#ifndef __GAME_H__
#define __GAME_H__

#include "defines.h"
#include "screen.h"
#include "mmu.h"
#include "sched.h"

typedef enum direccion_e { IZQ = 0xAAA, DER = 0x441, ADE = 0x83D, ATR = 0x732 } direccion;

void game_jugador_mover(unsigned int jugador, int value);

void game_lanzar_zombi(unsigned int jugador);

void game_move_current_zombi(direccion dir);

void game_sumar_punto(unsigned int jugador);

void game_matar_zombie_actual();

extern int inDebugMode;
extern int debugScreenOpen;

int game_todos_zombies_vivos(unsigned int jugador);

int game_todos_zombies_muertos(unsigned int jugador);

#endif  /* !__GAME_H__ */
