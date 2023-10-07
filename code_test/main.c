#include<stdio.h>
 
int main ()
{
   FILE *fp;
   char str[] = "This is w3cschool.cc";
 
   fp = fopen( "E:/Documents/Desktop/code_test/data/file.txt" , "w" );
   fwrite(str , 1 , sizeof(str) , fp );
 
   fclose(fp);
 
   return(0);
}