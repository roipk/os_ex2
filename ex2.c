/****************************
*   UserName:    roimd      *
*   Name:   Roi Madmon      *
*   ID:     203485164       *
*   Exercise 2              *
****************************/


#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>
#include<fcntl.h>
#include <errno.h>
#define BUFLEN 4096
#define NO_PIPE 0
#define CHANGE_OUTPUT 1
#define CHANGE_INPUT 2
#define CHANGE_ERRPUT 3
#define DEF 0
#define RESET -1


pid_t son_run = RESET;
pid_t son2_run = RESET;
pid_t son3_run = RESET;

/*-------------------------------------------------------------------------------*/
//The function counts how many arguments were entered from the writing line
int NumberOfWards(char *word, char *x)
{

   int n, count=DEF;
    n=strlen(word);
    for(int i=0;i<n;i++)
    {
        if(word[i]=='\"')
        {
            if(count==1)
                count--;
            else
               count++;
        }        
    }
    if(count > DEF)
        return RESET;
	int numword = DEF;
	char *ptr;
	ptr = strtok(word,x);

    if(ptr==NULL)
        return RESET;

	while(ptr != NULL)
	{
        numword++;
		ptr = strtok(NULL,x);
	}
    return numword;
}


/*-------------------------------------------------------------------------------*/
//A function that divides the arrays in the array
void arguments(char** arr,char *word, int i)
{
    int j=DEF;  
    char *temp=NULL, *ptr=NULL;
    temp=(char*)malloc((strlen(word)+1)*sizeof(char));//change
    strcpy(temp,word);
    ptr = strtok(temp,"\"\0");
    if(!strcmp(ptr,word))
    {
        ptr = strtok(word," \n\0");
	    while(ptr != NULL)
	    { 
            j=strlen(ptr);
            arr[i]=(char*)malloc((j+1)*sizeof(char));// change and work
		    strcpy(arr[i],ptr);
            i++;
            ptr = strtok(NULL," \n\0");
        }   

    }
    else
    {
       while(ptr != NULL)
	    { 
            j=strlen(ptr);
            arr[i]=(char*)malloc((j+1)*sizeof(char));//change and work
		    strcpy(arr[i],ptr);
            i++;
            ptr = strtok(NULL,"\"\n\0");
        }   
    }
    free(temp);
}



/*-------------------------------------------------------------------------------*/
//The function inserts the entered arguments into an array
void EnterWordToArray(char** arr,char *word,char *x)
{
    char *ptr=NULL;
    int i=DEF,j=DEF;
    ptr = strtok(word," \n");
	if(ptr != NULL)
	{ 
         j=strlen(ptr);
         arr[i]=(char*)malloc((j+1)*sizeof(char));//add +1
		 strcpy(arr[i],ptr);
         i++;   
    }
    ptr = strtok(NULL,"\n");
    if(ptr!= NULL)
        arguments(arr,ptr,i);
}



/*-------------------------------------------------------------------------------*/
//A function that returns the array after the argument is distributed
char **AddWord(char **arr,char *ptr,char* tempApostropy,char *temp,int j)
{
    arr=NULL;
    if(j==RESET)
    {
         arr=(char**)malloc((sizeof(char*)));
         arr[0]=NULL;    
    }
    else
    { 
        if(j != RESET)
            arr=(char**)malloc((j+1)*sizeof(char*));// todo
        if((strcmp(ptr,"\n")==DEF) && strlen(temp)>1)
        {
            ptr=strtok(tempApostropy,"\n");
            arr[0]=(char*)malloc(sizeof(char)+1);//add +1 no change wornings
            strcpy(arr[0],"");  
        }
        else
        {
            EnterWordToArray(arr,temp," \"\n");                     
        }
        arr[j]=NULL;
    } 
    return arr;
}

/*-------------------------------------------------------------------------------*/
//A function that frees the array memory
void freefromarray(char** arr,int num_start,int num_end)
{
    if (arr == NULL)
        return;
    for(int j=num_start;j<num_end;j++)
    {
        if (arr[j]!=NULL)
            free(arr[j]);
        arr[j]=NULL;
    }
    arr[num_start]=NULL;

}

/*-------------------------------------------------------------------------------*/
//The function creates a pipeline to communicate between processes
void createpipe(int *pipe_fd,int file_number,int std)
{
    int value =RESET;
    if(std==STDOUT_FILENO)
    {
        close(pipe_fd[0]);
        value = dup2(file_number,STDOUT_FILENO);
    }
    else if(std==STDIN_FILENO)
    {
        close(pipe_fd[1]);
        value = dup2(file_number,STDIN_FILENO); 
    }
    else if(std==STDERR_FILENO)
    {
        close(pipe_fd[0]);
        value = dup2(file_number,STDERR_FILENO); 
    }
	if(value ==RESET)
	{
	    fprintf(stderr,"dup2 failed\n");
	    exit(1);
	}
}

