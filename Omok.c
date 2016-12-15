#include <stdio.h>
#include <ncursesw/curses.h>
#include <locale.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include "Omok.h"
#include "server.c"
#define TIME 350000

int state=0;
int x, y;
int ex, ey;
int S, port;
int mystone, opponentstone;
int win, lose;
char map_mirror3[3][5];
char map_mirror4[8][17];
char map_mirror5[15][29];
int gamecheck = 0;
int socketcheck = 1;
int backtomenu = 0;

main(){	
	int key, q = 1, q2 = 1, q3 = 1;
	int i, j;
	char temp;
	int socket;
	pthread_t t1;
	void *flickering(void *);
	pthread_create(&t1, NULL, flickering, NULL);

	while(1){
		fflush(stdin);
		backtomenu = 0;
		socketcheck = 1;
		socket = 0;
		while(socketcheck){
			socketcheck = 0;
			start();
			socket=setSocket();
			set_up();
		}
		for(i=0; i<3; i++){
			for(j=0; j<5; j++){
				map3[i][j] = 0;
			}
		}
		for(i=0; i<8; i++){
			for(j=0; j<17; j++){
				map4[i][j] = 0;
			}
		}
		for(i=0; i<15; i++){
			for(j=0; j<29; j++){
				map5[i][j] = 0;
			}
		}

		if (gamecheck == 3){
			while(1){ //continue game
				if(backtomenu)
					break;
				resetMap();
				if(mystone == 11){
					mvaddstr(10,30,"Opponent's turn!!                 ");
					refresh();
					receiveXY(&ex, &ey, socket);
					map_mirror3[ey][ex] = opponentstone;
					print(ex,ey,opponentstone);
				}//receive first if stone color white
				q = 1;
				while(q){ //one game
					fflush(stdin);
					refresh();
					q2 = 1;q3 = 1;
					x=2;y=1;
					clearbottom();
					mvaddstr(10,30,"my turn!!                           ");
					print(x,y,mystone);//set game
					while(q2){ //put stone
						state=1;
						refresh();
						key = getch();
						print(x,y,mystone);
						switch(key){
						case KEY_UP:
							if(y-1>=0){
								state = 0;
								print(x,y, map_mirror3[y][x]);
								y--;
								print(x,y,mystone);
								state = 1;
							}
							break;
						case KEY_DOWN:
							if(y+1<=2){
								state = 0;
								print(x,y, map_mirror3[y][x]);
								y++;
								print(x,y,mystone);
								state = 1;
							}
							break;
						case KEY_LEFT:
							if(x-2>=0){
								state = 0;
								print(x,y, map_mirror3[y][x]);
								x-=2;
								print(x,y,mystone);
								state = 1;
							}
							break;
						case KEY_RIGHT:
							if(x+2<=5){
								state = 0;
								print(x,y, map_mirror3[y][x]);
								x+=2;
								print(x,y,mystone);
								state = 1;
							}
							break;
						case '\n':case KEY_ENTER:
							//my turn
							key = put3(x,y,mystone);
							if(!key){
								mvaddstr(15,0,"there's already stone there.              ");
								move(y,x);
								break;
							}
							if(key > 2 && key < 6){
								printbottom(key);
								move(y,x);
								break;
							} //lose
							sendXY(x,y,socket);
							if(key == 2){
								mvaddstr(15,0,"You win!!                         ");
								win++;
								print_stat();
								q = 0;
							} //win
							else if(key == 6){
								mvaddstr(15,0,"Draw!!                          ");
								q = 0;
							} //draw
							refresh();
							if(!q){
								mvaddstr(16,0,"Restart?(Y/N)                       ");
								refresh();
								while(q3){
									key = getch();
									switch(key){
									case 'Y':case 'y':
										sendXY(1,1,socket);
										mvaddstr(15,0,"wait for Opponent's input                 ");
										refresh();
										receiveXY(&x,&y,socket);
										if(x&&y){
											temp = opponentstone;
											opponentstone = mystone;
											mystone = temp;
											q3 = 0;
										}
										else if(S){
											clear();
											sleep(2);
											close(socket);
											refresh();
											endwin();
											q3 = 0;
											backtomenu = 1;
											break;
										}
										else{
											clear();
											sleep(2);
											endwin();
											q3 = 0;
											backtomenu = 1;
											break;
										}
										break;
									case 'N':case 'n':
										sendXY(0,0,socket);
										sleep(2);
										if(S){
											close(socket);
										}
										clear();
										refresh();
										endwin();
										q3 = 0;
										backtomenu = 1;
										break;
									}
								}
								q2 = 0;
							}
							fflush(stdin);
							//enemy turn
							if(q2 != 0){
							mvaddstr(10,30,"opponent's turn   ");
							clearbottom();
							refresh();
							receiveXY(&ex, &ey, socket);
							key = put3(ex,ey,opponentstone);
							map_mirror3[ey][ex]=opponentstone;
							print(ex,ey, map_mirror3[ey][ex]);
							if(key == 6){ 
								mvaddstr(15,0,"Draw!!                           ");
								print_stat();
								q = 0;
							} //draw
							else if(key == 2){
								mvaddstr(15,0,"Opponent win!!                           ");
								lose++;
								print_stat();
								q = 0;
							} //win
							refresh();
							}
							if(!q){
								mvaddstr(16,0,"restart?(Y/N)                     ");
								refresh();
								while(q3){
									key = getch();
									switch(key){
									case 'Y':case 'y':
										sendXY(1,1,socket);
										mvaddstr(15,0,"wait for Opponent's input                ");
										refresh();
										receiveXY(&x,&y,socket);
										if(x&&y){
											temp = opponentstone;
											opponentstone = mystone;
											mystone = temp;
											q3 = 0;
										}
										else if(S){
											clear();
											sleep(2);
											close(socket);
											refresh();
											endwin();
											q3 = 0;
											backtomenu = 1;
											break;
										}
										else{
											clear();
											sleep(2);
											refresh();
											endwin();
											q3 = 0;
											backtomenu = 1;
											break;
										}
										break;
									case 'N':case 'n':
										sendXY(0,0,socket);
										sleep(2);
										clear();
										if(S){
											close(socket);
										}
										clear();
										refresh();
										endwin();
										q3 = 0;
										backtomenu = 1;
										break;
									}
								}
								q2 = 0;
								break;
							}
							q2 = 0;
							break;
						}
						if(backtomenu)
							break;
					}
					if(backtomenu)
						break;
				}
			}
		}
		else if(gamecheck == 4){
			while(1){ //continue game
				if(backtomenu)
					break;
				resetMap();
				if(mystone == 11){
					mvaddstr(10,30,"Opponent's turn!!                 ");
					refresh();
					receiveXY(&ex, &ey, socket);
					map_mirror4[ey][ex] = opponentstone;
					print(ex,ey,opponentstone);
				}//receive first if stone color white
				q = 1;
				while(q){ //one game
					fflush(stdin);
					refresh();
					q2 = 1;q3 = 1;
					x=6;y=3;
					clearbottom();
					mvaddstr(10,30,"my turn!!                           ");
					print(x,y,mystone);//set game
					while(q2){ //put stone
						state=1;
						refresh();
						key = getch();
						print(x,y,mystone);
						switch(key){
						case KEY_UP:
							if(y-1>=0){
								state = 0;
								print(x,y, map_mirror4[y][x]);
								y--;
								print(x,y,mystone);
								state = 1;
							}
							break;
						case KEY_DOWN:
							if(y+1<=7){
								state = 0;
								print(x,y, map_mirror4[y][x]);
								y++;
								print(x,y,mystone);
								state = 1;
							}
							break;
						case KEY_LEFT:
							if(x-2>=0){
								state = 0;
								print(x,y, map_mirror4[y][x]);
								x-=2;
								print(x,y,mystone);
								state = 1;
							}
							break;
						case KEY_RIGHT:
							if(x+2<=17){
								state = 0;
								print(x,y, map_mirror4[y][x]);
								x+=2;
								print(x,y,mystone);
								state = 1;
							}
							break;
						case '\n':case KEY_ENTER:
							//my turn
							key = put4(x,y,mystone);
							if(!key){
								mvaddstr(15,0,"You Can't put stone there.              ");
								move(y,x);
								break;
							}
							if(key > 2 && key < 6){
								printbottom(key);
								move(y,x);
								break;
							} //lose
							sendXY(x,y,socket);
							if(key == 2){
								mvaddstr(15,0,"You win!!                         ");
								win++;
								print_stat();
								q = 0;
							} //win
							else if(key == 6){
								mvaddstr(15,0,"Draw!!                          ");
								q = 0;
							} //draw
							refresh();
							if(!q){
								mvaddstr(16,0,"Restart?(Y/N)                       ");
								refresh();
								while(q3){
									key = getch();
									switch(key){
									case 'Y':case 'y':
										sendXY(1,1,socket);
										mvaddstr(15,0,"wait for Opponent's input                 ");
										refresh();
										receiveXY(&x,&y,socket);
										if(x&&y){
											temp = opponentstone;
											opponentstone = mystone;
											mystone = temp;
											q3 = 0;
										}
										else if(S){
											clear();
											sleep(2);
											close(socket);
											refresh();
											endwin();
											q3 = 0;
											backtomenu = 1;
											break;
										}
										else{
											clear();
											sleep(2);
											endwin();
											q3 = 0;
											backtomenu = 1;
											break;
										}
										break;
									case 'N':case 'n':
										sendXY(0,0,socket);
										sleep(2);
										clear();
										if(S){
											close(socket);
										}
										clear();
										refresh();
										endwin();
										q3 = 0;
										backtomenu = 1;
										break;
									}
								}
								q2 = 0;
							}
							fflush(stdin);
							//enemy turn
							if(q2 != 0){
							mvaddstr(10,30,"opponent's turn   ");
							clearbottom();
							refresh();
							receiveXY(&ex, &ey, socket);
							key = put4(ex,ey,opponentstone);
							map_mirror4[ey][ex]=opponentstone;
							print(ex,ey, map_mirror4[ey][ex]);
							if(key == 6){ 
								mvaddstr(15,0,"Draw!!                           ");
								print_stat();
								q = 0;
							} //draw
							else if(key == 2){
								mvaddstr(15,0,"Opponent win!!                           ");
								lose++;
								print_stat();
								q = 0;
							} //win
							refresh();
							}
							if(!q){
								mvaddstr(16,0,"restart?(Y/N)                     ");
								refresh();
								while(q3){
									key = getch();
									switch(key){
									case 'Y':case 'y':
										sendXY(1,1,socket);
										mvaddstr(15,0,"wait for Opponent's input                ");
										refresh();
										receiveXY(&x,&y,socket);
										if(x&&y){
											temp = opponentstone;
											opponentstone = mystone;
											mystone = temp;
											q3 = 0;
										}
										else if(S){
											clear();
											sleep(2);
											close(socket);
											refresh();
											q3 = 0;
											backtomenu = 1;
											break;
										}
										else{
											clear();
											sleep(2);
											q3 = 0;
											backtomenu = 1;
											break;
										}
										break;
									case 'N':case 'n':
										sendXY(0,0,socket);
										sleep(2);
										clear();
										if(S){
											close(socket);
										}
										clear();
										refresh();
										q3 = 0;
										backtomenu = 1;
										break;
									}
								}
								q2 = 0;
								break;
							}
							q2 = 0;
							break;
						}
						if(backtomenu)
							break;
					}
					if(backtomenu)
						break;
				}
			}
		}
		else if(gamecheck == 5){ // omok.
			while(1){ //continue game
				if(backtomenu)
					break;
				resetMap();
				if(mystone == 11){
					mvaddstr(10,30,"Opponent's turn!!                 ");
					refresh();
					receiveXY(&ex, &ey, socket);
					map_mirror5[ey][ex] = opponentstone;
					print(ex,ey,opponentstone);
				}//receive first if stone color white
				q = 1;
				while(q){ //one game
					fflush(stdin);
					refresh();
					q2 = 1;q3 = 1;
					x=14;y=7;
					clearbottom();
					mvaddstr(10,30,"my turn!!                         ");
					print(x,y,mystone);//set game
					while(q2){ //put stone
						state=1;
						refresh();
						key = getch();
						print(x,y,mystone);
						switch(key){
						case KEY_UP:
							if(y-1>=0){
								state = 0;
								print(x,y, map_mirror5[y][x]);
								y--;
								print(x,y,mystone);
								state = 1;
							}
							break;
						case KEY_DOWN:
							if(y+1<=14){
								state = 0;
								print(x,y, map_mirror5[y][x]);
								y++;
								print(x,y,mystone);
								state = 1;
							}
							break;
						case KEY_LEFT:
							if(x-2>=0){
								state = 0;
								print(x,y, map_mirror5[y][x]);
								x-=2;
								print(x,y,mystone);
								state = 1;
							}
							break;
						case KEY_RIGHT:
							if(x+2<=29){
								state = 0;
								print(x,y, map_mirror5[y][x]);
								x+=2;
								print(x,y,mystone);
								state = 1;
							}
							break;
						case '\n':case KEY_ENTER:
							//my turn
							key = put5(x,y,mystone);
							if(!key){
								mvaddstr(15,0,"there's already stone there.              ");
								move(y,x);
								break;
							}
							if(key > 2 && key < 6){
								printbottom(key);
								move(y,x);
								break;
							} //lose
							sendXY(x,y,socket);
							if(key == 2){
								mvaddstr(15,0,"You win!!                         ");
								win++;
								print_stat();
								q = 0;
							} //win
							else if(key == 6){
								mvaddstr(15,0,"Draw!!                          ");
								q = 0;
							} //draw
							refresh();
							if(!q){
								mvaddstr(16,0,"Restart?(Y/N)                       ");
								refresh();
								while(q3){
									key = getch();
									switch(key){
									case 'Y':case 'y':
										sendXY(1,1,socket);
										mvaddstr(15,0,"wait for Opponent's input                 ");
										refresh();
										receiveXY(&x,&y,socket);
										if(x&&y){
											temp = opponentstone;
											opponentstone = mystone;
											mystone = temp;
											q3 = 0;
										}
										else if(S){
											clear();
											sleep(2);
											close(socket);
											refresh();
											endwin();
											q3 = 0;
											backtomenu = 1;
											break;
										}
										else{
											clear();
											sleep(2);
											refresh();
											endwin();
											q3 = 0;
											backtomenu = 1;
											break;
										}
										break;
									case 'N':case 'n':
										sendXY(0,0,socket);
										sleep(2);
										clear();
										if(S){
											close(socket);
										}
										clear();
										refresh();
										endwin();
										q3 = 0;
										backtomenu = 1;
										break;
									}
								}
								q2 = 0;
							}
							fflush(stdin);
							//enemy turn
							if(q2 != 0){
							mvaddstr(10,30,"opponent's turn   ");
							clearbottom();
							refresh();
							receiveXY(&ex, &ey, socket);
							key = put5(ex,ey,opponentstone);
							map_mirror5[ey][ex]=opponentstone;
							print(ex,ey, map_mirror5[ey][ex]);
							}
							if(key == 6){ 
								mvaddstr(15,0,"Draw!!                           ");
								print_stat();
								q = 0;
							} //draw
							else if(key == 2){
								mvaddstr(15,0,"Opponent win!!                           ");
								lose++;
								print_stat();
								q = 0;
							} //win
							refresh();
							if(!q){
								mvaddstr(16,0,"restart?(Y/N)                     ");
								refresh();
								while(q3){
									key = getch();
									switch(key){
									case 'Y':case 'y':
										sendXY(1,1,socket);
										mvaddstr(15,0,"wait for Opponent's input                 ");
										refresh();
										receiveXY(&x,&y,socket);
										if(x&&y){
											temp = opponentstone;
											opponentstone = mystone;
											mystone = temp;
											q3 = 0;
										}
										else if(S){
											clear();
											sleep(2);
											close(socket);
											refresh();
											endwin();
											q3 = 0;
											backtomenu = 1;
											break;
										}
										else{
											clear();
											sleep(2);
											q3 = 0;
											backtomenu = 1;
											break;
										}
										break;
									case 'N':case 'n':
										sendXY(0,0,socket);
										sleep(2);
										clear();
										if(S){
											close(socket);
										}
										clear();
										refresh();
										endwin();
										q3 = 0;
										backtomenu = 1;
										break;
									}
								}
								q2 = 0;
								break;
							}
							q2 = 0;
							break;
						}
						if(backtomenu)
							break;
					}
					if(backtomenu)
						break;
				}
			}
		}
	}
}

