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
	int accumCredit; //누적된 학점: 졸업을 위해서
	int flag_graduate; //플레이어가 졸업할 수 있는 상태인지 확인: 1 - graduated, 0 - yet
	int inLab; //실험중 상태 확인 
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
	if (cur_player[player].position == 0 && cur_player[player].accumCredit >= GRADUATE_CREDIT)
		{
			cur_player[player].flag_graduate = 1;
		}
	else
		cur_player[player].flag_graduate = 0;
		
	return cur_player[player].flag_graduate;
	//if any player is at Home Node and her accumCredit > GRADUATE_CREDIT: that player is Graduated
} //check if any player is graduated




void goForward(int player, int step)
{
	void *boardPtr;
	
	if((cur_player[player].position + step) >= smmdb_len(LISTNO_NODE))
	{
		cur_player[player].position -= smmdb_len(LISTNO_NODE);
	}
	
	cur_player[player].position += step;
	boardPtr = smmdb_getData(LISTNO_NODE, cur_player[player].position);
	
	printf("%s 가 %i로 이동합니다. (노드 이름: %s)\n\n\n",
		cur_player[player].name, cur_player[player].position,
		smmObj_getNodeName(boardPtr));
		
		
	
} //make player go "step" steps on the board


void printGrades(int player)
{
	int i;
	void *gradePtr;
//	void *lectureGradePtr;
//printf("PRINT GRADES!! of player %d\n", player);
	for (i=0;i<smmdb_len(LISTNO_OFFSET_GRADE + player);i++)
	{
		gradePtr = smmdb_getData(LISTNO_OFFSET_GRADE + player, i);
		
		
		// smmObj_getGradeName 함수에서 반환된 문자열의 앞 두 글자 가져오기
        char *gradeName = smmObj_getGradeName(smmObj_getNodeGrade(gradePtr));
        char firstTwoChars[3]; // 끝에 NULL 문자('\0')를 고려하여 크기를 3으로 지정
        strncpy(firstTwoChars, gradeName, 2);
        firstTwoChars[2] = '\0'; // NULL 문자 추가하기
		printf("%s: %d학점, 성적: %s\n", smmObj_getNodeName(gradePtr), smmObj_getNodeCredit(gradePtr), firstTwoChars);
     
	//	printf("%s: %s\n", smmObj_getNodeName(gradePtr), smmObj_getGradeName(smmObj_getNodeGrade(gradePtr)));
	}
/*	lectureGradePtr = smmdb_getData(LISTNO_OFFSET_GRADE + player, rand()%9);
	printf("%s: %s\n", smmObj_getNodeName(lectureGradePtr), smmObj_getGradeName(smmObj_getNodeGrade(lectureGradePtr)));
*/
}
//print the lecture names this player has taken and the grades of that lectures

void printPlayerStatus()
{
	int i;
//printf("Printing Player Info .. Total %d players:\n", player_nr);
	for (i=0; i<player_nr; i++)
	{
		printf("%s: 누적 학점 %i, 에너지 %i, 위치 %i, 실험 중 상태(0: 실험 중 아님, 1: 실험 중) %i\n\n\n",
		cur_player[i].name,
		cur_player[i].accumCredit,
		cur_player[i].energy,
		cur_player[i].position,
		cur_player[i].inLab );
	}
}
 //print all player status at the beginning of each turn: name, accumulated credit, energy, now position, whether or not this player is in Lab time
 
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
	
//printf("lecture name is %s\n", lectureName);
	for (index = 0; index < length; index++)
	{
		printf("비교: %s , %s\n\n", smmObj_getNodeName(smmdb_getData(list_nr, index)), lectureName);
		if(strcmp(smmObj_getNodeName(smmdb_getData(list_nr, index)), lectureName) == 0){
			printf("Index is %d\n", index);
			return index;
		}
	}
	
	return -1;
}
 //check if this player has taken this lecture before, by searching the lecture name on the list


