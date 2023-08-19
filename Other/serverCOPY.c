#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <semaphore.h>
#include <string.h>

#define handle_error(msg) \
	do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define MAXDIRPATH 1024
#define MAXKEYWORD 256
#define MAXLINESIZE 1024
#define MAXOUTSIZE 2048

struct Node {
  char *name;
  int id;
  int keyFreq;
  struct Node *next;
  struct Node *prev;
};

struct List {
  struct Node *head;
  struct Node *tail;
};

struct Node *create_node(char *name, int id) {
  struct Node *node = malloc(sizeof(struct Node));
  if (node == NULL) {
    fprintf (stderr, "%s: Couldn't create memory for the node; %s\n", "linkedlist", strerror(errno));
    exit(-1);
  }
  node->name = strdup(name);
  node->id = id;
  node->keyFreq = 0;
  node->next = NULL;
  node->prev = NULL;
  return node;
}

struct List *create_list() {
  struct List *list = malloc(sizeof(struct List));
  if (list == NULL) {
    fprintf (stderr, "%s: Couldn't create memory for the list; %s\n", "linkedlist", strerror (errno));
    exit(-1);
  }
  list->head = NULL;
  list->tail = NULL;
  return list;
}

void insert_tail(struct Node *node, struct List *list) {
  if (list->head == NULL && list->tail == NULL) {
    list->head = node;
    list->tail = node;
  } else {
    list->tail->next = node;
    node->prev = list->tail;
    list->tail = node;
  }
}

void print_list(struct List *list) {
  struct Node *ptr = list->head;  
  while (ptr != NULL) {
    if (ptr != list->head) {
      printf("->");
    }
    printf("(%s,%d,%d)", ptr->name, ptr->id,ptr->keyFreq);
    ptr = ptr->next;
  }
  printf("\n");
}

void destroy_list(struct List *list) {
  struct Node *ptr = list->head;
  struct Node *tmp;  
  while (ptr != NULL) {
    free(ptr->name);
    tmp = ptr;
    ptr = ptr->next;
    free(tmp);
  }
  free(list);
}

void insert_sorted(struct Node *node, struct List *list) {
  // Case 1: Empty list
  if (list->head == NULL && list->tail == NULL) {
    list->head = node;
    list->tail = node;
  }
  // Case 2: Node < list->head
  else if(strcmp(list->head->name,node->name)>0)
  {
    list->head->prev = node;
    node->next = list->head;
    list->head = node;
  }
  // Case 3: Traverse list, insert node, update surrounding nodes
  else
  { 
    struct Node *temp = list->head;
    while(temp != NULL)
    {
   
      if(strcmp(temp->name, node->name)>0) // Put in b/w nodes
      {
        temp->prev->next = node;
        node->prev = temp->prev;
        temp->prev = node;
        node->next = temp;
        break;
      }
    
      else if(temp->next == NULL)
      {
        temp->next = node;
        node->prev = temp;
        break;
      }
      temp=temp->next;         
    }
  }
}


int parseFile(char entityPath[], char keyword[])
{
	 
  FILE *file;
  file = fopen(entityPath, "r"); // check for same file name in different folders case
  //"/home/dakota/Desktop/CSE420/project2/newFolder/test"
  printf("\n\nentityPath:%s\n\n", entityPath);
    
  char line[2048];
  fgets(line,2048,file);
  
  
  char delimiter[] = " \t\n";
  char *savePtr;
  int keyFreq=0;
  char *token = strtok_r(line,delimiter,&savePtr);
 
 
  while(!feof(file))
  {
		
		while(token!=NULL)
		{
			if(token!=NULL)
			{
				printf("Token:%s\n",token); 
				if(strcmp(token,keyword)==0)
				{
					keyFreq++;
					printf("\n\nKEYWORD FOUND, keyFreq:%d\n\n", keyFreq);
				}
			}
			token = strtok_r(NULL,delimiter,&savePtr);
		}
		fgets(line,2048,file);
		token = strtok_r(line,delimiter,&savePtr);
  }
  
  fclose(file);		  
	
	
	return keyFreq;		 
}


