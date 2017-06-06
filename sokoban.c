#include <stdio.h>
#include <termio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define name_max_length 10  //이름의 최대 길이
#define map_height  13  //맵의 최대 세로 길이
#define map_width 24  //맵의 최대 가로 길이 + 1
#define max_hole 21  //맵의 최대 홀의 개수 + 1
#define map_max_number 5// 맵의 최대 개수

//입력 관련 함수
int getch(void); //엔터 없이 입력하게 하기 위한 함수

//이름 관련 함수
int scan_name(char name[name_max_length]);   //이름을 적기 위한 함수, 리턴값은 이름의 길이
int name_length;  //이름의 길이를 저장하는 변수

//맵 관련 함수
int map_number=0, hole_number=0;  //현재 맵의 숫자, 전체 맵의 홀의 개수
void print_map(char map_file[map_height][map_width], char name[name_max_length]);//맵을 출력하는 함수
int check_clear(char map_file[map_height][map_width], int hole_location[max_hole][2]);  //현재 맵을 클리어 했는지 확인하는 함수
void check_map(char map_file_all[map_max_number][map_height][map_width]);  //맵에서 박스와 보관장소가 일치하는지 확인

//캐릭터 관련 함수
int find_char_height(char map_file[map_height][map_width]);  //캐릭터의 위치의 좌표를 찾는 함수
int find_char_width(char map_file[map_height][map_width]);
void move_down(char map_file[map_height][map_width], int, int);  // 캐릭터를 움직이게 하는 함수
void move_top(char map_file[map_height][map_width], int, int);
void move_left(char map_file[map_height][map_width], int, int);
void move_right(char map_file[map_height][map_width], int, int);

//보관장소 관련 함수
void change_hole(char map_file[map_height][map_width], int hole_location[max_hole][2]); //맵의 보관장소가 아무것도 없으면 채우는 함수
void find_hole(char map_file[map_height][map_width], int hole_location[max_hole][2]);  //맵의 보관장소의 좌표를 찾는 함수

//도움말 관련 함수
void display_help(void);  //도움말을 출력하는 함수

//undo관련 함수
void undo_scan(char map_file[map_height][map_width]);   //움직일 때 마다 움직임을 저장하는 함수
int undo_print(char map_file[map_height][map_width]);   //이전 움직임을 불러오는 함수
char undo[5][map_height][map_width]={0};                //움직임을 저장할 5개의 배열

//replay관련 함수
char map_replay[map_height][map_width];  //맵 리플레이를 저장하기 위한 변수

//save관련 함수
int dif_save=-1; //LOAD를 했는지 안했는지 알려주는 변수
void save_file_load(char map_file[map_height][map_width], char name[name_max_length]);  //파일을 세이브 하는 함수
void save_file_save(char map_file[map_height][map_width], int, char name[name_max_length], int, int);  //세이브를 불러오는 함수

//ranking관련 함수
void save_ranking(int ranking_file[map_max_number][4][13], int, char [name_max_length], int);  //랭킹을 세이브 하는 함수
void display_top(int ranking_file[map_max_number][4][13]); //랭킹을 보여주는 함수
void display_top_number(int ranking_file[map_max_number][4][13], char insert);  //각 맵별로 랭킹을 보여주는 함수

//종료 관련 함수
void end_all(char name[name_length]);

