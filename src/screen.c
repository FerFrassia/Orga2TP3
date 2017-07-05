/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del scheduler
*/

#include "screen.h"

debug_info debug;

const char reloj[] = "|/-\\";
ca backup[VIDEO_FILS][VIDEO_COLS];



void print(const char * text, unsigned int x, unsigned int y, unsigned short attr) {
    ca (*p)[VIDEO_COLS] = (ca (*)[VIDEO_COLS]) VIDEO;
    int i;
    for (i = 0; text[i] != 0; i++) {
        p[y][x].c = (unsigned char) text[i];
        p[y][x].a = (unsigned char) attr;
        x++;
        if (x == VIDEO_COLS) {
            x = 0;
            y++;
        }
    }
}

void print_char(const char c, unsigned int x, unsigned int y, unsigned short attr) {
    ca (*p)[VIDEO_COLS] = (ca (*)[VIDEO_COLS]) VIDEO;
    p[y][x].c = (unsigned char) c;
    p[y][x].a = (unsigned char) attr;
}

void print_hex(unsigned int numero, int size, unsigned int x, unsigned int y, unsigned short attr) {
    ca (*p)[VIDEO_COLS] = (ca (*)[VIDEO_COLS]) VIDEO; // magia
    int i;
    char hexa[8];
    char letras[16] = "0123456789ABCDEF";
    hexa[0] = letras[ ( numero & 0x0000000F ) >> 0  ];
    hexa[1] = letras[ ( numero & 0x000000F0 ) >> 4  ];
    hexa[2] = letras[ ( numero & 0x00000F00 ) >> 8  ];
    hexa[3] = letras[ ( numero & 0x0000F000 ) >> 12 ];
    hexa[4] = letras[ ( numero & 0x000F0000 ) >> 16 ];
    hexa[5] = letras[ ( numero & 0x00F00000 ) >> 20 ];
    hexa[6] = letras[ ( numero & 0x0F000000 ) >> 24 ];
    hexa[7] = letras[ ( numero & 0xF0000000 ) >> 28 ];
    for(i = 0; i < size; i++) {
        p[y][x + size - i - 1].c = hexa[i];
        p[y][x + size - i - 1].a = attr;
    }
}

void print_int(unsigned int n, unsigned int x, unsigned int y, unsigned short attr) {
    ca (*p)[VIDEO_COLS] = (ca (*)[VIDEO_COLS]) VIDEO; // magia
    if( n > 9 ) {
      int a = n / 10;
      n -= 10 * a;
      print_int(a,x-1,y,attr);
    }
    p[y][x].c = '0'+n;
    p[y][x].a = attr;
}

void clear_screen() {
    int i;
    for (i=0; i<50; i++){
        print("                                                                                ", 0, i, C_BG_BLACK);
    }
}

void windows_screen() {
    int i;
    print("                                                          ", 10, 10, C_BG_BLACK);
    for (i=11; i<40; i++){
        if (i<38) print(" ", 10, i, C_BG_BLACK);
        print("                                                           ", 11, i, C_BG_BLUE);
    }   
}

void print_board() {
    //Primera linea negra
    print("                                                                                ", 0, 0, C_BG_BLACK);
    int i;
    for (i=1; i<45; i++){
        print(" ", 0, i, C_BG_RED);
        print("                                                                              ", 1, i, C_BG_GREEN);
        print(" ", 79, i, C_BG_BLUE);
    }
    for (i=45; i<50; i++){
        print("     ", 35, i, C_BG_RED);
        print("     ", 40, i, C_BG_BLUE);
    }
    print("1 2 3 4 5 6 7 8", 4, 46, C_FG_WHITE);
    print("1 2 3 4 5 6 7 8", 59, 46, C_FG_WHITE);

    //Inicializo jugadores en posicion 22
    print(zombie_to_char(A_MONK),0,START_Y_PLAYERS + 1, C_FG_WHITE_BG_RED);
    print(zombie_to_char(B_MONK),79,START_Y_PLAYERS + 1, C_FG_WHITE_BG_BLUE);

    //dibujo puntajes iniciales
    print_puntos(0, 0);
    print_puntos(1, 0);

    //dibujo lanzados iniciales
    print_lanzados(0, 0);
    print_lanzados(1, 0);

    //dibujo relojes iniciales
    print("- - - - - - - -", 4, 48, 0x7);
    print("- - - - - - - -", 59, 48, 0x7);

    //redibujo nombre(para el reiniciar)
    print_group_name();
}

