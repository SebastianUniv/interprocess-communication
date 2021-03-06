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
        //mq_send(mq_fd_request, (const char *) &req, sizeof(MQ_REQUEST_MESSAGE), 0); 
        // Receive message from worker
        //mq_receive(mq_fd_response, (char *) &rsp, sizeof(MQ_RESPONSE_MESSAGE), NULL);

        //fill queue
        int busy = 1;
        int start = (int) ALPHABET_START_CHAR;
        //start = (int) 'd';
        int end = (int) ALPHABET_END_CHAR;
        int currentChar = start;
        int currentHash = 0;

        while(busy){
            //Try place job on the queue      
            struct mq_attr      attr_queue;      
            int                 rtnval;

            rtnval = mq_getattr(mq_fd_request, &attr_queue);
            
            //printf("number of messages: %d\n", (int) attr_queue.mq_curmsgs);

            if(attr_queue.mq_curmsgs < MQ_MAX_MESSAGES){
                req.hash = UINT128(HI(md5_list[currentHash]),LO(md5_list[currentHash]));
                req.start = (char) currentChar;
                req.startAlfa = start;
                req.endAlfa = end;
                req.id = currentHash;                
                mq_send(mq_fd_request, (const char *) &req, sizeof(MQ_REQUEST_MESSAGE), 0);
                //printf ("0x%016lx%016lx\n", HI(req.hash), LO(req.hash ));
                //printf ("sending: %d\n", req.id);

                if(currentChar == end){
                   currentHash++;
                   currentChar = start;
                }else{
                    currentChar++; 
                }
            }   

            //Pull and print received messages

            //mq_getattr(mq_fd_response, &attr);
            
            //printf("number of messages: %d\n", attr.mq_curmsgs);

            mq_getattr(mq_fd_response, &attr_queue);
            //printf("number of messages: %d\n", (int) attr_queue.mq_curmsgs);

            if(attr_queue.mq_curmsgs > 0){
                //make sure message queue does not get overloaded
                mq_receive (mq_fd_response, (char *) &rsp, sizeof (rsp), NULL);
                //printf("length: %d\n", rsp.length);
                int length = rsp.length;
                char finalWord[length+1];
                for(int i = 0; i < rsp.length; i++){
                    finalWord[i] = rsp.word[i];
                }
                finalWord[length] = '\0';



                printf("%s\n", finalWord);   //might have to "trim" string first
                //printf("%d\n", rsp.id);

            
                if(rsp.id == currentHash){
                    //currentHash++;
                    //currentChar = start;
                }

                if(currentHash > MD5_LIST_NROF){
                    busy = 0;
                } 
            }
            
        }

    }
        

    
    // student name and the process id (to ensure uniqueness during testing)
    
    return (0);
}

