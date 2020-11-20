/* 
 * Operating Systems <2INC0> Practical 
 * 2016/2020 (c) Joris Geurts
 *
 * This program contains some C constructs which might be useful for
 * the Interprocess Communication assignment of 2INC0.
 *
 *
 *      I M P O R T A N T    M E S S A G E :
 *      ====================================
 *
 * For readability reasons, this program does not check the return value of 
 * the POSIX calls.
 * This is not a good habit.
 * Always check the return value of a system call (you never know if the disk is 
 * full, or if we run out of other system resources)!
 * Possible construction:
 *
 *      rtnval = <posix-call>();
 *      if (rtnval == <error-value-according-documentation>)
 *      {
 *          perror ("<your-message>");
 *          exit (1);
 *      }
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <mqueue.h>
#include <sys/wait.h>   /* for waitpid() */

#define STUDENT_NAME        "your_name_here"

static char                 mq_name1[80];
static char                 mq_name2[80];

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

/*-------------------------------------------------------------------------*/


static void
process_test (void)
{
    pid_t           processID;      /* Process ID from fork() */

    printf ("parent pid:%d\n", getpid());
    processID = fork();
    if (processID < 0)
    {
        perror("fork() failed");
        exit (1);
    }
    else
    {
        if (processID == 0)
        {
            printf ("child  pid:%d\n", getpid());
            execlp ("ps", "ps", "-l", NULL);
            // or try this one:
			//execlp ("./interprocess_basics", "my_own_name_for_argv0", "first_argument", NULL);
            
            // we should never arrive here...
            perror ("execlp() failed");
        }
        // else: we are still the parent (which continues this program)
        
        waitpid (processID, NULL, 0);   // wait for the child
        printf ("child %d has been finished\n\n", processID);
    }
}


static void 
getattr (mqd_t mq_fd)
{
    struct mq_attr      attr;
    int                 rtnval;
    
    rtnval = mq_getattr (mq_fd, &attr);
    if (rtnval == -1)
    {
        perror ("mq_getattr() failed");
        exit (1);
    }
    fprintf (stderr, "%d: mqdes=%d max=%ld size=%ld nrof=%ld\n",
                getpid(), 
                mq_fd, attr.mq_maxmsg, attr.mq_msgsize, attr.mq_curmsgs);
}


static void
message_queue_child (void)
{
    mqd_t               mq_fd_request;
    mqd_t               mq_fd_response;
    MQ_REQUEST_MESSAGE  req;
    MQ_RESPONSE_MESSAGE rsp;

    mq_fd_request = mq_open (mq_name1, O_RDONLY);
    mq_fd_response = mq_open (mq_name2, O_WRONLY);

    // read the message queue and store it in the request message
    printf ("                                   child: receiving...\n");
    mq_receive (mq_fd_request, (char *) &req, sizeof (req), NULL);

    printf ("                                   child: received: %d, %d, '%c'\n",
            req.a, req.b, req.c);
    
    // fill response message
    // e   contains the length of f[]
    // f[] contains characters abcd
    // g[] contains characters ABCDEFGHIJ + \0-terminator (so it is a C-string)
    rsp.e = 4;	// set 'e' to the number of characters in f[]
    for (int i = 0; i < rsp.e; i++)
    {
        rsp.f[i] = 'a' + i;
    }
    for (int i = 0; i < 10; i++)
    {
        rsp.g[i] = 'A' + i;
    }
    rsp.g[10] = '\0';

    sleep (3);
    // send the response
    printf ("                                   child: sending...\n");
    mq_send (mq_fd_response, (char *) &rsp, sizeof (rsp), 0);

    mq_close (mq_fd_response);
    mq_close (mq_fd_request);
}

static void
message_queue_test (void)
{
    pid_t               processID;      /* Process ID from fork() */
    mqd_t               mq_fd_request;
    mqd_t               mq_fd_response;
    MQ_REQUEST_MESSAGE  req;
    MQ_RESPONSE_MESSAGE rsp;
    struct mq_attr      attr;

    sprintf (mq_name1, "/mq_request_%s_%d", STUDENT_NAME, getpid());
    sprintf (mq_name2, "/mq_response_%s_%d", STUDENT_NAME, getpid());

    attr.mq_maxmsg  = 10;
    attr.mq_msgsize = sizeof (MQ_REQUEST_MESSAGE);
    mq_fd_request = mq_open (mq_name1, O_WRONLY | O_CREAT | O_EXCL, 0600, &attr);

    attr.mq_maxmsg  = 10;
    attr.mq_msgsize = sizeof (MQ_RESPONSE_MESSAGE);
    mq_fd_response = mq_open (mq_name2, O_RDONLY | O_CREAT | O_EXCL, 0600, &attr);

    getattr(mq_fd_request);
    getattr(mq_fd_response);

    processID = fork();
    if (processID < 0)
    {
        perror("fork() failed");
        exit (1);
    }
    else
    {
        if (processID == 0)
        {
            // child-stuff
            message_queue_child ();
            exit (0);
        }
        else
        {
            // remaining of the parent stuff
            
            // fill request message
            req.a = 73;
            req.b = 42;
            req.c = 'z';

            sleep (3);
            // send the request
            printf ("parent: sending...\n");
            mq_send (mq_fd_request, (char *) &req, sizeof (req), 0);

            sleep (3);
            // read the result and store it in the response message
            printf ("parent: receiving...\n");
            mq_receive (mq_fd_response, (char *) &rsp, sizeof (rsp), NULL);

            printf ("parent: received: %d, '", rsp.e);
            // printing characters of f[] separately:
            for (int i = 0; i < rsp.e; i++)
            {
                printf ("%c", rsp.f[i]);
            }
            // printing g[] in one step (because it has the \0-terminator):
            printf ("', '%s'\n", rsp.g);
    
            sleep (1);
    
            waitpid (processID, NULL, 0);   // wait for the child
            
            mq_close (mq_fd_response);
            mq_close (mq_fd_request);
            mq_unlink (mq_name1);
            mq_unlink (mq_name2);   
        }
    }
}


/*-------------------------------------------------------------------------*/

int main (int argc, char * argv[])
{
    int	i;

    // command-line arguments are available in argv[0] .. argv[argc-1]
    // argv[0] always contains the name of the program
    
    // check if the user has started this program with valid arguments
    if (argc != 1)
    {
        fprintf (stderr, "%s: %d arguments:\n", argv[0], argc);
        for (i = 1; i < argc; i++)
        {
            fprintf (stderr, "     '%s'\n", argv[i]);
        }
        exit (1);
    }
    // else: parse the arguments...
    
    process_test();
    message_queue_test();
    
    return (0);
}

