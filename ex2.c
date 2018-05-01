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
pid_t son_run=-1;
pid_t son2_run=-1;

//The function counts how many arguments were entered from the writing line
int NumberOfWards(char *word, char *x)
{
   int n, count=0;
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
   // printf("count=%d\n",count);
    if(count>0)
        return -1;
    //printf("in\n");
	int numword = 0;
	char *ptr;
	ptr = strtok(word,x);
	while(ptr != NULL)
	{
        numword++;
		ptr = strtok(NULL,x);
	}
    return numword;
}




void arguments(char** arr,char *word, int i)
{
    int j=0;  
    char *temp, *ptr;
    temp=(char*)malloc(strlen(word)*sizeof(char));
    strcpy(temp,word);
    ptr= strtok(temp,"\"\0");
    if(!strcmp(ptr,word))
    {
        ptr = strtok(word," \n\0");
	    while(ptr != NULL)
	    { 
            j=strlen(ptr);
            arr[i]=(char*)malloc(j*sizeof(char));
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
            arr[i]=(char*)malloc(j*sizeof(char));
		    strcpy(arr[i],ptr);
            i++;
            ptr = strtok(NULL,"\"\n\0");
        }   
    }
}



/*-------------------------------------------------------------------------------*/
//The function inserts the entered arguments into an array
void EnterWordToArray(char** arr,char *word,char *x)
{
    char *ptr;
    int i = 0,j=0;
    ptr = strtok(word," \n");
	if(ptr != NULL)
	{ 
         j=strlen(ptr);
         arr[i]=(char*)malloc(j*sizeof(char));
		 strcpy(arr[i],ptr);
         i++;   
    }
    ptr = strtok(NULL,"\n");
    if(ptr!= NULL)
        arguments(arr,ptr,i);
}




char **AddWord(char **arr,char *ptr,char* tempApostropy,char *temp,int j)
{
    if(j!= -1)
        arr=(char**)malloc((j+1)*sizeof(char*));
    else
         arr=(char**)malloc((sizeof(char*)));
    if(arr == NULL)
    {
        printf("ERR\n");
        exit(1);
    }
    if(j!=-1)
    { 
        arr[j]=NULL; 
        if(!strcmp(ptr,"\n") && strlen(temp)>1)
        {
            ptr=strtok(tempApostropy,"\n");
            arr[0]=(char*)malloc(sizeof(char));
            strcpy(arr[0],"");  
        }
        else
            EnterWordToArray(arr,temp," \"\n");                     
    }
    else
        arr[0]=NULL;  
    return arr;
}


/*
void pipewrite(int *pipe_fd)
{
    int value =-1;
    close(pipe_fd[0]);
    value = dup2(pipe_fd[1],STDOUT_FILENO);
	if(value ==-1)
	{
	    fprintf(stderr,"dup2 failed\n");
	    exit(1);
	}
}

void piperead(int *pipe_fd)
{
    int value=-1;
    close(pipe_fd[1]);
    value = dup2(pipe_fd[0],STDIN_FILENO);
	if(value ==-1)
	{
	    fprintf(stderr,"dup2 failed\n");
	    exit(1);
	}
}
*/


void freefromarray(char** arr,int num_start,int num_end)
{
//printf("ttttt= |%s|\n",arr[0]);
//printf("innnnnnnnnnn freeeeeee\n");
    for(int j=num_start;j<num_end;j++)
        free(arr[j]);
    arr[num_start]=NULL;
}


void createpipe(int *pipe_fd,int file_number,int std)
{
    int value =-1;
    if(std==STDOUT_FILENO)
    {
        //printf("file number is: %d\n",file_number);
        close(pipe_fd[0]);
        //printf(" pipe closed\n");
        value = dup2(file_number,STDOUT_FILENO);
        //fprintf(stderr,"value = %d\n",value);
    }
    else if(std==STDIN_FILENO)
    {
        //printf("file number2 is: %d\n",file_number);
        close(pipe_fd[1]);
        value = dup2(file_number,STDIN_FILENO); 
    }
    else if(std==STDERR_FILENO)
    {
        close(pipe_fd[0]);
        value = dup2(file_number,STDERR_FILENO); 
    }
	if(value ==-1)
	{
	    fprintf(stderr,"dup2 failed\n");
	    exit(1);
	}
    //fprintf(stderr,"pipe create done\n");
}


