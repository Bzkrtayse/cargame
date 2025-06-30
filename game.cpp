#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <ncurses.h>
#include <iostream>
#include <queue>
#include <ctime>
#include <string.h>
#define wHeight 40 // height of the road
#define wWidth 100 // width of the road
#define lineX 45 // x coordinate of the middle line
#define lineLEN 10 // distance of the middle line from the beginning and the end
#define EXITY 35 // coordinate showing the end of the road
#define leftKeyArrow 260 // ASCII code of the left arrow key
#define RightKeyArrow 261 // ASCII code of the right arrow key
#define leftKeyA 97// ASCII code of A
#define RightKeyD 100 // ASCII code of D
#define ESC 27 // // ASCII code of the ESC key
#define ENTER 10 // ASCII code of the ENTER key
#define KEYPUP 259 // ASCII code of the up arrow key
#define KEYDOWN 258 // ASCII code of the down arrow key
#define KEYERROR -1 // ASCII code returned if an incorrect key is pressed
#define SAVEKEY 115 // ASCII code of S
#define levelBound 300 // To increase level after 300 points
#define MAXSLEVEL 5 // maximum level
#define ISPEED 500000 // initial value for game moveSpeed
#define DRATESPEED 100000 // to decrease moveSpeed after each new level
#define MINX 5 // minimum x coordinate value when creating cars
#define MINY 10 // the maximum y coordinate value when creating the cars, then we multiply it by -1 and take its inverse
#define MINH 5 // minimum height when creating cars
#define MINW 5 // minimum width when creating cars
#define SPEEDOFCAR 3 // speed of the car driven by the player
#define YOFCAR 34 // y coordinate of the car used by the player
#define XOFCAR 45 // x coordinate of the car used by the player
#define IDSTART 10 // initial value for cars ID
#define IDMAX 20// maximum value for cars ID
#define COLOROFCAR 3 // color value of the car used by the player
#define POINTX 91 //x coordinate where the point is written
#define POINTY 42 //y coordinate where the point is written
#define MENUX 10 // x coordinate for the starting row of the menus
#define MENUY 5 // y coordinate for the starting row of the menus
#define MENUDIF 2 // difference between menu rows
#define MENUDIFX 20 // difference between menu columns
#define MENSLEEPRATE 200000 // sleep time for menu input
#define GAMESLEEPRATE 250000 // sleep time for player arrow keys
#define EnQueueSleep 1 // EnQueue sleep time
#define DeQueueSleepMin 2 // DeQueue minimum sleep time
#define numOfcolors 4 // maximum color value that can be selected for cars
#define maxCarNumber 5 // maximum number of cars in the queue
#define numOfChars 3 // maximum number of patterns that can be selected for cars
#define settingMenuItem 2 // number of options in the setting menu
#define mainMenuItem 6 // number of options in the main menu
using namespace std;
typedef struct Car{ 
    int ID;
    int x;
    int y;
    int height;
    int width;
    int speed;
    int clr;
    bool isExist;
    char chr;
}Car;
typedef struct Game{
    int leftKey;
    int rightKey;
    queue<Car> cars;
    bool IsGameRunning;
    bool IsSaveCliked;
    int counter;
    pthread_mutex_t mutexFile;
    Car current;
    int level;
    int moveSpeed;
    int points;
}Game;

Game playingGame; // Global variable used for new game
const char *gameTxt =  "game.txt";
const char *CarsTxt =  "cars.txt";
const char *pointsTxt =  "points.txt";
//Array with options for the Setting menu
const char *settingMenu[50] = {"Play with < and > arrow keys","Play with A and D keys"};
//Array with options for the Main menu
const char *mainMenu[50] = {"New Game","Load the last game","Instructions", "Settings","Points","Exit"};
void drawCar(Car c, int type, int direction); //prints or remove the given car on the screen
void printWindow(); //Draws the road on the screen
void *newGame(void *); // manages new game
void initGame(); // Assigns initial values to all control parameters for the new game
void initWindow(); //Creates a new window and sets I/O settings
void printTrees();//ayse
void printMenu();//ayse
void settings();//emine
void instructions();//emine
int randomNum(int min,int max);//ayse
void* enqueue(void*);//ayse
void *dequeue (void*);//emine
void *moveCar(void *data);//ayse
void *printPoint(void*);//emine
void* writePointText(void*);//emine

