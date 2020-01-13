
#define  _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_LENGTH 4096 

int main(int argc, char* argv[])
{
	char fpath[MAX_LENGTH] = "";
	char complete[MAX_LENGTH] = "";
	
	if(argc>1)
	{
	    char input[1024];
	    strcpy(input, argv[1]);

	    if(argv[1][0] == '/')
	    {
		 printf("ABSOLUTE PATH AT THE INPUT\n");
		 strcpy(fpath, input);
	    }else
	    {
		 printf("RELATIVE PATH AT THE INPUT\n");
		 char cwd[2048];
		 getcwd(cwd, sizeof(cwd));
		 strcat(fpath, cwd);
		 strcat(fpath, "/");
		 strcat(fpath, input);
	    }
	}else
	{
	     char cwd[2048];
	     getcwd(cwd, sizeof(cwd));
	     strcat(fpath, cwd);
	}

	//Read path from /etc/environment

	FILE *fptr;
    char ch;
    char path[MAX_LENGTH] = "";
    
    fptr = fopen("/etc/environment", "r");// /etc/environment
    
    if (fptr == NULL)
    {
        printf("Encountered problem when opening file! Are you not on SUDO?");
        exit(0);
    }
    ch = fgetc(fptr);
    while (ch != EOF)
    {
        strncat(path, &ch, 1);
        ch = fgetc(fptr);
    }
	
    fclose(fptr);
    
    //Check if parameter exist in path variable
	
	char* start = strstr(path, fpath);
	if(start != NULL)
	{
		int start_count = strlen(start);
		int fpath_count = strlen(fpath);
		int path_count = strlen(path);
		
		strncpy(complete, path, path_count-start_count);
		strcat(complete, start+fpath_count);
		
		printf("Path removed from $PATH\n");
	}else
	{
		int len = strlen(path);
		strncpy(complete, path, len-2); //len-1 dla Windows, len-2 dla Linuxa
		strcat(complete, ":");
		strcat(complete, fpath);
		strcat(complete, "\"");
		printf("Path appended to $PATH\n");
	}
	
	//Check if there are '::' characters left and reduce them to ':'
	
	char* check = strstr(complete, "::");
	if(check!=NULL)
	{
		int check_count = strlen(check);
		int compl_count = strlen(complete);
		
		strncpy(fpath, complete, compl_count-check_count);
		strcat(fpath, check+1);
		strcpy(complete, fpath);
	}
	
	//Update file & Request re-login to apply changes;
	
	fptr = fopen("/etc/environment", "w");
	if (fptr == NULL)
    {
        printf("Encountered problem when saving changes! Are you not on SUDO?");
        exit(0);
    }
	fprintf(fptr, "%s", complete);
	fclose(fptr);
	
	printf("Please log out and log in again to apply changes.\n\n");

	return 1;
}
