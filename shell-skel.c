// name: sooryanarayanan s
// snu id: 1610110374
// ss432

#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LINE 80

//tokenising user input
int tokenise(char *args[MAX_LINE],char string[MAX_LINE])
{	
	int i=0;
	char *token = strtok(string," ");
	while(token != NULL)
	{
		args[i++] = token;
		token = strtok(NULL," ");
	 }
	args[i] = NULL;

	return i;
}

//opening file .osh_history
void writeToFile(char command[MAX_LINE])
{
	FILE *writing;
	writing = fopen(".osh_history","a");

	if(writing == NULL)
		printf("unable to open file\n");

	//writing to file
	else
	{	
		fputs(command,writing);
		fputs("\n",writing);
		fclose(writing);	
	}

	return;
}

int main(int argc, char const *argv[])
{
	char *args[MAX_LINE/2 + 1],command[MAX_LINE],command_copy[MAX_LINE];
	int should_run = 1,i=0,status,flag=1;
	pid_t pid;

	while(should_run)
	{
		//printing the shell prompt and getting user input
		printf("osh>");
		fflush(stdout);
		fgets(command,sizeof command,stdin);
		command[strlen(command)-1] = '\0';
		strcpy(command_copy,command); //used because strtok changes the value of command
		
		i = tokenise(args,command);

		//if user input is quit, terminate the program
		if (!strcmp(args[0],"quit"))
		 		should_run = 0;

		//history command
		else if(!strcmp(args[0],"history"))
		{
			//reading the file
			FILE *reading;
			char *line;
			reading = fopen(".osh_history","r");

			for(int count =1; !feof(reading); count++)
			{
				fgets(line,sizeof command,reading);
				printf("%d %s",count,line);	
			}

			fclose(reading);			
		}

		//using the history
		if (command[0] == '!')
		{
			//breaking the command into an integer to read the file
			char *line,temp[5];
			int temp_int;
			strcpy(temp,args[0]);
			temp[0] = '0';
			sscanf(temp,"%d",&temp_int);

			line = malloc(100);						

			//reading the file
			FILE *reading;
			reading = fopen(".osh_history","r");

			for (int i = 0; i < temp_int; ++i)
				fgets(line,sizeof command,reading);	
			
			//get the most recent entry if command starts with !!
			if(command[1] == '!')
			{
				while(!feof(reading))
					fgets(line,sizeof command,reading);
			}

			//strip the retrieved command of the final \n 
			line[strlen(line)-1] = '\0';
			printf("%s\n",line);
			writeToFile(line);
			flag = 0;
			i = tokenise(args,line); //the new arguments will be from the temp_int line of file
			//free(line);
			fclose(reading);
		}
		
		if(flag)
			writeToFile(command_copy);

		flag = 1;

		//else run the user input
		pid = fork();
		
		//if fork fails
		if (pid < 0)
			printf("fork failed\n");
		
		//child process
		else if (pid == 0)
		{
			//printf("%s\n",args[0] ); was to check if frre(line); was causing any problems
			execvp(args[0], args);
			exit(0);
		}

		//parent process waits if user input doesnt end with &
		else
		{
			//printf("%c\n",args[--i][strlen(args[i])-1] );
			if (args[--i][strlen(args[i])-1] != '&')
				wait(&status);
			else
				printf("child process at %d\n",pid);
		}
	}
	return 0;
}