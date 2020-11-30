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

typedef unsigned __int128 uint128_t;

typedef struct
{
    // a data structure with 2 members
    uint128_t               hash;    
    char                    start;
    int                     startAlfa;
    int                     endAlfa;
    int                     id;
} MQ_REQUEST_MESSAGE;

typedef struct
{
    // a data structure with 2 members    
    char                    word[MAX_MESSAGE_LENGTH];    
    int                     id;
    int                     length;
} MQ_RESPONSE_MESSAGE;

#endif

