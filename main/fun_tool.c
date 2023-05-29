#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//字符串拼接函数：将两个字符串拼接然后复制到新的字符串上，且不修改原来的字符串
char* stradd_copy(char* output,char* left,char* right){
    strcpy(output,left);
    strcat(output,right);
    return output;
}
