#ifndef BUILTIN_H
#define BUILTIN_H

#include "parse.h"

int is_builtin(char *cmd);
void builtin_execute(Task *T);
int execute_cd(Task *T);

#endif
