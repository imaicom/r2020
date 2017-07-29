// cc -o remote remote.c -lm
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <math.h>


void main() {
	
   int xx,yy,ph,x,y,z,p,c1,c2,c3,c4,v1,v2,ww;


    v1 = 0;    // 縦軸入力
    v2 = 0;    // 横軸入力
    ww = 10;   // 回転入力

    c1 = ( 8 * v1 +  8 * v2 +  6 * ww ) / 10;   // 左前
    c2 = ( 8 * v1 + -8 * v2 + -6 * ww ) / 10;   // 右前
    c3 = ( 8 * v1 + -8 * v2 +  6 * ww ) / 10;   // 左後
    c4 = ( 8 * v1 +  8 * v2 + -6 * ww ) / 10;   // 右後

    printf(" c1=%4d ",c1);
    printf(" c2=%4d ",c2);
    printf(" c3=%4d ",c3);
    printf(" c4=%4d ",c4);
    printf("\n");

    
}
