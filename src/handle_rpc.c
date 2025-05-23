// src/rpc_handler.c
int handle_rpc(int function_id, int a, int b, int *status_code) {
    *status_code = 0;
    switch (function_id) {
        case 1: return a + b;
        case 2: return a - b;
        case 3: return a * b;
        case 4:
            if (b == 0) {
                *status_code = 1;
                return 0;
            }
            return a / b;
        default:
            *status_code = 2;
            return 0;
    }
}
