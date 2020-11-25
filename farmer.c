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
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>    
#include <unistd.h>         // for execlp
#include <mqueue.h>         // for mq

#include "settings.h"
#include "common.h"

static char mq_request[80];
static char mq_response[80];

int main (int argc, char * argv[])
{
    if (argc != 1)
    {
        fprintf (stderr, "%s: invalid arguments\n", argv[0]);
    }
        
    // TODO:
    //  * create the message queues (see message_queue_test() in interprocess_basic.c)
    //  * create the child processes (see process_test() and message_queue_test())
    //  * do the farming
    //  * wait until the chilren have been stopped (see process_test())
    //  * clean up the message queues (see message_queue_test())
    pid_t               processID;      /* Process ID from fork() */
    mqd_t               mq_fd_request;
    mqd_t               mq_fd_response;
    MQ_REQUEST_MESSAGE  req;
    MQ_RESPONSE_MESSAGE rsp;
    struct mq_attr      attr;

    // Set names of the queues
    snprintf(mq_request, 80,"/mq_request_%s_%d", STUDENT_NAME, getpid());
    snprintf(mq_response, 80,"/mq_response_%s_%d", STUDENT_NAME, getpid());

    attr.mq_maxmsg  = MQ_MAX_MESSAGES;
    attr.mq_msgsize = sizeof (MQ_REQUEST_MESSAGE);
    mq_fd_request = mq_open (mq_request, O_WRONLY | O_CREAT | O_EXCL, 0600, &attr);

    attr.mq_maxmsg  = MQ_MAX_MESSAGES;
    attr.mq_msgsize = sizeof (MQ_RESPONSE_MESSAGE);
    mq_fd_response = mq_open (mq_response, O_RDONLY | O_CREAT | O_EXCL, 0600, &attr);

    // Generate child processes
    int i;
    for (i = 0; i < NROF_WORKERS; ++i) {
        processID = fork();
        // Prevent from looping when a child process is created.
        if (processID == 0) {
            break;
        }
    }

    if (processID == 0)
    {
        //start worker
        execlp("./worker", mq_request, mq_response, (char *) NULL);
        exit (0);
    }else {
        // Send message to worker
        mq_send(mq_fd_request, (const char *) &req, sizeof(MQ_REQUEST_MESSAGE), 0); 
        // Receive message from worker
        mq_receive(mq_fd_response, (char *) &rsp, sizeof(MQ_RESPONSE_MESSAGE), NULL);

        //fill queue
        char alfabet[27] = "abcdefghijklmnopqrstuvwxyz";
        int busy = 1;
        int mod = ALPHABET_END_CHAR - ALPHABET_START_CHAR + 1;
        int current = 1;

        while(busy){
            //create new job and put job on queue
            char newWord[MAX_MESSAGE_LENGTH];
            int wordLength = 1;
            int q = current;
            int r = 0;

            while(q > 0){
                r = q % mod;
                q = q / mod;
                newWord[wordLength - 1] = alfabet[r-1];
                wordLength++;
            }

            current++;

            //the job to be send to the worker
            char job[wordLength];
            for(int i = 0; i < wordLength; i++){
                job[i] = newWord[wordLength - i - 1];
            }


            //remove completed job from farmer queue
        }

    }
        

    
    // student name and the process id (to ensure uniqueness during testing)
    
    return (0);
}

