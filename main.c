//
//  main.c
//  SMMarble
//
//  Created by Eunso Cho on 2023/11/05
//
//

#include <time.h>
#include <string.h>
#include "smm_object.h"
#include "smm_database.h"
#include "smm_common.h"

#define BOARDFILEPATH "marbleBoardConfig.txt"
#define FOODFILEPATH "marbleFoodConfig.txt"
#define FESTFILEPATH "marbleFestivalConfig.txt"

#define MAX_NODE		100
//board configuration parameters
static int board_nr;
static int food_nr;
static int festival_nr;
static int player_nr;

typedef struct player {
	int energy;
	int position;
	char name[MAX_CHARNAME];
	int accumCredit; //누적된 학점: 졸업을 위해서
	int flag_graduate; //플레이어가 졸업할 수 있는 상태인지 확인
	 
} player_t;

static player_t *cur_player;
//static player_t cur_player[MAX_PLAYER];

#if 0
static int player_energy[MAX_PLAYER];
static int player_position[MAX_PLAYER];
static char player_name[MAX_PLAYER][MAX_CHARNAME];
#endif


//function prototypes
#if 0
int isGraduated(void)
{
	//if any player's accumCredit > GRADUATE_CREDIT: that player is Graduated
} //check if any player is graduated

void printGrades(int player)
{
	//print the grades of the lectures that players have heard
} //print grade history of the player
#endif



void goForward(int player, int step)
{
	void *boardPtr;
	cur_player[player].position += step;
	boardPtr = smmdb_getData(LISTNO_NODE, cur_player[player].position);
	
	printf("%s go to node %i (name: %s)\n",
		cur_player[player].name, cur_player[player].position,
		smmObj_getNodeName(boardPtr));
} //make player go "step" steps on the board (check if player is graduated)


void printGrades(int player)
{
	int i;
	void *gradePtr;
	for (i=0;i<smmdb_len(LISTNO_OFFSET_GRADE + player);i++)
	{
		gradePtr = smmdb_getData(LISTNO_OFFSET_GRADE + player, i);
		printf("%s: %i\n", smmObj_getNodeName(gradePtr), smmObj_getNodeGrade(gradePtr));
	}
}


void printPlayerStatus(void)
{
	int i;
	
	for (i=0; i<player_nr; i++)
	{
		printf("%s: credit %i, energt %i, position %i\n",
		cur_player[i].name,
		cur_player[i].accumCredit,
		cur_player[i].energy,
		cur_player[i].position );
	}
}
 //print all player status at the beginning of each turn
 
 /*
float calcAverageGrade(int player); //calculate average grade of the player
smmGrade_e takeLecture(int player, char *lectureName, int credit); //take the lecture (insert a grade of the player)
void* findGrade(int player, char *lectureName); //find the grade from the player's grade history
void printGrades(int player); //print all the grade history of the player
*/

void generatePlayers(int n, int initEnergy) //generate a new player
{
	int i;
	//n time loop
	for (i=0; i<n; i++)
	{
		//input name
		printf("유저 이름을 입력하세요.\n"); //안내문구 
		//scanf("%s", player_name[i][0]);
		scanf("%s", cur_player[i].name);
		fflush(stdin);
		//set position
		//player_position[i] = 0;
		cur_player[i].position = 0;
		
		//set energy
		//player_energy[i] = initEnergy; 
		cur_player[i].energy = initEnergy;
		cur_player[i].accumCredit = 0;
		cur_player[i].flag_graduate = 0;
	}
}

	


int rolldice(int player)
{
    char c;
    printf(" Press any key to roll a dice (press g to see grade): ");
    c = getchar();
    fflush(stdin);
    

    if (c == 'g')
        printGrades(player);

    
    return (rand()%MAX_DICE + 1);
}


//action code when a player stays at a node
void actionNode(int player)
{
	void *boardPtr = smmdb_getData(LISTNO_NODE, cur_player[player].position);
	//int type = smmObj_getNodeType(cur_player[player].position); 
	int type = smmObj_getNodeType(boardPtr);
	char *name = smmObj_getNodeName(boardPtr);
	void *gradePtr;
	
    switch(type)
    {
        //case lecture:
        case SMMNODE_TYPE_LECTURE:
        	if(1)////////////////////////////////여기 if에는 뭐가 들어가야될까 
				cur_player[player].accumCredit += smmObj_getNodeCredit(boardPtr);
        		cur_player[player].energy -= smmObj_getNodeEnergy(boardPtr);
        	
        	//grade generation 
        	gradePtr = smmObj_genObject(name, smmObjType_grade, 0, smmObj_getNodeCredit(boardPtr), 0, 0);
        	smmdb_addTail(LISTNO_OFFSET_GRADE + player, gradePtr);
        	
			 
        	break;
        
        default:
            break;
    }
}



