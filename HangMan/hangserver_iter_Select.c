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
//Player
struct Player
{
  int lives,WordLength;//lives of player
  char * WordToGuess,guess[MAXLEN],CurrentKnownLetters[MAXLEN];//the word player must guess the players guess and the currentKnownLetters
  char outbuf[MAXLEN+100];
  int  good_guess,sockfd;
  char hostname[MAXLEN];
  char game_state;//N,U,C  not started, underway, complete
  
 };
 
void toString(struct Player player)
{
  printf("TOSTRING METHOD");
  printf("\n%dlives=",player.lives);
  printf("\n WordLength=");
  printf("%d",player.WordLength);
  printf("\n WordToGuess=");
  printf("%s",player.WordToGuess);
  printf("\n guess=");
  printf("%s",player.guess);
  printf("\n CurrentKnownLetters=");
  printf("%s",player.CurrentKnownLetters);
  printf("\n outbuf=");
  printf("%s",player.outbuf);
  printf("\n good_guess=");
  printf("%d",player.good_guess);
  printf("\n sockfd=");
  printf("%d",player.sockfd);
  printf("\n hostname="); 
  printf("%s",player.hostname);
  printf("\n game_state=");
  printf("%d",player.game_state);
  }
 /* ---------------- Play_hangman () ---------------------*/
 

struct Player PlayerGuessCheck(struct Player player,int out)
{
  int i;
  puts("\nPlayer has now guessed and check is being checked");
  for ( i = 0; i <player.WordLength; i++)
  {
    if (player.guess [0] == player.WordToGuess [i]) 
    {
 	player.good_guess = 1;
 	player.CurrentKnownLetters [i] = player.WordToGuess [i];
 	//puts("eroor in comparison");
    }
  }
  //sprintf (player.outbuf, "Please GUess \n%s %d \n", player.CurrentKnownLetters, player.lives);

 if (! player.good_guess) //they are using the fact that 1 and 0 can uals true and false here
 {
  puts("\n Player was wrong and lost a life");
  sprintf (player.outbuf, "Your guess was wrong ad job :(, \nPlease choose another letter \n%s %d \n", player.CurrentKnownLetters, player.lives);
  player.lives--;
 }//the function here below copies the left to the right
  else
  {
    sprintf (player.outbuf, "Your guess was right well done, \nPlease choose another letter \n%s %d \n", player.CurrentKnownLetters, player.lives);
    puts("Players guess was right and continues playing");
  }
 //puts("got Got stopped before words check");
 if (strcmp (player.WordToGuess, player.CurrentKnownLetters ) == 0)
 {
      puts("Player has now won");
     //player.game_state = 'W'; /* W ==> User Won */
      sprintf (player.outbuf, "Well Done you guessed correctly COngratulations :)\n%s %d \n", player.WordToGuess, player.lives);
       player.game_state= 'C';//C==Compelte
  }
  else if (player.lives == 0) 
  {
    //player.game_state = 'L'; /* L ==> User Lost */
    strcpy (player.CurrentKnownLetters , player.WordToGuess); /* User Show the word */
    puts("Player has lost all lives and died");
    sprintf (player.outbuf, "You have lost all lives game over:( \n%s %d \n", player.CurrentKnownLetters, player.lives);
    player.game_state= 'C';//C==Complete
  }
 
  write (out, player.outbuf, strlen (player.outbuf));//where we send it back to the user
  //toString(player);
  return player;


}
 
 struct Player PlayerGuess(struct Player  player,int in, int out)
{
  //toString(player);
 // sprintf (player.outbuf, "%s %d \n", player.CurrentKnownLetters , player.lives);
 
  puts("\nPlayer is now guessing word");
  while (read (in, player.guess, MAXLEN) <0) 
  {
    printf ("INfinite loop \n");
    if (errno != EINTR)
    {
      exit (4);
    }
    printf ("re-read the startin \n");
  } /* Re-start read () if interrupted by signal */
 	
  player.good_guess = 0;
  //puts("got here");
 	
  //toString(player);
 	
  
  player=PlayerGuessCheck(player, out);
  
  
  return player;


}

struct Player EndGame(struct Player player)
{
  puts("We in end method");
  //toString(player);
  
  return player;

}
struct Player StartNewGame(struct Player player,int in, int out )
{
  puts("\nPlayer started new game");
  //start new game
  player.game_state= 'N';//N== notstarted
  gethostname (player.hostname, MAXLEN);
  sprintf(player.outbuf, "%s Playing hangman on host:  \n", player.hostname);
  write(out, player.outbuf, strlen (player.outbuf));
  player.lives=maxlives;
  /* Pick a word at random from the list */
 	
  srand ((int) time ((long *) 0)); /* randomize the seed */      
  player.WordToGuess = word[rand() % NUM_OF_WORDS];
  player.WordLength = strlen(player.WordToGuess);
  syslog (LOG_USER | LOG_INFO, "server chose hangman word %s", player.WordToGuess);
  //puts(player.WordToGuess);
   //puts("Child has now picked letter");
  /* No letters are guessed Initially */
  int i;
  for ( i = 0; i <strlen(player.WordToGuess); i++)
    {
 	player.CurrentKnownLetters[i]='-';
 		
 	//puts("is problem here");
   }
  //puts("problem actualyl outta loop");
  player.CurrentKnownLetters[i] = '\0';
       