void change(char** arr, int j,int *pipe_fd)
{
   // char* temp;
    if(j<3)
        return;
    //printf("in1\n");
    //int have_assignment=0;   
    int fd;
	char file[100];
    strcpy(file,arr[j-1]);
    if(strcmp(arr[j-2],"<")==0)
    {
        fd = open(file,O_RDONLY,S_IRUSR | S_IWUSR | S_IROTH);        
        createpipe(pipe_fd,fd,STDIN_FILENO); 
       // have_assignment=CHANGE_INPUT;
        freefromarray(arr,j-2,j);
    }
    
    else if(strcmp(arr[j-2],">")==0)
    {
        fd= open(file,O_WRONLY | O_CREAT ,S_IRUSR | S_IWUSR | S_IROTH);
        if(fd==-1)
            exit(1);
        createpipe(pipe_fd,fd,STDOUT_FILENO);
        //shave_assignment=CHANGE_OUTPUT;
        //printf("done\n");
        freefromarray(arr,j-2,j);
    }
    
    else if(strcmp(arr[j-2],">>")==0)
    {
        fd= open(file,O_WRONLY | O_CREAT | O_APPEND,S_IRUSR | S_IWUSR | S_IROTH);
        if(fd==-1)
            exit(1);
        createpipe(pipe_fd,fd,STDOUT_FILENO);
        //have_assignment=CHANGE_OUTPUT;
        freefromarray(arr,j-2,j);
    }
    
    else if(strcmp(arr[j-2],"2>")==0)
    {
        fd= open(file,O_WRONLY | O_CREAT,S_IRUSR | S_IWUSR | S_IROTH);
        if(fd==-1)
            exit(1);
        createpipe(pipe_fd,fd,STDERR_FILENO);
        //have_assignment=CHANGE_ERRPUT;
        freefromarray(arr,j-2,j);
    }

    
    
   // return have_assignment;
}





void sig_handler(int signo)
{
    
	signal(SIGINT, sig_handler);
	int status;
	if(signo==SIGINT)
    {
        if(son_run!=0)
          kill(son_run,SIGINT);
             
    }
    else if(signo==SIGCHLD)
    {
        //printf("in\n");
        if(son_run!=0)
            waitpid(son_run,&status, WNOHANG);                   
        if(son2_run!=0)
            waitpid(son2_run,&status, WNOHANG);
        else
            wait(NULL);
             
    }
}






/*-------------------------------------------------------------------------------*/
//The function frees the array
void freearr(char** arr,int num_word)
{
//printf("ttttt= |%s|\n",arr[0]);
//printf("innnnnnnnnnn freeeeeee\n");
                //(stderr,"num word %d\n",num_word);   
    for(int j=0;j<num_word;j++)
        free(arr[j]);
    free(arr);
}