void printbottom(int a){
	switch(a){
	case 3: 
		mvaddstr(15,0,"6 stones in line is not allowd.                ");
		break;
	case 4:
		mvaddstr(15,0,"33 not allowed.                                ");
		break; 
	case 5:
		mvaddstr(15,0,"44 not allowed.                                ");
		break;
	}
}
void clearbottom(){
	mvaddstr(15,0,"                                             ");
	mvaddstr(16,0,"                                             ");
}
void *flickering(void * a){
	while(1){
		while(state){
			if(gamecheck == 3){
				print(x,y,mystone);
				usleep(TIME);
				print(x,y,map_mirror3[y][x]);
				usleep(TIME);
				if(!state){
					print(x,y, map_mirror3[y][x]);
				}
			}
			else if(gamecheck == 4){
				print(x,y,mystone);
				usleep(TIME);
				print(x,y,map_mirror4[y][x]);
				usleep(TIME);
				if(!state){
					print(x,y, map_mirror4[y][x]);
				}
			}
			else{
				print(x,y,mystone);
				usleep(TIME);
				print(x,y,map_mirror5[y][x]);
				usleep(TIME);
				if(!state){
					print(x,y, map_mirror5[y][x]);
				}
			}

		}
	}
}
void resetMap(){
	int i,j;
	clear();
	if(gamecheck == 3){
		for(i=0; i<3; i++){
			for(j=0; j<5; j++){
				map_mirror3[i][j] = map3[i][j];
			}
		}
		for(i=0; i<3; i++){
			for(j=0; j<5; j++){
				move(i,j);
				switch(map3[i][j]){
				case 0:
					addstr(" ");
					break;
				}
			}
		}
	}
	else if(gamecheck == 4){
		for(i=0; i<8; i++){
			for(j=0; j<17; j++){
				map_mirror4[i][j] = map4[i][j];
			}
		}
		for(i=0; i<8; i++){
			for(j=0; j<17; j++){
				move(i,j);
				switch(map4[i][j]){
				case 0:
					addstr(" ");
					break;
				}
			}
		}
	}
	else{
		for(i=0; i<15; i++){
			for(j=0; j<29; j++){
				map_mirror5[i][j] = map5[i][j];
			}
		}
		for(i=0; i<15; i++){
			for(j=0; j<29; j++){
				move(i,j);
				switch(map4[i][j]){
				case 0:
					addstr(" ");
					break;
				}
			}
		}
	}
	print_stat();
}

