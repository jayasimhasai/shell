#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <wait.h>
#include <string.h>
#include <pthread.h>
#include <sys/resource.h>
#include "queue.h"


int fg_pid=0;
int fg_suspended=0;
int run=1;
int trigger=1;
int handler=1;	
pthread_mutex_t mutex;
struct queue pid_list,sq[10],sjf;
struct Par{
	int schedpolicy;
	int qt;
	int qc;
};
struct Par *par={0};

void help() {
	printf("This is manual page\n");
	printf("This shell supports the following commands:\n");
	printf("\tver\n\texec\n\tps\n\tkill\tset_scheduling\thelp\n\texit\n");
	printf("For more details please type 'help command'\n");
}

void helpcmd(char *cmd) {
	printf("This is manual page\n\n");
	if (strcmp(cmd,"ver")==0)
	{
		printf("\nver:\tShows details about the shell version\n");
	}
	else if (strcmp(cmd,"exec")==0) 
	{
		printf("\nexec p1(n1,qt1) p2(n2,qt2) ...:\nExecutes the programs p1, p2 ...\nEach program types a message for n times and it is given a time quantum of qt msec.\n");
		printf("If parameter (&) is given the program will be executed in the background\n");
	}
	else if (strcmp(cmd,"ps")==0)
	{
		printf("\nps:\tShows the living process with the given pid\n");
	}
	else if (strcmp(cmd,"kill")==0)
	{
		printf("\nkill pid:\tEnds the process with the given pid\n");
	}
	else if (strcmp(cmd,"help")==0)
	{
		printf("\nhelp:\tYou should know this command by now\n");
	}
	else if (strcmp(cmd,"exit")==0)
	{
		printf("\nexit:\tEnds the experience of working in the new shell\n");
	}
	else if (strcmp(cmd,"set_scheduling")==0)
	{
		printf("\nset_scheduling:\tsets the scheduling policy for the processes to RR,MFQ,FCFS,SJF\n");
	}
	else 
	{
		printf("\nNo Such command. Type help to see a list of commands\n");
	}
}

void ver() {
	printf("\nNew Shell. Works properly!!\n");
}

void ps() {
	struct node *p;
	printf("\nNEW SHELL presents the following living processes\n");
	printf("\tPID\tNAME\n");
	for (p=pid_list.head; p!=NULL; p=p->next)
	{
		printf("\t%d\t%s\n",p->pid,p->name);
	}
}

void mykill(int pid) {
	kill(pid,SIGTERM);
	printf("You have just killed process %d\n",pid );
}

void childdead(int signum){
	int dead_pid, status;
	signal(SIGCHLD,childdead);
	
	handler=0;
	dead_pid=wait(&status);
	printf("The child %d is dead\n",dead_pid );

	delete(&pid_list,dead_pid);
	
	if (dead_pid==fg_pid)
	{
		fg_pid=0;
	}
	
	kill(getpid(),SIGUSR2);
}
void brkpause(int signum)
{
signal(SIGUSR2,brkpause);
pthread_mutex_unlock(&mutex);
}

void *RR(void *dum)
{		sleep(2);
			while(sq[0].head!=NULL)// scheduling loop
			{	
			
				kill(sq[0].head->pid,SIGCONT);
				usleep(1000*par->qt);
				

			 	if(handler)
				{
				    kill(sq[0].head->pid,SIGUSR1); 
				    usleep(1000);
				    enqueue(dequeue(&sq[0]),NULL,0,&sq[0]);   
				   
				  /* dequeue and re-enqueue */
				    }

				else
				  {
				    /* remove from list */
				   dequeue(&sq[0]);
				    handler=1;
				    
				    /* mark chils as dead */
				    }
			       
			    }
}
void *MFQ(void *dum)
{		  sleep(2);
		     for(int i=0;i<par->qc;i++)
			while(sq[i].head!=NULL)// scheduling loop
			{	
			    // printf("%d %d", par->schedpolicy,par->qt);
			      /* send the signal SIGCONT to the first element in queue */
				
				kill(sq[i].head->pid,SIGCONT);
				usleep(1000*(2*i+1)*par->qt);
				

			 	if(handler)
				{
				    kill(sq[i].head->pid,SIGUSR1); 
				    usleep(1000);
				    if(i==par->qc-1)
				    enqueue(dequeue(&sq[i]),NULL,0,&sq[i]);  
				    else
					enqueue(dequeue(&sq[i]),NULL,0,&sq[i+1]); 
				   
				  /* dequeue and re-enqueue */
				    }

				else
				  {
				    /* remove from list */
				   dequeue(&sq[i]);
				    handler=1;
				    
				    /* mark chils as dead */
				    }
			       
			    }
}
void *FCFS(void *dum)
{
		sleep(2);
			while(sq[0].head!=NULL)// scheduling loop
			{	signal(SIGUSR2,brkpause);
				pthread_mutex_lock(&mutex);
			
				kill(sq[0].head->pid,SIGCONT);
				
				pthread_mutex_lock(&mutex);
				pthread_mutex_unlock(&mutex);
				
				   dequeue(&sq[0]);
				    handler=1;
			       
			    }
		}