void generatePlayers(int n, int initEnergy) //generate a new player
{
	int i;
	//n time loop
	for (i=0; i<n; i++)
	{
		//input name
		printf("유저 이름을 입력하세요.\n\n"); //안내문구 
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
    int dice_no;
    printf("%d 번째 Player의 차례입니다! 주사위를 굴리기 위해 아무 키나 눌러주세요. (성적을 보려면 g를 입력하세요): \n\n", player+1);
    c = getchar();
    fflush(stdin);
    

    if (c == 'g')
        printGrades(player);

    dice_no = (rand()%MAX_DICE + 1);
	printf("주사위 숫자는 %d 입니다.\n", dice_no);
    return dice_no;
}
//roll the dice and if you press 'g': show the grades by 'printGrades(player)' function


//action code when a player stays at a node
void actionNode(int player)
{
	void *boardPtr = smmdb_getData(LISTNO_NODE, cur_player[player].position);
	//int type = smmObj_getNodeType(cur_player[player].position); 
	int type = smmObj_getNodeType(boardPtr);
	char *name = smmObj_getNodeName(boardPtr);
	void *gradePtr;
	int labPosition;
	int randomFoodCard;
	char *CardName;
	int randomFestCard;
	int answer;
	
	
	

    switch(type)
    {

//printf("type is %d \n\n\n\n\n", type);

        //case lecture:
        case SMMNODE_TYPE_LECTURE:

        		if(cur_player[player].energy < smmObj_getNodeEnergy(boardPtr)){
					printf("에너지가 부족합니다.\n\n");
					
        			break;
				}//if player doesn't have enough energy to take lecture: break
        		if(checkLecture(LISTNO_OFFSET_GRADE + player, smmObj_getNodeName(boardPtr)) > -1){
        			printf("이미 수강한 강의입니다.\n\n");
					break;
				} //if player has taken this lecture before: break
				//신호및시스템이 자꾸 수강 안했는데 수강했다고 뜸 
				//통신이론은 수강 했는데 재수강이됨;; 

				
				printf("수강하시겠습니까? Y: 1, N: press any key - \n"); //수강 의사 묻기 
				scanf("%d", &answer);
				fflush(stdin);
				if (answer == 1)
				{
					cur_player[player].accumCredit += smmObj_getNodeCredit(boardPtr);
        			cur_player[player].energy -= smmObj_getNodeEnergy(boardPtr);
        			//grade generation 
       				gradePtr = smmObj_genObject(smmObj_getNodeName(boardPtr), smmObjType_grade, 0, smmObj_getNodeCredit(boardPtr), cur_player[player].position, rand()%9);
       				smmdb_addTail(LISTNO_OFFSET_GRADE + player, gradePtr);
       				printGrades(player); //플레이어의 성적표 출력하기 
       				break;
				}
				else
					("수강 포기...\n\n");
        		//check if player wants to take this lecture -> 'y': credit+, energy-, give grade
        	break;
        	
        case SMMNODE_TYPE_RESTAURANT:
        	cur_player[player].energy += smmObj_getNodeEnergy(boardPtr);
        	printf("%d 만큼의 에너지를 보충합니다. \n\n", smmObj_getNodeEnergy(boardPtr));
        	break;
        //식당 노드의 경우 해당 노드의 에너지 만큼 보충 
        	
        case SMMNODE_TYPE_LABORATORY:
        	break;
        //실험실은 그냥 지나가기. 실험시간으로 왔을 경우는 main 함수에서 구현 
        
        case SMMNODE_TYPE_HOME:
        	cur_player[player].energy += smmObj_getNodeEnergy(boardPtr);
        	//If player is at Home Node; Player earns energy
        	break;
        
        case SMMNODE_TYPE_GOTOLAB:
        	labPosition = checkLecture(LISTNO_NODE,"전자공학실험실");
        	cur_player[player].position = labPosition; // player goes to Lab Node
        	cur_player[player].inLab = 1; //player is in the lab time
        	break;
        	
        case SMMNODE_TYPE_FOODCHANCE:
			randomFoodCard = rand()%smmdb_len(LISTNO_FOODCARD);
      		CardName = smmObj_getNodeName(smmdb_getData(LISTNO_FOODCARD, randomFoodCard));
	    	cur_player[player].energy += smmObj_getNodeEnergy(smmdb_getData(LISTNO_FOODCARD, randomFoodCard));
        	
        	printf("%s가 선택되었습니다. %d 만큼의 에너지를 보충합니다.\n\n", CardName, smmObj_getNodeEnergy(smmdb_getData(LISTNO_FOODCARD, randomFoodCard)));
        	break;
        	
        case SMMNODE_TYPE_FESTIVAL:
        	randomFestCard = rand()%smmdb_len(LISTNO_FESTCARD);
        	printf("%s\n\n", smmObj_getNodeName(smmdb_getData(LISTNO_FESTCARD, randomFestCard)));
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
    
    printf("Reading board component......\n\n");
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
    printf("Total number of board nodes : %i\n\n", board_nr);
    
    for (i = 0;i<board_nr;i++)
    {
    	void *boardObj = smmdb_getData(LISTNO_NODE, i);
    	
        printf("node %i : %s, %i(%s), credit %i, energy %i\n\n",
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
    
    printf("\n\nReading food card component......\n\n");

    
    while ( fscanf(fp, "%s %i", name, &energy) == 2 ) //read a food parameter set
    {
        //store the parameter set
        void *foodObj = smmObj_genObject(name, smmObjType_card, 0, 0, energy, 0);
        smmdb_addTail(LISTNO_FOODCARD, foodObj);
		
		
        
		
		food_nr++;
    }
    fclose(fp);

    
    printf("Total number of food cards : %i\n\n", food_nr);
    
    
    
    //3. festival card config 
    if ((fp = fopen(FESTFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", FESTFILEPATH);
        return -1;
    }
    
    printf("\n\nReading festival card component......\n\n");


    while ( fscanf(fp, "%s", name) == 1 ) //read a festival card string
    {
        //store the parameter set
        void *festObj = smmObj_genObject(name, smmObjType_card, 0, 0, 0, 0);/*성적값 보드에 표시 안해서 아무거나 넣기 일단 0으로*/
        smmdb_addTail(LISTNO_FESTCARD, festObj);
		  
        
		festival_nr++;
    }
    fclose(fp);
    

    printf("Total number of festival cards : %i\n\n", festival_nr);
    

    
    //2. Player configuration ---------------------------------------------------------------------------------
    do
    {
        //input player number to player_nr
        printf("플레이어 수를 입력하세요 :");
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

        printPlayerStatus();

        if (cur_player[turn].inLab > 0){
        	outLabNo = rand()%MAX_DICE+1;
        	cur_player[turn].energy -= smmObj_getNodeEnergy(smmdb_getData(LISTNO_NODE, SMMNODE_TYPE_LABORATORY));
        	printf("%s는 실험중입니다. \n\n", cur_player[turn].name);
	        printf("Lab exodus no is. %d !!\n\n", outLabNo);
	        if (rolldice(turn) >= outLabNo){
	        	cur_player[turn].inLab = 0;
	        	printf("실험이 종료되었습니다!!\n\n");
			}
			else
	        	printf("조금 더 실험해주세요!! \n\n");
		}
		else
		{ //Normal Process

	        //4-2. die rolling (if not in experiment)
	        
	        dice_result = rolldice(turn);
	        
	        //4-3. go forward
	        goForward(turn, dice_result);
	        //node 순서 체크하고 dice값만큼 이동하기: 마지막 node 지나가면 home으로 귀환 
	        
	        //graduate credit check하는 function 호출
			if(isGraduated(turn) == 1)
				break;
			//isGraduated() == 1이면 게임 종료 
			//isGraduated() == 0이면  actionNode로 넘어가기 
			
			//4-4. take action at the destination node of the board
	        actionNode(turn);
	        //home node에 가면 energy+
			//lecture node에 가면 energy-, credit+; energy 없으면 수강포기; 수강 성공하면 grade print
			//실험시간 node에 가면 lab으로 이동하기: 주사위 값이 특정 랜덤 값 이상이 나오면 탈출 
			//cafe, burger에 가면 해당 node의 energy 만큼 energy+
			//간식시간 node에 가면 food card 뽑고 해당 card energy 만큼 energy+
			//festival node에 가면 festival card 뽑고 해당 mission 수행하기

		}
        
        //4-5. next turn
        
        turn ++;
        if (turn == player_nr) turn = 0;
        
    }
    
    
    //게임 종료 후 종료 안내 문구 출력 
    printf("\n\n\n\n\n\nGame End!!!!!!!!!\n\n\n\n\n");
    
    //플레이어 별 성적표(강의 이름, 학점, 성적) 출력 
    for (i=0;i<player_nr;i++)
	{
		printf("player %d의 성적표\n\n", i+1);
    	printGrades(i);
    	printf("\n\n\n\n");
	}
    
    free(cur_player);

    system("PAUSE");
    return 0;
	
}