/*-------------------------------------------------------------------------------*/
//The function changes the output or input port into a pipe or file
void change(char** arr, int j,int *pipe_fd)
{
    if(j<3)
        return;
    int fd;
	char file[100];
    strcpy(file,arr[j-1]);
    if(strcmp(arr[j-2],"<")==DEF)
    {
        fd = open(file,O_RDONLY,S_IRUSR | S_IWUSR | S_IROTH);        
        createpipe(pipe_fd,fd,STDIN_FILENO); 
        freefromarray(arr,j-2,j);
    }
    
    else if(strcmp(arr[j-2],">")==DEF)
    {
        fd= open(file,O_WRONLY | O_CREAT ,S_IRUSR | S_IWUSR | S_IROTH);
        if(fd == RESET)
            exit(1);
        createpipe(pipe_fd,fd,STDOUT_FILENO);
        freefromarray(arr,j-2,j);
    }
    
 


   else if(strcmp(arr[j-2],">>")==DEF)
    {
        fd= open(file,O_WRONLY | O_CREAT | O_APPEND,S_IRUSR | S_IWUSR | S_IROTH);
        if(fd==RESET)
            exit(1);
        createpipe(pipe_fd,fd,STDOUT_FILENO);
        freefromarray(arr,j-2,j);
    }
    
    else if(strcmp(arr[j-2],"2>")==DEF)
    {
        fd= open(file,O_WRONLY | O_CREAT,S_IRUSR | S_IWUSR | S_IROTH);
        if(fd==RESET)
            exit(1);
        createpipe(pipe_fd,fd,STDERR_FILENO);
        freefromarray(arr,j-2,j);
    } 
}




/*-------------------------------------------------------------------------------*/
//A function captures signals sent in the program and responds to the sent signal
void sig_handler(int signo)
{
    
	signal(SIGINT, sig_handler);
	int status;
	if(signo==SIGINT)
    {
        if(son_run > DEF)
          kill(son_run,SIGINT);
        if(son2_run > DEF)
          kill(son2_run,SIGINT);
        if(son3_run > DEF)
          kill(son3_run,SIGINT);    
    }
    else if(signo==SIGCHLD)
    {

        if(son_run!=DEF)
            waitpid(son_run,&status, WNOHANG);                   
        if(son2_run!=DEF)
            waitpid(son2_run,&status, WNOHANG);
        if(son3_run!=DEF)
           waitpid(son3_run,&status, WNOHANG);
        else
            wait(NULL);  

    }
}






/*-------------------------------------------------------------------------------*/
//The function frees the array
void freearr(char** arr,int num_word)
{
    if (arr == NULL)
        return;
    for(int j=0;j<num_word;j++)
    {   
        if (arr[j] != NULL)
            free(arr[j]);
        arr[j] = NULL;
    }
    free(arr);
    arr = NULL;
}