int main()
{
    while(true){
        initGame();
        srand(time(NULL));
        initWindow();
        printMenu();
   
    pthread_t th1; //create new thread
    pthread_create(&th1, NULL, newGame,NULL);// Run newGame function with thread
    pthread_join(th1, NULL); //Wait for the thread to finish, when the newGame function finishes, the thread will also finish.
 }
       
}
    
void initGame()
{
    playingGame.cars = queue<Car>();
    playingGame.counter =IDSTART;
    playingGame.mutexFile = PTHREAD_MUTEX_INITIALIZER; //assigns the initial value for the mutex
    playingGame.level = 1;
    playingGame.moveSpeed = ISPEED;
    playingGame.points = 0;
    playingGame.IsSaveCliked = false;
    playingGame.IsGameRunning = true;
    playingGame.current.ID = IDSTART-1;
    playingGame.current.height = MINH;
    playingGame.current.width = MINW;
    playingGame.current.speed = SPEEDOFCAR;
    playingGame.current.x = XOFCAR;
    playingGame.current.y = YOFCAR;
    playingGame.current.clr = COLOROFCAR;
    playingGame.current.chr = '*';
}
void *newGame(void *){
    printWindow();
    printTrees();
    pthread_t thenqueue;
    pthread_t thdequeue;
    pthread_t thpoints;
    pthread_create(&thenqueue,NULL,enqueue,NULL);
    pthread_create(&thdequeue,NULL,dequeue,NULL);
    pthread_create(&thpoints,NULL,printPoint,NULL);
    drawCar(playingGame.current,2,1); // Draw the car the player is driving on the screen
    int key;
    while (playingGame.IsGameRunning) { //continue until the game is over
            key = getch(); //Get input for the player to press the arrow keys
            if (key != KEYERROR) {
                 if (key == playingGame.leftKey && playingGame.current.x>4) { // If the left  key is pressed
                        drawCar(playingGame.current,1,1); // removes player's car from screen
                        playingGame.current.x-=playingGame.current.speed; // update position
                        drawCar(playingGame.current,2,1); // draw player's car with new position
                }
                else{
                    drawCar(playingGame.current,1,1); 
                    playingGame.current.x+=0; 
                    drawCar(playingGame.current,2,1); 
                }
                if (key == playingGame.rightKey  && playingGame.current.x<90) {    
                    drawCar(playingGame.current,1,1); 
                    playingGame.current.x+=playingGame.current.speed; 
                    drawCar(playingGame.current,2,1); 
            }
            else{
                drawCar(playingGame.current,1,1); 
                    playingGame.current.x+=0; 
                    drawCar(playingGame.current,2,1); 
            }
         usleep(GAMESLEEPRATE); 
        }
}

pthread_join(thenqueue,NULL);
pthread_join(thdequeue,NULL);
pthread_join(thpoints,NULL);
}
void initWindow()
{  
	initscr();            // initialize the ncurses window
	start_color();        // enable color manipulation
	keypad(stdscr, true); // enable the keypad for the screen
	nodelay(stdscr, true);// set the getch() function to non-blocking mode
	curs_set(0);          // hide the cursor
	cbreak();             // disable line buffering
	noecho();             // don't echo characters entered by the user
	clear();              // clear the screen
    sleep(1);
}
void printWindow()
{
     for (int i = 1; i < wHeight - 1; ++i) {
		//mvprintw: Used to print text on the window, paramters order: y , x , string
        mvprintw(i, 2, "*"); //left side of the road
        mvprintw(i, 0, "*");
        mvprintw(i, wWidth - 1, "*");// right side of the road
        mvprintw(i, wWidth - 3, "*");
    }
    for (int i = lineLEN; i < wHeight -lineLEN ; ++i) { //line in the middle of the road
        mvprintw(i, lineX, "#");
    }
}
void printTrees()
{
    initscr();
    start_color();
    init_pair(1,COLOR_GREEN,COLOR_BLACK);
    init_pair(2,COLOR_RED,COLOR_BLACK);
    for (int i = 5; i < wHeight - 10; i+=10) {

        attron(COLOR_PAIR(1));
        mvprintw(i, wWidth+5, "*");

        mvprintw(i+1, wWidth+4, "*");
        mvprintw(i+1, wWidth+6, "*");

        mvprintw(i+2, wWidth+3, "*");
        mvprintw(i+2, wWidth+5, "*");
        mvprintw(i+2, wWidth+7, "*");
        attroff(COLOR_PAIR(1));
        attron(COLOR_PAIR(2));

        mvprintw(i+3, wWidth+5, "#");

        mvprintw(i+4, wWidth+5, "#");
        attroff(COLOR_PAIR(2));
        refresh();
        getch();
        endwin();
    }
}
void drawCar(Car c, int type, int direction )
{
	//If the user does not want to exit the game and the game continues
    if(playingGame.IsSaveCliked!=true && playingGame.IsGameRunning==true)
    {
            init_pair(c.ID, c.clr, 0);// Creates a color pair: init_pair(short pair ID, short foregroundcolor, short backgroundcolor);
            //0: Black (COLOR_BLACK)
			//1: Red (COLOR_RED)
			//2: Green (COLOR_GREEN)
			//3: Yellow (COLOR_YELLOW)
			//4: Blue (COLOR_BLUE)
			attron(COLOR_PAIR(c.ID));//enable color pair
            char drawnChar;
            if (type == 1 )
               drawnChar = ' '; // to remove car
            else
               drawnChar= c.chr; //  to draw char
		    //mvhline: used to draw a horizontal line in the window
			//shallow. : mvhline(int y, int x, chtype ch, int n)
			//y: horizontal coordinate
			//x: vertical coordinate
			//ch: character to use
			//n: Length of the line
            mvhline(c.y, c.x, drawnChar, c.width);// top line of rectangle
            mvhline(c.y + c.height - 1, c.x, drawnChar, c.width); //bottom line of rectangle
            if(direction == 0) // If it is any car on the road
                mvhline(c.y + c.height, c.x, drawnChar, c.width);
            else //player's card
                mvhline(c.y -1, c.x, drawnChar, c.width);
		    //mvvline: used to draw a vertical line in the window
			//shallow. : mvhline(int y, int x, chtype ch, int n)
			//y: horizontal coordinate
			//x: vertical coordinate
			//ch: character to use
			//n: Length of the line
            mvvline(c.y, c.x, drawnChar, c.height); //left line of rectangle
            mvvline(c.y, c.x + c.width - 1, drawnChar, c.height); //right line of rectangle
            char text[5];
            if (type == 1 )
                sprintf(text,"  "); //to remove point
            else
                 sprintf(text,"%d",c.height * c.width); // to show car's point in rectangle
            mvprintw(c.y+1, c.x +1, text);// display car's point in rectangle
            attroff(COLOR_PAIR(c.ID));// disable color pair
    }
}
void printMenu(){
        initscr();
        start_color();
        init_pair(1,COLOR_GREEN,COLOR_BLACK);
        init_pair(2,COLOR_RED,COLOR_BLACK);
        init_pair(3,COLOR_BLACK,COLOR_BLACK);

        int input;
        int selected=1;

        bool isEnter=false;
            while(!isEnter){
            int y=MENUY;
            for(int i=0;i<mainMenuItem;i++){    
                if(selected==(i+1)){
                    attron(COLOR_PAIR(2));
                    mvprintw(y,MENUX-2,"->");
                    mvprintw(y,MENUX,"%s",mainMenu[i]);
                    attroff(COLOR_PAIR(2));
                }
                else{
                    attron(COLOR_PAIR(3));
                    mvprintw(y,MENUX-2,"->");
                    attroff(COLOR_PAIR(3));

                    attron(COLOR_PAIR(1));
                    mvprintw(y,MENUX,"%s",mainMenu[i]);
                    attroff(COLOR_PAIR(1));
                }
                y+=MENUDIF;
            }
            refresh();
            usleep(MENSLEEPRATE);
            input=getch();
            switch (input)
            {
            case KEYDOWN:
                if(selected==mainMenuItem){
                    selected=mainMenuItem;
                }
                else selected++;
                break;
            case KEYPUP:
                if(selected==1){
                    selected=1;
                }
                else selected--;
                break;
            case ENTER:
                isEnter=true;
                break;
            default:
                break;
            }
    }
        clear();
        switch (selected)
        {
        case 1:
        if( playingGame.leftKey !=leftKeyA &&  playingGame.rightKey!=RightKeyD){
              playingGame.leftKey=leftKeyArrow;
           playingGame.rightKey=RightKeyArrow;
        }
        initGame();
            break;
        case 2:
        printw("case2");
            break;
        case 3:
        instructions();
        printMenu();
            break;
        case 4:
        settings();
        printMenu();
            break;
        case 5:
        printw("case5");
            break;
        case 6:
            exit(1);
        
        default:
            break;
        }
        refresh();
        sleep(2);
  endwin();
}
void settings(){
    initscr();
        start_color();
        init_pair(1,COLOR_GREEN,COLOR_BLACK);
        init_pair(2,COLOR_RED,COLOR_BLACK);
        init_pair(3,COLOR_BLACK,COLOR_BLACK);
        int input;
        int selected=1;
        bool isEnter=false;
            while(!isEnter){
            
            int y=MENUY;
            for(int i=0;i<settingMenuItem;i++){    
                if(selected==(i+1)){
                    attron(COLOR_PAIR(2));
                    mvprintw(y,MENUX-2,"->");
                    mvprintw(y,MENUX,"%s",settingMenu[i]);
                    attroff(COLOR_PAIR(2));
                }
                else{
                    attron(COLOR_PAIR(3));
                    mvprintw(y,MENUX-2,"->");
                    attroff(COLOR_PAIR(3));

                    attron(COLOR_PAIR(1));
                    mvprintw(y,MENUX,"%s",settingMenu[i]);
                    attroff(COLOR_PAIR(1));
                }
                y+=MENUDIF;
            }
            refresh();
            sleep(1);
            input=getch();
            switch (input)
            {
            case KEYDOWN:
                if(selected==settingMenuItem){
                    selected=settingMenuItem;
                }
                else selected++;
                break;
            case KEYPUP:
                if(selected==1){
                    selected=1;
                }
                else selected--;
                break;
            case ENTER:
                isEnter=true;
                break;
            default:
                break;
            }
            
    }
        clear();
        switch (selected)
        {
        case 1:
           playingGame.leftKey=leftKeyArrow;
           playingGame.rightKey=RightKeyArrow;
            
            break;
        case 2:
         playingGame.leftKey=leftKeyA;
         playingGame.rightKey=RightKeyD;
                  break;
               default:
          
            break;
        }
        refresh();
        sleep(5);
        endwin();
}
void instructions(){
     int x =10, y = 5;
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    attron(COLOR_PAIR(1));
    char text[50];
    sprintf(text,"< or A: moves the car to the left");
    mvprintw(y, x, text);
    y+= MENUDIF;
    sprintf(text,"> or D: moves the car to the right");
    mvprintw(y, x, text);
    y+= MENUDIF;
    sprintf(text,"ESC : exists the game without saving");
    mvprintw(y, x, text);
    y+= MENUDIF;
    sprintf(text,"S: saves and exits the game");
    mvprintw(y, x, text);
    attroff(COLOR_PAIR(1));
    refresh();
    initWindow();
	refresh();
}
void* enqueue(void*){
    int id=IDSTART;
    int rnd;
    int randomX1;
    int randomX2;
    while(playingGame.IsGameRunning){
       if(playingGame.cars.size()<5){
        Car newCar;   
        newCar.ID=id;
        newCar.height=randomNum(5,7);   
        newCar.width=randomNum(5,7);
        newCar.speed=newCar.height/2;
        newCar.clr=randomNum(1,4);
        rnd=randomNum(1,2);
        randomX1=randomNum(5,lineX-newCar.width-1);
        randomX2=randomNum(lineX+1,90);
        if(rnd==1){
            newCar.x=randomX1;
        }
        else {
            newCar.x=randomX2;
        }
        newCar.y=randomNum(-10,0);
        newCar.isExist=false;
        int randomShape=randomNum(1,3);
        if(randomShape==1) newCar.chr='*';
        else if(randomShape==2) newCar.chr='#';
        else newCar.chr='+';

        id++;
        playingGame.cars.push(newCar);
        usleep(EnQueueSleep);
        }
        
    }
    return 0;
}
int randomNum(int min,int max){
    int num= rand()% (max-min+1)+min;
    return num;
}
void *dequeue (void*){
    pthread_t arr[10];
    int tcount=0;
    while (playingGame.IsGameRunning) {
     Car *frontcar= new Car(playingGame.cars.front());
        if (!playingGame.cars.empty()) {
            pthread_create(arr,NULL,moveCar,(void*)frontcar);
            tcount++;
            playingGame.cars.pop();        
     }
     int rndsleepTime =randomNum(1,2);
     if(rndsleepTime==1){
        sleep(DeQueueSleepMin);
     }
     else 
     sleep(4);
} if(!playingGame.IsGameRunning){
     for (int i = 0; i < tcount; i++) {
        pthread_join(arr[i], NULL);
    }
}


return NULL;
}
void *moveCar(void *data){
    Car *carOnRoad = (Car *)data;
    while(carOnRoad->y<35){
            drawCar(*carOnRoad,1,0);
            carOnRoad->y+=1+rand()%carOnRoad->speed;
            drawCar(*carOnRoad,2,0);
            usleep(playingGame.moveSpeed);
    }
    if(carOnRoad->y >= 35){
        drawCar(*carOnRoad,1,0);
        playingGame.points+=(carOnRoad->height)*(carOnRoad->width);
    }
    int x1=playingGame.current.x;
    int y1=playingGame.current.y;
    int w1=playingGame.current.width;
    int h1=playingGame.current.height;
    int x2=carOnRoad->x;
    int y2=carOnRoad->y;
    int w2=carOnRoad->width;
    int h2=carOnRoad->height;
   if( x1<x2+w2 && x1+w1>x2 && y1-h1<=y2  ){
        pthread_t th; 
        playingGame.IsGameRunning=false;
        pthread_create(&th,NULL,writePointText,NULL);
        clear();
        initWindow();
        initGame();
        printMenu();
        refresh();
          pthread_join(th,NULL);
  }
    else
    drawCar(*carOnRoad,1,0);
}
void *printPoint(void*){
      while(1){
        int t=levelBound;
        if(playingGame.points >= t && playingGame.level!=MAXSLEVEL){
             playingGame.level++;
            playingGame.moveSpeed -= DRATESPEED;
            mvprintw(POINTY, POINTX,"Point :%d",playingGame.points);
           t+=levelBound;
            } 
            else{ 
              mvprintw(POINTY, POINTX,"Point :%d",playingGame.points);
              refresh();
           }
        }
} 
void* writePointText(void*)
{
    if(playingGame.IsGameRunning==false){
    pthread_mutex_lock(&playingGame.mutexFile);
    FILE *file=fopen(pointsTxt,"a+");
    fprintf(file," %d",playingGame.points);
    pthread_mutex_unlock(&playingGame.mutexFile);
    sleep(1);
    }
}