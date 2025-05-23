// examples/client.c
#include <stdio.h>
#include "rpc.h"

int main() {
    int result = rpc_call(1, 10, 5); // add(10, 5)
    if (result >= -9999) {
        printf("Result: %d\n", result);
    } else {
        printf("RPC error occurred: code %d\n", result);
    }
    return 0;
}
