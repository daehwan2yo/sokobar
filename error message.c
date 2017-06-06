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
