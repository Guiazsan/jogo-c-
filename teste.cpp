#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <iostream>
#include <time.h>
#include <termios.h>
#include <string.h>

using namespace std;
struct winsize w;

string* linhas;
struct screenObject{
    int y;
    int x;
    string sprite;
    void move(int posx,int posy){
        if(y < w.ws_row && x < w.ws_col){
            //paint(y)
            x += posx;
            y += posy;
            //paint()
        }
    }
};
//Personagem principal
screenObject player;
// Inimigos
screenObject* enemies = new screenObject[20];
int deaths; //Quantidade de mortos
screenObject shoot;
string shootdir; //Direção do tiro
bool live = true;
struct termios orig_term_attr;

int getkey() {
    int character;    
    struct termios new_term_attr;

    memcpy(&new_term_attr, &orig_term_attr, sizeof(struct termios));
    new_term_attr.c_lflag &= ~(ECHO|ICANON);
    new_term_attr.c_cc[VTIME] = 0;
    new_term_attr.c_cc[VMIN] = 0;
    tcsetattr(fileno(stdin), TCSANOW, &new_term_attr);
    /* read a character from the stdin stream without blocking */
    /*   returns EOF (-1) if no character is available */
    character = fgetc(stdin);
    return character;
}

void start(){
    system("clear");
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    linhas = new string[w.ws_row];
    system("stty raw");  
    player.sprite = "A";
    shoot.sprite = ".";
    int i;
    for(i = 0; i < 20; i++){
        enemies[i].x = rand()%(w.ws_col + 1);
        enemies[i].y = rand()%(w.ws_row + 1);
        enemies[i].sprite = "\033[0;31mX\033[1;37;0m";
    }
}

void paint(int y) {
    system("clear");
    string linha;
    bool vazio;
    int x, i;
    for(x = 0; x < w.ws_col; x++){
        vazio = true;
        if(player.y == y && player.x == x){
            linha = linha + player.sprite;
            vazio = false;
        } else if (shoot.x == x && shoot.y == y) {
            linha = linha + shoot.sprite;
            vazio = false;
            
        } else {
            for(i = 0; i < 20; i++){
                if(enemies[i].x == x && enemies[i].y == y){
                    linha = linha + enemies[i].sprite;
                    vazio = false;
                }    
            }

        }
        if (vazio){
            linha = linha + " ";
        } 
        
    }
    for(i = 0; i < w.ws_row; i++){
        cout << linhas[i];    
    }
}

void KeyEvent(){
    int key = 0;
    key = getkey();
    switch (key){
        case 'w':
            if(player.y > 0)
                player.move(0,-1);
                player.sprite = "A";
            break;
        case 's':
            if(player.y < w.ws_row)
                player.move(0,1);
                player.sprite = "V";
            break;
        case 'a':
            if(player.x > 0)
                player.move(-1,0);
                player.sprite = "<";
            break;
        case 'd':
            if(player.x < w.ws_col)
                player.move(1,0);
                player.sprite = ">";
            break; 

        case 'f':
            shoot.x = player.x;
            shoot.y = player.y;
            shootdir = player.sprite;
            break;

        case 'q':
            system("clear");
            terminate();
            break;              
    
        default:
            break;
    }

}

void process(){ 
    bool vazio;

    if(shootdir == "A"){
        shoot.move(0,-1);
    }else if(shootdir == "V"){
        shoot.move(0,1);
    }else if(shootdir == "<"){
        shoot.move(-1,0);
    }else if(shootdir == ">"){
        shoot.move(1,0);
    } 
}

void update(){
    while(live){        
        KeyEvent();
        system("clear");
        process();
        if(!live){
            system("clear");
            
            cout << "Game Over \n";
            terminate();
        }
    }
}

int main (){
    start();
    update();
    return 0;
}