void print(int xx, int yy, char a){
	if(gamecheck == 3)
	{
		switch(a){
		case 0:
			move(yy*5 + 0, xx*5); 
			addstr("         ");
			move(yy*5 + 1, xx*5);
			addstr("         ");
			move(yy*5 + 2, xx*5);
			addstr("         ");
			move(yy*5 + 3, xx*5);
			addstr("         ");
			break;
		case 10:
			move(yy*5 + 0, xx*5); 
			addstr("---------");
			move(yy*5 + 1, xx*5);
			addstr("|+++++++|");
			move(yy*5 + 2, xx*5);
			addstr("|+++++++|");
			move(yy*5 + 3, xx*5);
			addstr("---------");
			break;
		case 11:
			move(yy*5 + 0, xx*5); 
			addstr("---------");
			move(yy*5 + 1, xx*5);
			addstr("|       |");
			move(yy*5 + 2, xx*5);
			addstr("|       |");
			move(yy*5 + 3, xx*5);
			addstr("---------");
			break;
		}
	}
	else
	{
		move(yy,xx);
		switch(a){
		case 0:
			addstr(" ");
			break;
		case 10:
			addstr("o");
			break;
		case 11:
			addstr("x");
			break;
		}
	}
	refresh();
}
void print_stat(){
	int i,j,k;
	mvprintw(1,34,"my state");
	mvprintw(2,31,"win :%2d", win);
	mvprintw(3,31,"lose:%2d", lose);
	mvprintw(6,31,"opponent's state");
	mvprintw(7,31,"win :%2d", lose);
	mvprintw(8,31,"lose:%2d", win);
	refresh();
}

