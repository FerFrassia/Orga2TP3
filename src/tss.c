/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de estructuras para administrar tareas
*/

#include "tss.h"

tss tss_inicial;
tss tss_idle;

tss tss_zombisA[CANT_ZOMBIS];
tss tss_zombisB[CANT_ZOMBIS];

/*
int ocupadaA[CANT_ZOMBIS]={0,0,0,0,0,0,0,0};
int ocupadaB[CANT_ZOMBIS]={0,0,0,0,0,0,0,0};*/

void tss_inicializar() {
	gdt[GDT_IDX_TAREA_INICIAL].base_0_15= (unsigned short) ((unsigned int)(&tss_inicial) & (unsigned int) 0xFFFF);
	gdt[GDT_IDX_TAREA_INICIAL].base_23_16= (unsigned char) ((unsigned int)(&tss_inicial) >> 16 & (unsigned int) 0xFFFF);
	gdt[GDT_IDX_TAREA_INICIAL].base_31_24= (unsigned char) ((unsigned int)(&tss_inicial) >> 24 & (unsigned int) 0xFFFF);
}

void tss_inicializar_idle() {
	gdt[GDT_IDX_TAREA_IDLE].base_0_15= (unsigned short) ((unsigned int)(&tss_idle) & (unsigned int) 0xFFFF);
	gdt[GDT_IDX_TAREA_IDLE].base_23_16= (unsigned char) ((unsigned int)(&tss_idle) >> 16 & (unsigned int) 0xFFFF);
	gdt[GDT_IDX_TAREA_IDLE].base_31_24= (unsigned char) ((unsigned int)(&tss_idle) >> 24 & (unsigned int) 0xFFFF);

	//unsigned int ss = mmu_proxima_pagina_fisica_libre();
	//tss_idle.esp0 = ss + PAGE_SIZE;
    //tss_idle.ss0 = GDT_IDX_DAT_KERNEL<<3;
    tss_idle.cr3 = CR3KERNEL;
    tss_idle.eip = 0x16000;
    tss_idle.eflags = 0x202;
    tss_idle.cs = GDT_IDX_COD_KERNEL << 3;
    tss_idle.es = GDT_IDX_DAT_KERNEL << 3;
    tss_idle.ss = GDT_IDX_DAT_KERNEL << 3;
    tss_idle.ds = GDT_IDX_DAT_KERNEL << 3;
    tss_idle.fs = GDT_IDX_DAT_KERNEL << 3;
    tss_idle.gs = GDT_IDX_DAT_KERNEL << 3;
	tss_idle.esp = 0x27000;
	tss_idle.ebp = 0x27000;
}

unsigned int tss_inicializar_zombie(unsigned short x, unsigned short y, zombie z) {

	int isPlayerA = (z == A_MONK || z == A_SUICIDE_UNIT || z == A_DRUNK_DRIVER);
	unsigned short* ocupada = isPlayerA ? playerA.gdt_indexes_tasks : playerB.gdt_indexes_tasks;
	//BUSCAMOS SLOT LIBRE
	int new_index = -1;
	int i;
	for (i=0; i<CANT_ZOMBIS && new_index == -1; i++){
		if (! ocupada[i]){
			new_index = i;
		}
	}
	if (new_index == -1) return 0; //no hay slots libres
	
	
	tss* tss_zombi = isPlayerA ? &tss_zombisA[new_index] : &tss_zombisB[new_index];
	
	int gdt_index = isPlayerA ? 15 + new_index : 23 + new_index;
	gdt[gdt_index].base_0_15= (unsigned short) ((unsigned int)(tss_zombi) & (unsigned int) 0xFFFF);
	gdt[gdt_index].base_23_16= (unsigned char) ((unsigned int)(tss_zombi) >> 16 & (unsigned int) 0xFFFF);
	gdt[gdt_index].base_31_24= (unsigned char) ((unsigned int)(tss_zombi) >> 24 & (unsigned int) 0xFFFF);
	gdt[gdt_index].p = 1;

	unsigned int dir_z = mmu_inicializar_dir_zombi(x, y, z);
	unsigned int ss = mmu_proxima_pagina_fisica_libre();
	tss_zombi->esp0 = ss + PAGE_SIZE;
    tss_zombi->ss0 = GDT_IDX_DAT_KERNEL<<3;
    tss_zombi->cr3 = dir_z;
    tss_zombi->eip = VIRTUAL_COD_ZOMBIE_1;
    tss_zombi->eflags = 0x202;
    tss_zombi->cs = GDT_IDX_COD_USER << 3 | 0x3;
    tss_zombi->es = GDT_IDX_DAT_USER << 3 | 0x3;
    tss_zombi->ss = GDT_IDX_DAT_USER << 3 | 0x3;
    tss_zombi->ds = GDT_IDX_DAT_USER << 3 | 0x3;
    tss_zombi->fs = GDT_IDX_DAT_USER << 3 | 0x3;
    tss_zombi->gs = GDT_IDX_DAT_USER << 3 | 0x3;
	tss_zombi->esp = VIRTUAL_COD_ZOMBIE_1 + PAGE_SIZE;
	tss_zombi->ebp = VIRTUAL_COD_ZOMBIE_1 + PAGE_SIZE;

	return gdt_index<<3;
}