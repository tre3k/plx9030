#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "plx9030c.h"

void plxWriteCS0(int f,long offset,char byte){
  ioctl(f,CS0_SET_ADDR,offset);
  write(f,&byte,1);
  return;
}

char plxReadCS0(int f,long offset){
  char byte = 0x00;
  ioctl(f,CS0_SET_ADDR,offset);
  read(f,&byte,1);
  return byte;
}

int main(int argc,char **argv){
  int f = open(argv[1],O_RDWR);
  if(f<0){
    printf("Error open device file\n");
    return 0; 
  }
  
  char buff=0xfa;

  printf("\nCS0: \n");
  for(int i=0;i<32;i++){
    ioctl(f,CS0_SET_ADDR,i);
    read(f,&buff,1);
    printf("%d: 0x%.2x\n",i,buff & 0xff);
  }

  printf("\nCS1: \n");
  for(int i=0;i<32;i++){
    ioctl(f,CS1_SET_ADDR,i);
    read(f,&buff,1);
    printf("%d: 0x%.2x\n",i,buff & 0xff);
  }

  printf("\nCS2: \n");
  for(int i=0;i<32;i++){
    ioctl(f,CS2_SET_ADDR,i);
    read(f,&buff,1);
    printf("%d: 0x%.2x\n",i,buff & 0xff);
  }

  printf("\nCS3: \n");
  for(int i=0;i<32;i++){
    ioctl(f,CS3_SET_ADDR,i);
    read(f,&buff,1);
    printf("%d: 0x%.2x\n",i,buff & 0xff);
  }

  
  plxWriteCS0(f,3,0);
  printf("\n%x\n",plxReadCS0(f,3));
  

  
  close(f);
  return 0;
}
