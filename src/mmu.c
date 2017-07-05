/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del manejador de memoria
*/

#include "mmu.h"


void mmu_inicializar() {
	inicializar_kernel_mapping();
	proxima_pagina_libre = INICIO_PAGINAS_LIBRES;
}

void inicializar_kernel_mapping() {
	page_directory_entry* dir_pagina_kernel = (page_directory_entry*) CR3KERNEL;
	page_table_entry* dir_table_kernel = (page_table_entry*) 0x28000;
	dir_pagina_kernel->p = 1;
	dir_pagina_kernel->rw = 1;
	dir_pagina_kernel->s = 0;
	dir_pagina_kernel->pwt = 0;
	dir_pagina_kernel->pcd = 0;
	dir_pagina_kernel->a = 0;
	dir_pagina_kernel->i = 0;
	dir_pagina_kernel->ps = 0;
	dir_pagina_kernel->g = 0;
	dir_pagina_kernel->disp = 0;
	dir_pagina_kernel->base = 0x28;

	int i;
	for (i=0; i < 1024 ; i++){
		dir_table_kernel[i].p = 1;
		dir_table_kernel[i].rw = 1;
		dir_table_kernel[i].s = 0;
		dir_table_kernel[i].pwt = 0;
		dir_table_kernel[i].pcd = 0;
		dir_table_kernel[i].a = 0;
		dir_table_kernel[i].d = 0;
		dir_table_kernel[i].pat = 0;
		dir_table_kernel[i].g = 0;
		dir_table_kernel[i].disp = 0;
		dir_table_kernel[i].base = i;
	}
}

void inicializar_con_identity_mapping(page_directory_entry* pde) {
	page_table_entry* pte = (page_table_entry*) mmu_proxima_pagina_fisica_libre();
	
	pde->p = 1;
	pde->rw = 1;
	pde->s = 1;  // Si esto se rompe es porque esta al reves este bit
	pde->pwt = 0;
	pde->pcd = 0;
	pde->a = 0;
	pde->i = 0;
	pde->ps = 0;
	pde->g = 0;
	pde->disp = 0;
	pde->base = ((int) pte) >> 12;

	int i;
	for (i=0; i < 1024 ; i++){
		pte[i].p = 1;
		pte[i].rw = 1;
		pte[i].s = 1;
		pte[i].pwt = 0;
		pte[i].pcd = 0;
		pte[i].a = 0;
		pte[i].d = 0;
		pte[i].pat = 0;
		pte[i].g = 0;
		pte[i].disp = 0;
		pte[i].base = i;
	}
}

//malloc
unsigned int mmu_proxima_pagina_fisica_libre() {
	unsigned int pagina_libre = proxima_pagina_libre;
	proxima_pagina_libre += PAGE_SIZE;
	return pagina_libre;
}

#define PDE_INDEX(virtual) virtual >> 22
#define PTE_INDEX(virtual) (virtual << 10) >> 22
#define PG_READ_WRITE 1 
#define PG_USER 1
#define PG_PRESENT 0x00000001

void mmu_map_page(unsigned int virtual, unsigned int cr3, unsigned int fisica, char sup, char rw){
	page_directory_entry* page_dir = (page_directory_entry*) ((cr3 >> 3) << 3);

	page_table_entry* pte;
	if (page_dir[PDE_INDEX(virtual)].p){
		pte = (page_table_entry*) (page_dir[PDE_INDEX(virtual)].base << 12);
	} else {
		pte = (page_table_entry*) mmu_proxima_pagina_fisica_libre();
		page_dir[PDE_INDEX(virtual)].base = ((unsigned int) pte) >> 12;
		page_dir[PDE_INDEX(virtual)].p = 1;
		page_dir[PDE_INDEX(virtual)].rw = 1;
		page_dir[PDE_INDEX(virtual)].s = 1;
	}
	pte[PTE_INDEX(virtual)].p = 1;
	pte[PTE_INDEX(virtual)].rw = rw;
	pte[PTE_INDEX(virtual)].s = sup;
	pte[PTE_INDEX(virtual)].pwt = 0;
	pte[PTE_INDEX(virtual)].pcd = 0;
	pte[PTE_INDEX(virtual)].a = 0;
	pte[PTE_INDEX(virtual)].d = 0;
	pte[PTE_INDEX(virtual)].pat = 0;
	pte[PTE_INDEX(virtual)].g = 0;
	pte[PTE_INDEX(virtual)].disp = 0;
	pte[PTE_INDEX(virtual)].base = fisica >> 12;		
	tlbflush();
}

void mmu_unmap_page(unsigned int virtual, unsigned int cr3){
	page_directory_entry* page_dir = (page_directory_entry*) ((cr3 >> 3) << 3);
	page_table_entry* pte = (page_table_entry*) (page_dir[PDE_INDEX(virtual)].base << 12);
	pte[PTE_INDEX(virtual)].p = 0;
}

