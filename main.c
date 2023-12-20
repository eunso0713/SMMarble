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
	int position; //player's position
	char name[MAX_CHARNAME];
	int accumCredit; //������ ����: ������ ���ؼ�
	int flag_graduate; //�÷��̾ ������ �� �ִ� �������� Ȯ��: 1 - graduated, 0 - yet
	int inLab; //������ ���� Ȯ�� 
} player_t;

static player_t *cur_player;
//static player_t cur_player[MAX_PLAYER];

#if 0
static int player_energy[MAX_PLAYER];
static int player_position[MAX_PLAYER];
static char player_name[MAX_PLAYER][MAX_CHARNAME];
#endif


//function prototypes

int isGraduated(int player)
{
	if (cur_player[player].accumCredit >= GRADUATE_CREDIT)
		{
			cur_player[player].flag_graduate = 1;
		}
	else
		cur_player[player].flag_graduate = 0;
		
	return cur_player[player].flag_graduate;
	//if any player's accumCredit > GRADUATE_CREDIT: that player is Graduated
} //check if any player is graduated




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


void printPlayerStatus()
{
	int i;
printf("Printing Player Info .. Total %d players:\n", player_nr);
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
void* findGrade(int player, char *lectureName)
void printGrades(int player); //print all the grade history of the player
*/


int checkLecture(int list_nr, char *lectureName)
{
	int index = 0;
	int length = smmdb_len(list_nr);
	
	for (index = 0; index < length; index++)
	{
		if(strcmp(smmObj_getNodeName(smmdb_getData(list_nr, index)), lectureName) == 1)
			return index;
	}
	
	return -1;
}
 //find the grade from the player's grade history


void generatePlayers(int n, int initEnergy) //generate a new player
{
	int i;
	//n time loop
	for (i=0; i<n; i++)
	{
		//input name
		printf("���� �̸��� �Է��ϼ���.\n"); //�ȳ����� 
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
    printf(" Press any key to roll a dice (press g to see grade): \n");
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
	int labPosition = checkLecture(LISTNO_NODE,"���ڰ��н����" );//�� ������ ���� ���� ���ؾ� �Ǵ� �� �ƴѰ�??? �װ� ���� ������  
	int randomFoodCard;
	char *CardName;
	int randomFestCard;
	int answer;
	

    switch(type)
    {
        //case lecture:
        case SMMNODE_TYPE_LECTURE:

        		if(cur_player[player].energy < smmObj_getNodeEnergy(boardPtr)){
					printf("�������� �����մϴ�.\n");
					
        			break;
				}
        		if(checkLecture(LISTNO_OFFSET_GRADE + player, smmObj_getNodeName(boardPtr)) > -1){
        			printf("�̹� ������ �����Դϴ�.\n");
					break;
				} 

				
				printf("�����Ͻðڽ��ϱ�? Y: 1, N: press any key - \n");
				scanf("%d", &answer);
				if (answer == 1)
				{
					cur_player[player].accumCredit += smmObj_getNodeCredit(boardPtr);
        			cur_player[player].energy -= smmObj_getNodeEnergy(boardPtr);
        			//grade generation 
       				gradePtr = smmObj_genObject(smmObj_getNodeName(boardPtr), smmObjType_grade, 0, smmObj_getNodeCredit(boardPtr), 0, rand()%9);
       				smmdb_addTail(LISTNO_OFFSET_GRADE + player, gradePtr);
				}
				else
					("���� ����...\n");
        		
        	break;
        	
        case SMMNODE_TYPE_RESTAURANT:
        	cur_player[player].energy += smmObj_getNodeEnergy(boardPtr);
        	break;
        	
        case SMMNODE_TYPE_LABORATORY:
        	break;
        
        case SMMNODE_TYPE_HOME:
        	cur_player[player].energy += smmObj_getNodeEnergy(boardPtr);
        	//If player is on Home Node; Player earns energy
        	break;
        
        case SMMNODE_TYPE_GOTOLAB:
        	cur_player[player].position = labPosition; // player goes to Lab Node
        	cur_player[player].inLab = 1; //player is in the lab time
        	break;
        	
        case SMMNODE_TYPE_FOODCHANCE:
        	
			randomFoodCard = rand()%smmdb_len(LISTNO_FOODCARD);
      		CardName = smmObj_getNodeName(smmdb_getData(LISTNO_FOODCARD, randomFoodCard));
	    	cur_player[player].energy += smmObj_getNodeEnergy(smmdb_getData(LISTNO_FOODCARD, randomFoodCard));
        	
        	printf("%s�� ���õǾ����ϴ�. %d ��ŭ�� �������� �����մϴ�.\n", CardName, smmObj_getNodeEnergy(smmdb_getData(LISTNO_FOODCARD, randomFoodCard)));
        	break;
        case SMMNODE_TYPE_FESTIVAL:
        	randomFestCard = rand()%smmdb_len(LISTNO_FESTCARD);
        	printf("%s\n", smmObj_getNodeName(smmdb_getData(LISTNO_FESTCARD, randomFestCard)));
        	
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
        void *boardObj = smmObj_genObject(name, smmObjType_board, type, credit, energy, 0);/*������ ���忡 ǥ�� ���ؼ� �ƹ��ų� �ֱ� �ϴ� 0����*/
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
        void *festObj = smmObj_genObject(name, smmObjType_card, 0, 0, 0, 0);/*������ ���忡 ǥ�� ���ؼ� �ƹ��ų� �ֱ� �ϴ� 0����*/
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
  		scanf("%d", &player_nr);
  		fflush(stdin);
    }
    while (player_nr<0 || player_nr > MAX_PLAYER);

    cur_player = (player_t*)malloc(player_nr*sizeof(player_t));
    
    generatePlayers(player_nr, initEnergy);
    

    


    //3. SM Marble game starts ---------------------------------------------------------------------------------
    while (1) //is anybody graduated?
    {
        int dice_result;
        int outLabNo;

        
        //4-1. initial printing
printf("wowwwwwwwwww %d\n", player_nr);
        printPlayerStatus();
printf("wwwwwwwwwwwwwwwwwwwwwwow\n");
        if (cur_player[turn].inLab > 0){
        	outLabNo = rand()%MAX_DICE+1;
        	
        	printf("%s�� �������Դϴ�. \n", cur_player[turn].name);
	        printf("Lab exodus no is. %d !!", outLabNo);
	        if (rolldice(turn) >= outLabNo){
	        	cur_player[turn].inLab = 0;
	        	printf("Lab Time End!!");
			}
			else
	        	printf("More Lab Time Plz!!");
		}
		else
		{ //Normal Process

	        //4-2. die rolling (if not in experiment)
	        dice_result = rolldice(turn);
	        
	        //4-3. go forward
	        goForward(turn, dice_result);
	        //node ���� üũ�ϰ� dice����ŭ �̵��ϱ�: ������ node �������� home���� ��ȯ 
	        //home���� ���� �� graduate credit check�ϴ� function ȣ��
			//isGraduated() == 1�̸� ���� ���� 
			//isGraduated() == 0�̸�  actionNode�� �Ѿ�� 
			
			
			//4-4. take action at the destination node of the board
	        actionNode(turn);
	        //home node�� ���� energy +18
			//lecture node�� ���� energy �Ұ� credit ����; energy ������ ��������; ���� �����ϸ� grade print
			//����ð� node�� ���� lab���� �̵��ϱ�: 3 turn pass�ϱ�
			//cafe; burger�� ���� �ش� node�� energy ��ŭ energy ����
			//���Ľð� node�� ���� food card �̰� �ش� card energy ��ŭ energy ���� 
			//festival node�� ���� festival card �̰� �ش� mission �����ϱ�

		}
        
        //4-5. next turn
        turn ++;
        if (turn == player_nr) turn = 0;
    }
    
    
    free(cur_player);

    system("PAUSE");
    return 0;
	
}
