#include "collatz.h"

int collatz_conjecture(int input){
    if(input % 2 == 0){
        return input/2;
    }

    return 3 * input + 1;
}

int test_collatz_convergence(int input, int max_iter){
    if(input == 1){
        return 0;
    }

    int cnt = 0;
    
    while(cnt < max_iter && input != 1){
        input = collatz_conjecture(input);
        cnt++;
    }

    if(cnt == max_iter && input != 1){
        return -1;
    }

    return cnt;
}