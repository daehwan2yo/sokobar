#include <stdio.h>
#include <termio.h>
#include <stdlib.h>

#define name_max_length 10  //이름의 최대 길이
#define map_height 11  //맵의 세로 길이
#define map_width 24  //맵의 가로 길이

int scan_name(char name[name_max_length]);  //이름을 적기 위한 함수, 리턴값은 이름의 길이

int getch(void); //엔터 없이 입력하게 하기 위한 함수

int find_char_height(char map_file[map_height][map_width]);  //캐릭터의 위치를 찾는 함수
int find_char_width(char map_file[map_height][map_width]);

void move_down(char map_file[map_height][map_width], int, int);  // 캐릭터를 움직이게 하는 함수
void move_top(char map_file[map_height][map_width], int, int);
void move_left(char map_file[map_height][map_width], int, int);
void move_right(char map_file[map_height][map_width], int, int);

void print_map(char map_file[map_height][map_width]);//맵을 출력하는 함수

int main(void)
{
      FILE * map_load;  //맵 파일 불러오기 위한 파일 변수
      char map_file[map_height][map_width];  //맵 파일을 저장하기 위한 변수
      char name[name_max_length]; //이름을 입력받는 함수
      char insert;  //입력을 입력받는 변수
      int name_length;

      name_length = scan_name(name);

      map_load = fopen("map", "r");  //맵 파일 불러오기
      for(int i=0;i<map_height;i++) //맵 파일을 map_file에 저장
      {
        for(int j=0;j<map_width;j++)
        {
          fscanf(map_load, "%c", &map_file[i][j]);
        }
      }

      print_map(map_file);
      printf("(Command)\n");  //맵파일을 출력한 뒤 값을 입력받기 위해 개행

      while(1)  //값을 입력받는다(e를 누르기 전까지)
      {
        insert = getch();  //값을 입력 받는다
        switch(insert)
        {
          case 'j' :
            move_down(map_file, find_char_height(map_file), find_char_width(map_file));
            print_map(map_file);
            break;
          case 'k' :
            move_top(map_file, find_char_height(map_file), find_char_width(map_file));
            print_map(map_file);
            break;
          case 'l' :
            move_right(map_file, find_char_height(map_file), find_char_width(map_file));
            print_map(map_file);
            break;
          case 'h' :
            move_left(map_file, find_char_height(map_file), find_char_width(map_file));
            print_map(map_file);
            break;
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

void print_map(char map_file[][map_width])
{
  for(int i=0;i<map_height;i++) //맵 파일 출력하기
  {
    for(int j=0;j<map_width;j++)
    {
      printf("%c", map_file[i][j]);
    }
  }
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
  if(map_file[height+2][width] == '0' && map_file[height+1][width] == '$')
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
  if(map_file[height-2][width] == '0' && map_file[height-1][width] == '$')
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
  else if(map_file[height-1][width] != '#')
  {
      char temp = map_file[height-1][width];
      map_file[height-1][width] = map_file[height][width];
      map_file[height][width] = temp;
  }
}

void move_right(char map_file[][map_width], int height, int width)
{
  if(map_file[height][width+2] == '0' && map_file[height][width+1] == '$')
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
  else if(map_file[height][width+1] != '#')
  {
    char temp = map_file[height][width+1];
    map_file[height][width+1] = map_file[height][width];
    map_file[height][width] = temp;
  }
}

void move_left(char map_file[][map_width], int height, int width)
{
  if(map_file[height][width-2] == '0' && map_file[height][width-1] == '$')
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
  else if(map_file[height][width-1] != '#')
  {
    char temp = map_file[height][width-1];
    map_file[height][width-1] = map_file[height][width];
    map_file[height][width] = temp;
  }
}

int scan_name(char name[name_max_length])
{
  int i_name=0;  //이름 길이
  printf("Start.....\n");    //이름을 입력받기 위한 것
  printf("Input name : ");

	for ( i_name=0; i_name < name_max_length ; i_name++)
  {
     scanf("%c", &name[i_name]);
		 if( name[i_name] == '\n')		//(enter) 개행 문자를 받을시 탈출
			break;
	}

  return i_name;
}
