#include <stdio.h>
#include <termio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define name_max_length 10  //이름의 최대 길이
#define map_height  11
#define map_width 24  //맵의 최대 가로 길이
#define max_hole 7
#define map_max_number 2// 맵의 최대 개수

int scan_name(char name[name_max_length]);   //이름을 적기 위한 함수, 리턴값은 이름의 길이

int getch(void); //엔터 없이 입력하게 하기 위한 함수_

int find_char_height(char map_file[map_height][map_width]);  //캐릭터의 위치를 찾는 함수
int find_char_width(char map_file[map_height][map_width]);

void move_down(char map_file[map_height][map_width], int, int);  // 캐릭터를 움직이게 하는 함수
void move_top(char map_file[map_height][map_width], int, int);
void move_left(char map_file[map_height][map_width], int, int);
void move_right(char map_file[map_height][map_width], int, int);

void print_map(char map_file[map_height][map_width]);//맵을 출력하는 함수

void change_hole(char map_file[map_height][map_width], int hole_location[max_hole][2]); //맵의 0개수를 처리하는 함수
void find_hole(char map_file[map_height][map_width], int hole_location[max_hole][2]);

void display_help(void);
void display_top(void);

int check_clear(char map_file[map_height][map_width], int hole_location[max_hole][2]);

int main(void)
{
      FILE * map_load;  //맵 파일 불러오기 위한 파일 변수
      char name[name_max_length]; //이름을 입력받는 함수
      char insert;  //입력을 입력받는 변수
      int name_length, help_state=0;
      int hole_location[max_hole][2]={0};
      int map_number=0;
      char map_file[map_height][map_width];  //맵 파일을 저장하기 위한 변수
      char map_file_all[map_max_number][map_height][map_width];
      name_length = scan_name(name);

      map_load = fopen("map", "r");  //맵 파일 불러오기
      for(int k=0;k<map_max_number;k++)
      {
        for(int i=0;i<map_height;i++) //맵 파일을 map_file에 저장
        {
          for(int j=0;j<map_width;j++)
          {
              fscanf(map_load, "%c", &map_file_all[k][i][j]);
          }
        }
      }

      for(int i=0;i<map_height;i++) //맵 파일을 map_file에 저장
      {
        for(int j=0;j<map_width;j++)
        {
          map_file[i][j] = map_file_all[0][i][j];
        }
      }

      find_hole(map_file, hole_location);
      print_map(map_file);

      while(1)  //값을 입력받는다(e를 누르기 전까지)
      {
        insert = getch();  //값을 입력 받는다
        switch(insert)
        {
          case 'j' :
          if(help_state==0)
          {
            move_down(map_file, find_char_height(map_file), find_char_width(map_file));
            change_hole(map_file, hole_location);
            print_map(map_file);
            break;
          }
          else if(help_state==1)
            break;
          case 'k' :
          if(help_state==0)
          {
            move_top(map_file, find_char_height(map_file), find_char_width(map_file));
            change_hole(map_file, hole_location);
            print_map(map_file);
            break;
          }
          else if(help_state==1)
            break;
          case 'l' :
          if(help_state==0)
          {
            move_right(map_file, find_char_height(map_file), find_char_width(map_file));
            change_hole(map_file, hole_location);
            print_map(map_file);
            break;
          }
          else if(help_state==1)
            break;
          case 'h' :
          if(help_state==0)
          {
            move_left(map_file, find_char_height(map_file), find_char_width(map_file));
            change_hole(map_file, hole_location);
            print_map(map_file);
            break;
          }
          else if(help_state==1)
            break;
          case 'd' :
            if(help_state==1)
            {
              print_map(map_file);
              help_state--;
              break;
            }
            else if(help_state==0)
            {
              display_help();
              help_state++;
              break;
            }
          case 't' :
            display_top();
        }
        if (check_clear(map_file, hole_location)==1)
        {
          map_number++;
          if(map_number==1)
          {
            for(int i=0;i<map_height;i++) //맵 파일을 map_file에 저장
            {
              for(int j=0;j<map_width;j++)
              {
                map_file[i][j]=map_file_all[map_number][i][j];
              }
            }
            find_hole(map_file, hole_location);
            print_map(map_file);
            map_number--;
          }
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

void print_map(char map_file[map_height][map_width])
{
  system("clear");
  for(int i=0;i<map_height;i++) //맵 파일 출력하기
  {
    for(int j=0;j<map_width;j++)
    {
      printf("%c", map_file[i][j]);
    }
  }
  printf("\n (Command)");
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

void move_down(char map_file[][map_width], int height, int width)
{
  if(map_file[height+1][width] == '0')
  {
    map_file[height+1][width] = map_file[height][width];
    map_file[height][width] = ' ';
  }
  else if(map_file[height+1][width] == '$' && map_file[height+2][width] == '$');
  else if(map_file[height+1][width] == '$' && map_file[height+2][width] == '#');
  else if(map_file[height+2][width] == '0' && map_file[height+1][width] == '$')
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
  if(map_file[height-1][width] == '0')
  {
    map_file[height-1][width] = map_file[height][width];
    map_file[height][width] = ' ';
  }
  else if(map_file[height-1][width] == '$' && map_file[height-2][width] == '$');
  else if(map_file[height-1][width] == '$' && map_file[height-2][width] == '#');
  else if(map_file[height-2][width] == '0' && map_file[height-1][width] == '$')
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
  if(map_file[height][width+1] == '0')
  {
    map_file[height][width+1] = map_file[height][width];
    map_file[height][width] = ' ';
  }
  else if(map_file[height][width+1] == '$' && map_file[height][width+2] == '$');
  else if(map_file[height][width+1] == '$' && map_file[height][width+2] == '#');
  else if(map_file[height][width+2] == '0' && map_file[height][width+1] == '$')
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
  if(map_file[height][width-1] == '0')
  {
    map_file[height][width-1] = map_file[height][width];
    map_file[height][width] = ' ';
  }
  else if(map_file[height][width-1] == '$' && map_file[height][width-2] == '$');
  else if(map_file[height][width-1] == '$' && map_file[height][width-2] == '#');
  else if(map_file[height][width-2] == '0' && map_file[height][width-1] == '$')
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
	scanf("%10s", name);
	name_size = strlen(name);
	return name_size;
}

void find_hole(char map_file[map_height][map_width], int hole_location[max_hole][2])
{
  int location_height = 0;
  for(int i=0;i<map_height;i++)
  {
    for(int j=0;j<map_width;j++)
    {
      if(map_file[i][j]=='0')
      {
        hole_location[location_height][0] = i;
        hole_location[location_height][1] = j;
        location_height++;
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
      map_file[hole_location[i][0]][hole_location[i][1]]='0';
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

void display_top()
{
  FILE *top_load;
  top_load = fopen("map", "r");
  char top_file[100][100];
  for(int i=0;i<map_height;i++)
  {
    for(int j=0;j<map_width;j++)
    {
      fscanf(top_load, "%c", &top_file[i][j]);
    }
  }
}

int check_clear(char map_file[map_height][map_width], int hole_location[max_hole][2])
{
  int hole_clear=0;
  for(int i=0;i<max_hole-1;i++)
  {
    if(map_file[hole_location[i][0]][hole_location[i][1]]=='$')
    {
      hole_clear++;
    }
  }
  if(hole_clear==max_hole-1)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}
