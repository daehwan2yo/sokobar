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