int isVictory3(char mystonec){

	int tx, ty;

	for(tx=0; tx<5; tx+=2)
	{
		if(map_mirror3[0][tx] == mystonec)	
			if(map_mirror3[1][tx] == mystonec)
				if(map_mirror3[2][tx] == mystonec)
					return 1;
	}

	for(ty=0; ty<3; ty++)
	{
		if(map_mirror3[ty][0] == mystonec)	
			if(map_mirror3[ty][2] == mystonec)
				if(map_mirror3[ty][4] == mystonec)
					return 1;
	}

	if(map_mirror3[0][0] == mystonec)
		if(map_mirror3[1][2] == mystonec)
			if(map_mirror3[2][4] == mystonec)
				return 1;

	if(map_mirror3[0][4] == mystonec)
		if(map_mirror3[1][2] == mystonec)
			if(map_mirror3[2][0] == mystonec)
				return 1;

	return 0;
}

int isVictory4( char mystonec){
	int i,j;

	for(j=0;j<8;j++) // ㅡ 모양의 4목
	{
		for(i=0;i<11;i++)
			if(map_mirror4[j][i] == mystonec && map_mirror4[j][i+2] == mystonec && map_mirror4[j][i+4] == mystonec && map_mirror4[j][i+6] == mystonec)
				return 1;
	}
	for(i=0;i<17;i++) // ㅣ모양 4목
	{
		for(j=0;j<5;j++)
			if(map_mirror4[j][i] == mystonec && map_mirror4[j+1][i] == mystonec && map_mirror4[j+2][i] == mystonec && map_mirror4[j+3][i] == mystonec)
				return 1;
	}
	for(i=0;i<11;i++) // \모양 4목
	{
		for(j=0;j<5;j++)
			if(map_mirror4[j][i] == mystonec && map_mirror4[j+1][i+2] == mystonec && map_mirror4[j+2][i+4] == mystonec && map_mirror4[j+3][i+6] == mystonec)
				return 1;
	}
	for(i=6;i<17;i++) // /모양 4목
	{
		for(j=0;j<5;j++)
			if(map_mirror4[j][i] == mystonec && map_mirror4[j+1][i-2] == mystonec && map_mirror4[j+2][i-4] == mystonec && map_mirror4[j+3][i-6] == mystonec)
				return 1;
	}

	return 0;
}