void print_group_name() {
    print("Nombre: Guiso and RT NaN JE fun", 49, 0, C_FG_LIGHT_GREY);
}

void print_move_player(int player, unsigned int orig_y, unsigned int dest_y, zombie z){
    unsigned int x = player ? 79 : 0;
    unsigned short color = player ? C_FG_WHITE_BG_BLUE : C_FG_WHITE_BG_RED; 
    print(" ", x, orig_y+1, color);
    print(zombie_to_char(z), x, dest_y+1, color);
}

void print_change_zombie(int player, unsigned int y, zombie z) {
    unsigned int x = player ? 79 : 0;
    unsigned short color = player ? C_FG_WHITE_BG_BLUE : C_FG_WHITE_BG_RED;
    print(zombie_to_char(z), x, y+1, color);
}

char* zombie_to_char(zombie z){
    char* letter = "a"; 
    switch (z) {
        case A_MONK:
            *letter = 0xEB;
            break;
        case A_SUICIDE_UNIT:
            *letter = 0xE6;
            break;
        case A_DRUNK_DRIVER:
            *letter = 0x0E;
            break;
        case B_MONK:
            *letter = 0x06;
            break;
        case B_SUICIDE_UNIT:
            *letter = 0x0F;
            break;
        case B_DRUNK_DRIVER:
            *letter = 0xEC;
            break;
    }
    return letter;
}

void print_throw_zombie(int player, unsigned int y, zombie z){
    unsigned int x = player ? POS_INIT_ZOMBI_B : POS_INIT_ZOMBI_A ;
    unsigned short color = player ? C_FG_WHITE_BG_BLUE : C_FG_WHITE_BG_RED;
    print(zombie_to_char(z), x, y+1, color);
}

void print_move_zombie(int player, unsigned int x_orig, unsigned int y_orig, unsigned int x_dst, unsigned int y_dst, zombie z){
    unsigned short color = player ? C_FG_WHITE_BG_BLUE : C_FG_WHITE_BG_RED;
    print("X", x_orig, y_orig+1, C_FG_LIGHT_GREY_BG_GREEN);
    print(zombie_to_char(z), x_dst, y_dst+1, color);
}

void print_death_zombie(int player, unsigned int x, unsigned int y){
    unsigned short color = player ? C_FG_BLUE_BG_GREEN : C_FG_RED_BG_GREEN;
    short offset = x==1 ? 1 : -1;
    print("X", x, y+1, color);
    print("X",x+offset,y+1,C_FG_LIGHT_GREY_BG_GREEN);
}

void print_death_task() {
    info_player* player = playerActual ? &playerB : &playerA;

    unsigned int x = playerActual ? 59 : 4;
    unsigned short attr = playerActual ? C_FG_BLUE : C_FG_RED;
    print("X", x+2*(player->curr_zombie), 48, attr);
}

void print_info_player(int player){
    int j;
    for(j=18; j<32; j++){
        print("                                  ",18,j,C_FG_WHITE);
    }    
    info_player* jugador = player ? &playerB : &playerA;
    print("INFO PLAYER:", 20, 20, C_FG_WHITE );
    print_int(player,35, 20, C_FG_WHITE);
    print("SELECTED:", 20, 22, C_FG_WHITE );
    print_int((int)jugador->selected_type,35, 22, C_FG_WHITE);
    
    print("Y:", 20, 24, C_FG_WHITE );
    print_int(jugador->y,35, 24, C_FG_WHITE);


    print("CANT LANZ:", 20, 26, C_FG_WHITE );
    print_int(jugador->cant_lanzados,35, 26, C_FG_WHITE);


    print("ACT ZOMBIE:", 20, 28, C_FG_WHITE );
    print_int(jugador->curr_zombie,35, 28, C_FG_WHITE);


    print("GDT_IND_TASKS:", 20, 30, C_FG_WHITE );
    print_int(jugador->gdt_indexes_tasks[0],35, 30, C_FG_WHITE);
    print_int(jugador->gdt_indexes_tasks[1],37, 30, C_FG_WHITE);
    print_int(jugador->gdt_indexes_tasks[2],39, 30, C_FG_WHITE);
    print_int(jugador->gdt_indexes_tasks[3],41, 30, C_FG_WHITE);
    print_int(jugador->gdt_indexes_tasks[4],43, 30, C_FG_WHITE);
    print_int(jugador->gdt_indexes_tasks[5],45, 30, C_FG_WHITE);
    print_int(jugador->gdt_indexes_tasks[6],47, 30, C_FG_WHITE);
    print_int(jugador->gdt_indexes_tasks[7],49, 30, C_FG_WHITE);
}
void print_current_zombi(int player, int zombie_index){
    info_player* jugador = player ? &playerB : &playerA;
    info_zombie* zombie =&(jugador->info_zombies[zombie_index]);

    int j;
    for(j=18; j<32; j++){
        print("                                  ",18,j,C_FG_WHITE);
    }
    print("INFO PLAYER:", 20, 20, C_FG_WHITE );
    print_int(player,35, 20, C_FG_WHITE);
    print("CURRENT_ZOMBIE:", 20, 21, C_FG_WHITE );
    print_int(zombie_index,35, 21, C_FG_WHITE);
    print("TYPE:", 20, 22, C_FG_WHITE );
    print_int((int)zombie->type,35, 22, C_FG_WHITE);
    print("X:", 20, 24, C_FG_WHITE );
    print_int(zombie->x,35, 24, C_FG_WHITE);
    print("Y:", 20, 25, C_FG_WHITE );
    print_int(zombie->y,35, 25, C_FG_WHITE);
    
}

