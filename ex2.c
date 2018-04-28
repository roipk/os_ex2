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
#define HAVE_PIPE 1
#define NO_PIPE 0


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
        {
            //printf("befor split\n");   
           // printf("This is the child process. My pid is %d  , ptr= |%s|\n", getpid(),temp);
            EnterWordToArray(arr,temp," \"\n");         
            //printf("after split\n");
        }            
    }
    else
        arr[0]=NULL;

    if(arr[0]!=NULL && !strcmp(arr[0],"cd"))
    {
            if(arr[1]!=NULL)
                chdir(arr[1]);         
    }
   // else if(arr[0]!=NULL)
   // {
        //Num_of_cmd++;
        //if(arr[0]!=NULL)
        //Cmd_length = Cmd_length + strlen(arr[0]);
    //}   
return arr;
}




/*-------------------------------------------------------------------------------*/
//The function frees the array
void freearr(char** arr,int num_word)
{
//printf("ttttt= |%s|\n",arr[0]);
//printf("innnnnnnnnnn freeeeeee\n");
    for(int j=0;j<num_word;j++)
        free(arr[j]);
    free(arr);
}


/*-------------------------------------------------------------------------------*/
int main()
{

    struct passwd pw ,*pwp;
    char buf1[BUFLEN];
    setpwent();
    getpwent_r(&pw,buf1,BUFLEN,&pwp);
    
    int value=-1;

   unsigned long Num_of_cmd=0, Cmd_length=0;
    char buf[BUFSIZ];
    char temp[510];
    char temppipe[510];
    char tempSize[510],tempApostropy[510],tempSpace[510];
    char **arr;
    char *ptr=NULL, *left=NULL,*right=NULL;
    int j=-1,have_pipe;
    pid_t t;
    int pipe_fd[2] ;//create pipe
	
	printf("%s@%s>",pwp->pw_name,getcwd(buf, sizeof(buf)));
    fgets(temp,sizeof(temp),stdin);
    
    
    while(strcmp(temp,"done\n"))
    {
        
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
            have_pipe=HAVE_PIPE;//have pipe on string
            if ((pipe(pipe_fd)) == -1)
	        {
		        perror("cannot open pipe");
		        exit(EXIT_FAILURE) ;
	        }
        }        


        if(have_pipe==HAVE_PIPE)
        {
            t=fork();
            if(t==-1)
            {
                printf("ERR\n");
                exit(1);
            }		
        }
        if(t==0)
        {
            //printf("This is the child process left. My pid is %d\n", getpid());
            strcpy(tempSpace,left);
            ptr = strtok(tempSpace," \0");
            strcpy(tempSize,left);
            strcpy(tempApostropy,left);
            j=NumberOfWards(tempSize," \"\n");
            arr=AddWord(arr,ptr,tempApostropy,left,j);
            //printf("command left in arr[0] = |%s|\n",arr[0]);
            if(have_pipe==HAVE_PIPE)
            {
                close(pipe_fd[0]);
                value = dup2(pipe_fd[1],STDOUT_FILENO);
	            if(value ==-1)
	            {
		            fprintf(stderr,"dup2 failed\n");
		            exit(1);
	            }
            }
            else
            {
               t=fork();
                if(t==-1)
                {
                    printf("ERR\n");
                    exit(1);
                } 
            }
            if(t==0)
            {
                // printf("test1\n");
                 //fprintf(stderr,"test2\n");
               //printf("This is the child process. My pid is %d and my parent's id is %d.\n", getpid(), getppid());
                
		        if(execvp(arr[0],arr)!=0)
                   printf("%s: command not found\n",arr[0]);
                 exit(0);
            }
            
        }
        
        else        
        {   
            int pidchildleft=getpid();  
            t=fork();
            if(t==-1)
            {
                printf("ERR\n");
                exit(1);
            }
            int pidchildright=getpid();
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
                arr=AddWord(arr,ptr,tempApostropy,right,j);
                //printf("command right in arr1[0] = |%s|\n",arr[0]);   
                close(pipe_fd[1]);
                value = dup2(pipe_fd[0],STDIN_FILENO);
                if(value ==-1)
	            {
	                fprintf(stderr,"dup2 failed\n");
		            exit(1);
	            }
                //printf("This is the child process. My pid is %d and my parent's id is %d.\n", getpid(), getppid());
		        if(execvp(arr[0],arr)!=0)
                    printf("%s: command not found\n",arr[0]);
                exit(0);
            }
            
            
            
        waitpid(pidchildleft,NULL,0);//whait to child 
        close(pipe_fd[1]); 
       // fprintf(stderr,"son left done and closed\n");        
        waitpid(pidchildright,NULL,0);//whait to child 
        close(pipe_fd[0]);
        //fprintf(stderr,"son right done and closed\n");        
        } 

    
            
    wait(NULL);//whait to child 
    wait(NULL);//whait to child
    freearr(arr,j);
	printf("%s@%s>",pwp->pw_name,getcwd(buf, sizeof(buf)));
    fgets(temp,sizeof(temp),stdin);
    }
    
    printf("Num of cmd: %lu\n",Num_of_cmd);
    printf("Cmd length: %lu\n",Cmd_length);
    printf("Bye !\n");
    

return 0;
}








