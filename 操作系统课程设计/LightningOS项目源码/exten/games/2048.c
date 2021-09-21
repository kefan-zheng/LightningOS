/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		               Kefan-Zheng,  2021   		
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#include "type.h"
#include "stdio.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "fs.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "proto.h"
#include "rand.h"

#define NULL ((void*)0)

// Global vals
int anim;
char emptyCell[5];
char cell[4][4][5];
unsigned short score;
int gameEnded;
int displaychanged = 0;
int checkAction = 1;
int if2048start = 0;

// Prototypes
void center(char*);
void init();
void welcomeView();
void endView();
void display();
int isEmpty(char*);
void addRandomValue2048(int);
short mergeable(short,short);
short merge(char*,char*,int);
int doProcess(unsigned short);
void change_checkAction(int);

void center(char *str)
{
    printf("%s\n",str);
}



void init_2048()
{
 // Game coded to learn C

    srand(2048);

    strcpy(emptyCell,"    ");

    anim = 0;
    gameEnded = 0;
    displaychanged = 0;
    checkAction = 1;

    unsigned short x,y;
    for(x=0; x<4; x++)
    {
        for(y=0; y<4; y++)
        {
            strcpy(cell[x][y], emptyCell);
        }
    }

    score = 0; 

    addRandomValue2048(1);
    addRandomValue2048(1);
}


void welcomeView()
{
    center("2048");
    printf("Press 'ENTER' to begin:");

    while(if2048start == 0){}
    printf("OK! Game starts.\n");
}

void changeif2048start(int newif2048start){ if2048start = newif2048start;}

void endView()
{
    gameEnded = 1;
    checkAction = -2;
}

void change_checkAction(int newcheckAction)
{
    checkAction = newcheckAction;
}

void display(){
    printf("================================================================================\n");
    printf("         restart:f5       exit:esc       move digits:up/down/left/right         \n");
    printf("================================================================================\n");
    char str[13] = "Score : ";
    char sc[5];
    sprintf(sc,"%d",score);
    for(int i=8;i<13;i++)
    {
        str[i] = sc[i-8];
    }
    center(str);

    unsigned short x,y;
    if (!gameEnded)
    {
        for (x=0; x<4; x++)
        {
            for(y = 0; y < 4; y++)
            {
                printf("|");
                printf("%s", cell[x][y]);
            }
            printf("|\n");
        }
    }
    center("");
}


int isEmpty(char *cell1)
{
    return strcmp(cell1,emptyCell) ? 0 : 1;
}

void addRandomValue2048(int add)
{
    unsigned short x,y;
    short symb = 0;
    if (add)
    {
        do
        {
            x = rand()%4;
            y = rand()%4;
        } while (!isEmpty(cell[x][y]));

        char str[5];
        sprintf(str, "%4d",(rand()%2+1)*2);
        strcpy(cell[x][y], str);
        int end = 1;
        int fullBoard = 1;
        for(x=0; x<4; x++)
        {
            for(y=0; y<4; y++)
            {
                if (isEmpty(cell[x][y]))
                {
                    fullBoard = 0;
                }
                if (mergeable(x,y) > 0)
                {
                    end = 0;
                }
            }
        }
        symb = 1;
        if (end && fullBoard)
        {
            endView();
            symb = 0;
        }
    }
    displaychanged = symb;
}

short mergeable(short r, short c)
{
    unsigned short i=0;

    if (c > -1)
    {
        if (!isEmpty(cell[0][c]))
        {
            if (!strcmp(cell[0][c],cell[1][c]))
            {
                i++;
            } 
            else if (!strcmp(cell[0][c],cell[2][c]) && isEmpty(cell[1][c]))
            {
                i++;
            } 
            else if (!strcmp(cell[0][c],cell[3][c]) && isEmpty(cell[1][c]) && isEmpty(cell[2][c]))
            {
                i++;
            }
        }

        if (!isEmpty(cell[1][c]) && i == 0)
        {
            if (!strcmp(cell[1][c],cell[2][c]))
            {
                i++;
            } 
            else if (!strcmp(cell[1][c],cell[3][c]) && isEmpty(cell[2][c]))
            {
                i++;
            }
        }

        if (!strcmp(cell[2][c],cell[3][c]) && !isEmpty(cell[2][c]))
        {
            i++;
        }
    }

    if (r > -1)
    {
        if (!isEmpty(cell[r][0]))
        {
            if (!strcmp(cell[r][0],cell[r][1]))
            {
                i++;
            } 
            else if (!strcmp(cell[r][0],cell[r][2]) && isEmpty(cell[r][1]))
            {
                i++;
            } 
            else if (!strcmp(cell[r][0],cell[r][3]) && isEmpty(cell[r][1]) && isEmpty(cell[r][2]))
            {
                i++;
            }
        }

        if (!isEmpty(cell[r][1]) && i == 0)
        {
            if (!strcmp(cell[r][1],cell[r][2]))
            {
                i++;
            } 
            else if (!strcmp(cell[r][1],cell[r][3]) && isEmpty(cell[r][2]))
            {
                i++;
            }
        }

        if (!strcmp(cell[r][2],cell[r][3]) && !isEmpty(cell[r][2]))
        {
            i++;
        }
    }

    return i;
}