void girar_reloj_actual(){
    info_player* player = playerActual ? &playerB : &playerA;
    info_zombie* zombie = &((player->info_zombies)[player->curr_zombie]);

    zombie->reloj_actual = (zombie->reloj_actual + 1) % RELOJ_SIZE;

    unsigned int x = playerActual ? 59 : 4;
    print_char(reloj[zombie->reloj_actual], x+2*(player->curr_zombie), 48, 0x7);
}

void print_debugger(){
    print("                              ",25,7,C_BG_BLACK);
    int i;
    for (i=8;i<42;i++){
        print(" ", 25, i, C_BG_BLACK);
        print("                            ", 26, i, C_BG_LIGHT_GREY);
        print(" ", 54, i, C_BG_BLACK);
    }
    print("                              ",25,42,C_BG_BLACK);
    
    unsigned short color = playerActual ? C_FG_WHITE_BG_BLUE : C_FG_WHITE_BG_RED;
    info_player* jugador = playerActual ? &playerB : &playerA;
    info_zombie* zombie =&(jugador->info_zombies[jugador->curr_zombie]);
    char* zombie_name = "                            ";
    switch (zombie->type) {
        case A_MONK:
            zombie_name = "Zombie Monk                 ";
            break;
        case A_SUICIDE_UNIT:
            zombie_name = "Zombie Suicide Unit         ";
            break;
        case A_DRUNK_DRIVER:
            zombie_name = "Zombie Drunk Driver         ";
            break;
        case B_MONK:
            zombie_name = "Zombie Monk                 ";
            break;
        case B_SUICIDE_UNIT:
            zombie_name = "Zombie Suicide Unit         ";
            break;
        case B_DRUNK_DRIVER:
            zombie_name = "Zombie Drunk Driver         ";
            break;
    }
    print(zombie_name, 26, 8, color);

    print("eax", 27, 10, C_FG_BLACK_BG_LIGHT_GREY);
    print_hex(debug.eax, 8, 31, 10, C_FG_WHITE_BG_LIGHT_GREY);
    print("ebx", 27, 12, C_FG_BLACK_BG_LIGHT_GREY);
    print_hex(debug.ebx, 8, 31, 12, C_FG_WHITE_BG_LIGHT_GREY);
    print("ecx", 27, 14, C_FG_BLACK_BG_LIGHT_GREY);
    print_hex(debug.ecx, 8, 31, 14, C_FG_WHITE_BG_LIGHT_GREY);
    print("edx", 27, 16, C_FG_BLACK_BG_LIGHT_GREY);
    print_hex(debug.edx, 8, 31, 16, C_FG_WHITE_BG_LIGHT_GREY);
    print("esi", 27, 18, C_FG_BLACK_BG_LIGHT_GREY);
    print_hex(debug.esi, 8, 31, 18, C_FG_WHITE_BG_LIGHT_GREY);
    print("edi", 27, 20, C_FG_BLACK_BG_LIGHT_GREY);
    print_hex(debug.edi, 8, 31, 20, C_FG_WHITE_BG_LIGHT_GREY);
    print("ebp", 27, 22, C_FG_BLACK_BG_LIGHT_GREY);
    print_hex(debug.ebp, 8, 31, 22, C_FG_WHITE_BG_LIGHT_GREY);
    print("esp", 27, 24, C_FG_BLACK_BG_LIGHT_GREY);
    print_hex(debug.esp, 8, 31, 24, C_FG_WHITE_BG_LIGHT_GREY);
    print("eip", 27, 26, C_FG_BLACK_BG_LIGHT_GREY);
    print_hex(debug.eip, 8, 31, 26, C_FG_WHITE_BG_LIGHT_GREY);
    print("cs", 28, 28, C_FG_BLACK_BG_LIGHT_GREY);
    print_hex(debug.cs, 8, 31, 28, C_FG_WHITE_BG_LIGHT_GREY);
    print("ds", 28, 30, C_FG_BLACK_BG_LIGHT_GREY);
    print_hex(debug.ds, 8, 31, 30, C_FG_WHITE_BG_LIGHT_GREY);
    print("es", 28, 32, C_FG_BLACK_BG_LIGHT_GREY);
    print_hex(debug.es, 8, 31, 32, C_FG_WHITE_BG_LIGHT_GREY);
    print("fs", 28, 34, C_FG_BLACK_BG_LIGHT_GREY);
    print_hex(debug.fs, 8, 31, 34, C_FG_WHITE_BG_LIGHT_GREY);
    print("gs", 28, 36, C_FG_BLACK_BG_LIGHT_GREY);
    print_hex(debug.gs, 8, 31, 36, C_FG_WHITE_BG_LIGHT_GREY);
    print("ss", 28, 38, C_FG_BLACK_BG_LIGHT_GREY);
    print_hex(debug.ss, 8, 31, 38, C_FG_WHITE_BG_LIGHT_GREY);
    print("eflags", 28, 40, C_FG_BLACK_BG_LIGHT_GREY);
    print_hex(debug.eflags, 8, 34, 40, C_FG_WHITE_BG_LIGHT_GREY);

    print("cr0", 41, 10, C_FG_BLACK_BG_LIGHT_GREY);
    print_hex(debug.cr0, 8, 45, 10, C_FG_WHITE_BG_LIGHT_GREY);
    print("cr2", 41, 12, C_FG_BLACK_BG_LIGHT_GREY);
    print_hex(debug.cr2, 8, 45, 12, C_FG_WHITE_BG_LIGHT_GREY);
    print("cr3", 41, 14, C_FG_BLACK_BG_LIGHT_GREY);
    print_hex(debug.cr3, 8, 45, 14, C_FG_WHITE_BG_LIGHT_GREY);
    print("cr4", 41, 16, C_FG_BLACK_BG_LIGHT_GREY);
    print_hex(debug.cr4, 8, 45, 16, C_FG_WHITE_BG_LIGHT_GREY);

    print("excep", 41, 19, C_FG_BLACK_BG_LIGHT_GREY);
    print_hex(debug.exception, 4, 47, 19, C_FG_WHITE_BG_LIGHT_GREY);
    print("errcode", 41, 21, C_FG_BLACK_BG_LIGHT_GREY);
    print_hex(debug.errcode, 8, 45, 22, C_FG_WHITE_BG_LIGHT_GREY);
    
    print("stack", 41, 27, C_FG_BLACK_BG_LIGHT_GREY);
    print_hex(debug.stack0, 8, 41, 30, C_FG_WHITE_BG_LIGHT_GREY);
    print_hex(debug.stack1, 8, 41, 31, C_FG_WHITE_BG_LIGHT_GREY);
    print_hex(debug.stack2, 8, 41, 32, C_FG_WHITE_BG_LIGHT_GREY);
    print_hex(debug.stack3, 8, 41, 33, C_FG_WHITE_BG_LIGHT_GREY);
    print_hex(debug.stack4, 8, 41, 34, C_FG_WHITE_BG_LIGHT_GREY);
    
}