int isVictory5( char mystonec ){
	int dx[] ={1, 0, 1,-1};
	int dy[] = {0, 1, 1,1};
	int i,x1,y1,d;
	int tx;
	int ty;
	const int NOD = 4;


	for(d=0; d<NOD; d++){
		for(x1=0; x1<29; x1+=2){
			for(y1=0; y1<15; y1++){
				for(i=0 ; i<5; i++){
					tx = x1 + dx[d] * i*2;
					ty =  y1 + dy[d] * i;
					if(  map_mirror5[ty][tx] != mystonec ) 
						break;
					if( ty>15||tx>31) 
						break;
				}
				if( i == 5 ) 
					return 1;
			}
		}
	}
	return 0;
}
int six(int xx, int yy, char mystonec) {
	int dx[] = { 1, 0, 1,-1 };
	int dy[] = { 0, 1, 1,1 };
	int i, x, y, d;
	int tx;
	int ty;
	int temp;
	const int NOD = 4;
	temp = map_mirror5[yy][xx];
	map_mirror5[yy][xx] = mystonec;

	if (mystonec == 10) {
		for (d = 0; d < NOD; d++) {
			for (x = 0; x < 29; x += 2) {
				for (y = 0; y < 15; y++) {
					for (i = 0; i < 10; i++) {
						tx = x + dx[d] * i * 2;
						ty = y + dy[d] * i;
						if (map_mirror5[ty][tx] != mystonec)
							break;
						if (ty > 14 || tx > 29)
							break;
						if (i == 5) {
							map_mirror5[yy][xx] = temp;
							return 1;
						}
					}
				}
			}
		}
	}
	map_mirror5[yy][xx] = temp;
	return 0;
}
int threethree(int xx, int yy, char mystonec)
{
	int t, c, i, j, k, three, four, cc;
	char tempMap;

	three = 0, four = 0;

	for (j = 0; j < 4; j++){
		t = 0, cc = 0;
		for (k = j; k < 8; k = k + 4){
			c = 0;
			for (i = 1; i < 5; i++){
				if (k == 0){
					if (yy - i < 0){
						i--;
						if (map_mirror5[yy - i][xx] != 10 && map_mirror5[yy - i][xx] != 11){
							i++;
							break;
						}
						i++;
						cc++;
						break;
					}
					tempMap = map_mirror5[yy - i][xx];
				}
				else if (k == 1){ 
					if (yy - i < 0 || xx + i * 2 > 28){
						i--;
						if (map_mirror5[yy - i][xx + i * 2] != 10 && map_mirror5[yy - i][xx + i * 2] != 11){
							i++;
							break;
						}
						i++;
						cc++;
						break;
					}
					tempMap = map_mirror5[yy - i][xx + i * 2];
				}
				else if (k == 2){
					if (xx + i * 2 > 28){
						i--;
						if (map_mirror5[yy][xx + i * 2] != 10 && map_mirror5[yy][xx + i * 2] != 11){
							i++;
							break;
						}
						i++;
						cc++;
						break;
					}
					tempMap = map_mirror5[yy][xx + i * 2];
				}
				else if (k == 3){ 
					if (yy + i > 14 || xx + i * 2 > 28){
						i--;
						if (map_mirror5[yy + i][xx + i * 2] != 10 && map_mirror5[yy + i][xx + i * 2] != 11){
							i++;
							break;
						}
						i++;
						cc++;
						break;
					}
					tempMap = map_mirror5[yy + i][xx + i * 2];
				}
				else if (k == 4){
					if (yy + i > 14){
						i--;
						if (map_mirror5[yy + i][xx] != 10 && map_mirror5[yy + i][xx] != 11){
							i++;
							break;
						}
						i++;
						cc++;
						break;
					}
					tempMap = map_mirror5[yy + i][xx];
				}
				else if (k == 5){ 					if (yy + i > 14 || xx - i * 2 < 0){
					i--;
					if (map_mirror5[yy + i][xx - i * 2] != 10 && map_mirror5[yy + i][xx - i * 2] != 11){
						i++;
						break;
					}
					i++;
					cc++;
					break;
				}
				tempMap = map_mirror5[yy + i][xx - i * 2];
				}
				else if (k == 6){
					if (xx - i * 2 < 0){
						i--;
						if (map_mirror5[yy][xx - i * 2] != 10 && map_mirror5[yy][xx - i * 2] != 11){
							i++;
							break;
						}
						i++;
						cc++;
						break;
					}
					tempMap = map_mirror5[yy][xx - i * 2];
				}
				else if (k == 7){ 
					if (yy - i < 0 || xx - i * 2 < 0){
						i--;
						if (map_mirror5[yy - i][xx - i * 2] != 10 && map_mirror5[yy - i][xx - i * 2] != 11){
							i++;
							break;
						}
						i++;
						cc++;
						break;
					}
					tempMap = map_mirror5[yy - i][xx - i * 2];
				}

				if (mystonec == tempMap){
					t++;
				}
				else if (tempMap == 10 || tempMap == 11){
					cc++;
					break;
				}
				else{
					if (c == 1)
						break;

					if (i == 1){
						t = t + 10;
					}
					c = 1;
				}
			}
		}

		if (t == 2 || t == 12){
			if (cc == 0)
				three++;
		}
		if (t == 3 || t == 13 || t == 23){
			if (cc != 2)
				four++;
		}
	}

	if (three >= 2)
		return 1;
	if (four >= 2)
		return 2;	

	return 0;
}


