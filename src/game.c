/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
*/

#include "game.h"

int inDebugMode = 0;
int debugScreenOpen = 0;


void game_jugador_mover(unsigned int jugador, int value) {
	info_player* player = jugador ? &playerB : &playerA;
	unsigned int orig_y = player->y;
	int a = value + orig_y;
	player->y = MOD(a, 44);

	//print_info_player(jugador);

	print_move_player(jugador, orig_y, player->y, player->selected_type);
}

void game_change_zombie(unsigned int jugador, int value) {
	info_player* player = jugador ? &playerB : &playerA;
	unsigned int offset = jugador ? 3 : 0;

	player->selected_type = MOD((int)player->selected_type + value, 3) + offset;

	//print_info_player(jugador);

	print_change_zombie(jugador, player->y, player->selected_type);

}

void game_lanzar_zombi(unsigned int jugador) {
    info_player* player = jugador ? &playerB : &playerA;
    int todos_vivos_jugador = game_todos_zombies_vivos(jugador);
    if (player->cant_lanzados>=20 || todos_vivos_jugador == 1) {
        return;
    }
	unsigned int x = jugador ? POS_INIT_ZOMBI_B : POS_INIT_ZOMBI_A;
	int indice = tss_inicializar_zombie(x,player->y,player->selected_type);
    /*print_int(x,30,30,0x7);
    print_int(player->y,30,31,0x7);
    print_int(player->selected_type,30,32,0x7);
    print_int(indice,30,41,0x7);
    print_int((indice>>3)-15,30,42,0x7);
*/
    if(jugador){
    	player->gdt_indexes_tasks[(indice>>3) - 23] = indice;
	    (player->info_zombies[(indice>>3) - 23]).x = x;
	    (player->info_zombies[(indice>>3) - 23]).y = player->y;
	    (player->info_zombies[(indice>>3) - 23]).type = player->selected_type;
    }else{
	    player->gdt_indexes_tasks[(indice>>3) - 15] = indice;
	    (player->info_zombies[(indice>>3) - 15]).x = x;
	    (player->info_zombies[(indice>>3) - 15]).y = player->y;
	    (player->info_zombies[(indice>>3) - 15]).type = player->selected_type;
	  } 
    player->cant_lanzados++;
	//print_current_zombi(playerActual, 0);
    print_throw_zombie(jugador, player->y, player->selected_type);

    //return indice;
    print_lanzados(jugador, player->cant_lanzados);
}
void game_move_current_zombi(direccion dir) {

    //print_current_zombi(playerActual, 0);
    //breakpoint();
    

    unsigned int* orig = (unsigned int*) VIRTUAL_COD_ZOMBIE_1;
    unsigned int* dest;
	switch (dir) {
	case IZQ:
    	dest = (unsigned int*)VIRTUAL_COD_ZOMBIE_6;
		break;
    case DER:
    	dest = (unsigned int*)VIRTUAL_COD_ZOMBIE_5; 
    	break;
    case ADE:
    	dest = (unsigned int*)VIRTUAL_COD_ZOMBIE_2;
    	break;
    case ATR:
    	dest = (unsigned int*)VIRTUAL_COD_ZOMBIE_7;
    	break;
    }
    /* mover su codigo */
    int i;
    for(i=0; i<1024; i++){
    	dest[i] = orig[i];
    }
    info_player* current_player = get_current_player();
    unsigned short current_zombie = current_player->curr_zombie;
    unsigned short x_orig = (current_player->info_zombies[current_zombie]).x;
    unsigned short y_orig = (current_player->info_zombies[current_zombie]).y;
    unsigned short x_dst = x_orig;
    unsigned short y_dst = y_orig;
    //print_current_zombi(playerActual, 0);
    //breakpoint();

    if(y_dst==0 || y_dst==43){
        if ((dir == IZQ && playerActual == 0) || (dir == DER && playerActual == 1)) {
            y_dst=43;
        } else if ((dir == DER && playerActual == 0) || (dir == IZQ && playerActual == 1)) {
            y_dst=0;
        } else if ((dir == ADE && playerActual == 0) || (dir == ATR && playerActual == 1)) {
            x_dst++;

        } else if ((dir == ATR && playerActual == 0) || (dir == ADE && playerActual == 1)) {
            x_dst--;
        }
    }else{
        if ((dir == IZQ && playerActual == 0) || (dir == DER && playerActual == 1)) {
            y_dst--;
        } else if ((dir == DER && playerActual == 0) || (dir == IZQ && playerActual == 1)) {
            y_dst++;
        } else if ((dir == ADE && playerActual == 0) || (dir == ATR && playerActual == 1)) {
            x_dst++;

        } else if ((dir == ATR && playerActual == 0) || (dir == ADE && playerActual == 1)) {
            x_dst--;
        }
    }   



    (current_player->info_zombies[current_zombie]).x = x_dst;
    (current_player->info_zombies[current_zombie]).y = y_dst;
    
    if(x_dst==1){
        game_sumar_punto(1/*jugador azul*/);
        game_matar_zombie_actual();
        return;
    }
    if(x_dst==78){
        game_sumar_punto(0/*jugador rojo*/);
        game_matar_zombie_actual();
        return;
    }

    /* remapear sus direcciones */
    
    mmu_map_adjacent_to_zombi(playerActual, rcr3(), x_dst, y_dst);
    //print_current_zombi(playerActual, 0);
    print_move_zombie(playerActual, x_orig, y_orig, x_dst, y_dst, (current_player->info_zombies[current_zombie]).type);

    isIdle=1;
}


void game_sumar_punto(unsigned int jugador){
	info_player* player = jugador ? &playerB : &playerA;
	player->puntos++;
	print_puntos(jugador, player->puntos);
}


void game_matar_zombie_actual(){
    info_player* player = playerActual ? &playerB : &playerA;

    int gdt_index = player->gdt_indexes_tasks[player->curr_zombie];
    player->gdt_indexes_tasks[player->curr_zombie] = 0;

    gdt[gdt_index >>3].p=0;

    print_death_zombie(playerActual, player->info_zombies[player->curr_zombie].x, player->info_zombies[player->curr_zombie].y);
    print_death_task();

    player->info_zombies[player->curr_zombie].x=0;
    player->info_zombies[player->curr_zombie].y=0; 
    player->info_zombies[player->curr_zombie].type=0; 
    player->info_zombies[player->curr_zombie].type=0;   

    //victoria
    if (playerA.cant_lanzados == 20 && playerB.cant_lanzados == 20) {
	    int todos_muertos_A = game_todos_zombies_muertos(0);
	    int todos_muertos_B = game_todos_zombies_muertos(1);
	    if (todos_muertos_A == 1 && todos_muertos_B == 1) {
	    	print_victoria();
	    }
	}
    isIdle=1;
}

int game_todos_zombies_vivos(unsigned int jugador) {
	info_player* player = jugador ? &playerB : &playerA;
    int playerVivos = 0;
    int i;
    for (i=0; i<8; ++i) {
    	if (player->gdt_indexes_tasks[i] != 0) {playerVivos++;}
    }  

    return playerVivos == 8;
}

int game_todos_zombies_muertos(unsigned int jugador) {
	info_player* player = jugador ? &playerB : &playerA;
    int playerVivos = 0;
    int i;
    for (i=0; i<8; ++i) {
    	if (player->gdt_indexes_tasks[i] == 0) {playerVivos++;}
    }  

    return playerVivos == 8;
}
