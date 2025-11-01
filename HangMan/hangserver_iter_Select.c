 /* Network server for hangman game */
 /* File: hangserver.c */

 #include <sys/types.h>
 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <stdio.h>
 #include <syslog.h>
 #include <signal.h>
 #include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <sys/time.h>
#include <poll.h>
#include <stdbool.h>
 extern time_t time ();

 int maxlives = 12;
 char *word [] = {
 # include "words"
 };
 # define NUM_OF_WORDS (sizeof (word) / sizeof (word [0]))
 # define MAXLEN 80 /* Maximum size in the world of Any string */
 # define HANGMAN_TCP_PORT 1066
 
 /* ---------------- Play_hangman () ---------------------*/

 int play_hangman (int in, int out)
 {
 	char * whole_word, part_word [MAXLEN],
 	guess[MAXLEN], outbuf [MAXLEN];

 	int lives = maxlives;
 	int game_state = 'I';//I = Incomplete
 	int i, good_guess, word_length,sockfd;
 	char hostname[MAXLEN];

 	gethostname (hostname, MAXLEN);
 	sprintf(outbuf, "Playing hangman on host% s: \n \n", hostname);
 	write(out, outbuf, strlen (outbuf));

 	/* Pick a word at random from the list */
 	whole_word = word[rand() % NUM_OF_WORDS];
 	word_length = strlen(whole_word);
 	syslog (LOG_USER | LOG_INFO, "server chose hangman word %s", whole_word);
        puts("Child has now picked letter");
 	/* No letters are guessed Initially */
 	for (i = 0; i <word_length; i++)
 		part_word[i]='-';
 	
	part_word[i] = '\0';

 	sprintf (outbuf, "%s %d \n", part_word, lives);
 	write (out, outbuf, strlen(outbuf));

 	while (game_state == 'I')
 	/* Get a letter from player guess */
 	{
		while (read (in, guess, MAXLEN) <0) {
 			if (errno != EINTR)
 				exit (4);
 			printf ("re-read the startin \n");
 			} /* Re-start read () if interrupted by signal */
 	good_guess = 0;
 	for (i = 0; i <word_length; i++) {
 		if (guess [0] == whole_word [i]) {
 		good_guess = 1;
 		part_word [i] = whole_word [i];
 		}
 	}
 	if (! good_guess) //they are using the fact that 1 and 0 can uals true and false here
 	{
 	lives--;
 	}//the function here below copies the left to the right
 	if (strcmp (whole_word, part_word) == 0)
 		game_state = 'W'; /* W ==> User Won */
 	else if (lives == 0) {
 		game_state = 'L'; /* L ==> User Lost */
 		strcpy (part_word, whole_word); /* User Show the word */
 	}
 	sprintf (outbuf, "Good Guess \n%s %d \n", part_word, lives);
 	
 	write (out, outbuf, strlen (outbuf));
 	}
 	
 	return(0);
 }
  

 int main ()
 {
 	int sockfd,client, fd, client_len,r,option,max_connect;
 	fd_set main_fd,read_fd;;
 	struct sockaddr  address;
 	struct addrinfo hints,*server;
 	 pid_t pid=getpid();
 	 const int hostname_size = 32;//size in bytes of hostname
 	 char hostname[hostname_size];
	char buffer[BUFSIZ];
 	const char *port = "65001";	
 	socklen_t address_len = sizeof(struct sockaddr);
 	 const int backlog = 10;
 	 struct timeval max={1,0};
 	 char connection[backlog][hostname_size];
 	 bool done=false;
 	 
 	 //HangManVariables
 	 char* AllWords[backlog][MAXLEN];//this will holdevery word we use
 				
 	

 	sockfd = socket (AF_INET, SOCK_STREAM, 0);//0 or IPPROTO_TCP
 	if (sockfd <0) { //This error checking is the code Stevens wraps in his Socket Function etc
 		perror ("creating stream socket");
 		exit (1);
 	}

 	memset( &hints, 0, sizeof(struct addrinfo));
 	hints.ai_family = AF_INET6;			/* IPv4 connection */
	hints.ai_socktype = SOCK_STREAM;	/* TCP, streaming */
	hints.ai_flags=AI_PASSIVE;
        r = getaddrinfo( 0,  port, &hints, &server );
	if( r!= 0 )
	{
		perror("failed1");
		exit(1);
	}
	puts("done");
	sockfd = socket(
			server->ai_family,		/* domain, TCP/UDP */
			server->ai_socktype,	/* type, stream or datagram */
			server->ai_protocol		/* protocol */
			);
	
	if( sockfd==-1 )
	{
		perror("Socket failed2 ");
		exit(1);
	}
	option=0;
	r=setsockopt(
	  sockfd,
	    IPPROTO_IPV6,
	    IPV6_V6ONLY,
	    (void*) option,
	    sizeof(option)
	
	
	);
	r = bind(sockfd,server->ai_addr,server->ai_addrlen);
 	
        if( r==-1 )
	{
		perror("failed3");
		exit(1);
	}
	puts("done");
	puts("TCP Server is listening...");
 	r=listen (sockfd, backlog);
         if( r==-1 )
	{
		perror("failed");
		exit(1);
	}

	
        int status;
        int counter=0;
       
        /* deal with multiple connections */
	max_connect = backlog;		/* maximum connections */
	FD_ZERO(&main_fd);			/* initialize file descriptor set */
	FD_SET(sockfd, &main_fd);	/* set the server's file descriptor */
	bool dave=true;
 	while (dave) {
 	 struct timeval max={3,0};
 	        
 	       
 		
 		/* backup the main set into a read set for processing */
		read_fd = main_fd;
                printf("Hey loop");
		/* scan the connections for any activity */
		r = select(max_connect+1,&read_fd, NULL, NULL, &max);
		if( r==-1 )
		{
			perror("failed");
			exit(1);
		}
 		for( fd=1; fd<=max_connect; fd++)
		{
		
			/* filt
			er only active or new clients */
			if( FD_ISSET(fd,&read_fd) )	/* returns true for any fd waiting */
			{
			puts("hey Client waiting");
			

				/* filter out the server, which indicates a new connection */
				if( fd==sockfd )
				{
				  client= accept (sockfd,&address,&address_len);
				  puts("GOT NEW CLIENT");
				  if( client==-1 )
					{
						perror("failed");
						exit(1);
					}
 			              
 			              
 			              
 			                counter=0;
 			                
 			                /* connection accepted, get name */
					r = getnameinfo(&address,address_len,hostname,hostname_size,0,0,NI_NUMERICHOST);
					/* update array */
					strcpy(connection[client],hostname);
					printf("New connection from %s\n",connection[client]);

					/* add new client socket to the master list */
					FD_SET(client, &main_fd);
					
					    puts("Seed randomized");
 		                        srand ((int) time ((long *) 0)); /* randomize the seed */
 		                        
 		                        
 		                        /* respond to the connection */
					strcpy(buffer,"Hello to NEW CLIENT DAVE ");
					strcat(buffer,connection[client]);
					send(client,buffer,strlen(buffer),0);
 		                    }//end of if
 		                    else
				    {
				    puts("we have an active child waiting");
					 /* read the input buffer for the current fd */
					 r = recv(fd,buffer,BUFSIZ,0);
					 /* if nothing received, disconnect them */
					 if( r<1 )
					 {
					 	/* clear the fd and close the connection */
				    	   FD_CLR(fd, &main_fd);	/* reset in the list */
				    	   close(fd);				/* disconnect */
					    /* update the screen */
					    printf("%s closed\n",connection[fd]);
					  }
					  /* something has been received */
					  else
					  {
					      puts("we recieved some info form child"+fd);
					      buffer[r] = '\0';		/* terminate the string */
					      /* if 'shutdown' sent... */
					      if( strcmp(buffer,"shutdown\n")==0 )
					      {
					      	dave = false;		/* terminate the loop */
					      }
					      /* otherwise, echo back the text */
					      else
					      {
					      	send(fd,buffer,strlen(buffer),0);
							      
					      }
				          } /* end else to send/recv from client(s) */
			              } /* end if */
			            }//end of waiting if check
			 }//for loop ending
 			              
 		
 			
 		counter++;
 		printf("\ncounter num= %d",counter);
 		if(counter==20)
 		{
 		dave=false;
 		puts("and now its the end ");
 		}
 		//puts("SO this is how loops work "+counter);
 		
 		
 	}
 	
 	
 	return(0);
 }