/*-------------------------------------------------------------------------------*/
int main()
{
    signal(SIGINT, sig_handler);
    signal(SIGCHLD, sig_handler);
    struct passwd pw ,*pwp;
    char buf1[BUFLEN];
    setpwent();
    getpwent_r(&pw,buf1,BUFLEN,&pwp);
    unsigned long Num_of_cmd=0, Cmd_length=0;
    unsigned long Num_cmd[2];
    
    char buf[BUFSIZ];
    char temp[510];
    char temppipe[510];
    char tempSize[510],tempApostropy[510],tempSpace[510];
    char **arr;
    char *ptr=NULL, *left=NULL,*right=NULL;
    int j=-1,have_pipe,flag;
    pid_t t;
    int pipe_fd[2],pipe_command[2],pipe_folder[2] ;//create pipe
	unsigned long buff[2],n;
	printf("%s@%s>",pwp->pw_name,getcwd(buf, sizeof(buf)));
    fgets(temp,sizeof(temp),stdin);
    Num_cmd [0]=Num_of_cmd;
    Num_cmd [1]=Cmd_length;
    //Num_cmd [2]=j;
    
    while(strcmp(temp,"done\n"))
    {
        flag=1;
        strcpy(temppipe,temp);
        ptr = strtok(temppipe,"|\0");
        left=(char*)malloc(strlen(ptr)*sizeof(char));
        strcpy(left,ptr);
        // printf("command left is: |%s|\n",left);  
        if(strcmp(ptr,temp)==0)
        {
            have_pipe=NO_PIPE;//no pipe on string
            t=0;
        }
        else
        {
            ptr = strtok(NULL,"\0");
            right=(char*)malloc(strlen(ptr)*sizeof(char));
            strcpy(right,ptr);
            have_pipe=CHANGE_OUTPUT;//have pipe on string
            if ((pipe(pipe_fd)) == -1)
	        {
		        perror("cannot open pipe");
		        exit(EXIT_FAILURE) ;
	        }
             if ((pipe(pipe_command)) == -1)
	        {
		        perror("cannot open pipe");
		        exit(EXIT_FAILURE) ;
	        }
        }        
         
        if ((pipe(pipe_folder)) == -1)
	    {
		    perror("cannot open pipe");
		    exit(EXIT_FAILURE) ;
	    }

        if(have_pipe==CHANGE_OUTPUT)
        {
            t=fork();
            if(t==-1)
            {
                printf("ERR\n");
                exit(1);
            }		
        }
        son_run=t;
        if(t==0)
        {   
            //printf("This is the child process left. My pid is %d\n", getpid());
            strcpy(tempSpace,left);
            ptr = strtok(tempSpace," \0");
            strcpy(tempSize,left);
            strcpy(tempApostropy,left);
            j=NumberOfWards(tempSize," \"\n");
            //fprintf(stderr,"left son - j = %d\n",j);   
            arr=AddWord(arr,ptr,tempApostropy,left,j);
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
                    if(Cmd_length>0)
                        Cmd_length = strlen(arr[0]);    
                    close (pipe_command[0]);
                    Num_cmd[0] = Num_of_cmd;
                    Num_cmd[1] = Cmd_length;
                    write(pipe_command[1], &Num_cmd, 2*sizeof(unsigned long*));
                    close(pipe_command[1]);
                }
            } 
            //printf("command left in arr[0] = |%s|\n",arr[0]);
            if(have_pipe==CHANGE_OUTPUT)
            {
                createpipe(pipe_fd,pipe_fd[1],STDOUT_FILENO);
            }
   
                if(strcmp(arr[j-1],"&")==0)
                {
                    flag = 0;
                    freefromarray(arr,j-1,j);
                    raise(SIGCHLD);
                }  
                else
                    flag =1;       
          //  fprintf(stderr,"in1\n");
            t=fork();
            if(t==-1)
            {
                printf("ERR\n");
                exit(1);
            } 
            son_run=t;
            if(t==0)
            {                 
                change( arr,j,pipe_folder);
		        if(execvp(arr[0],arr)!=0)
                   printf("%s: command not found\n",arr[0]);
                 exit(0);
            }
            //fprintf(stderr,"in2\n");
            freearr(arr,j);
            if(have_pipe==CHANGE_OUTPUT)
            {
                exit(0);
            }
            
            
        }
        
        else        
        {   
           // int pidchildleft=getpid();  
            t=fork();
            if(t==-1)
            {
                printf("ERR\n");
                exit(1);
            }
            son2_run=t;
           // int pidchildright=getpid();
            if(t==0)
	        {       
                // printf("This is the child process right. My pid is %d \n", getpid());
                //printf("command right is: |%s|\n",right); 
		        strcpy(tempSpace,right);
                ptr = strtok(tempSpace," \0");
                //printf("command ptr right is: |%s|\n",ptr); 
                strcpy(tempSize,right);
                strcpy(tempApostropy,right);
                j=NumberOfWards(tempSize," \"\n");
                //fprintf(stderr," right son - j = %d\n",j);  
                arr=AddWord(arr,ptr,tempApostropy,right,j);
                createpipe(pipe_fd,pipe_fd[0],STDIN_FILENO);
                change(arr,j,pipe_folder);
		        if(execvp(arr[0],arr)!=0)
                    printf("%s: command not found\n",arr[0]);
                freearr(arr,j);
                exit(0);
            }
            
            
            close (pipe_command[1]);
            n=read(pipe_command[0],&buff,2*sizeof(unsigned long*));
	        if(n<=0)
	        {
                perror("read failed\n");
                exit(EXIT_FAILURE);
	        }
            Num_of_cmd+=buff[0];
            Cmd_length+=buff[1];
            close(pipe_command[0]);    
           // waitpid(pidchildleft,NULL,0);//whait to child 
            close(pipe_fd[1]); 
            // fprintf(stderr,"son left done and closed\n");        
            //waitpid(pidchildright,NULL,0);//whait to child 
            close(pipe_fd[0]);
        //fprintf(stderr,"son right done and closed\n");        
           
        }
    printf("flag: %d", flag);
    if(flag == 1)
        pause();
    //wait(NULL);//whait to child         
    //wait(NULL);//whait to child 
    //wait(NULL);//whait to child
    //fprintf(stderr,"j done = %d\n",j);  
	printf("%s@%s>",pwp->pw_name,getcwd(buf, sizeof(buf)));
    fgets(temp,sizeof(temp),stdin);
    }  
    
    printf("Num of cmd: %lu\n",Num_of_cmd);
    printf("Cmd length: %lu\n",Cmd_length);
    printf("Bye !\n");
    

return 0;
}









