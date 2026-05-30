#include <stdio.h>
#include <unistd.h>

int main(void) {
    for (size_t counter = 0;; counter++) {
        printf("%zu\n", counter);
        
        sleep(1);
    }

    return 0;
}
