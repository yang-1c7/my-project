// Author: Yanhong Hou 104085020
//         Jiaqi Yang 110008505

#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/un.h>



void sigchld_handler(int sig){
    while (waitpid(-1, NULL, WNOHANG) > 0);
       //wait and check for all terminated child processes
    //return 0 should there be no terminated processes
    //return pid of terminated process
    //return -1 for error

    //so this function can  prevent zombie process by using a signal handler
}

void servicePlayers(int sd_toto, int sd_titi){
  
    static const char *playMessage = "You can now play\n";
    static const char *winMessage ="Game over: you won the game\n";
    static const char *lostMessage ="Game over: you lost the game\n";


    int score;
    // total score for toto and titi
    int totoSum = 0, titiSum = 0;
	int round =0;
    while (1){
		round++;
		
        // send play msg
        write(sd_toto, playMessage, strlen(playMessage));
        // read score
        read(sd_toto, &score, sizeof(score));
        // add toto total score
        totoSum += score;

        // send play msg
        write(sd_titi, playMessage, strlen(playMessage));
        // read score
        read(sd_titi, &score, sizeof(score));
        // add score
        titiSum += score;
        printf("we are in round %d\n", round);
        printf("toto: %5.0d        titi: %5.0d\n",totoSum,titiSum);
        printf("\n");
        
        if (totoSum >= 100 || titiSum >= 100) {
            // toto win
            if (totoSum >= 100) {
                write(sd_toto, winMessage, strlen(winMessage));
                write(sd_titi, lostMessage, strlen(lostMessage));
                //sent msg to client
            } else{ // titi win
                write(sd_toto, lostMessage, strlen(lostMessage));
                write(sd_titi, winMessage, strlen(winMessage));
                   //sent msg to client
		printf("Game Over. TITI win\n");// sent msg to screen
            }
            break;
        }
    }
    close(sd_toto);
    close(sd_titi);
}

int main(int argc, char *argv[]){
    

        // set signal handler
    signal(SIGCHLD, sigchld_handler);    
       //  SIGCHLD: When a child process stops or terminates, SIGCHLD is sent to the parent process.
    // sigchld_handler : how to do when you get SIGCHL
    // get server socket, sd is file descriptors

    int portNumber;	

    if(argc != 2){
	printf("Call model: %s <Port #>\n", argv[0]);
	exit(0);//security check, make sure user input port number
    }	


    int sd = socket(AF_INET, SOCK_STREAM, 0);
    // create and init sock addr
      //AF_INET: for intenet  sockets 
    //SOCK_STREAM :TCP
    struct sockaddr_in servAdd;
    bzero(&servAdd, sizeof(servAdd));


	servAdd.sin_family = AF_INET;
    servAdd.sin_addr.s_addr = htonl(INADDR_ANY);
    sscanf(argv[1], "%d", &portNumber);
    servAdd.sin_port = htons((uint16_t)portNumber);
    

    // bind
    bind(sd, (struct sockaddr*)&servAdd, sizeof(servAdd));
    // reuse port
    int opt = 1;
    setsockopt(sd, SOL_SOCKET, SO_REUSEPORT, (const void*)&opt, sizeof(opt));

    //SO_REUSEPORT : 2 client use one port


    // listen for client
    listen(sd, 10);

    while(1){
        // wait for 1st client
        int sd_client1= accept(sd, NULL, NULL);
	     printf("The 1st client connected, Wait for 2nd client...\n");
        int sd_client2 = accept(sd, NULL, NULL);// wait for 2nd client (block)
	       printf("The 2nd client connected. Let's start\n");
        if (fork() == 0){ // child process will do servicePlayers
            servicePlayers(sd_client1, sd_client2);
            break;// child process jump out of the loop
        }// parent process will keep in this loop and block itself in line 12
    }

    close(sd);
    return EXIT_SUCCESS;
}
