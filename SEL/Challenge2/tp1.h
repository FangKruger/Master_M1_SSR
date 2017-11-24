/*
*
*SEL TP
*Author: Manon Derocles & Fanny Prieur
*
*
*/

#define _TP1_H
#include <stdio.h>
#include <stdlib.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/user.h>
#include <sys/mman.h>


#define MAX_LENGTH 512