int main(void)
{
      //이름 관련 변수
      char insert;  //입력을 받는 변수
      char name[name_max_length]; //이름을 저장하는 배열

      //보관장소 관련 변수
      int hole_location[max_hole][2]={0};  //보관장소의 좌표를 저장하는 변수

      //맵 관련 변수
      FILE * map_load;  //맵 파일을 불러오는 변수
      char map_file[map_height][map_width];  //맵 파일 하나를 저장하기 위한 변수
      char map_file_all[map_max_number][map_height][map_width];  //맵 파일 전체를 저장하기 위한 변수

      //replay관련 함수
      int replay_i=0;   //맵이 초기 상태임을 정해주는 변수, 0이면 초기, 0이 아니면 움직인 맵을 의미.

      //랭킹 관련 변수
      FILE *ranking_load;  //랭킹 파일을 불러오는 변수
      int ranking_file[map_max_number][4][13];

      // 시간 관련 변수
      time_t start, end;  //플레이 시간의 시작과 끝을 저장하는 변수
      int dif;  //끝에서 부터 시작까지의 시간의 차이를 변환하는 함수

      //랭킹파일과 맵 파일을 불러오기
      ranking_load = fopen("ranking.txt", "r");
      map_load = fopen("map.txt", "r");

      //맵 전체 파일을 map_file_all변수에 저장
      for(int k=0;k<map_max_number;k++)
      {
        for(int i=0;i<map_height;i++)
        {
          for(int j=0;j<map_width;j++)
          {
              fscanf(map_load, "%c", &map_file_all[k][i][j]);
          }
        }
      }
      fclose(map_load);

      //첫 번째 맵 파일을 map_file에 저장
      for(int i=0;i<map_height;i++)
      {
        for(int j=0;j<map_width;j++)
        {
          map_file[i][j] = map_file_all[0][i][j];
        }
      }

      find_hole(map_file, hole_location);  //맵의 보관장소를 찾음

      //랭킹 파일을 ranking_file변수에 저장
      for(int k=0;k<map_max_number;k++)
      {
        for(int j=0;j<4;j++)
        {
          for(int i=0;i<10;i++)
          {
            fscanf(ranking_load, "%c", &ranking_file[k][j][i]);
          }
          fscanf(ranking_load, "%c", &ranking_file[k][j][10]);
          if(j==0)
            fscanf(ranking_load, "%c", &ranking_file[k][j][11]);
          else
            fscanf(ranking_load, "%d", &ranking_file[k][j][11]);
            fscanf(ranking_load, "%c", &ranking_file[k][j][12]);
        }
      }
      fclose(ranking_load);

      for (int i=0; i<map_height; i++)    //undo배열 5개를 맵의 초기상태로 초기화함.
        for(int j=0; j<map_width; j++) {
          undo[0][i][j]= map_file[i][j];
          undo[1][i][j]= map_file[i][j];
          undo[2][i][j]= map_file[i][j];
          undo[3][i][j]= map_file[i][j];
          undo[4][i][j]= map_file[i][j];
        }

      check_map(map_file_all);  //박스의 개수와 보관장소의 개수가 일치하는지 확인

      name_length = scan_name(name);  //이름을 입력받고 그 길이를 name_length에 저장

      print_map(map_file, name);  //맵파일을 출력
      time(&start);  //게임 시작지점
      while(1)  //값을 입력받는다(e를 누르기 전까지)
      {
        if (replay_i==0) {
        for(int i=0; i<map_height; i++){
          for(int j=0; j<map_width; j++){
            map_replay[i][j] = map_file[i][j];  //초기 맵파일을 맵리플레이 배열에 저장
              }
            }
          }

        replay_i =1 ;

        insert = getch();  //값을 입력 받는다

        switch(insert)
        {
          case 'j' :
              undo_scan(map_file);  //움직이기 전 맵을 저장
              move_down(map_file, find_char_height(map_file), find_char_width(map_file));  //아래로 이동
              change_hole(map_file, hole_location);  //만약 보관장소가 비어있다면 O로 매우기
              print_map(map_file, name);  //맵 출력
              break;

          case 'k' :
              undo_scan(map_file);  //움직이기 전 맵을 저장
              move_top(map_file, find_char_height(map_file), find_char_width(map_file));  //위로 이동
              change_hole(map_file, hole_location);  //만약 보관장소가 비어있다면 O로 매우기
              print_map(map_file, name);  //맵 출력
              break;

          case 'l' :
              undo_scan(map_file);   //움직이기 전 맵을 저장
              move_right(map_file, find_char_height(map_file), find_char_width(map_file));  //오른쪽으로 이동
              change_hole(map_file, hole_location);  //만약 보관장소가 비어있다면 O로 매우기
              print_map(map_file, name); //맵 출력
              break;

          case 'h' :
              undo_scan(map_file);  //움직이기 전 맵을 저장
              move_left(map_file, find_char_height(map_file), find_char_width(map_file));  //왼쪽으로 이동
              change_hole(map_file, hole_location);  //만약 보관장소가 비어있다면 O로 매우기
              print_map(map_file, name);  //맵 출력
              break;

          case 'd' :
              display_help();  //도움말을 보여줌
              do
              {
                insert = getch();  //입력받음
              }while(insert!='d');  //d를 입력할 때 까지 도움말을 빠져나갈 수 없음
              print_map(map_file, name);  //만약 빠져나가면 맵을 출력
              break;

          case 'u' :
              undo_print(map_file); //움직이기 전 맵을 불러옴
              print_map(map_file, name);  //움직이기 전 맵을 출력
              break;

          case 'r' :
              //초기 맵화면을 불러와 출력함
             for(int i=0; i<map_height; i++){
               for(int j=0; j<map_width; j++){
                 map_file[i][j] = map_replay[i][j];
                   }
                 }
              print_map(map_file, name);
              break;

          case 'n' :
            //첫번째 맵 파일을 map_file에 저장
            for(int i=0;i<map_height;i++)
            {
              for(int j=0;j<map_width;j++)
              {
                map_file[i][j] = map_file_all[0][i][j];
              }
            }

            //보관장소의 좌표를 다시 찾기 위해 배열 초기화
            for(int i=0;i<max_hole;i++)
            {
              for(int j=0;j<2;j++)
              {
                hole_location[i][j]=0;
              }
            }
            find_hole(map_file, hole_location);  //보관장소 찾기
            time(&start);  //시작시간을 이때로 변경함
            print_map(map_file, name); //맵 출력
            map_number=0;  //현재 맵의 순서를 첫번째로 변경
            break;

          case 't' :
              display_top(ranking_file);  //랭킹 파일을 보여줌
              do
              {
                insert = getch();
                if(insert<='5' && '1'<=insert)
                {
                  display_top_number(ranking_file, insert);
                }
                else if(insert=='t')
                {
                  break;
                }
              }while(insert!='t');
              print_map(map_file, name);
            break;

          case 's' :
            time(&end);
            dif_save=difftime(end, start);
            save_file_save(map_file, map_number, name, name_length, dif_save);
            break;

          case 'f' :
            save_file_load(map_file, name);
            for(int i=0;i<max_hole;i++)
            {
              for(int j=0;j<2;j++)
              {
                hole_location[i][j]=0;
              }
            }
            find_hole(map_file, hole_location);
            time(&start);
            print_map(map_file, name);
            break;

          case 'e' :
            end_all(name);

        }
        if (check_clear(map_file, hole_location)==1||insert=='c')
        {
          if(dif_save==-1)
          {
            time(&end);
            dif=difftime(end, start);
          }
          else
          {
            time(&end);
            dif=dif_save+difftime(end, start);
            dif_save=-1;
          }

          save_ranking(ranking_file, dif, name, name_length);
          if(map_number==map_max_number-1)
            end_all(name);
          map_number++;
          for(int i=0;i<map_height;i++)
          {
            for(int j=0;j<map_width;j++)
            {
              map_file[i][j]=map_file_all[map_number][i][j];
            }
          }
          for(int i=0;i<max_hole;i++)
          {
            for(int j=0;j<2;j++)
            {
              hole_location[i][j]=0;
            }
          }
          find_hole(map_file, hole_location);
          print_map(map_file, name);
          replay_i=0;   //다음 단계 초기 화면임을 의미.
          time(&start);
        }
    }
      return 0;
  }