int isDraw3() {
	int  x1, y1;
	for (x1 = 0; x1 < 5; x1 += 2) {
		for (y1 = 0; y1 < 3; y1++) {
			if (map_mirror3[y1][x1] == 10 || map_mirror3[y1][x1] == 11);
			else return 0;
		}
	}
	return 1; //draw.
}

int isDraw4() {
	int  x1, y1;
	for (x1 = 0; x1 < 17; x1 += 2) {
		for (y1 = 0; y1 < 8; y1++) {
			if (map_mirror4[y1][x1] == 10 || map_mirror4[y1][x1] == 11);
			else	return 0;
		}
	}
	return 1; //draw.
}

int isDraw5() {
	int  x1, y1;
	for (x1 = 0; x1 < 29; x1 += 2) {
		for (y1 = 0; y1 < 15; y1++) {
			if (map_mirror5[y1][x1] == 10 || map_mirror5[y1][x1] == 11);
			else	return 0;
		}
	}
	return 1; //draw.
}



int put3(int xx,int yy,char mystonec){
	int End, i, j;
	char temp;
	if( map_mirror3[yy][xx]==10 || map_mirror3[yy][xx] == 11)	//stone exist at xy
		return 0;

	map_mirror3[yy][xx] = mystonec;
	state = 0;
	print(xx,yy, map_mirror3[yy][xx]);

	End = isVictory3(mystonec);
	if(End)
		return 2; //victory or lose

	End = isDraw3();
	if (End)
		return 6; //draw

	return 1; //continue game
}

int put4(int xx,int yy,char mystonec){
	int End, i,j;
	char temp;

	if(map_mirror4[yy][xx] == 10 || map_mirror4[yy][xx] == 11) //돌이 현 위치에 있음.
		return 0;

	if((yy+1) < 8){ 
		if(map_mirror4[yy+1][xx] == 10 || map_mirror4[yy+1][xx] == 11){ //바닥이 아닐때 돌을 두는 경우 -> 
			map_mirror4[yy][xx] = mystonec;
		//	temp = map_mirror4[yy][xx];
		}
		else
			return 0;
		//}
	}
	if((yy+1) == 8){
		if(map_mirror4[yy][xx] != 10 || map_mirror4[yy][xx] != 11){ // 제일 바닥에 돌을 두는 경우
		//if (mystonec == 10){
		//temp = map_mirror4[yy][xx];
		map_mirror4[yy][xx] = mystonec;
		}
		else 
			return 0;
		//}
	}

	//map_mirror4[yy][xx] = mystonec;
	state = 0;
	print(xx, yy, map_mirror4[yy][xx]);
	End = isVictory4(mystonec);
	if(End) //win
		return 2;

	End = isDraw4();
	if (End)
		return 6;

	return 1; //continue game
}

int put5(int xx,int yy,char mystonec){
	int End, ban, i, j;
	char temp;
	if( map_mirror5[yy][xx]==10 || map_mirror5[yy][xx] == 11)	//stone exist at xy
		return 0;

	ban=six(xx,yy,mystonec);
	if(ban)
		return 3; //if black, lose

	if (mystonec == 10){
		temp = map_mirror5[yy][xx];
		map_mirror5[yy][xx] = mystonec;
		for (i = 0; i < 29; i = i + 2){
			for (j = 0; j < 15; j++){
				if (mystonec == map_mirror5[j][i]){
					ban = threethree(i, j, mystonec);
					if (ban == 1){
						map_mirror5[yy][xx] = temp;
						return 4;//33 rule
					}
					if (ban == 2){
						map_mirror5[yy][xx] = temp;
						return 5;//44 rule
					}
				}
			}
		}
	}

	map_mirror5[yy][xx] = mystonec;
	state = 0;
	print(xx,yy, map_mirror5[yy][xx]);
	End = isVictory5(mystonec);

	if(End)
		return 2; //victory or lose
	End = isDraw5();
	if (End)
		return 6; //draw

	return 1; //continue game
}