unsigned int mmu_inicializar_dir_zombi(unsigned short x, unsigned short y, zombie z) {
	page_directory_entry* dir_pagina = (page_directory_entry*) mmu_proxima_pagina_fisica_libre();
	inicializar_con_identity_mapping(dir_pagina);

	unsigned int* origin;
	switch (z) {
		case A_MONK:
			origin = (unsigned int*)0x10000;
			break;
        case A_SUICIDE_UNIT:
        	origin = (unsigned int*)0x11000;
        	break;
        case A_DRUNK_DRIVER:
        	origin = (unsigned int*)0x12000;
        	break;
        case B_MONK:
        	origin = (unsigned int*)0x13000;
        	break;
        case B_SUICIDE_UNIT:
        	origin = (unsigned int*)0x14000;
        	break;
        case B_DRUNK_DRIVER:
        	origin = (unsigned int*)0x15000;
        	break;
    }

	unsigned int* dest = (unsigned int*) mmu_get_map_position(x, y); 
	mmu_map_page((unsigned int)dest, rcr3(), (unsigned int)dest, 0, 1);
	unsigned int i;
	for (i=0; i<1024; i++){
		dest[i] = origin[i];
	}
	mmu_unmap_page((unsigned int)dest, rcr3());

	if (x == POS_INIT_ZOMBI_A){
		mmu_map_adjacent_to_zombi(0, (unsigned int) dir_pagina, x, y);
	} else if (x == POS_INIT_ZOMBI_B){
		mmu_map_adjacent_to_zombi(1, (unsigned int) dir_pagina, x, y);
	}

	return (unsigned int)dir_pagina;
}

unsigned int mmu_get_map_position ( int x,  int y) {
	return 0x400000 + (MOD(y,44) * 78 + x) * PAGE_SIZE;
}

void mmu_map_adjacent_to_zombi(unsigned int player, unsigned int dir_pagina, unsigned int x, unsigned int y){
	mmu_map_page(VIRTUAL_COD_ZOMBIE_1, dir_pagina, mmu_get_map_position(x, y), 1, 1);
	if (player == 0) {
		mmu_map_page(VIRTUAL_COD_ZOMBIE_2, dir_pagina, mmu_get_map_position(x+1, y), 1, 1);
		mmu_map_page(VIRTUAL_COD_ZOMBIE_3, dir_pagina, mmu_get_map_position(x+1, y+1), 1, 1);
		mmu_map_page(VIRTUAL_COD_ZOMBIE_4, dir_pagina, mmu_get_map_position(x+1, y-1), 1, 1);
		mmu_map_page(VIRTUAL_COD_ZOMBIE_5, dir_pagina, mmu_get_map_position(x, y+1), 1, 1);
		mmu_map_page(VIRTUAL_COD_ZOMBIE_6, dir_pagina, mmu_get_map_position(x, y-1), 1, 1);
		mmu_map_page(VIRTUAL_COD_ZOMBIE_7, dir_pagina, mmu_get_map_position(x-1, y), 1, 1);
		mmu_map_page(VIRTUAL_COD_ZOMBIE_8, dir_pagina, mmu_get_map_position(x-1, y-1), 1, 1);
		mmu_map_page(VIRTUAL_COD_ZOMBIE_9, dir_pagina, mmu_get_map_position(x-1, y+1), 1, 1);
	} else {
		mmu_map_page(VIRTUAL_COD_ZOMBIE_2, dir_pagina, mmu_get_map_position(x-1, y), 1, 1);
		mmu_map_page(VIRTUAL_COD_ZOMBIE_3, dir_pagina, mmu_get_map_position(x-1, y-1), 1, 1);
		mmu_map_page(VIRTUAL_COD_ZOMBIE_4, dir_pagina, mmu_get_map_position(x-1, y+1), 1, 1);
		mmu_map_page(VIRTUAL_COD_ZOMBIE_5, dir_pagina, mmu_get_map_position(x, y-1), 1, 1);
		mmu_map_page(VIRTUAL_COD_ZOMBIE_6, dir_pagina, mmu_get_map_position(x, y+1), 1, 1);
		mmu_map_page(VIRTUAL_COD_ZOMBIE_7, dir_pagina, mmu_get_map_position(x+1, y), 1, 1);
		mmu_map_page(VIRTUAL_COD_ZOMBIE_8, dir_pagina, mmu_get_map_position(x+1, y+1), 1, 1);
		mmu_map_page(VIRTUAL_COD_ZOMBIE_9, dir_pagina, mmu_get_map_position(x+1, y-1), 1, 1);
	}
}