int getch(void){
   int ch;
   struct termios buf;
   struct termios save;
   tcgetattr(0, &save);
   buf = save;
   buf.c_lflag&=~(ICANON|ECHO);
   buf.c_cc[VMIN] = 1;
   buf.c_cc[VTIME] = 0;
   tcsetattr(0, TCSAFLUSH, &buf);
   ch = getchar();
   tcsetattr(0, TCSAFLUSH, &save);
   return ch;
}

void print_map(char map_file[map_height][map_width], char name[name_max_length])
{
  system("clear");
  printf("\n      Hello ");
  printf("%s\n\n", name);
  for(int i=0;i<map_height;i++) //맵 파일 출력하기
  {
    for(int j=0;j<map_width;j++)
    {
      printf("%c", map_file[i][j]);
    }
  }

  printf("\n(Command)\n");
}

int find_char_height(char map_file[][map_width])
{
  for(int i=0;i<map_height;i++)
  {
    for(int j=0;j<map_width;j++)
    {
      if(map_file[i][j]=='@')
          return i;
    }
  }
}

int find_char_width(char map_file[][map_width])
{
  for(int i=0;i<map_height;i++)
  {
    for(int j=0;j<map_width;j++)
    {
      if(map_file[i][j]=='@')
          return j;
    }
  }
}