void set_up(){
	clear();
	noecho();

	resetMap();
	refresh();
}

int setSocket(){
	int q=1, i, j;
	int clientgame, nullpoint, nullpoint2;
	char ipaddr[20];
	char temp[10];
	while(1){
		clear();
		if(gamecheck == 3)
			mvaddstr(15,0,"                                        you select 3-mok!            ");
		else if(gamecheck == 4)
			mvaddstr(15,0,"                                        you select 4-mok!            ");
		else
			mvaddstr(15,0,"                                        you select 5-mok!            ");
		mvaddstr(16,0,"                                        1. create room            ");
		mvaddstr(17,0,"                                        2. join room              ");
		mvaddstr(18,0,"                                        3. go back                ");
		i = getch();
		switch(i){
		case '1':
				clear();
				echo();
				mvaddstr(15,0,"                                        Port: ");
				refresh();
				getstr(temp);
				port = atoi(temp);
				clear();
				if(gamecheck == 3)
					mvaddstr(15,0,"                                        3-mok room create.            ");
				else if(gamecheck == 4)
					mvaddstr(15,0,"                                        4-mok room create.            ");
				else
					mvaddstr(15,0,"                                        5-mok room create.            ");
				mvaddstr(16,0,"                                        wait for another player             ");
				refresh();
				if((q = open_server(port)) == -1){
					getch();
					break;
				}
				sendXY(gamecheck,0,q);
				sleep(1);
				receiveXY(&nullpoint2, &nullpoint, q);
				if(nullpoint2 == -1){
					clear();
					mvaddstr(15,0,"                                        Opponent join wrong game.                 ");
					refresh();
					sleep(3);
					break;
				}
				clear();
				refresh();
				S = 1;
				win = 0; lose = 0;
				mystone = 10;
				opponentstone = 11;
				noecho();
				print_stat();
				return q;
		case '2':
			clear();
			echo();
			if(gamecheck == 3)
				mvaddstr(15,0,"                                        you select 3-mok!            ");
			else if(gamecheck == 4)
				mvaddstr(15,0,"                                        you select 4-mok!            ");
			else 
				mvaddstr(15,0,"                                        you select 5-mok!            ");
			mvaddstr(16,0,"                                        IP address: ");
			refresh();
			getstr(ipaddr);
			mvaddstr(17,0,"                                        Port: ");
			refresh();
			getstr(temp);
			port = atoi(temp);
			if((q = connect_server(port, ipaddr)) == -1){
				clear();
				mvaddstr(15,0,"                                        already used port or not correct ip address.                    ");
				mvaddstr(16,0,"                                        please try again.                                               ");
				refresh();
				sleep(3);
				noecho();
				break;
			}
			sleep(1);
			receiveXY(&clientgame,&nullpoint,q);
			if(clientgame != gamecheck){
				clear();
				mvaddstr(15,0,"                                        game is not match.                ");
				mvaddstr(16,0,"                                        please try again.                 ");
				sendXY(-1,0,q);
				refresh();
				sleep(3);
				break;
			}
			sendXY(1,0,q);
			S = 0;
			win = 0; lose = 0;
			mystone=11;
			opponentstone=10;
			noecho();
			return q;
		case '3':
			clear();
			echo();
			q=3;
			socketcheck = 1;
			return q;
		}
	}
}

