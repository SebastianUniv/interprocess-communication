/* 
 * Operating Systems <2INCO> Practical Assignment
 * Interprocess Communication
 *
 * STUDENT_NAME_1 (STUDENT_NR_1)
 * STUDENT_NAME_2 (STUDENT_NR_2)
 *
 * Grading:
 * Students who hand in clean code that fully satisfies the minimum requirements will get an 8. 
 * Extra steps can lead to higher marks because we want students to take the initiative. 
 * These extra steps must be agreed with the tutor before delivery.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>          // for perror()
#include <unistd.h>         // for getpid()
#include <mqueue.h>         // for mq-stuff
#include <time.h>           // for time()
#include <complex.h>

#include "common.h"
#include "md5s.h"

#include <math.h>

static void rsleep (int t);

static char *mq_request;
static char *mq_response;

int main (int argc, char * argv[])
{
    // TODO:
    // (see message_queue_test() in interprocess_basic.c)
    //  * open the two message queues (whose names are provided in the arguments)
    //  * repeatingly:
    //      - read from a message queue the new job to do
    //      - wait a random amount of time (e.g. rsleep(10000);)
    //      - do that job 
    //      - write the results to a message queue
    //    until there are no more tasks to do
    //  * close the message queues

    if (argc < 2) {
        fprintf (stderr, "Invalid arguments\n");
        return (-1);
    }

    int working = 1;
    mqd_t               mq_fd_request;
    mqd_t               mq_fd_response;
    MQ_REQUEST_MESSAGE  req;
    MQ_RESPONSE_MESSAGE rsp;

    printf("Worker started\n");

    // Get parameters from parent process
    mq_request = argv[0];
    mq_response = argv[1];

    // Open queues
    mq_fd_request = mq_open(mq_request, O_RDONLY);
    mq_fd_response = mq_open(mq_response, O_WRONLY);



    while (working) {
        // Receive message from farmer (blocking)
        mq_receive(mq_fd_request, (char *) &req, sizeof(MQ_REQUEST_MESSAGE), NULL);

        int wordLength = 1;
        char word[MAX_MESSAGE_LENGTH];

        //////////////////loop/////////////////////////////////////////////////// 
        //format new word
        int tryingHash = 1;
        while(tryingHash){

            //trim word
            char finalWord[wordLength];
            for(int i = 0; i < wordLength; i++){
                finalWord[i] = word[i];
        
            }

            //compare hash
            uint128_t new_hash;
            new_hash = md5s(finalWord, wordLength);

            if(new_hash == req.hash){
                // Send message to farmer if matching hash found, else pick new reqest message (blocking)
                for(int i = 0; i < wordLength; i++){
                    rsp.word[i] = finalWord[i];        
                }
                rsp.id = req.id;
                rsp.lenght = wordLength;
                mq_send(mq_fd_response, (const char *) &rsp, sizeof(MQ_RESPONSE_MESSAGE), 0);
            }
        }
        //////////////////////////loop end////////////////////////////////
    }
    
    return (0);
}

/*
 * rsleep(int t)
 *
 * The calling thread will be suspended for a random amount of time
 * between 0 and t microseconds
 * At the first call, the random generator is seeded with the current time
 */
static void rsleep (int t)
{
    static bool first_call = true;
    
    if (first_call == true)
    {
        srandom (time (NULL) % getpid ());
        first_call = false;
    }
    usleep (random() % t);
}