void move_down(char map_file[map_height][map_width], int height, int width)
{
  if(map_file[height+1][width] == 'O')
  {
    map_file[height+1][width] = map_file[height][width];
    map_file[height][width] = ' ';
  }
  else if(map_file[height+1][width] == '$' && map_file[height+2][width] == '$');
  else if(map_file[height+1][width] == '$' && map_file[height+2][width] == '#');
  else if(map_file[height+2][width] == 'O' && map_file[height+1][width] == '$')
  {
    char temp;
    map_file[height+2][width] = ' ';
    temp = map_file[height+2][width];
    map_file[height+2][width] = map_file[height+1][width];
    map_file[height+1][width] = temp;
    temp = map_file[height+1][width];
    map_file[height+1][width] = map_file[height][width];
    map_file[height][width] = temp;
  }
  else if(map_file[height+1][width] == '$')
  {
      char temp = map_file[height+2][width];
      map_file[height+2][width] = map_file[height+1][width];
      map_file[height+1][width] = temp;
      temp = map_file[height+1][width];
      map_file[height+1][width] = map_file[height][width];
      map_file[height][width] = temp;
  }
  else if(map_file[height+1][width] != '#')
  {
    char temp = map_file[height+1][width];
    map_file[height+1][width] = map_file[height][width];
    map_file[height][width] = temp;
  }
}

void move_top(char map_file[][map_width], int height, int width)
{
  char temp;
  if(map_file[height-1][width] == 'O')
  {
    map_file[height-1][width] = map_file[height][width];
    map_file[height][width] = ' ';
  }
  else if(map_file[height-1][width] == '$' && map_file[height-2][width] == '$');
  else if(map_file[height-1][width] == '$' && map_file[height-2][width] == '#');
  else if(map_file[height-2][width] == 'O' && map_file[height-1][width] == '$')
  {
    char temp;
    map_file[height-2][width] = ' ';
    temp = map_file[height-2][width];
    map_file[height-2][width] = map_file[height-1][width];
    map_file[height-1][width] = temp;
    temp = map_file[height-1][width];
    map_file[height-1][width] = map_file[height][width];
    map_file[height][width] = temp;
  }
  else if(map_file[height-1][width] == '$')
  {
      char temp = map_file[height-2][width];
      map_file[height-2][width] = map_file[height-1][width];
      map_file[height-1][width] = temp;
      temp = map_file[height-1][width];
      map_file[height-1][width] = map_file[height][width];
      map_file[height][width] = temp;
  }
  else if(map_file[height-1][width] == ' ')
  {
      char temp = map_file[height-1][width];
      map_file[height-1][width] = map_file[height][width];
      map_file[height][width] = temp;
  }
}

