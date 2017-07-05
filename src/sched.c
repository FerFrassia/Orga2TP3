/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del scheduler
*/

#include "sched.h"

info_player playerA = {START_SELECTED_ZOMBIE_A, START_Y_PLAYERS,0,{0,0,0,0,0,0,0,0}, {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0,0}},32,0};
info_player playerB = {START_SELECTED_ZOMBIE_B, START_Y_PLAYERS,0,{0,0,0,0,0,0,0,0}, {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0,0}},32,0};

int playerActual = 0;

int isIdle=1;



unsigned short sched_proximo_indice() {
	
	int sigPlayer = (playerActual+1) %2;
	info_player* infoSig = sigPlayer ? &playerB : &playerA;
	int i;
	unsigned short hasZombie = 0; 
	for(i=0; i<8; i++){
		if (infoSig->gdt_indexes_tasks[i] != 0) hasZombie =1;
	}
	if (hasZombie) playerActual = (playerActual+1) %2;
	
	info_player* player = get_current_player();
	//Los prints comentados son para debuging.
	//unsigned short* gdt_indexes = player->gdt_indexes_tasks;
	int current_zombie = player->curr_zombie;
	if(player->curr_zombie==32){
    //MAGIA NEGRA 
		i=0;
		hasZombie=0;
		for(i=0; i<8; i++){
			if (player->gdt_indexes_tasks[i] != 0) hasZombie =1;
		}
		if (hasZombie){
        	player->curr_zombie = 0;
        	isIdle=0;
        	return player->gdt_indexes_tasks[0];
    	}else{
    		return 0;
    	}
    }

	//print_hex(player->gdt_indexes_tasks[current_zombie],2, 20,17, 0xf);
	//print_hex(current_zombie,2, 30,17, 0xf);
	current_zombie=(current_zombie +1) %8;
	short gdt_index=0;
	/*i=0;
	for (i=0;i<8;i++){
		print_hex(playerA.gdt_indexes_tasks[i],2, 3,20+i, 0xf);
		print_hex(playerB.gdt_indexes_tasks[i],2, 6,20+i, 0xf);
		print_hex(player->gdt_indexes_tasks[i],2, 9,20+i, 0xf);
	}*/
	//i=0;
	while(current_zombie!=player->curr_zombie || isIdle){
		//breakpoint();
		//print_hex(current_zombie,2, 30,20 + ++i, 0xf);
		//print_hex(player->gdt_indexes_tasks[current_zombie],2, 20,20 + i, 0xf);
		if(player->gdt_indexes_tasks[current_zombie]!=0){
			//breakpoint();
			gdt_index = player->gdt_indexes_tasks[current_zombie];
			player->curr_zombie = current_zombie;
			isIdle=0;
			break;
		}

		if(isIdle && current_zombie==player->curr_zombie) return player->gdt_indexes_tasks[current_zombie];

		current_zombie=(current_zombie +1) %8;

	}


  	return gdt_index;

}


info_player* get_current_player(){
	return playerActual == 0 ? &playerA : &playerB;
}