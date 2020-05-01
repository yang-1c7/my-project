// Author: Yanhong Hou 104085020 
//         Jiaqi Yang 110008505

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>


int main(int argc, char*argv[]) {
    
    int portNumber;

    static const char *playMessage = "You can now play\n";
    static const char *winMessage = "Game over: you won the game\n";
    static const char *lostMessage = "Game over: you lost the game\n";
  
    if(argc != 3){
	printf("Call model: %s <IP> <Port #>\n", argv[0]);
	exit(0);
    }

    int server = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
	
    server_addr.sin_family = AF_INET;
    sscanf(argv[2], "%d", &portNumber);
    server_addr.sin_port = htons((uint16_t)portNumber);
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

    if (connect(server, (struct sockaddr *) &server_addr, sizeof(server_addr)) <0){
        perror("Connect failed: ");
        exit(1);
    }

    int dice;
    char msg[100];
    int total = 0;
    // Use current time as seed for random generator(produce different random number every time)
    srand(time(0));
    while (1) {
		//clear the array
        memset(msg, 0, sizeof(msg));
        // read message from server
        read(server, msg, 100);
        // if is play message
        if (strcmp(msg, playMessage) == 0) {
            // get dice by random number
            dice = rand() % 10 + 1;
			total += dice;
            printf("got %d points\n", dice);
			printf("total points : %d\n", total);
            write(server, &dice, sizeof(dice));
        } else {
            // if receive win message
            if (strcmp(msg, winMessage) == 0) {
                printf("I won the game\n");
            } else if (strcmp(msg, lostMessage) == 0) { 
                printf("I lost the game\n");
            }
            break;
        }
        // slow down the execution
        sleep(5);
    }
    close(server);
    return 0;
}
