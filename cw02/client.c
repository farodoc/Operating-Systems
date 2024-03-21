#include <stdio.h>

#ifndef DYNAMIC
    #include "collatz.h"
#endif

#ifdef DYNAMIC
    #include <dlfcn.h>
    int (*test_collatz_convergence)(int input, int max_iter);
#endif

int main() {
#ifdef DYNAMIC
    void* handle = dlopen("./libcollatz.so", RTLD_LAZY);
    if(!handle){
        printf("Failed to open libcollatz.so\n");
        return 1;
    }

    test_collatz_convergence = dlsym(handle, "test_collatz_convergence");
#endif

    int input;
    int max_iter;

    printf("Enter a number: ");
    scanf("%d", &input);
    printf("Max iterations: ");
    scanf("%d", &max_iter);

    int result = test_collatz_convergence(input, max_iter);

    if(result == -1){
        printf("The sequence did not converge after %d iterations\n", max_iter);
    } else {
        printf("The sequence converged after %d iterations\n", result);
    }

#ifdef DYNAMIC
    dlclose(handle);
#endif
}