void print_puntos(unsigned int jugador, unsigned int puntos){
    unsigned int x = jugador ? 42 : 37;
    unsigned short color = jugador ? C_FG_WHITE_BG_BLUE : C_FG_WHITE_BG_RED;
    print_int(puntos,x,47,color);
}

void print_lanzados(unsigned int jugador, unsigned int lanzados) {
    unsigned int x = jugador ? 47 : 32;
    unsigned short color = jugador ? C_FG_WHITE_BG_BLUE : C_FG_WHITE_BG_RED;
    print_int(lanzados,x,47,color);
}

void print_victoria() {
    int i;
    for (i=3; i<40; ++i) {
        int j;
        for (j=10; j<71; ++j) {
            print(" ", j, i, C_FG_WHITE);
        }
    }
    print_skull();
    print("GANADOR: ", 35, 34, C_FG_WHITE);


    if (playerA.puntos == playerB.puntos) {
        print("EMPATE", 44, 34, C_FG_WHITE);
    } else if (playerA.puntos > playerB.puntos) {
        print("A", 44, 34, C_FG_RED);
    } else {
        print("B", 44, 34, C_FG_BLUE);
    }
}

void print_skull() {
    int x = 32;
    int y = 4;
    unsigned short c = C_FG_WHITE;

    print(".ed\"\"\"\" \"\"\"$$$$be.", x, y, c);
    print("-\"           ^\"\"**$$$e.", x-2, y+1, c);
    print(".\"                   '$$$c", x-4, y+2, c);
    print("/                      \"4$$b", x-5, y+3,c);
    print("d  3                      $$$$", x-6, y+4, c);
    print("$  *                   .$$$$$$", x-6, y+5, c);
    print(".$  ^c           $$$$$e$$$$$$$$.", x-7, y+6, c);
    print("d$L  4.         4$$$$$$$$$$$$$$b", x-7, y+7, c);
    print("$$$$b ^ceeeee.  4$$ECL.F*$$$$$$$", x-7, y+8, c);
    print("e$\"\"=.      $$$$P d$$$$F $ $$$$$$$$$- $$$$$$", x-19, y+9, c);
    print("z$$b. ^c     3$$$F \"$$$$b   $\"$$$$$$$  $$$$*\"      .=\"\"$c", x-20, y+10, c);
    print("4$$$$L        $$P\"  \"$$b   .$ $$$$$...e$$        .=  e$$$.", x-21, y+11, c);
    print("^*$$$$$c  \"..   *c    ..    $$ 3$$$$$$$$$$eF     zP  d$$$$$", x-21, y+12, c);
    print("\"**$$$ec   \"   \"ce\"\"    $$$  $$$$$$$$$$*    .r\" =$$$$P\"\"", x-19, y+13, c);
    print("\"*$b.  \"c  *$e.    *** d$$$$$\"L$$    .d\"  e$$***\"", x-13, y+14, c);
    print("^*$$c ^$c $$$      4J$$$$$\" $$$ .e*\".eeP\"", x-11, y+15, c);
    print("\"$$$$$$\"'$=e....$*$$**$cz$$\" \"..d$*\"", x-8, y+16, c);
    print("\"*$$$  *=\"4.$ L L$ P3$$$F $$$P\"", x-6, y+17, c);
    print("\"$   \"\"*ebJLzb$e$$$$$b $P\"", x-3, y+18, c);
    print("\"..      4$$$$$$$$$$ \"", x-1, y+19, c);
    print("$$$e   z$$$$$$$$$$\"", x, y+20, c);
    print("\"*$c  \"$$$$$$$P\"", x+1, y+21, c);
    print(".\"\"\"*$$$$$$$$bc", x+2, y+22, c);
    print(".-\"    .$***$$$\"\"\"*e.", x-1, y+23, c);
    print(".-\"    .e$\"     \"*$c  ^*b.", x-4, y+24, c);
    print(".=*\"\"\"\"    .e$*\"          \"*bc  \"*$e..", x-11, y+25, c);
    print(".$\"        .z*\"               ^*$e.   \"*****e.", x-13, y+26, c);
    print("$$ee$c   .d\"                     \"*$.        3.", x-13, y+27, c);
    print("^*$E\")$..$\"                         *   .ee==d\"", x-13, y+28, c);
    print("$.d$$$*                           *  J$$$e*", x-10, y+29, c);
    print("\"\"\"\"\"                              \"$$$\"", x-9, y+30, c);
}

void backup_screen(){
    ca (*p)[VIDEO_COLS] = (ca (*)[VIDEO_COLS]) VIDEO;
    unsigned int i, j;
    for(i = 0; i<VIDEO_FILS; i++){
        for(j = 0; j<VIDEO_COLS; j++){
            backup[i][j] = p[i][j];
        }
    }
}

void backup_restore_screen(){
    ca (*p)[VIDEO_COLS] = (ca (*)[VIDEO_COLS]) VIDEO;
    unsigned int i, j;
    for(i = 0; i<VIDEO_FILS; i++){
        for(j = 0; j<VIDEO_COLS; j++){
            p[i][j] = backup[i][j];
        }
    }
}