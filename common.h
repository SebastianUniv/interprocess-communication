/* 
 * Operating Systems <2INCO> Practical Assignment
 * Interprocess Communication
 *
 * Contains definitions which are commonly used by the farmer and the workers
 *
 */

#ifndef COMMON_H
#define COMMON_H


// maximum size for any message in the tests
#define MAX_MESSAGE_LENGTH	6
 

// TODO: put your definitions of the datastructures here

#define STUDENT_NAME "RobbenGerritsen"

typedef struct
{
    // a data structure with 3 members
    int                     a;
    int                     b;
    char                    c;
} MQ_REQUEST_MESSAGE;

typedef struct
{
    // a data structure with 3 members
    int                     e;
    char                    f[20];
    char                    g[20];
} MQ_RESPONSE_MESSAGE;

#endif

