/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de las rutinas de atencion de interrupciones
*/

#include "defines.h"
#include "idt.h"
#include "isr.h"

#include "tss.h"
#include "screen.h"
#include "i386.h"

#include "game.h"

idt_entry idt[255] = { };

idt_descriptor IDT_DESC = {
    sizeof(idt) - 1,
    (unsigned int) &idt
};

/*
    La siguiente es una macro de EJEMPLO para ayudar a armar entradas de
    interrupciones. Para usar, descomentar y completar CORRECTAMENTE los
    atributos y el registro de segmento. Invocarla desde idt_inicializar() de
    la siguiene manera:

    void idt_inicializar() {
        IDT_ENTRY(0);
        ...
        IDT_ENTRY(19);

        ...
    }
*/


#define IDT_ENTRY_SYSTEM(numero)                                                                                        \
    idt[numero].offset_0_15 = (unsigned short) ((unsigned int)(&_isr ## numero) & (unsigned int) 0xFFFF);        \
    idt[numero].segsel = (unsigned short) 0x0040;  /* 0000000001000 000 */                                                             \
    idt[numero].attr = (unsigned short) 0x8E00;    /* 1000 1110 0000 0000 */                                                              \
    idt[numero].offset_16_31 = (unsigned short) ((unsigned int)(&_isr ## numero) >> 16 & (unsigned int) 0xFFFF);

#define IDT_ENTRY_USER(numero)                                                                                        \
    idt[numero].offset_0_15 = (unsigned short) ((unsigned int)(&_isr ## numero) & (unsigned int) 0xFFFF);        \
    idt[numero].segsel = (unsigned short) 0x0040;  /* 0000000001000 000 */                                                       \
    idt[numero].attr = (unsigned short) 0xEE00;    /* 1110 1110 0000 0000 */                                                                \
    idt[numero].offset_16_31 = (unsigned short) ((unsigned int)(&_isr ## numero) >> 16 & (unsigned int) 0xFFFF);

void idt_inicializar() {
    IDT_ENTRY_SYSTEM(1);
    IDT_ENTRY_SYSTEM(0);
    IDT_ENTRY_SYSTEM(2);
    IDT_ENTRY_SYSTEM(3);
    IDT_ENTRY_SYSTEM(4);
    IDT_ENTRY_SYSTEM(5);
    IDT_ENTRY_SYSTEM(6);
    IDT_ENTRY_SYSTEM(7);
    IDT_ENTRY_SYSTEM(8);
    IDT_ENTRY_SYSTEM(9);
    IDT_ENTRY_SYSTEM(10);
    IDT_ENTRY_SYSTEM(11);
    IDT_ENTRY_SYSTEM(12);
    IDT_ENTRY_SYSTEM(13);
    IDT_ENTRY_SYSTEM(14);
    IDT_ENTRY_SYSTEM(15);
    IDT_ENTRY_SYSTEM(16);
    IDT_ENTRY_SYSTEM(17);
    IDT_ENTRY_SYSTEM(18);
    IDT_ENTRY_SYSTEM(19);
    IDT_ENTRY_SYSTEM(32);
    IDT_ENTRY_SYSTEM(33);

    IDT_ENTRY_USER(102);
}


void catch_exception(int num){
    game_matar_zombie_actual();
    if (inDebugMode & !debugScreenOpen) {
        debugScreenOpen = 1;
        backup_screen();
        print_debugger();
    }
}

void interrupcion_teclado(unsigned short letra){
    print_hex((unsigned int)letra, 2, 25, 40, 0x07);
}

void catch_y_press(){
    if (inDebugMode & debugScreenOpen){
        debugScreenOpen = 0;
        backup_restore_screen();
    } else {
        inDebugMode = !inDebugMode;
        if(inDebugMode){
            print("DEBUG MODE", 0, 0, 0x7);
        } else {
            print("          ", 0, 0, 0x7);
        }
    }
}