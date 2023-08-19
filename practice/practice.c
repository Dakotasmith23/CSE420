#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <stdbool.h>
#include <pthread.h>

  
struct Node {
  char *name;
  int id;
  int keyFreq;
  pthread_t tid;
  char *entityPath;
  char *keyword;
  int isDir;
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
  node->isDir=0;
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
  //uncomment//printf("\n\nentityPath:%s\n\n", entityPath);
    
  char line[1025];
  fgets(line,1025,file);
  
  
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
				//Uncomment//printf("Token:%s\n",token); 
				if(strcmp(token,keyword)==0)
				{
					keyFreq++;
					//printf("\n\nKEYWORD FOUND, keyFreq:%d\n\n", keyFreq); //UNCOMMENT//
				}
			}
			token = strtok_r(NULL,delimiter,&savePtr);
		}
		fgets(line,1025,file);// check here
		token = strtok_r(line,delimiter,&savePtr);
  }
  
  fclose(file);		  
	
	
	return keyFreq;		 
}


void *parseFileParallel(void *arg)
{
	
	struct Node *node = (struct Node *)arg;
	
			
	FILE *file;
  file = fopen(node->entityPath, "r"); // check for same file name in different folders case
  //uncomment//printf("\n\nentityPath:%s\n\n", entityPath);
    
  char line[1025];
  fgets(line,1025,file);
  
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
				//Uncomment//printf("Token:%s\n",token); 
				if(strcmp(token,node->keyword)==0)
				{
					keyFreq++;
					//printf("\n\nKEYWORD FOUND, keyFreq:%d\n\n", keyFreq); //UNCOMMENT//
				}
			}
			token = strtok_r(NULL,delimiter,&savePtr);
		}
		fgets(line,1025,file);// check here
		token = strtok_r(line,delimiter,&savePtr);
  }
  
  fclose(file);		  
	
	// Update keyFreq member in node
	node->keyFreq = keyFreq;
	
	
	
	return 0;
}


void putEntitiesInList(const char *dirname, int initLevel, struct List *list,char *keyword, char *ispar)
{
  DIR *dir = opendir(dirname); //open directory stream
  struct dirent *entity; // Make ptr that represents location of entity
  entity = readdir(dir); // Set entity to the next directory entry
  
  // Find current entity's level
  char curPath[2000] = { 0 };
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
  		//printf("\n\nFile starts with '.'. Not included.\n\n");
  		entity = readdir(dir);
  	}
  	else
  	{
			// insert to list sorted 	
			if(strcmp(entity->d_name, ".")!=0 && strcmp(entity->d_name, "..")!=0)
			{
		
				
				 //creating string dynamically
			 	struct Node *tmp = NULL;
			 	
				char entityPath[1000]={0};//malloc size of 
				strcat(entityPath,curPath);
				strcat(entityPath,"/");
				strcat(entityPath,entity->d_name);
				
				
								  
			  if(entity->d_type != DT_DIR) // parseFile and insert
			  {
				  
				  if(strcmp(ispar,"0")==0)//sequential
				  {
				  	int keyFreq = parseFile(entityPath,keyword);
					  //printf("\nkeyFreq:%d\n",keyFreq); //UNCOMMENT//
				  
					  tmp = create_node(entityPath, entityLevel);
					  tmp->keyFreq = keyFreq;
					  tmp->entityPath = entityPath;
					  tmp->keyword = keyword;
					  insert_sorted(tmp,list); 
				  }
				  else//parallel
				  {
				  	tmp = create_node(entityPath, entityLevel);
					  tmp->entityPath = entityPath;
					  tmp->keyword = keyword;
					  insert_sorted(tmp,list);								  
				  }
				   
			  }
			  else //dont parse directory and insert
			  {
			  	tmp = create_node(entityPath, entityLevel);
			  	tmp->isDir = 1;
				  insert_sorted(tmp,list); 
			  }
		 	}
			
			// Ignore "." and ".." which are present and previous directories
			if(entity->d_type == DT_DIR && strcmp(entity->d_name, ".")!=0 && strcmp(entity->d_name, "..")!=0)
			{
				char path[1000] = { 0 };
				strcat(path, dirname);
				strcat(path, "/");
				strcat(path, entity->d_name);
				putEntitiesInList(path, initLevel,list,keyword,ispar);
			}
			entity = readdir(dir);
  	}
  }
  closedir(dir);
}

void output_list(struct List *list,char *argv[])
{
  FILE *fp;
  fp = fopen(argv[3], "a");
  
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
			int keyFreq = iter->keyFreq;
			struct Node *tmp;
			tmp = create_node(iter->name, iter->id);
			tmp->keyFreq = keyFreq;
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



  
int main(int argc, char *argv[]) {
  int x = 15;
  printf("Hello there!\n");
  
  
  
  
  
  
  
  
  return 0;
}