int main(int argc, const char * argv[])
{
    
    FILE* fp;
    char name[MAX_CHARNAME];
    int type;
    int credit;
    int energy;
    int i;
    int initEnergy;
    int turn = 0;
    
    board_nr = 0;
    food_nr = 0;
    festival_nr = 0;
    
    srand(time(NULL));
    
    
    //1. import parameters ---------------------------------------------------------------------------------
    //1-1. boardConfig 
    if ((fp = fopen(BOARDFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", BOARDFILEPATH);
        getchar();
        return -1;
    }
    
    printf("Reading board component......\n");
    while ( fscanf(fp, "%s %i %i %i", name, &type, &credit, &energy) == 4 ) //read a node parameter set
    {
        //store the parameter set
        void *boardObj = smmObj_genObject(name, smmObjType_board, type, credit, energy, 0);/*성적값 보드에 표시 안해서 아무거나 넣기 일단 0으로*/
        smmdb_addTail(LISTNO_NODE, boardObj);
		  
        if (type == SMMNODE_TYPE_HOME)
        	initEnergy = energy;
		
		board_nr++;
    }
    fclose(fp);
    printf("Total number of board nodes : %i\n", board_nr);
    
    for (i = 0;i<board_nr;i++)
    {
    	void *boardObj = smmdb_getData(LISTNO_NODE, i);
    	
        printf("node %i : %s, %i(%s), credit %i, energy %i\n",
			i, smmObj_getNodeName(boardObj),
			smmObj_getNodeType(boardObj), smmObj_getTypeName(smmObj_getNodeType(boardObj)),
			smmObj_getNodeCredit(boardObj), smmObj_getNodeEnergy(boardObj));
		 
	}
    

    //2. food card config 
    if ((fp = fopen(FOODFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", FOODFILEPATH);
        return -1;
    }
    
    printf("\n\nReading food card component......\n");

    
    while ( fscanf(fp, "%s %i", name, &energy) == 2 ) //read a food parameter set
    {
        //store the parameter set
        void *foodObj = smmObj_genObject(name, smmObjType_card, 0, 0, energy, 0);
        smmdb_addTail(LISTNO_FOODCARD, foodObj);
		  
        
		
		food_nr++;
    }
    fclose(fp);

    
    printf("Total number of food cards : %i\n", food_nr);
    
    
    
    //3. festival card config 
    if ((fp = fopen(FESTFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", FESTFILEPATH);
        return -1;
    }
    
    printf("\n\nReading festival card component......\n");


    while ( fscanf(fp, "%s", name) == 1 ) //read a festival card string
    {
        //store the parameter set
        void *festObj = smmObj_genObject(name, smmObjType_card, 0, 0, 0, 0);/*성적값 보드에 표시 안해서 아무거나 넣기 일단 0으로*/
        smmdb_addTail(LISTNO_FESTCARD, festObj);
		  
        
		festival_nr++;
    }
    fclose(fp);
    

    printf("Total number of festival cards : %i\n", festival_nr);
    

    
    //2. Player configuration ---------------------------------------------------------------------------------
    
    do
    {
        //input player number to player_nr
        printf("input player No. :");
  		scanf("%d", player_nr);
  		fflush(stdin);
    }
    while (player_nr<0 || player_nr > MAX_PLAYER);
    
    cur_player = (player_t*)malloc(player_nr*sizeof(player_t));
    
    generatePlayers(player_nr, initEnergy);
    
    


    //3. SM Marble game starts ---------------------------------------------------------------------------------
    while (1) //is anybody graduated?
    {
        int dice_result;
        
        
        //4-1. initial printing
        printPlayerStatus();
        
        //4-2. die rolling (if not in experiment)
        dice_result = rolldice(turn);
        
        //4-3. go forward
        goForward(turn, dice_result);
        //node 순서 체크하고 dice값만큼 이동하기: 마지막 node 지나가면 home으로 귀환 
        //home으로 지나갔을 때 graduate credit check하는 function 호출 //home+6까지 확인해야되는걸까?? 
		//graduate 안했으면 action 하기 
		
		
		//4-4. take action at the destination node of the board
        actionNode(turn);
        //home node에 가면 energy +18
		//lecture node에 가면 energy 잃고 credit 증가; energy 없으면 수강포기; 수강 성공하면 grade print
		//실험시간 node에 가면 lab으로 이동하기: 3 turn pass하기
		//cafe; burger에 가면 해당 node의 energy 만큼 energy 증가
		//간식시간 node에 가면 food card 뽑고 해당 card energy 만큼 energy 증가 
		//festival node에 가면 festival card 뽑고 해당 mission 수행하기

        
        //4-5. next turn
        turn ++;
        if (turn == player_nr) turn = 0;
    }
    
    
    free(cur_player);

    system("PAUSE");
    return 0;
	
}
