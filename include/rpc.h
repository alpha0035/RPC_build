// include/rpc.h
#ifndef RPC_H
#define RPC_H

int rpc_call(int function_id, int a, int b);
int handle_rpc(int function_id, int a, int b, int *status_code);

#endif
