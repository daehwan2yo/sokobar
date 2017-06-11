#include <stdio.h>
#include <termio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

//맵 관련 상수
#define map_max_number 5// 맵의 최대 개수
#define map_width 25
#define map_height 15

//이름 관련 상수
#define name_max_length 10  //이름의 최대 길이

//보관장소 관련 상수
#define max_hole 21  //맵의 최대 홀의 개수 + 1

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


char map_replay[map_height][map_width];  //맵 리플레이를 저장하기 위한 변수

//save관련 함수
int dif_save=-1; //LOAD를 했는지 안했는지 알려주는 변수
void save_file_load(char map_file[map_height][map_width], char name[name_max_length]);  //파일을 로드 하는 함수
void save_file_save(char map_file[map_height][map_width], int, char name[name_max_length], int, int);  //파일을 세이브 하는 함수

//ranking관련 함수
void save_ranking(int ranking_file[map_max_number][4][17], int, char [name_max_length], int);  //랭킹을 세이브 하는 함수
void display_top(int ranking_file[map_max_number][4][17]); //랭킹을 보여주는 함수
void display_top_number(int ranking_file[map_max_number][4][17], char insert);  //각 맵별로 랭킹을 보여주는 함수

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
      char map_file[map_height][map_width]={0};  //맵 파일 하나를 저장하기 위한 변수
      char map_file_all[map_max_number][map_height][map_width]={0};  //맵 파일 전체를 저장하기 위한 변수

      //replay관련 함수
      int replay_i=0;   //맵이 초기 상태임을 정해주는 변수, 0이면 초기, 0이 아니면 움직인 맵을 의미.

      //랭킹 관련 변수
      FILE *ranking_load;  //랭킹 파일을 불러오는 변수
      int ranking_file[map_max_number][4][17];  //랭킹 파일을 저장하는 변수

      // 시간 관련 변수
      time_t start, end;  //플레이 시간의 시작과 끝을 저장하는 변수
      int dif;  //끝에서 부터 시작까지의 시간의 차이를 변환하는 함수

      //랭킹파일과 맵 파일을 불러오기
      ranking_load = fopen("ranking.txt", "r");
      map_load = fopen("map.txt", "r");

      //맵 전체 파일을 map_file_all변수에 저장
      for(int i=0;i<5;i++)
      {
        fscanf(map_load, "%c", &map_file_all[0][0][i]);
      }
      for(int k=0;k<map_max_number;k++)
      {
        for(int j=1;map_file_all[k][j][0] != 'm' && map_file_all[k][j][0] != 'e';)
        {
          j++;
          for(int i=0;map_file_all[k][j][i-1] != 10 ;i++)
          {
              fscanf(map_load, "%c", &map_file_all[k][j][i]);
          }
        }
      }
      //첫 번째 맵 파일을 map_file에 저장
      int q=0;
      for(int j=1;map_file_all[0][j][0] != 'm' && map_file_all[0][j][0] != 'e';j++)
      {
        for(int i=0;map_file_all[0][j][i] != 0 ;i++)
        {
            map_file[q][i] = map_file_all[0][j][i];
        }
        q++;
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
          for(int i=0; i<5; i++)
          {
            fscanf(ranking_load, "%c", &ranking_file[k][j][12+i]);
          }
        }
      }
      fclose(ranking_load);


      check_map(map_file_all);  //박스의 개수와 보관장소의 개수가 일치하는지 확인

      name_length = scan_name(name);  //이름을 입력받고 그 길이를 name_length에 저장

      print_map(map_file, name);  //맵파일을 출력
      time(&start);  //게임 시작지점
      while(1)  //값을 입력받는다(e를 누르기 전까지)
      {
        if (replay_i==0) {
        for(int i=0; i<map_height; i++){
          for(int j=0; j<map_width; j++){
            map_replay[i][j] = map_file_all[map_number][i][j];  //초기 맵파일을 맵리플레이 배열에 저장
            undo[0][i][j]= map_file[i][j];      //다음 단계로 넘어가면 undo배열을 초기화시켜줌
            undo[1][i][j]= map_file[i][j];
            undo[2][i][j]= map_file[i][j];
            undo[3][i][j]= map_file[i][j];
            undo[4][i][j]= map_file[i][j];
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
               for (int i=0; i<map_height; i++)    //undo배열 5개를 맵의 초기상태로 초기화함.
                  for(int j=0; j<map_width; j++) {
                    undo[0][i][j]= map_file[i][j];
                    undo[1][i][j]= map_file[i][j];
                     undo[2][i][j]= map_file[i][j];
                     undo[3][i][j]= map_file[i][j];
                     undo[4][i][j]= map_file[i][j];
                   }
              print_map(map_file, name);
              break;

          case 'n' :
            //맵파일 초기화
            for(int i=0;i<map_height;i++)
            {
              for(int j=0;j<map_width;j++)
              {
                map_file[i][j] = 0;
              }
            }
            //첫번째 맵 파일을 map_file에 저장
            q=0;
            for(int j=1;map_file_all[0][j][0] != 'm' && map_file_all[0][j][0] != 'e' ; j++)
            {
              for(int i=0;map_file_all[0][j][i] != 0; i++)
              {
                map_file[q][i] = map_file_all[0][j][i];
              }
                q++;
            }

            //보관장소의 좌표를 다시 찾기 위해 배열 초기화
            for(int i=0; i<max_hole; i++)
            {
              for(int j=0; j<2; j++)
              {
                hole_location[i][j]=0;
              }
            }
            find_hole(map_file, hole_location);  //보관장소 찾기


            for (int i=0; i<map_height; i++)    //undo배열 5개를 맵의 초기상태로 초기화함.
               for(int j=0; j<map_width; j++) {
                 undo[0][i][j]= map_file[i][j];
                 undo[1][i][j]= map_file[i][j];
                  undo[2][i][j]= map_file[i][j];
                  undo[3][i][j]= map_file[i][j];
                  undo[4][i][j]= map_file[i][j];
                }

            time(&start);  //시작시간을 이때로 변경함
            print_map(map_file, name); //맵 출력
            map_number=0;  //현재 맵의 순서를 첫번째로 변경
            break;

          case 't' :
              display_top(ranking_file);  //랭킹 파일을 보여줌
              //t를 누르지 않을 시 맵파일을 보여주지 않게 하는 반복문
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
              print_map(map_file, name);  //while문을 빠져나가면 맵 파일 출력
            break;

          case 's' :
            time(&end);  //시간을 멈춤
            dif_save=difftime(end, start);  //플레이시간을 저장
            save_file_save(map_file, map_number, name, name_length, dif_save);  //저장하는 함수
            break;

          case 'f' :
            save_file_load(map_file, name);  //세이브 파일을 로드
            // 보관장소의 좌표를 초기화
            for(int i=0;i<max_hole;i++)
            {
              for(int j=0;j<2;j++)
              {
                hole_location[i][j]=0;
              }
            }
            find_hole(map_file, hole_location); //세이브 파일에서 불러온 맵의 보관장소의 좌표를 저장

            replay_i=0;
            time(&start);  //시간을 여기부터 시작함
            print_map(map_file, name);  //맵을 출력

            break;

          case 'e' :
            end_all(name);  //종료

        }

        //클리어 조건을 확인함.
        if (check_clear(map_file, hole_location)==1)
        {
          if(dif_save==-1)  //만약 맵파일을 로드하지 않았을 시
          {
            time(&end);  //시간 종료
            dif=difftime(end, start);  //시간의 차이를 반환해서 시간을 잼
          }
          else  //만약 맵 파일을 로드 했을 시
          {
            time(&end);  //시간 종료
            dif=dif_save+difftime(end, start);  //세이브 파일에서 불러온 시간 + 파일을 불러왔을 때부터 클리어 할 때 까지 잰 시간
            dif_save=-1;  //그 다음부터는 불러온 맵파일을 읽는것이 아니니 -1로 초기화
          }

          save_ranking(ranking_file, dif, name, name_length);  //랭킹 저장

          if(map_number==map_max_number-1)  //맵을 끌까지 클리어 했는지 확인함
            end_all(name);

          map_number++;  //현재 맵의 번호를 하나 늘림

          //현재 맵 파일을 0으로 초기화
          for(int i=0;i<map_height;i++)
          {
            for(int j=0;j<map_width;j++)
            {
                map_file[i][j]=0;
            }
          }

          //전체 맵 파일에서 다음 맵의 파일을 불러와서 저장함
          int q=0;
          for(int j=1;map_file_all[map_number][j][0] != 'm' && map_file_all[map_number][j][0] != 'e';j++)
          {
            for(int i=0;map_file_all[map_number][j][i] != 0 ;i++)
            {
                map_file[q][i] = map_file_all[map_number][j][i];
            }
            q++;
          }

          //보관장소의 좌표를 초기화
          for(int i=0;i<max_hole;i++)
          {
            for(int j=0;j<2;j++)
            {
              hole_location[i][j]=0;
            }
          }

          find_hole(map_file, hole_location);  //보관장소의 좌표를 찾음
          replay_i=0;   //다음 단계 초기 화면임을 의미.
          print_map(map_file, name);  //맵을 출력함
          time(&start);  //시작 시간을 잰다.
        }
    }
      return 0;
  }

//엔터없이 입려하는 함수
int getch(void)
{
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

//맵 파일을 출력하는 함수
void print_map(char map_file[map_height][map_width], char name[name_max_length])
{
  system("clear");  //깔끔한 출력을 위해

  //이름을 출력함
  printf("\n Hello ");
  printf("%s\n\n", name);

  //맵 파일을 출력한다
  for(int j=0; j<map_height;j++)
  {
    for(int i=0; map_file[j][i] != 0;i++)
    {
        printf("%c", map_file[j][i]);
    }
  }

  printf("\n(Command)\n");  //Command 출력
}

//캐릭터의 높이 좌표를 찾는 함수, 리턴값이 캐릭터의 높이 좌표
int find_char_height(char map_file[][map_width])
{
  for(int i=0;i<map_height;i++)
  {
    for(int j=0;j<map_width;j++)
    {
      if(map_file[i][j]=='@')  //전체 맵파일에서 캐릭터를 찾는다
          return i;  //캐릭터의 높이 좌표로 리턴한다.
    }
  }
}

//캐릭터의 좌우 좌표를 찾는 함수,리턴값이 캐릭터의 좌우 좌표
int find_char_width(char map_file[][map_width])
{
  for(int i=0;i<map_height;i++)
  {
    for(int j=0;j<map_width;j++)
    {
      if(map_file[i][j]=='@')  //전체 맵파일에서 캐릭터를 찾는다
          return j;  //캐릭터의 좌우 좌표로 리턴
    }
  }
}

//아래로 움직이는 함수, height는 캐릭터의 높이 좌표, width는 캐릭터의 좌우 좌표
void move_down(char map_file[map_height][map_width], int height, int width)
{
  char temp; //임시 저장을 위한 변수
  if(map_file[height+1][width] == 'O')  //만약 캐릭터의 아래에 보관장소가 있을시
  {
    map_file[height+1][width] = map_file[height][width];  //캐릭터를 아래로 이동
    map_file[height][width] = ' ';  //캐릭터가 있던 위치는 빈공간으로 남음
  }
  else if(map_file[height+1][width] == '$' && map_file[height+2][width] == '$');  //물건 두개가 아래에 동시에 있을 때 아무 것도 안함
  else if(map_file[height+1][width] == '$' && map_file[height+2][width] == '#');  //아래에 물건과 그 아래에 벽이 있을 때 아무 것도 안함
  else if(map_file[height+2][width] == 'O' && map_file[height+1][width] == '$')  //아래에 물건과 그 아래에 보관장소가 있을 때
  {
    map_file[height+2][width] = ' ';  //보관장소를 비움
    //빈공간과 물건의 위치를 바꿈
    temp = map_file[height+2][width];
    map_file[height+2][width] = map_file[height+1][width];
    map_file[height+1][width] = temp;
    //빈공간과 캐릭터의 위치를 바꿈
    temp = map_file[height+1][width];
    map_file[height+1][width] = map_file[height][width];
    map_file[height][width] = temp;
  }
  else if(map_file[height+1][width] == '$')  //아래에 물건이 있을 때
  {
      //물건 아래와 물건의 위치를 바꿈
      temp = map_file[height+2][width];
      map_file[height+2][width] = map_file[height+1][width];
      map_file[height+1][width] = temp;
      //캐릭터 아래와 캐릭터의 위치를 바꿈
      temp = map_file[height+1][width];
      map_file[height+1][width] = map_file[height][width];
      map_file[height][width] = temp;
  }
  else if(map_file[height+1][width] != '#')  //만약 캐릭터 아래가 벽이 없으면
  {
    //캐릭터 아래와 캐릭터의 위치를 바꿈
    temp = map_file[height+1][width];
    map_file[height+1][width] = map_file[height][width];
    map_file[height][width] = temp;
  }
}

//위로 움직이는 함수, height는 캐릭터의 높이 좌표, width는 캐릭터의 좌우 좌표
void move_top(char map_file[map_height][map_width], int height, int width)
{
  char temp;  //임시 저장을 위한 변수
  if(map_file[height-1][width] == 'O')  //만약 캐릭터의 위에 보관장소가 있을시
  {
    map_file[height-1][width] = map_file[height][width];  //캐릭터를 위로 이동
    map_file[height][width] = ' ';  //캐릭터가 있던 위치는 빈공간으로 남음
  }
  else if(map_file[height-1][width] == '$' && map_file[height-2][width] == '$');  //물건 두개가 위에 동시에 있을 때 아무 것도 안함
  else if(map_file[height-1][width] == '$' && map_file[height-2][width] == '#');  //위에 물건과 그 위에 벽이 있을 때 아무 것도 안함
  else if(map_file[height-2][width] == 'O' && map_file[height-1][width] == '$')  //위에 물건과 그 위에 보관장소가 있을 때
  {
    map_file[height+2][width] = ' ';  //보관장소를 비움
    //빈공간과 물건의 위치를 바꿈
    temp = map_file[height-2][width];
    map_file[height-2][width] = map_file[height-1][width];
    map_file[height-1][width] = temp;
    //빈공간과 캐릭터의 위치를 바꿈
    temp = map_file[height-1][width];
    map_file[height-1][width] = map_file[height][width];
    map_file[height][width] = temp;
  }
  else if(map_file[height-1][width] == '$')//위에 물건이 있을 때
  {
      //물건 위와 물건의 위치를 바꿈
      temp = map_file[height-2][width];
      map_file[height-2][width] = map_file[height-1][width];
      map_file[height-1][width] = temp;
      //캐릭터 위와 캐릭터의 위치를 바꿈
      temp = map_file[height-1][width];
      map_file[height-1][width] = map_file[height][width];
      map_file[height][width] = temp;
  }
  else if(map_file[height-1][width] == ' ')//만약 캐릭터 위에 벽이 없으면
  {
      //캐릭터 위와 캐릭터의 위치를 바꿈
      temp = map_file[height-1][width];
      map_file[height-1][width] = map_file[height][width];
      map_file[height][width] = temp;
  }
}

//오른쪽으로 움직이는 함수, height는 캐릭터의 높이 좌표, width는 캐릭터의 좌우 좌표
void move_right(char map_file[map_height][map_width], int height, int width)
{
  char temp;  //임시 저장을 위한 변수
  if(map_file[height][width+1] == 'O')  //만약 캐릭터의 오른쪽에 보관장소가 있을시
  {
    map_file[height][width+1] = map_file[height][width];  //캐릭터를 오른쪽으로 이동
    map_file[height][width] = ' ';  //캐릭터가 있던 위치는 빈공간으로 남음
  }
  else if(map_file[height][width+1] == '$' && map_file[height][width+2] == '$');  //물건 두개가 오른쪽에 동시에 있을 때 아무 것도 안함
  else if(map_file[height][width+1] == '$' && map_file[height][width+2] == '#');  //오른쪽에 물건과 그 오른쪽에 벽이 있을 때 아무 것도 안함
  else if(map_file[height][width+2] == 'O' && map_file[height][width+1] == '$')  //오른쪽에 물건과 그 오른쪽에 보관장소가 있을 때
  {
    //보관장소를 비움
    map_file[height][width+2] = ' ';
    //빈공간과 물건의 위치를 바꿈
    temp = map_file[height][width+2];
    map_file[height][width+2] = map_file[height][width+1];
    map_file[height][width+1] = temp;
    //빈공간과 캐릭터의 위치를 바꿈
    temp = map_file[height][width+1];
    map_file[height][width+1] = map_file[height][width];
    map_file[height][width] = temp;
  }
  else if(map_file[height][width+1] == '$') //오른쪽에 물건이 있을 때
  {
    //물건 오른쪽과 물건의 위치를 바꿈
    temp = map_file[height][width+2];
    map_file[height][width+2] = map_file[height][width+1];
    map_file[height][width+1] = temp;
    //캐릭터 오른쪽과 캐릭터의 위치를 바꿈
    temp = map_file[height][width+1];
    map_file[height][width+1] = map_file[height][width];
    map_file[height][width] = temp;
  }
  else if(map_file[height][width+1] == ' ')  //만약 캐릭터 아래가 벽이 없으면
  {
    //캐릭터 아래와 캐릭터의 위치를 바꿈
    temp = map_file[height][width+1];
    map_file[height][width+1] = map_file[height][width];
    map_file[height][width] = temp;
  }
}

//왼쪽으로 움직이는 함수, height는 캐릭터의 높이 좌표, width는 캐릭터의 좌우 좌표
void move_left(char map_file[map_height][map_width], int height, int width)
{
  char temp;  //임시 저장을 위한 변수
  if(map_file[height][width-1] == 'O')  //만약 캐릭터의 왼쪽에 보관장소가 있을시
  {
    map_file[height][width-1] = map_file[height][width];  //캐릭터를 왼쪽으로 이동
    map_file[height][width] = ' ';  //캐릭터가 있던 위치는 빈공간으로 남음
  }
  else if(map_file[height][width-1] == '$' && map_file[height][width-2] == '$');  //물건 두개가 왼쪽에 동시에 있을 때 아무 것도 안함
  else if(map_file[height][width-1] == '$' && map_file[height][width-2] == '#');  //왼쪽에 물건과 그 왼쪽에 벽이 있을 때 아무 것도 안함
  else if(map_file[height][width-2] == 'O' && map_file[height][width-1] == '$')  //왼쪽에 물건과 그 왼쪽에 보관장소가 있을 때
  {
    //보관장소를 비움
    map_file[height][width-2] = ' ';
    //빈공간과 물건의 위치를 바꿈
    temp = map_file[height][width-2];
    map_file[height][width-2] = map_file[height][width-1];
    map_file[height][width-1] = temp;
    //빈공간과 캐릭터의 위치를 바꿈
    temp = map_file[height][width-1];
    map_file[height][width-1] = map_file[height][width];
    map_file[height][width] = temp;
  }
  else if(map_file[height][width-1] == '$')  //왼쪽에 물건이 있을 때
  {
    //물건 왼쪽과 물건의 위치를 바꿈
    temp = map_file[height][width-2];
    map_file[height][width-2] = map_file[height][width-1];
    map_file[height][width-1] = temp;
    //캐릭터 왼쪽과 캐릭터의 위치를 바꿈
    temp = map_file[height][width-1];
    map_file[height][width-1] = map_file[height][width];
    map_file[height][width] = temp;
  }
  else if(map_file[height][width-1] == ' ')  //만약 캐릭터 아래가 벽이 없으면
  {
    //캐릭터 아래와 캐릭터의 위치를 바꿈
    temp = map_file[height][width-1];
    map_file[height][width-1] = map_file[height][width];
    map_file[height][width] = temp;
  }
}

//이름을 스캔하는 함수
int scan_name(char name[name_max_length])
{
	int name_size;  //이름 길이
	printf("Start.....\n");
	printf("Input name : ");
	scanf("%s", name);  //이름을 입력받음
  //이름의 크기가 10이하로 입력받게 함
  while(1)
  {
    if(strlen(name)<=10)  //이름의 크기가 10이하일 때 while문을 빠져나감
    {
      break;
    }
    printf("이름의 길이가 10보다 큽니다. 다시입력하세요\n");
    printf("Input name : ");
    scanf("%s", name);
  }
	name_size = strlen(name);  //이름의 길이 반환
	return name_size;  //리턴값은 이름의 길이
}

//보관장소의 좌표를 찾는 함수
void find_hole(char map_file[map_height][map_width], int hole_location[max_hole][2])
{
  hole_number=0;  //보관장소의 개수 초기화
  int location_height = 0; //배열을 첫번째 부터 저장하기 위한 변수 선언
  for(int i=0;i<map_height;i++)
  {
    for(int j=0;j<map_width;j++)
    {
      if(map_file[i][j]=='O')
      {
        hole_location[location_height][0] = i;
        hole_location[location_height][1] = j;
        location_height++;  //다음 번째에 저장하기 위해
        hole_number++;  //보관장소 개수를 늘림
      }
    }
  }
}

//만약 보관장소가 비어있다면 그 장소를 채워주는 함수
void change_hole(char map_file[map_height][map_width], int hole_location[max_hole][2])
{
  for(int i=0;hole_location[i][0]!=0;i++)
  {
    if(map_file[hole_location[i][0]][hole_location[i][1]]==' ')  //보관장소가 비어있다면
    {
      map_file[hole_location[i][0]][hole_location[i][1]]='O';  //그 좌표에 보관장소로 채운다
    }
  }
}

//help를 출력함
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
  printf("\n*****도움말을 나가고 싶으면 d를 누르시오*****");
}

//랭킹을 출력하는 함수
void display_top(int ranking_file[map_max_number][4][17])
{
  system("clear");  //깔끔하게 출력하게 하기 위해
  //랭킹파일을 출력함
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
        for(int i=0; i<5; i++)
        {
          printf("%c", ranking_file[k][j][12+i]);
        }
    }
  }
  printf("\n*****맵 별 랭킹을 보려면 1~5(맵 번호)를 입력*****");
  printf("\n*****랭킹을 빠져나가고 싶으면 t를 누르시오*****\n");
}