void putEntitiesInList(const char *dirname, int initLevel, struct List *list)
{
  DIR *dir = opendir(dirname); //open directory stream
  struct dirent *entity; // Make ptr that represents location of entity
  entity = readdir(dir); // Set entity to the next directory entry
  
  // Find current entity's level
  char curPath[5000] = { 0 };
  strcat(curPath, dirname);
  int curLevel=0;
  for(int i=0;i<strlen(curPath);i++)
  {
    if(curPath[i]=='/')
    {
      curLevel++;
    }
  }
  int entityLevel=(curLevel-initLevel)+2;
  
  // Print next entity,recursively call putEntitiesInList() for directories
  while(entity != NULL) 
  {
  	// Dont include files that start with "."
  	char firstChar = entity->d_name[0];
  	if(firstChar == '.')
  	{
  		printf("\n\nFile starts with '.'. Not included.\n\n");
  		entity = readdir(dir);
  	}
  	else
  	{
			// insert to list sorted 	
			if(strcmp(entity->d_name, ".")!=0 && strcmp(entity->d_name, "..")!=0)
			{
				 
			  struct Node *tmp = NULL;
			  char entityPath[1000]={0};
			  strcat(entityPath,curPath);
			  strcat(entityPath,"/");
			  strcat(entityPath,entity->d_name);
			  
			  int keyFreq = parseFile(entityPath,"shark");
			  
			  printf("\nkeyFreq:%d\n",keyFreq);
			  
			  tmp = create_node(entityPath, entityLevel);
			  tmp->keyFreq = keyFreq;
			  insert_sorted(tmp,list);
		 	}
			
			// Ignore "." and ".." which are present and previous directories
			if(entity->d_type == DT_DIR && strcmp(entity->d_name, ".")!=0 && strcmp(entity->d_name, "..")!=0)
			{
				char path[1000] = { 0 };
				strcat(path, dirname);
				strcat(path, "/");
				strcat(path, entity->d_name);
				putEntitiesInList(path, initLevel,list);
			}
			entity = readdir(dir);
  	}
  }
  closedir(dir);
}

void output_list(struct List *list,char *argv[])
{
  FILE *fp;
  fp = fopen(argv[2], "a");
  
  int order=1;
  struct Node *ptr = list->head;  
  while (ptr != NULL)
  {
    if(ptr->next == NULL)
    {
    	fprintf(fp, "%d:%d:%d:%s", ptr->id, order,ptr->keyFreq, ptr->name);
    	break;
    }
    fprintf(fp, "%d:%d:%d:%s \n", ptr->id, order,ptr->keyFreq, ptr->name);
    if(ptr->id != ptr->next->id)
    {
    	order=1;
    }
    if(ptr->id == ptr->next->id)
    {
    	order++;
    }
    ptr = ptr->next;
  }
  fprintf(fp, "\n");
  
  fclose(fp);
}


int getNumElem(struct List *list)
{
  int n = 0;	
 	struct Node *ptr = list->head;  
  while (ptr != NULL) {
    n++;
    ptr = ptr->next;
  }
  return n;
}

void sort_level(struct List *list, int numElem)
{
	struct Node *iter;
	iter = list->head;

	int n = 0;
	while(n != numElem)//Dont know end condition
	{
		if(iter->next == NULL)
		{
			iter = list->head;
			n++;
		}
		if(iter->id > iter->next->id)
		{
			struct Node *tmp;
			tmp = create_node(iter->name, iter->id);
			free(iter->name);
			iter->name = iter->next->name;
			iter->id = iter->next->id;
			iter->keyFreq = iter->next->keyFreq;
			iter->next->name = tmp->name;
			iter->next->id = tmp->id;
			iter->next->keyFreq = tmp->keyFreq;
			free(tmp);
		}
		iter = iter->next;
	}
}

int findLevel(char path[])
{
	int level = 0;
	for(int i=0;i<strlen(path);i++)
	{
		if(path[i]=='/')
		{
			level++;
		}
	}

	return level;

}

char *getRequest(void **requestQueueOut,int *OUT,  sem_t** empty, sem_t** full, int REQ_QUEUE_SIZE, void *requestQueueBeginning)
{
  // To change OUT you must do (*OUT)++
  sem_wait(*full);
  // request must be tokenized
  char *savePtr;
  char delimiter[] = "\n";
  char *requestDup = strdup(*requestQueueOut);
  char *request = strtok_r(requestDup, delimiter, &savePtr);
  *requestQueueOut += (MAXDIRPATH + MAXKEYWORD + 2);
  (*OUT)++;
  sem_post(*empty);

  if((*OUT) == REQ_QUEUE_SIZE)
  {
    (*OUT) = 0;
    *requestQueueOut = requestQueueBeginning;
  }

  return request;
}