void move_right(char map_file[][map_width], int height, int width)
{
  if(map_file[height][width+1] == 'O')
  {
    map_file[height][width+1] = map_file[height][width];
    map_file[height][width] = ' ';
  }
  else if(map_file[height][width+1] == '$' && map_file[height][width+2] == '$');
  else if(map_file[height][width+1] == '$' && map_file[height][width+2] == '#');
  else if(map_file[height][width+2] == 'O' && map_file[height][width+1] == '$')
  {
    char temp;
    map_file[height][width+2] = ' ';
    temp = map_file[height][width+2];
    map_file[height][width+2] = map_file[height][width+1];
    map_file[height][width+1] = temp;
    temp = map_file[height][width+1];
    map_file[height][width+1] = map_file[height][width];
    map_file[height][width] = temp;
  }
  else if(map_file[height][width+1] == '$')
  {
    char temp = map_file[height][width+2];
    map_file[height][width+2] = map_file[height][width+1];
    map_file[height][width+1] = temp;
    temp = map_file[height][width+1];
    map_file[height][width+1] = map_file[height][width];
    map_file[height][width] = temp;
  }
  else if(map_file[height][width+1] == ' ')
  {
    char temp = map_file[height][width+1];
    map_file[height][width+1] = map_file[height][width];
    map_file[height][width] = temp;
  }
}

void move_left(char map_file[][map_width], int height, int width)
{
  if(map_file[height][width-1] == 'O')
  {
    map_file[height][width-1] = map_file[height][width];
    map_file[height][width] = ' ';
  }
  else if(map_file[height][width-1] == '$' && map_file[height][width-2] == '$');
  else if(map_file[height][width-1] == '$' && map_file[height][width-2] == '#');
  else if(map_file[height][width-2] == 'O' && map_file[height][width-1] == '$')
  {
    char temp;
    map_file[height][width-2] = ' ';
    temp = map_file[height][width-2];
    map_file[height][width-2] = map_file[height][width-1];
    map_file[height][width-1] = temp;
    temp = map_file[height][width-1];
    map_file[height][width-1] = map_file[height][width];
    map_file[height][width] = temp;
  }
  else if(map_file[height][width-1] == '$')
  {
    char temp = map_file[height][width-2];
    map_file[height][width-2] = map_file[height][width-1];
    map_file[height][width-1] = temp;
    temp = map_file[height][width-1];
    map_file[height][width-1] = map_file[height][width];
    map_file[height][width] = temp;
  }
  else if(map_file[height][width-1] == ' ')
  {
    char temp = map_file[height][width-1];
    map_file[height][width-1] = map_file[height][width];
    map_file[height][width] = temp;
  }
}

int scan_name(char name[name_max_length])
{
	int name_size;  //이름 길이
	printf("Start.....\n");//이름을 입력받기 위한 것
	printf("Input name : ");
	scanf("%s", name);
  while(1)
  {
    if(strlen(name)<=10)
    {
      break;
    }
    printf("이름의 길이가 10보다 큽니다. 다시입력하세요\n");
    printf("Input name : ");
    scanf("%s", name);
  }
	name_size = strlen(name);
	return name_size;
}

void find_hole(char map_file[map_height][map_width], int hole_location[max_hole][2])
{
  hole_number=0;
  int location_height = 0;
  for(int i=0;i<map_height;i++)
  {
    for(int j=0;j<map_width;j++)
    {
      if(map_file[i][j]=='O')
      {
        hole_location[location_height][0] = i;
        hole_location[location_height][1] = j;
        location_height++;
        hole_number++;
      }
    }
  }
}

void change_hole(char map_file[map_height][map_width], int hole_location[max_hole][2])
{
  for(int i=0;hole_location[i][0]!=0;i++)
  {
    if(map_file[hole_location[i][0]][hole_location[i][1]]==' ')
    {
      map_file[hole_location[i][0]][hole_location[i][1]]='O';
    }
  }
}

void display_help()
{
  system("clear");
  printf("h(왼쪽), j(아래), k(위), l(오른쪽) : 창고지기 조정\n");
  printf("u(undo) : 취소기능, 최대 5번 할 수 있음\n");
  printf("r(replay) : 현재 맵을 처음부터 다시 시작(게임시간은 계속 유지)\n");
  printf("n(new) : 첫 번째 맵부터 다시 시작(현재까지의 시간 기록 삭제)\n");
  printf("e(exit) : 게임 종료.\n");
  printf("s(save) : 현재 상태 파일에 저장.\n");
  printf("f(file load) : save 시점에서부터 이어서 게임하게 함\n");
  printf("d(display help) : 명령 내용 불러줌. 만약 명령내용이 띄어진 상태로 누르면 맵으로 이동함\n");
  printf("t(top) : 게임 순위 보여줌. t만 입력하면 전체 순위, t 다음 숫자가 오면 해당 맵의 순위\n");
  printf("\n***도움말을 나가고 싶으면 d를 누르시오***");
}