  player.game_state= 'U';//U== underway
  //end of start new game
  //toString(player);
 	
  sprintf (player.outbuf, "Start Game Please choose a letter \n%s %d \n", player.CurrentKnownLetters, player.lives);
  write (out, player.outbuf, strlen (player.outbuf));//where we send it back to the user
 	
 	
 	
  //player=PlayerGuess( player, in,  out);
          
  return player;


}




 struct Player play_hangman (int in, int out,struct Player player)
 {
        
 	
  puts("\nEntered HangMan loop");
 	
  //this function starts the game and sets up the new word we use
  if(player.game_state=='N')
    {
       puts("\nPLayer starting GameLoop");
       player=StartNewGame(player,in,out);
          
 	
      //puts("got here3");
     }
    else if(player.game_state=='U')
      {
       
        
        puts("\nPlayer entering game loop game Underway");
        player=PlayerGuessCheck( player,out);
        //write (out, player.outbuf, strlen(player.outbuf));
 	 
 	 
 	
      }
      
      
      
     if(player.game_state=='C')
      {
        puts("\nPLayer has won and is done playing");
        player=EndGame(player);
       // toString(player);
        
      }
        
        
        
    return player;
        
        
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
 	 char* AllWords[backlog][MAXLEN];//this will hold every word we use
 				
 	

 	sockfd = socket (AF_INET, SOCK_STREAM, 0);//0 or IPPROTO_TCP
 	if (sockfd <0) 
 	{ //This error checking is the code Stevens wraps in his Socket Function etc
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
	//puts("done");
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
	    &option,
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
	struct Player players[backlog];
	int children[MAXLEN];
 	while (dave) 
 	{
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
	  int i=0;
	  int num=0;
	  printf("%d",i,num);
 	  for( fd=1; fd<=max_connect; fd++)
	    {
		  
	     // printf("\n hey checkon thingy%d ",i);
	    
		  /* filter only active or new clients */
		if( FD_ISSET(fd,&read_fd) )	/* returns true for any fd waiting */
		    {
		      puts("A client is waiting");
			printf("\nleft this place");

		      /* filter out the server, which indicates a new connection */
			if( fd==sockfd )
			    {
			     client= accept (sockfd,&address,&address_len);
			      puts("A new client has joined T");
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
                                    children[num]=i;
				    /* add new client socket to the master list */
				    FD_SET(client, &main_fd);
					printf("%d welcome child new to this thing",children[num]);
					 num++;   
 		                        
 		                    /* respond to the connection */
				    strcpy(buffer,"Hello to NEW CLIENT DAVE \n please enter start to start playing hangman ");
				    strcat(buffer,connection[client]);
				    send(client,buffer,strlen(buffer),0);
 		                }//end of if
 		             else
				 {
				    puts("There is a child waiting for us");
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
				   }/* something has been received */
				 else
				   {
				      puts("\nWe recieved some infomation from a child "+fd);
				       buffer[r] = '\0';		/* terminate the string */
				       printf("buffer is %s",buffer);
				       /* if 'shutdown' sent... */
				      if( strcmp(buffer,"shutdown\n")==0 )
					{
					 puts("\nChild ended server");
				      	dave = false;		/* terminate the loop */
					}
					else if(strcmp(buffer,"start\n")==0)
					  {
					    puts("\nChild started HangMan ");
					    printf("\n %dchild Num ",i);
					    players[i].game_state= 'N';
					    players[i]=play_hangman(fd,fd,players[i]);
					    puts("\nCHild is done setting up hangman\n");
					    toString(players[i]);
					      
					  }/* otherwise, echo back the text */
					  else 
					  {
					      if(players[i].game_state=='U')
					      {
					          puts("\nChild is guessing again");
					      //r = read (fd, players[i].guess, MAXLEN,0);
                                                
 	
					       players[i].guess[0]=buffer[0];
					       toString(players[i]);
					        players[i]= play_hangman(fd,fd,players[i]);
					        printf("\n %d num fd ",i);
					        puts("\nChild left guessign area");
					       // printf("\nchild fd num %d",i);
					        //toString(players[i]);
					      }
					      else
					      {
					        puts("\n stuck  ");
					       sprintf ( players[i].outbuf, "Hey Please typed  start to begin ");
                                                 write (fd, players[i].outbuf, strlen (players[i].outbuf));//where we send it back to the user
 	
					      
					      }
					      printf("\nleft this place");
					    }
				       } /* end else to send/recv from client(s) */
			          } /* end if */
			    }//end of waiting if check
			            
		      i++;//this is for the individual games
		  }//for loop ending
		  
		  for(int j=0;j<MAXLEN;j++)
		  { 
		  if(children[j]!=NULL)
		  {
		    printf("\n Hey child %d",children[j]);
		    }
		  }
 			              
 		
 			
 	  //counter++;
 	  //printf("\ncounter num= %d",counter);
 	  if(counter==20)
 	    {
 	      dave=false;
 	      puts("\nGame over ");
 	    }
 	//puts("SO this is how loops work "+counter);
 		
 		
      }
 	
 	
 	return(0);
 }