int main(int argc, char *argv[]) {
	// Create semaphores
	const char *sem1_name = "semaphore1";
	sem_t *sem1 = sem_open(sem1_name, O_CREAT, 0666, 0);
	if(sem1 == SEM_FAILED)
	{
		printf("Error opening semaphore1\n");
	}
	const char *sem2_name = "semaphore2";
	sem_t *sem2 = sem_open(sem2_name, O_CREAT, 0666, 0);
	if(sem2 == SEM_FAILED)
	{
		printf("Error opening semaphore2\n");
	}
	
	const int REQ_QUEUE_SIZE = atoi(argv[1]);
  const int SHARED_MEM_SIZE = REQ_QUEUE_SIZE * (MAXDIRPATH + MAXKEYWORD + 2);
  const char *shm_name = "OS";
  int shm_fd;
  

  // Create, open, and configure size of shared memory segment for queue
  shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
  ftruncate(shm_fd,SHARED_MEM_SIZE);

   // Map requestQueue struct to shared memory
  void *requestQueue;
	requestQueue = mmap(0,SHARED_MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
  if (requestQueue == MAP_FAILED) {
	  printf("Map failed\n");
	  exit(-1);
	}
  

  void *requestQueueBeginning = requestQueue;
  void *requestQueueOut = requestQueueBeginning;

	// Create, open, and configure size of shared memory segment for index
	const char *shm_name_index = "Index";
	int shm_fd_index;
	shm_fd_index = shm_open(shm_name_index, O_CREAT | O_RDWR, 0666);
	ftruncate(shm_fd_index, sizeof(int *));
	// Map index to shared memory
	int *index;
	index = mmap(0,sizeof(int *), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd_index, 0);
	if(index == MAP_FAILED)
	{
		printf("Map failed\n");
		exit(-1);
	}
  // Initialize index to 0 and set indexPtr to index
  *index = 0;

  const char *sem3_name = "semaphore3";
  sem_t *sem_empty = sem_open(sem3_name, O_CREAT, 0666, REQ_QUEUE_SIZE);
  if(sem_empty == SEM_FAILED)
  {
    printf("Error opening semaphore3\n");
  }
  const char *sem4_name = "semaphore4";
  sem_t *sem_full = sem_open(sem4_name, O_CREAT, 0666, 0);
  if(sem_full == SEM_FAILED)
  {
    printf("Error opening semaphore4\n");
  }


  // Have server read in and print requests from requestQueue
  int OUT = 0;
  char *request = "";


  
  
  request = getRequest(&requestQueueOut, &OUT, &sem_empty, &sem_full, REQ_QUEUE_SIZE, requestQueueBeginning);
  // parse request into directory path and keyword
  char *dirPath = strtok(request, " ");
  char *keyword = strtok(NULL, "\n");
  printf("dirPath: %s\n", dirPath);
  printf("keyword: %s\n", keyword);

  
  pid_t pid;
  pid = fork();
  int numRequest = 1;
  int childProcesses = 1;
  while(strcmp(dirPath, "exit") != 0)
  {
    // PUT INSIDE OF CHILD PROCESS WHEN DONE DEBUGGING
    // process request. open directory and start worker threads for first level files
    DIR *dir;
    struct dirent *entity;
    dir = opendir(dirPath);
    if(dir == NULL)
    {
      printf("Error opening directory\n");
      exit(-1);
    }
    entity = readdir(dir);
    // get first level files
    while(entity!=NULL)
    {
      if(entity->d_type != DT_DIR && strcmp(entity->d_name, ".")!=0 && strcmp(entity->d_name, "..")!=0)
      {
        printf("entity: %s\n", entity->d_name);
        // create worker thread to search file for keyword and put output text in bounded buffer

      }
      entity = readdir(dir);
    
    }
    closedir(dir);
    // create a printer thread that reads from bounded buffer and prints to output file

    //EVERYTHING ABOVE GOES IN CHILD PROCESS. HERE NOW FOR DEBUGGING
  
    // if child process then process request
    if(pid == 0)
    {
      
      printf("child #%d processing request %d\n", childProcesses,numRequest);
      break;
    }
    // if parent process then read in next request and fork
    if(pid != 0)
    {
      request = getRequest(&requestQueueOut, &OUT, &sem_empty, &sem_full, REQ_QUEUE_SIZE, requestQueueBeginning);
      dirPath = strtok(request, " ");
      if(strcmp(dirPath, "exit")!=0)
      {
        keyword = strtok(NULL, "\n");
      }
      else
      {
        printf("exit request received\n");
      }     

      printf("next request to be processed is %s\n", request);
      numRequest++;
      pid = fork();
      childProcesses++;
      
    }

  }
  
  // If parent process then wait for all children to finish
  if(pid != 0)
  {
    for(int i = 0; i < childProcesses; i++)
    {
      wait(NULL);
    }

    printf("Postsem2 waitsem1 \n");
	
	  sem_post(sem2);
	  sem_wait(sem1);	

	
	  printf("%s", (char *)requestQueue);




    // Close semaphores and unlink shared memory segments
    sem_close(sem1);
    sem_close(sem2);
    sem_close(sem_empty);
    sem_close(sem_full);
    sem_unlink(sem1_name);
    sem_unlink(sem2_name);
    sem_unlink(sem3_name);
    sem_unlink(sem4_name);
    shm_unlink(shm_name);
    shm_unlink(shm_name_index);
  }





	
  // Old code for removing shared memory
  // {
  //   /* remove the shared memory segment */
  //   if (shm_unlink(shm_name) == -1) {
  //   printf("Error removing %s\n",shm_name);
  //   exit(-1);
  //   }
	// /* remove the shared memory segment */
  //   if (shm_unlink(shm_name_index) == -1) {
  //   printf("Error removing %s\n",shm_name_index);
  //   exit(-1);
  //   }
  // }

	return 0;
}