void display_top(int ranking_file[map_max_number][4][13])
{
  system("clear");
  for(int k=0;k<map_max_number;k++)
  {
    for(int j=0;j<4;j++)
    {
      for(int i=0;i<10;i++)
      {
        printf("%c", ranking_file[k][j][i]);
      }
      printf("%c", ranking_file[k][j][10]);
      if(j==0)
        printf("%c", ranking_file[k][j][11]);
      else
        printf("%d", ranking_file[k][j][11]);
        printf("%c", ranking_file[k][j][12]);
    }
  }
  printf("\n*****맵 별 랭킹을 보려면 1~5(맵 번호)를 입력*****");
  printf("\n*****랭킹을 빠져나가고 싶으면 t를 누르시오*****\n");
}

void display_top_number(int ranking_file[map_max_number][4][13], char insert)
{
  system("clear");
  int k;
  if(insert=='1')
  {
    k=0;
  }
  else if(insert=='2')
  {
    k=1;
  }
  else if(insert=='3')
  {
    k=2;
  }
  else if(insert=='4')
  {
    k=3;
  }
  else if(insert=='5')
  {
    k=4;
  }
  for(int j=0;j<4;j++)
  {
    for(int i=0;i<10;i++)
    {
      printf("%c", ranking_file[k][j][i]);
    }
    printf("%c", ranking_file[k][j][10]);
    if(j==0)
      printf("%c", ranking_file[k][j][11]);
    else
      printf("%d", ranking_file[k][j][11]);
      printf("%c", ranking_file[k][j][12]);
  }
    printf("\n*****맵 별 랭킹을 보려면 1~5(맵 번호)를 입력*****");
    printf("\n*****랭킹을 빠져나가고 싶으면 t를 누르시오*****\n");
}

