#include <stdio.h>
#include <stdlib.h>
#define N 10

int main(void)
{
	int name[N];                            	//이름 한글자씩 배열에 입력 최대 10자
	int i_name=0;					//글자수 	
	printf("Start....\n");
	printf("Input name : ");
	

	for ( i_name=0; i_name < N ; i_name++) { 	//한글자씩 입력

	scanf("%c", &name[i_name]);

		if( name[i_name] == '\n')		//(enter) 개행 문자를 받을시 탈출
			break;

	}

	return 0;
}
	/* name[N]은 플레이어의 이름을 담은 배열*/
