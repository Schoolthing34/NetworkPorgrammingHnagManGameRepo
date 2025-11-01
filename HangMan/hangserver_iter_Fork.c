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
#include <poll.h>
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
void my_sig_handler(int signum)
{

  if(signum==SIGCHLD)
  {
    puts("CHild dead");
  }
}

 int main ()
 {
 	int sock, fd, client_len,r,option,ChildAmount;
 	struct sockaddr_in  client;
 	struct addrinfo hints,*server;
 	 pid_t pid=getpid();
 	 pid_t ChildArray[20]={-1};
        
        struct pollfd MyPoll;
 	

 	sock = socket (AF_INET, SOCK_STREAM, 0);//0 or IPPROTO_TCP
 	if (sock <0) { //This error checking is the code Stevens wraps in his Socket Function etc
 		perror ("creating stream socket");
 		exit (1);
 	}

 	
 	hints.ai_family = AF_INET6;			/* IPv4 connection */
	hints.ai_socktype = SOCK_STREAM;	/* TCP, streaming */
	hints.ai_flags=AI_PASSIVE;
        r = getaddrinfo( 0, "1066", &hints, &server );
	if( r!= 0 )
	{
		perror("failed");
		exit(1);
	}
	puts("done");
	sock = socket(
			server->ai_family,		/* domain, TCP/UDP */
			server->ai_socktype,	/* type, stream or datagram */
			server->ai_protocol		/* protocol */
			);
	
	if( sock==-1 )
	{
		perror("Socket failed ");
		exit(1);
	}
	option=0;
	r=setsockopt(
	  sock,
	    IPPROTO_IPV6,
	    IPV6_V6ONLY,
	    (void*) option,
	    sizeof(option)
	
	
	);
	r = bind(sock,server->ai_addr,server->ai_addrlen);
 	
        if( r==-1 )
	{
		perror("failed");
		exit(1);
	}
	puts("done");
 	listen (sock, 5);
        ChildAmount=0;
        int status;
        int counter=0;
        memset(&MyPoll,0,sizeof(MyPoll));
        MyPoll.fd=0;
        MyPoll.events=POLLIN;
        
 	while (1) {
 	        puts("in Loop");
 	        if(ChildAmount>0)
 	        {
 	        puts("Child here");
 	        int ChildCounter=0;
 	          
 	          if(ChildArray[0]!=-1)
 	          {
 	          puts("Found Child");
 	          //This is ony for 1 as a test
 	          pid_t result=waitpid(ChildArray[0],&status,WNOHANG);
 	          
 	          if(result==0)
 	          {
 	            puts("CHild Alive");
 	          }
 	          else if(result==-1)
 	          {
                      perror("Error hapened when checking childen");
         	  }
         	  else
         	  {
         	  puts("CHild Dead");
         	  }
         	  
 	          }
 	        }
 		client_len = sizeof (client);
 		puts("Hey");
 		//the accept function goes through every connection and blocks so now we need to tell it tocalm down
 		if(poll(&MyPoll,1,100)==0)
 		{
 		if ((fd = accept (sock, (struct sockaddr *) &client, &client_len)) <0) {
 			perror ("Failed connection no one to connect tooo");
 			puts("in Fail");
 			exit (3);
 		}
 		else
 		{
 		  if(ChildAmount<20)
 		  {
 		    puts("in fork");
 		    pid= fork();
 		    printf("Forking");
 		  
 		    ChildArray[ChildAmount]=pid;
 		    ChildAmount++;
 		  } 
 		  else
 		  {
 		  puts("Error tooo many connections");
 		  }
 		}
 		}
 		puts("SO this is how loops work");
 		
 		if(pid==0)
 		{
 		puts("Seed randomized");
 		srand ((int) time ((long *) 0)); /* randomize the seed */
 		puts("in child");
 		printf("child 1 accepts");
 		play_hangman (fd, fd);
 		close (fd);
 		puts("dead child");
 		return(0);
 		}
 		else
 		{
 		  close(fd);
 		}
 	}
 	
 	
 	return(0);
 }