/*-------------------------------------------------------------------------------*/
/*
The Maine gets a long string of commands and arguments
When a method is used to separate the accepted word into the right and left parts
If the pipe exists
If not held in the pipeline the program creates only a left part and runs the program.
If there is a left-handed pipe running the left program and a right-hand man will run the right program
 and the father will wait for new commands if the boys finish or the father will continue to receive more commands 
if one of the inputs was &
*/
int main()
{
    signal(SIGINT, sig_handler);
    signal(SIGCHLD, sig_handler);
    struct passwd pw ,*pwp;
    char buf1[BUFLEN]; 
    setpwent();
    getpwent_r(&pw,buf1,BUFLEN,&pwp);
    
    unsigned long Num_of_cmd=DEF, Cmd_length=DEF;
    unsigned long Num_cmd[2];
    
    char buf[BUFSIZ];
    char temp[510];
    char temppipe[510];
    char tempSize[510],tempApostropy[510],tempSpace[510];
    char **arr=NULL;
    char *ptr=NULL, *left=NULL,*right=NULL;
    int j=RESET,have_pipe,flag;
    pid_t t=0;
    int pipe_fd[2],pipe_command[2],pipe_folder[2] ;//create pipe
	unsigned long buff[2],n;
	printf("%s@%s>",pwp->pw_name,getcwd(buf, sizeof(buf)));
    fgets(temp,sizeof(temp),stdin);
    Num_cmd [0]=Num_of_cmd;
    Num_cmd [1]=Cmd_length;
    
    while(strcmp(temp,"done\n"))
    {
        flag=1;
        strcpy(temppipe,temp);
        ptr = strtok(temppipe,"|\0");
        left=(char*)malloc((strlen(ptr)+1)*sizeof(char));//change not check
        strcpy(left,ptr);
        if(strcmp(ptr,temp)==DEF)
        {
            have_pipe=NO_PIPE;//no pipe on string
            t=DEF;
        }
        else
        {
            ptr = strtok(NULL,"\0");
            right=(char*)malloc((strlen(ptr)+1)*sizeof(char));//change not check
            strcpy(right,ptr);
            have_pipe=CHANGE_OUTPUT;//have pipe on string
            if ((pipe(pipe_fd)) == RESET)
	        {
		        perror("cannot open pipe");
		        exit(EXIT_FAILURE) ;
	        }
             if ((pipe(pipe_command)) == RESET)
	        {
		        perror("cannot open pipe");
		        exit(EXIT_FAILURE) ;
	        }
        }        
         
        if ((pipe(pipe_folder)) == RESET)
	    {
		    perror("cannot open pipe");
		    exit(EXIT_FAILURE) ;
	    }

        if(have_pipe==CHANGE_OUTPUT)
        {
            t=fork();
            if(t==RESET)
            {
                printf("ERR\n");
                exit(1);
            }		
        }
        son_run=t;
        if(t==DEF)
        {   
            strcpy(tempSpace,left);
            ptr = strtok(tempSpace," \0");
            strcpy(tempSize,left);
            strcpy(tempApostropy,left);
            j=NumberOfWards(tempSize," \"\n");
            arr=AddWord(arr,ptr,tempApostropy,left,j);
            if(j>DEF)
            {
                if(strcmp(arr[j-1],"&")==DEF)
                {
                    flag = DEF;
                    freefromarray(arr,j-1,j);
                    raise(SIGCHLD);
                    j--;           
                } 
            }
            
           // free(left);
           //left = NULL;
            if(arr[0]!=NULL && !strcmp(arr[0],"cd"))
            {
                    if(arr[1]!=NULL)
                        chdir(arr[1]);         
            }
            else if(arr[0]!=NULL)
            {  
          
                Num_of_cmd++;
                if(arr[0]!=NULL)
                    Cmd_length = Cmd_length + strlen(arr[0]);
                if(have_pipe==CHANGE_OUTPUT)                
                {      
                    Num_of_cmd=1;
                    if(Cmd_length>DEF)
                        Cmd_length = strlen(arr[0]);    
                    close (pipe_command[0]);
                    Num_cmd[0] = Num_of_cmd;
                    Num_cmd[1] = Cmd_length;
                    write(pipe_command[1], &Num_cmd, 2*sizeof(unsigned long*));
                    close(pipe_command[1]);
                }
            } 
            if(have_pipe==CHANGE_OUTPUT)
                createpipe(pipe_fd,pipe_fd[1],STDOUT_FILENO);
 
                       
            t=fork();
            son2_run=t;
            if(t==RESET)
            {
                printf("ERR\n");
                exit(1);
            } 
            son2_run=t;
            if(t==DEF)
            {   
                if(j!=RESET)              
                    change(arr,j,pipe_folder);
		        if(execvp(arr[0],arr)!=0)
                   printf("%s: command not found\n",arr[0]);
                 //freearr(arr,j);
                 exit(DEF);
            }
            freearr(arr,j);
            if(have_pipe==CHANGE_OUTPUT)
            {
                exit(0);
            }
            
            
        }
        
        else        
        {   
            flag =1;
            t=fork();
            if(t==RESET)
            {
                printf("ERR\n");
                exit(1);
            }
            son3_run=t;
            if(t==DEF)
	        {       
		        strcpy(tempSpace,right);
                ptr = strtok(tempSpace," \0");
                strcpy(tempSize,right);
                strcpy(tempApostropy,right);
                j=NumberOfWards(tempSize," \"\n");
                arr=AddWord(arr,ptr,tempApostropy,right,j);
              //free(right);
               //  right=NULL;
                createpipe(pipe_fd,pipe_fd[0],STDIN_FILENO);
                change(arr,j,pipe_folder);
		        if(execvp(arr[0],arr)!=DEF)
                    printf("%s: command not found\n",arr[0]);
                exit(DEF);
            }
            
            //freearr(arr,j);
            close (pipe_command[1]);
            n=read(pipe_command[0],&buff,2*sizeof(unsigned long*));
	        if(n<=DEF)
	        {
                perror("read failed\n");
                exit(EXIT_FAILURE);
	        }
            Num_of_cmd+=buff[0];
            Cmd_length+=buff[1];
            close(pipe_command[0]);    
            close(pipe_fd[1]); 
            close(pipe_fd[0]);         
        }

      
    if(left != NULL) 
        free(left);
    if(right != NULL)
        free(right);
    if(flag == 1)
        pause();

	printf("%s@%s>",pwp->pw_name,getcwd(buf, sizeof(buf)));
    fgets(temp,sizeof(temp),stdin);
    }
    endpwent(); 
    printf("Num of cmd: %lu\n",Num_of_cmd);
    printf("Cmd length: %lu\n",Cmd_length);
    printf("Bye !\n");

    return DEF;
}









