#include <stdio.h>

int main(){
    int id = 6;
    int pai;
    if (id % 2){
        printf("\nid percent 2");
        pai = (id - 1)/2;
    } else {
        printf("\nelse");
        pai = (id - 2)/2;
    }
    printf("\nid: %d, pai: %d", id, pai);
}