void start(){
	int q=1;
	int i, j, k;
	int introcheck;
	initscr();
	keypad(stdscr, TRUE);
	noecho();
	while(q){
		clear();
		mvaddstr( 0,0,"         ::::::::::            ::::::::::::::::::::::    :::::::::::::   ::::  ::::      :::::::::       :::::");
		mvaddstr( 1,0,"       .::::::::::::.          ::::::::::::::::::::::    :::::::::::::   ::::  ::::    .:::::::::::      :::::");
		mvaddstr( 2,0,"      .::::::::::::::.         ::::::::::::::::::::::    '::::::::::::   ::::  ::::   .:::::::::::::     :::::");
		mvaddstr( 3,0,"      ::::::::::::::::.        :::::            :::::            :::::   ::::  ::::   ::::::'':::::::    :::::");
		mvaddstr( 4,0,"     ::::::'    '::::::        :::::            :::::            :::::   ::::  ::::  :::::'     :::::.   :::::");
		mvaddstr( 5,0,"     :::::        ::::::       :::::           .:::::            :::::...::::  ::::  :::::       :::::   :::::");
		mvaddstr( 6,0,"    :::::'         :::::       :::::.::::::::::::::::            ::::::::::::  ::::  :::::       .::::   :::::");
		mvaddstr( 7,0,"    :::::          :::::       ::::::::::::::::::::::            ::::::::::::  ::::  :::::       .::::   :::::");
		mvaddstr( 8,0,"    :::::          .::::       :::::::::::::::::::''             ::::::::::::  ::::  :::::       .::::   :::::");
		mvaddstr( 9,0,"    ::::.          :::::       ::::::::::::'''                   :::::   ::::  ::::  '::::       :::::   :::::");
		mvaddstr(10,0,"    :::::          :::::                ::::.....::::::          :::::   ::::  ::::   :::::.    :::::'   :::::");
		mvaddstr(11,0,"    :::::         .:::::            ....:::::::::::::::          :::::   ::::  ::::   '::::::::::::::    :::::");
		mvaddstr(12,0,"    ::::::       .:::::'      .::::::::::::::::::::::''          :::::   ::::  ::::    '::::::::::::'    :::::");
		mvaddstr(13,0,"    '::::::     ::::::'       ::::::::::::::::'''                :::::   ::::  ::::     ':::::::::::     :::::");
		mvaddstr(14,0,"    ':::::::::::::::::        ::::::::'''         ...            :::::   ::::  ::::       ':::::::'      :::::");
		mvaddstr(15,0,"     ':::::::::::::::         :'''         ..::::::::             ''''   ::::  ::::         :::::        :::::");
		mvaddstr(16,0,"      ':::::::::::::                ..:::::::::::::::                    ::::  ::::                      :::::");
		mvaddstr(17,0,"       '::::::::::             .:::::::::::::::::::::                    ::::  ::::       ....           '':::");
		mvaddstr(18,0,"         '::::::.              .:::::::::::''   :::::                    ::::  ::::       :::::::::...        ");
		mvaddstr(19,0,"            ::::.              .::::::''        :::::                    ::::  ::::       :::::::::::::::::.. ");
		mvaddstr(20,0,"           .::::.              .''              :::::                    ::::  ::::       ::::::::::::::::::::");
		mvaddstr(21,0,"           .::::.                               :::::                    ::::  ::::       ::::::::::::::::::::");
		mvaddstr(22,0,"           .::::.                               ::''                     ::::  ::::       :::::    '':::::::::");
		mvaddstr(23,0,"           .::::.     ....:                                              ::::  ::::       :::::          :::::");
		mvaddstr(24,0,"           .::::..:::::::::                                              ::::  ::::       :::::          :::::");
		mvaddstr(25,0,"          .::::::::::::::::                                               '''  ::::       :::::          :::::");
		mvaddstr(26,0,"    ..::::::::::::::::::::'                                                     '''       :::::.         :::::");
		mvaddstr(27,0," .:::::::::::::::::::''                                                                   :::::::::..    :::::");
		mvaddstr(28,0," ::::::::::::::::'                                                                        ::::::::::::::::::::");
		mvaddstr(29,0," :::::::::::'                                   1. game start                              '::::::::::::::::::");
		mvaddstr(30,0," ::::::'                                        2. game introduce                             ''::::::::::::::");
		mvaddstr(31,0," ''                                             3. game exit                                      ''':::::::::");
		mvaddstr(32,0,"                                                                                                        ':::::");
		mvaddstr(33,0,"                                                                                                            ':");

		refresh();
		i = getch();
		switch(i){
		case '1':
			clear();
			mvaddstr(15,0,"                                        1. Play Sam mok(3-mok)             ");
			mvaddstr(16,0,"                                        2. Play Sa mok (4-mok)             ");
			mvaddstr(17,0,"                                        3. Play O mok  (5-mok)             ");
			mvaddstr(18,0,"                                             else. go back.                ");
			refresh();
			j = getch();
			switch(j){
			case '1' :
				clear();
				gamecheck = 3;
				q = 0;
				break;
			case '2' :
				clear();
				gamecheck = 4;
				q = 0;
				break;
			case '3' :
				clear();
				gamecheck = 5;
				q = 0;
				break;
			default :
				q = 1;
				break;
			}

			break;
		case '2':
			introcheck = 1;
			while(introcheck){
				clear();
				mvaddstr(15,0,"                                        1. Sammok Introduce(3-mok)           ");
				mvaddstr(16,0,"                                        2. Samok Introduce (4-mok)           ");
				mvaddstr(17,0,"                                        3. Omok Introduce  (5-mok)           ");
				mvaddstr(18,0,"                                               else. go back.                ");
				k = getch();
				switch(k){
				case '1' :
					clear();
					mvaddstr(15,0,"                           1.If you put 3 stones in line, you win.          ");
					mvaddstr(16,0,"                           2.If all the spaces are filled with stones, Draw.");

					mvaddstr(18,0,"                                        PRESS ANY KEY.                      ");
					getch();
					break;
				case '2' :
					clear(); 
					mvaddstr(15,0,"                           1.If you put 4 stones in line, you win.          ");
					mvaddstr(16,0,"                           2.Stones must piled on the stone.                ");
					mvaddstr(17,0,"                           3.If all the spaces are filled with stones, Draw.");

					mvaddstr(19,0,"                                        PRESS ANY KEY.                      ");
					getch();
					break;
				case '3' :
					clear();
					mvaddstr(15,0,"                           1.If you put 5 stones in line, you win.");
					mvaddstr(16,0,"                           2.first player is not allowd 33, 44");
					mvaddstr(17,0,"                           3.second player is allowd 33, 44");
					mvaddstr(18,0,"                           4.If all the spaces are filled with stones, Draw.");

					mvaddstr(20,0,"                                        PRESS ANY KEY.                  ");
					getch();
					break;
				
				default :
					introcheck = 0;
					break;
				}
			}
			break;
		case '3':
			endwin();
			exit(1);
		}
	}
}