int check_clear(char map_file[map_height][map_width], int hole_location[max_hole][2])
{
  int hole_clear=0;
  for(int i=0;i<hole_location[i][0]!=0;i++)
  {
    if(map_file[hole_location[i][0]][hole_location[i][1]]=='$')
    {
      hole_clear++;
    }
  }
  if(hole_clear==hole_number)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

void undo_scan(char map_file[map_height][map_width])
{
for(int i=0; i<map_height; i++)   //움직이기 전 맵을 undo배열 5개에 0~4의 순서로 하나씩 저장.
for(int j=0; j<map_width; j++){
 undo[4][i][j] =  undo[3][i][j];
 undo[3][i][j] =  undo[2][i][j];
 undo[2][i][j] =  undo[1][i][j];
 undo[1][i][j] =  undo[0][i][j];
 undo[0][i][j] = map_file[i][j];
}

}

int undo_print(char map_file[][map_width])
{
int undo_num =0; //최대 5번 을 하기위한 변수

 if (undo_num==5)   //5번을 하면 아무런 반응이 없음
    return 0;
for(int i=0; i<map_height; i++)   //저장된 맵파일을 4~0순서로 map_file배열에 불러옴
  for(int j=0; j<map_width; j++){
  map_file[i][j] = undo[0][i][j];
  undo[0][i][j] =  undo[1][i][j];
  undo[1][i][j] =  undo[2][i][j];
  undo[2][i][j] =  undo[3][i][j];
  undo[3][i][j] =  undo[4][i][j];
}
  undo_num++;

return 0;

}

void save_file_save(char map_file[map_height][map_width], int map_number, char name[name_max_length], int name_length, int dif)
{
  FILE *save_file;
  save_file=fopen("sokoban.txt", "w");
  for(int i=0;i<map_height;i++) //맵 파일을 map_file에 저장
  {
    for(int j=0;j<map_width;j++)
    {
        fprintf(save_file, "%c", map_file[i][j]);
    }
  }
  fprintf(save_file, "%d", map_number);
  fprintf(save_file, " ");
  for(int i=0;name_length>i;i++)
  {
      fprintf(save_file, "%c", name[i]);
  }
  fprintf(save_file, " ");
  fprintf(save_file, "%d", dif);
  fclose(save_file);
}

void save_file_load(char map_file[map_height][map_width], char name[name_max_length])
{
  FILE *save_file;
  save_file=fopen("sokoban.txt", "r");
  char space;
  for(int i=0;i<map_height;i++)
  {
    for(int j=0;j<map_width;j++)
    {
        fscanf(save_file, "%c", &map_file[i][j]);
    }
  }
  extern int map_number, dif_save;
  fscanf(save_file,"%d", &map_number);
  fscanf(save_file, "%c", &space);
  fscanf(save_file, "%s", name);
  fscanf(save_file, "%c", &space);
  fscanf(save_file, "%d", &dif_save);
  fclose(save_file);
  name_length=strlen(name);
}

void check_map(char map_file_all[map_max_number][map_height][map_width])
{
  int hole_num_file=0, box_num_file=0;
  for(int k=0;k<map_max_number;k++)
  {
    for(int i=0;i<map_height;i++)
    {
      for(int j=0;j<map_width;j++)
      {
          if(map_file_all[k][i][j]=='O')
            hole_num_file++;
          else if(map_file_all[k][i][j]=='$')
            box_num_file++;
      }
    }
  }
  if(hole_num_file!=box_num_file)
  {
    printf("박스의 개수와 보관장소의 개수가 다릅니다\n");
    printf("프로그램을 종료합니다.");
    exit(-1);
  }
}

void save_ranking(int ranking_file[map_max_number][4][13], int dif, char name[name_max_length], int name_length)
{
  FILE *ranking_load;
  int temp[13];
  ranking_load = fopen("ranking.txt", "w");
  if(dif<ranking_file[map_number][1][11])
  {
    for(int i=0;i<13;i++)
    {
      ranking_file[map_number][3][i]=ranking_file[map_number][2][i];
      ranking_file[map_number][2][i]=ranking_file[map_number][1][i];
    }
    for(int i=0;i<name_max_length;i++)
    {
      ranking_file[map_number][1][i]=' ';
    }
    for(int i=0;i<name_length;i++)
    {
      ranking_file[map_number][1][i]=name[i];
    }
    ranking_file[map_number][1][11]=dif;
  }
  else if(dif<ranking_file[map_number][2][11] && dif>=ranking_file[map_number][1][11])
  {
    for(int i=0;i<13;i++)
    {
      ranking_file[map_number][3][i]=ranking_file[map_number][2][i];
    }
    for(int i=0;i<name_max_length;i++)
    {
      ranking_file[map_number][2][i]=' ';
    }
    for(int i=0;i<name_length;i++)
    {
      ranking_file[map_number][2][i]=name[i];
    }
    ranking_file[map_number][2][11]=dif;
  }
  else if(dif<ranking_file[map_number][3][11] && dif>=ranking_file[map_number][2][11])
  {
    for(int i=0;i<name_max_length;i++)
    {
      ranking_file[map_number][3][i]=' ';
    }
    for(int i=0;i<name_length;i++)
    {
      ranking_file[map_number][3][i]=name[i];
    }
    ranking_file[map_number][3][11]=dif;
  }
  for(int k=0;k<map_max_number;k++)
  {
    for(int j=0;j<4;j++)
    {
      for(int i=0;i<10;i++)
      {
        fprintf(ranking_load, "%c", ranking_file[k][j][i]);
      }
      fprintf(ranking_load, "%c", ranking_file[k][j][10]);
      if(j==0)
        fprintf(ranking_load, "%c", ranking_file[k][j][11]);
      else
        fprintf(ranking_load, "%d", ranking_file[k][j][11]);
        fprintf(ranking_load, "%c", ranking_file[k][j][12]);
    }
  }
  fclose(ranking_load);
}

void end_all(char name[name_length])
{
  system("clear");
  printf("\n   SEE YOU ");
  printf("%s", name);
  printf(". . . .");
  exit(-1);
}
