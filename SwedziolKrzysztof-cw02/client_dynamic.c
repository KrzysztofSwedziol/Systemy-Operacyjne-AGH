#include <stdio.h>
#include <dlfcn.h>

//int test_collatz_convergence(int input, int max_iter);
int main() {
    void *handle;
    char *error;
    int (*test_collatz_convergence)(int, int);


    handle = dlopen("./libcollatz.so", RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "%s\n", dlerror());
        return 1;
    }

    dlerror();

    *(void **)(&test_collatz_convergence) = dlsym(handle, "test_collatz_convergence");

    if ((error = dlerror()) != NULL) {
        fprintf(stderr, "%s\n", error);
        dlclose(handle);
        return 1;
    }

    int inputs[] = {10, 20, 30, 40, 50};
    int max_iter = 80;

    for (int i = 0; i < sizeof(inputs) / sizeof(inputs[0]); i++) {
        int currInput = inputs[i];
        int iterations = test_collatz_convergence(currInput, max_iter);
        if (iterations == -1)
            printf("input %d, did not manage to finish with given %d iterations.\n", currInput, max_iter);
        else
            printf("input %d, taken to 1 in %d iterations.\n", currInput, iterations);
    }

    dlclose(handle);
    return 0;
}