short merge(char *cell1, char *cell2, int dontMerge)
{
    if (isEmpty(cell2))
    {   //Nothing
        return -1;
    }

    if (isEmpty(cell1))
    {   //Move
        strcpy(cell1, cell2);
        strcpy(cell2, emptyCell);
        return 0;
    } 
    else if (!strcmp(cell1, cell2) && !dontMerge)
    { //Merge
        strcpy(cell2, emptyCell);

        char str[5];
        sprintf(str, "%4d",atoi(cell1)*2);
        strcpy(cell1, str);
        score += atoi(cell1);
        return 1;
    } 
    else 
    {
        return -1;
    }
}

int doProcess(unsigned short direction){
    short x,y,mergeStatus,nbMerge;
    int redo;
    int done = 0, merged = 0;

    switch (direction)
    {
        case 0:
        for(y=0; y<4; y++)
        {
            redo = 1;
            merged = 0;
            nbMerge = mergeable(-1,y);
            while(redo)
            {
                nbMerge--;
                redo = 0;
                for(x=0; x<3; x++)
                {
                    mergeStatus = merge(cell[x][y],cell[x+1][y], merged);
                    if (mergeStatus > -1){done = 1;}
                    if (mergeStatus == 0){redo = 1;}
                    if (mergeStatus == 1){merged = 1;}
                }
                if (nbMerge > 0)
                {
                    redo=1;
                    merged = 0;
                }
            }
        }
        break;

        case 1:
        for(x=0; x<4; x++)
        {
            redo = 1;
            merged = 0;
            nbMerge = mergeable(x,-1);
            while(redo)
            {
                nbMerge--;
                redo = 0;
                for(y=3; y>0; y--)
                {
                    mergeStatus = merge(cell[x][y],cell[x][y-1], merged);
                    if (mergeStatus > -1){done = 1;}
                    if (mergeStatus == 0){redo = 1;}
                    if (mergeStatus == 1){merged = 1;}
                }
                if (nbMerge > 0)
                {
                    redo=1;
                    merged = 0;
                }
            }
        }
        break;

        case 2:
        for(y=0; y<4; y++)
        {
            redo = 1;
            merged = 0;
            nbMerge = mergeable(-1,y);
            while(redo)
            {
                nbMerge--;
                redo = 0;
                for(x = 3; x>0; x--)
                {
                    mergeStatus = merge(cell[x][y],cell[x-1][y], merged);
                    if (mergeStatus > -1){done = 1;}
                    if (mergeStatus == 0){redo = 1;}
                    if (mergeStatus == 1){merged = 1;}
                    }
                if (nbMerge > 0)
                {
                    redo=1;
                    merged = 0;
                }
            }
        }
        break;

        case 3:
        for(x=0; x<4; x++)
        {
            redo = 1;
            merged = 0;
            nbMerge = mergeable(x,-1);
            while(redo)
            {
                nbMerge--;
                redo = 0;
                for(y=0; y < 3; y++)
                {
                    mergeStatus = merge(cell[x][y],cell[x][y+1], merged);
                    if (mergeStatus > -1){done = 1;}
                    if (mergeStatus == 0){redo = 1;}
                    if (mergeStatus == 1){merged = 1;}
                }
                if (nbMerge > 0)
                {
                    redo=1;
                    merged = 0;
                }
            }
        }
        break;
    }
    return done;
}


PUBLIC int g2048_main(int *fd_stdin)
{
    TESTATASK = 2;
    clear();
    init_2048();
    welcomeView();
    while(1)
    {
        if(checkAction == 0)
        {
            clear();
            init_2048();
            checkAction = 1;
        }
        else if(checkAction == -1)
        {
            break;
        }
        else if(checkAction == -2)
        {
            printf("================================================================================\n");
            center("Sorry, You lost !");
            char str[13] = "Score : ";
            char sc[5];
            sprintf(sc,"%d",score);
            for(int i=8;i<13;i++)
            {
                str[i] = sc[i-8];
            }
            printf("Final ");
            center(str);

            unsigned short x,y;
            for (x=0; x<4; x++)
            {
                for(y = 0; y < 4; y++)
                {
                    printf("|");
                    printf("%s", cell[x][y]);
                }
                printf("|\n");
            }
            center("");
            center("Press esc/f5 to exit/restart.");
            while(checkAction == -2){}
        }
        else if(checkAction == 1)
        {
            if(displaychanged == 1 )
            {
                display();
                displaychanged = 0;
            }
        }
    }
    TESTATASK = 0;
    return 0;
}
