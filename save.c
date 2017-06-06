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
