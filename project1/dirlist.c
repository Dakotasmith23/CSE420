
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <stdbool.h>

/*
TO DO FOR STUDENTS: 
1) Keep track of the number of elements in the list
2) Write a find by ID function
3) Write a remove by ID function
4) Write different versions of the insert function, such as insert_sorted(), insert_after(), etc.
5) Write an insertion sort function sorting records by their IDs.
*/

struct Node {
  char *name;
  int id;
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
    printf("(%s,%d)", ptr->name, ptr->id);
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

void putEntitiesInList(const char *dirname, int initLevel, struct List *list)
{
  DIR *dir = opendir(dirname); //open directory stream
  struct dirent *entity; // Make ptr that represents location of entity
  entity = readdir(dir); // Set entity to the next directory entry
  
  // Find current entity's level
  char curPath[1000] = { 0 };
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
  // Print next entity, recursively call listFiles() for directories
  while(entity != NULL) 
  {
  	// Dont include files that start with "."
  	char firstChar = entity->d_name[0];
  	if(firstChar == '.')
  	{
  		printf("\n\nHERE\n\n");
  		entity = readdir(dir);
  	}
  	else
  	{
			// insert to list sorted 	
			if(strcmp(entity->d_name, ".")!=0 && strcmp(entity->d_name, "..")!=0)
			{
			  struct Node *tmp = NULL;
			  char test[1000]={0};
			  strcat(test,curPath);
			  strcat(test,"/");
			  strcat(test,entity->d_name);
			  tmp = create_node(test, entityLevel);
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
  while (ptr != NULL) {



    if(ptr->next == NULL)
    {
    	fprintf(fp, "%d:%d:%s", ptr->id, order, ptr->name);
    	break;
    }
    fprintf(fp, "%d:%d:%s \n", ptr->id, order, ptr->name);
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
			iter->next->name = tmp->name;
			iter->next->id = tmp->id;
			free(tmp);
			
		}
		
		iter = iter->next;
	}
		
}

int main(int argc, char *argv[]) {
  
  // Create character array with initial path. Find initial level
  char initPath[1000] = { 0 };
  strcat(initPath, argv[1]);
  printf("\ninitPath: %s\n", argv[1]);
  
  int initLevel = 0;
  for(int i=0;i<strlen(initPath);i++)
  {
    if(initPath[i]=='/')
    {
      initLevel++;
    }
  }
  printf("\ninitLevel: %d\n", initLevel);
  
  /*
  //open output.txt and delete everything
  FILE *fp;
  fp = fopen("output.txt", "w+");
  fclose(fp);
  */
  struct List *list = create_list();
  
	putEntitiesInList(argv[1],initLevel, list); // argv[1] will be a pathname
  
  struct Node *tmp;
  
  tmp = create_node(initPath,1);
  
  insert_sorted(tmp,list);
  
  print_list(list);
	
	int numElem = getNumElem(list);
  
  sort_level(list,numElem); // argv[1] will be a pathname
  output_list(list,argv);
  
  print_list(list);


  
  destroy_list(list);
  
  return 0;
}