//맵 별 랭킹 출력
void display_top_number(int ranking_file[map_max_number][4][17], char insert)
{
  system("clear");
  int k;
  //각각 번호별로 변수를 저장해 그 맵별 랭킹을 출력하게 함
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

  //랭킹 출력
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
    for(int i=0; i<5; i++)
    {
      printf("%c", ranking_file[k][j][12+i]);
    }
  }
    printf("\n*****맵 별 랭킹을 보려면 1~5(맵 번호)를 입력*****");
    printf("\n*****랭킹을 빠져나가고 싶으면 t를 누르시오*****\n");
}

//클리어 했는지 확인하는 함수
int check_clear(char map_file[map_height][map_width], int hole_location[max_hole][2])
{
  int hole_clear=0;  //클리어를 판단하는 변수
  for(int i=0;i<hole_location[i][0]!=0;i++)
  {
    if(map_file[hole_location[i][0]][hole_location[i][1]]=='$')  //만약 모든 보관장소 좌표에 $가 채워졌다면
    {
      hole_clear++;  //클리어
    }
  }
  if(hole_clear==hole_number)
  {
    return 1;  //클리어
  }
  else
  {
    return 0;  //미클리어
  }
}

//undo를 저장하는 함수
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

//undo를 눌렀을 때 출력하는 함수
int undo_print(char map_file[][map_width])
{
static int undo_num =0; //최대 5번 을 하기위한 변수

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

//세이브하는 함수
void save_file_save(char map_file[map_height][map_width], int map_number, char name[name_max_length], int name_length, int dif)
{
  //sokoban.txt를 쓰기 모드로 연다.
  FILE *save_file;
  save_file=fopen("sokoban.txt", "w");

  //맵 파일을 저장
  for(int i=0;i<map_height;i++)
  {
    for(int j=0;j<map_width;j++)
    {
        fprintf(save_file, "%c", map_file[i][j]);
    }
  }
  fprintf(save_file, "%d", map_number);  //맵 번호 저장
  fprintf(save_file, " ");  //공백 저장

  //이름 저장
  for(int i=0;name_length>i;i++)
  {
      fprintf(save_file, "%c", name[i]);
  }
  fprintf(save_file, " ");  //공백 저장
  fprintf(save_file, "%d", dif);  //시간저장
  fclose(save_file);
}

//세이브 파일을 로드하는 함수
void save_file_load(char map_file[map_height][map_width], char name[name_max_length])
{
  //sokoban.txt파일을 읽기 모드로 염
  FILE *save_file;
  save_file=fopen("sokoban.txt", "r");
  char space;  //공백을 읽기 위한 변수 선언
  //맵파일을 읽어드림
  for(int i=0;i<map_height;i++)
  {
    for(int j=0;j<map_width;j++)
    {
        fscanf(save_file, "%c", &map_file[i][j]);
    }
  }
  extern int map_number, dif_save;  //맵 번호와 시간 변수
  fscanf(save_file,"%d", &map_number);  //맵 번호 저장
  fscanf(save_file, "%c", &space);  //공백 제거
  fscanf(save_file, "%s", name);  //이름 저장
  fscanf(save_file, "%c", &space);  //공백제거
  fscanf(save_file, "%d", &dif_save);  //시간 저장
  fclose(save_file);
  name_length=strlen(name);  //이름의 길이 저장
}

//저장장소와 박스의 개수 체크
void check_map(char map_file_all[map_max_number][map_height][map_width])
{
  int hole_num_file=0, box_num_file=0;  //저장장소와 박스의 개수 변수 선언
  for(int k=0;k<map_max_number;k++)
  {
    for(int i=0;i<map_height;i++)
    {
      for(int j=0;j<map_width;j++)
      {
          if(map_file_all[k][i][j]=='O')  //저장장소를 발견하면 하나씩 늘어남
            hole_num_file++;
          else if(map_file_all[k][i][j]=='$')  //박스를 발견하면 하나씩 늘어남
            box_num_file++;
      }
    }
  }
  if(hole_num_file!=box_num_file)  //만약 박스와 저장장소의 개수가 다르면
  {
    printf("박스의 개수와 보관장소의 개수가 다릅니다\n");
    printf("프로그램을 종료합니다.");
    exit(-1);
  }
}

//랭킹을 세이브 하는 함수
void save_ranking(int ranking_file[map_max_number][4][17], int dif, char name[name_max_length], int name_length)
{
  FILE *ranking_load;  //랭킹 파일을 열기 위해 사용하는 변수
  int temp[14];
  ranking_load = fopen("ranking.txt", "w");  //쓰기 모드로 랭킹 파일을 염
  if(dif<ranking_file[map_number][1][11])  //1등보다 시간이 작을 경우
  {
    //2등은 3등으로 1등은 2등으로 밀려남
    for(int i=0;i<14;i++)
    {
      ranking_file[map_number][3][i]=ranking_file[map_number][2][i];
      ranking_file[map_number][2][i]=ranking_file[map_number][1][i];
    }
    //이름을 저장하는 공간을 빈공간으로 바꿈
    for(int i=0;i<name_max_length;i++)
    {
      ranking_file[map_number][1][i]=' ';
    }
    //빈공간에 이름을 저장
    for(int i=0;i<name_length;i++)
    {
      ranking_file[map_number][1][i]=name[i];
    }
    //시간을 저장
    ranking_file[map_number][1][11]=dif;
  }
  else if(dif<ranking_file[map_number][2][11] && dif>=ranking_file[map_number][1][11])  //2등보다 시간이 작고 1등보다 크거나 같을경우
  {
    //2등은 3등으로 밀려남
    for(int i=0;i<14;i++)
    {
      ranking_file[map_number][3][i]=ranking_file[map_number][2][i];
    }
    //이름을 저장하는 공간을 빈공간으로 바꿈
    for(int i=0;i<name_max_length;i++)
    {
      ranking_file[map_number][2][i]=' ';
    }
    //빈공간에 이름을 저장
    for(int i=0;i<name_length;i++)
    {
      ranking_file[map_number][2][i]=name[i];
    }
    //시간을 저장
    ranking_file[map_number][2][11]=dif;
  }
  else if(dif<ranking_file[map_number][3][11] && dif>=ranking_file[map_number][2][11])  //3등보다 시간이 적고 2등보다 크거나 같을 때
  {
    //이름을 저장하는 공간을 빈공간으로 바꿈
    for(int i=0;i<name_max_length;i++)
    {
      ranking_file[map_number][3][i]=' ';
    }
    //빈공간에 이름을 저장
    for(int i=0;i<name_length;i++)
    {
      ranking_file[map_number][3][i]=name[i];
    }
    //시간을 저장
    ranking_file[map_number][3][11]=dif;
  }
  //랭킹파일에 랭킹을 저장함
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
        for(int i=0; i<5; i++)
        {
          fprintf(ranking_load, "%c", ranking_file[k][j][12+i]);
        }
    }
  }
  fclose(ranking_load);
}

//종료하는 함수
void end_all(char name[name_length])
{
  system("clear");  //깔끔한 출력을 위해
  printf("\n SEE YOU ");
  printf("%s", name);  //이름 출력
  printf(". . . .");
  exit(-1);  //프로그램 종료
}
