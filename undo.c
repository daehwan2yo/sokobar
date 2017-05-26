//전역변수로 3차원 배열을 만들어야함
#define undo_num_freze 6                       //최대 5번 실행하기 위해서 1개의 여유의 배열을 더 생성

char undo[undo_num_freze][height][width] ={0} ;    //undo_num_freze는 5개의 2차원 배열형성, 모든 배열을 0으로 초기화
int undo_num ;                               //undo_num을 전역변수로 설정하


void undo_scan(map_file[height][width])           //맵 파일을 5번 기억 하는 함수 (움직일때마다 사용 - 이 전 5번의 움직임을 저장)
{
   undo[5][height][width] = undo[4][height][width];
   undo[4][height][width] =  undo[3][height][width];
   undo[3][height][width] =  undo[2][height][width];
   undo[2][height][width] =  undo[1][height][width];
   undo[1][height][width] =  undo[0][height][width];
   undo[0][height][width] = map_file[height][width];

}


//switch문에서 case 'u'일때 사용하는 함수   - 저장된 이전 5번의 배열을 앞으로 땡기는 작업
char undo_print(map_file[height][width])
{
    if (undo[0][height][width] == 0) {
      printf("undo명령은 최대 5번만 가능합니다.");
      return map_file[height][width];
    }

    map_file[height][width] = undo[0][height][width];
    undo[0][height][width] =  undo[1][height][width];
    undo[1][height][width] =  undo[2][height][width];
    undo[2][height][width] =  undo[3][height][width];
    undo[3][height][width] =  undo[4][height][width];
    undo[4][height][width] =  undo[5][height][width];

   undo[5][height][width] = {0};


    return map_file[height][width];
}
