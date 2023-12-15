//
//  smm_node.c
//  SMMarble
//
//  Created by Juyeop Kim on 2023/11/05.
//

#include "smm_common.h"
#include "smm_object.h"
#include <string.h>

#define MAX_NODETYPE    7
#define MAX_GRADE       9
#define MAX_NODE        100

static char smmNodeName[SMMNODE_TYPE_MAX][MAX_CHARNAME] = {
	"lecture",
	"restaurant",
	"laboratory",
	"home",
	"gotolab",
	"foodchance",
	"festival"
};

char *smmObj_getTypeName (int type)
{
	return (char*)smmNodeName[type];
}
/*유형의 이름을 출력할 수 있는 함수 */

//1. 구조체 형식 정의


//main.c에서 필요할 때 (genObject 부를 때) 이거를 .c에서 .h로 옮기면 main에서 사용 가능 
typedef enum smmObjType {
	smmObjType_board = 0,
	smmObjType_card,
	smmObjType_grade
} smmObjType_e;

typedef enum smmObjGrade {
	smmObjGrade_Ap = 0,
	smmObjGrade_A0,
	smmObjGrade_Am,
	smmObjGrade_Bp,
	smmObjGrade_B0,
	smmObjGrade_Bm,
	smmObjGrade_Cp,
	smmObjGrade_C0,
	smmObjGrade_Cm
} smmObjGrade_e;



typedef struct smmObject{
	char name[MAX_CHARNAME];
	smmObjType_e objtype;
	int type;
	int credit;
	int energy;
	smmObjGrade_e grade;
} smmObject_t;

//2. 구조체 배열 변수 정의 

//static smmObject_t smm_node[MAX_NODE];
 
#if 0 //if 0 - end if 하면 중간에 있는 건 다 지워짐! 
static char smmObj_name[MAX_NODE][MAX_CHARNAME];
static int smmObj_type[MAX_NODE];
static int smmObj_credit[MAX_NODE];
static int smmObj_energy[MAX_NODE];
static int smmObj_noNode=0;
#end if


//object generation
void* smmObj_genObject(char* name, smmObjType_e objtype, smmObjGrade_e type, int type, int credit, int energy)
{
	smmObject_t*ptr;
	
	ptr = (smmObject_t*)malloc(sizeof(smmObject_t));
	
	strcpy (ptr->name, name);
    ptr->objtype = objtype;
    ptr->type = type;
    ptr->credit = credit;
    ptr->energy = energy;
    ptr->grade = grade;
    
    return ptr;
    //strcpy (smm_node[smmObj_noNode].name, name);
    //smm_node[smmObj_noNode].type = type;
    //smm_node[smmObj_noNode].credit = credit;
    //smm_node[smmObj_noNode].energy = energy;
    
    //smmObj_noNode++;
}

char* smmObj_getNodeName(int node_nr)
{
    return smm_node[node_nr].name;
}

int smmObj_getNodeType(int node_nr)
{
    return smm_node[node_nr].type;
}

int smmObj_getNodeCredit(int node_nr)
{
	return smm_node[node_nr].credit;
}

int smmObj_getNodeEnergy(int node_nr)
{
	return smm_node[node_nr].energy;
}
//member retrieving



//element to string
char* smmObj_getNodeName(void* obj)
{
	
	smmObject_t* ptr = (smmObject_t*)obj;
	
	return ptr->name;
    //return smmNodeName[type];
}

char* smmObj_getGradeName(smmGrade_e grade)
{
    return smmGradeName[grade];
}


