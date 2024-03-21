#include <stdio.h>
int test_collatz_convergence(int input, int max_iter);
int main() {
    int inputs[] = {10, 20, 30, 40, 50};
    int max_iterations= 80;

    for (int i = 0; i < sizeof(inputs) / sizeof(inputs[0]); i++) {
        int currInput = inputs[i];
        int iterations = test_collatz_convergence(currInput, max_iterations);
        if (iterations == -1)
            printf("input %d, did not manage to finish with given %d iterations.\n", currInput, max_iterations);
        else
            printf("input %d, taken to 1 in %d iterations.\n", currInput, iterations);
    }

    return 0;
}