void *SJF(void *dum)
{
		
			sleep(2);
			while(sq[0].head!=NULL)// scheduling loop
			{	
				//sort(&sq[0],&sjf);
				
				while(sjf.head!=NULL)
				{
				signal(SIGUSR2,brkpause);
				pthread_mutex_lock(&mutex);

				kill(sjf.head->pid,SIGCONT);

				printf("%d %d %d", par->schedpolicy,par->qt,sjf.head->pid);
				pthread_mutex_lock(&mutex);
				pthread_mutex_unlock(&mutex);
				   dequeue(&sjf);
				    handler=1;
			       }
			    }
		}

void scheduling(char* policy)
{
	par= (struct Par *)malloc(sizeof(struct Par));
        if (strcmp(policy,"RR")==0)
	{
		printf("enter quntum time:");
		scanf("%d",&par->qt);
		par->schedpolicy=0;
	}
	else if (strcmp(policy,"MFQ")==0)
	{
		printf("enter quntum time:");
		scanf("%d",&par->qt);
		printf("enter number of queues:");
		scanf("%d",&par->qc);
		par->schedpolicy=1;
	}
	else if (strcmp(policy,"FCFS")==0)
	{
		
		par->schedpolicy=2;
	}
	
	else if (strcmp(policy,"SJF")==0)
	{
		par->schedpolicy=3;
	}
	else
	{
	printf("no such policy\n policies list:\n\t RR\n\t MFQ\n\t FCFS\n\t SJF\n");
	return;
	}

	
}

void exec(char *input) {
	int i,t,status;
	char *args[10];
	char *temp;
	
	struct node *p;

	for (i = 0; i < 10; i++)
	{
		args[i]=(char *)malloc(10*sizeof(char));
	}

	strcpy(args[0],strtok(input,"(,"));
	for (i=1; (temp=strtok(NULL,",)"))!=NULL; i++) 
		strcpy(args[i],temp);
	printf("\n");
	if (strcmp(args[i-1],"&")==0)
	{
		args[i-1]=NULL;
	}
	else
		args[i]=NULL;
	if ((t=fork())==0)
	{
		execv(args[0],args);
	}
	enqueue(t,args[0],0,&pid_list);
	enqueue(t,args[0],atoi(args[1])*atoi(args[2]),&sq[0]);
	insert(t,args[0],atoi(args[1])*atoi(args[2]),&sjf);
	/*if (args[i-1]!=NULL)
	{
		fg_pid=t;
		while(fg_pid!=0 && fg_suspended!=1)
			pause();
	}*/

	


}


void myexit() {
	char yesno;
	if (pid_list.head==pid_list.tail)
	{
		run=0;
	}
	else {
		printf("There are still living processes Do you want to kill them? (y/n): ");
		yesno=getchar();
		if (yesno == 'y')
		{
			while(pid_list.head!=pid_list.tail)
			{
				mykill(pid_list.tail->pid);
				usleep(10000);
			}
			run=0;
		}
	}
}


void susp (int signum) {
	fg_suspended=1;
	printf("All processes supspended\n");
}

void cont (int signum) {
	fg_suspended=0;
	printf("Waking all processes...\n");
	while (fg_pid!=0 && fg_suspended!=1)
		pause();
}

int main(int argc, char const *argv[])
{
	char input[15][30];
	int argnum, i;
	long dum;
	pthread_t threads[1];
	pid_list.head=NULL;
	pid_list.tail=NULL;
	enqueue(getppid(),"NEW SHELL",0,&pid_list); 

	signal(SIGCHLD,childdead);
	signal(SIGTSTP,susp);
	signal(SIGQUIT,cont);
	setpriority(PRIO_PROCESS,0,-20);

	ver();
	

	while (run){
		printf("=>");
		for (argnum=0; (scanf("%s",&input[argnum]))==1;argnum++)
			if (getchar()=='\n') break;
		if (strcmp(input[0],"ver")==0 && argnum==0) ver();
		else if (strcmp(input[0],"help")==0 && argnum==0) help();
		else if (strcmp(input[0],"help")==0 && argnum==1) helpcmd(input[1]);
		else if (strcmp(input[0],"ps")==0 && argnum==0) ps();
		else if (strcmp(input[0],"kill")==0 && argnum==1) mykill(atoi(input[1]));
		else if (strcmp(input[0],"set_scheduling")==0 && argnum==1) scheduling(input[1]);
		else if (strcmp(input[0],"exec")==0 && argnum!=0) 
			{
			for (i=1; i<=argnum; i++) exec(input[i]);
			if(par->schedpolicy==0)
			pthread_create(&threads[0], NULL, RR, (void *)dum);
			else if(par->schedpolicy==1)
			pthread_create(&threads[0], NULL, MFQ, (void *)dum);
			else if(par->schedpolicy==2)
			pthread_create(&threads[0], NULL, FCFS, (void *)dum);
			else if(par->schedpolicy==3)
			pthread_create(&threads[0], NULL, SJF, (void *)dum);
			}
		else if (strcmp(input[0],"exit")==0 && argnum==0) myexit();
	    else printf("Nosuch command. Check help for help.\n");
	}

}
