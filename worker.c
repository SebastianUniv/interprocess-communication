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

    //printf("Worker started\n");

    // Get parameters from parent process
    mq_request = argv[0];
    mq_response = argv[1];

    // Open queues
    mq_fd_request = mq_open(mq_request, O_RDONLY);
    mq_fd_response = mq_open(mq_response, O_WRONLY);



    while (working) {
        // Receive message from farmer (blocking)
        mq_receive(mq_fd_request, (char *) &req, sizeof(MQ_REQUEST_MESSAGE), NULL);
        char start = (char) req.startAlfa;
        char end = (char) req.endAlfa;
        //printf ("0x%016lx%016lx\n", HI(req.hash), LO(req.hash ));
        //int request = req.id;
        //printf("%d\n", request);
        //printf("start: %c\n",req.start);
        //int wordLength = 1;
        char word[MAX_MESSAGE_LENGTH - 1];
        for(int i = 0; i < MAX_MESSAGE_LENGTH -1; i++){
            word[i] = start;
        }   

        //////////////////loop/////////////////////////////////////////////////// 
        
        int tryingHash = 1;
        int currentLength = 0;
        while(tryingHash){
            //sleep(1);

            //trim word            
            char finalWord[currentLength + 2];
            finalWord[0] = req.start;
            for(int i = 1; i < currentLength + 1; i++){
                finalWord[i] = word[i - 1];
        
            }           
            finalWord[currentLength + 1] = '\0';

            /*
            printf("length: %d\n", currentLength);
            for(int m = 0; m < currentLength + 1; m++){
                printf("word: ");
                printf("%c",finalWord[m]);
                printf("\n");
            }
            */
            

            //printf("Trying word: %s\n", finalWord);

            //compare hash
            uint128_t new_hash;
            new_hash = md5s(finalWord, currentLength + 1);            

            if(new_hash == req.hash){
                // Send message to farmer if matching hash found, else pick new reqest message (blocking)
                //printf("found match\n",0);
                for(int i = 0; i < currentLength + 1; i++){
                    rsp.word[i] = finalWord[i];                            
                }
                rsp.id = req.id;
                rsp.length = currentLength + 1;
                  


                mq_send(mq_fd_response, (const char *) &rsp, sizeof(MQ_RESPONSE_MESSAGE), 0);
                tryingHash = 0;
                break;
            }

            if(currentLength == 0){
                currentLength++;
            }

            //format new word        
            for(int i = currentLength - 1; i >= 0; i--){
                if((int) word[i] < (int) end ){
                    word[i] = (char) ((int) word[i] + 1);
                    break;
                } else if((int) word[i] == (int) end){
                    word[i] = start;
                }
            
                if((int) word[0] == (int) start && i == 0){
                    currentLength++;
                    break;
                }
            }

            if(currentLength + 1 > MAX_MESSAGE_LENGTH){
                tryingHash = 0;
                break;
            }
        }
        //////////////////////////loop end////////////////////////////////
        //break;
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


