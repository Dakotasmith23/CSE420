Jacob Bennett-5242085 & Dakota Smith-5162129

Parts not fully implemented

The server side accepts the first command line argument for <req-queue-size> but not for <buffersize>. The last step in our process for this project we completed was having the server fork and create a child process to serve each request. The child thread does not create the worker/printer threads but we do have some code commented out that shows our progress. If we had semaphores and the proper threads created, then the worker threads would utilize the search() function as the start routine that would search the current input file for the keyword. export() is the routine for the commented out printer thread that will print to the output file. The file locking code has some progress but is not completely finished and is commented out. 


Shared memory region

There were two allocations of shared memory in our project. The first one was allocated to a void pointer which held requestQueue, or the first byte in shared memory that would later be typecasted to a (char*). The size of this shared memory region was SHARED_MEM_SIZE = REQ_QUEUE_SIZE * (MAXDIRPATH + MAXKEYWORD + 2). Each request needed enough space for the maximum directory path and keyword and then an extra two bytes for the space in between and the null terminating character at the end. The other shared memory was allocated to a pointer to an integer value. The integer value held the index of the requestQueue and was used to see if the queue was full. The size of this memory allocation was just the sizeof(int*) because all it needed to hold was an address of the integer value. Each of the shared memory segments only contained one type of information.
