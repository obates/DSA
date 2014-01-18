#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

int size;

typedef struct graphNode
{
	int x;	//x coord of node
	int y;	//y coord of node

	int up;	//1 if valid move up
	int left;	//1 if valid move left
	int down;	//1 if valid move down
	int right; //1 if valid move right

	int teleport; //1 if teleport, 0 if not

	int distance; //Distance from source
	struct graphNode* pi; //Previous node
	int visited; //0 if not visited, 1 if visited

	int position; //Holds position in the heap

}Node;

//
// TELEPORT FUNCTIONS
//

typedef struct tele
{
	int x;
	int y;
	int distance;
}teleport;

teleport* newTeleport(int x, int y, int d)
{
	teleport* new = malloc(sizeof(teleport));
	new->x = x;
	new->y = y;
	new->distance = d;
	return new;
}

//
//HEAP FUNCTIONS
//

typedef struct heap
{
	int size; //Holds current heap size
	int capacity; //Holds max heap size
	Node **array; //Array of nodes in the heap
}minHeap;

minHeap* createMinHeap(int capacity)
{
	minHeap* heap = (minHeap*)malloc(sizeof(minHeap)); //Allocates memory for heap
	heap->size = 0; //Sets inital size to 0
	heap->capacity = capacity; //Sets max size of heap
	heap->array = (Node**)malloc(capacity * sizeof(Node*)); //Allocates memory for the array of Node*
	return heap;
}

//Function recursively swaps a parent with the smallest child, ensuring the heap is valid
void minHeapify(minHeap* heap,int idx)
{
	int smallest,left,right;
	smallest = idx;
	left = 2 * idx + 1;
	right = 2 * idx + 2;

	if (left < heap->size && heap->array[left]->distance < heap->array[smallest]->distance)
	{
		smallest = left;
	}

	if (right < heap->size && heap->array[right]->distance < heap->array[smallest]->distance)
	{
		smallest = right;
	}

	if(smallest != idx)
	{
		Node* smallestNode = heap->array[smallest];
		Node* idxNode = heap->array[idx];

		heap->array[smallest] = idxNode;
		heap->array[idx] = smallestNode;

		minHeapify(heap,smallest);
	}
}

//Returns 1 if the heap is empty
int isEmpty(minHeap* heap)
{
	if(heap->size < 1)
	{
		return 1;
	}
	return 0;
}

//Returns the minimum priority node from the heap. Returns NULL if the heap is empty. Reduces the heap size
//due to extracting a node. 
Node* extractMin(minHeap* heap)
{
	if (isEmpty(heap))
	{
		return NULL;
	}
	Node* root = heap->array[1];

	Node* lastNode = heap->array[heap->size];
	heap->array[1] = lastNode;

	--heap->size;
	minHeapify(heap,1);

	return root;
}

//Swaps child with parent whilst parent is larger than child
void decreaseKey(minHeap* heap,Node* node,int key)
{
	int pos = node->position;
	heap->array[pos]->distance = key;

	int parent = floor(pos/2);

	while (pos && heap->array[pos]->distance < heap->array[parent]->distance)
	{
		Node* temp = heap->array[pos];
		heap->array[pos] = heap->array[parent];
		heap->array[parent] = temp;

		pos = parent;
	}

}

//Inserts a node to the end of the heap. Then calls decreasekey on the heap to ensure 
void insert(minHeap* heap,Node* node)
{
	int k;
	heap->size++;
	heap->array[heap->size] = node;
	heap->array[heap->size]->position = heap->size;
	k = heap->array[heap->size]->distance;

	if(heap->size > 1)
	{
		decreaseKey(heap,heap->array[heap->size],k);
	}
}


//
// NODE FUNCTIONS
//

Node* createNode(int j,int i,int d)
{
	Node *newAdj = (Node*)malloc(sizeof(Node)); //Create size for new node

	newAdj->x = j;
	newAdj->y = i;
	newAdj->left = 0;
	newAdj->right = 0;
	newAdj->down = 0;
	newAdj->up = 0;
	newAdj->teleport = 0;
	newAdj->distance = d;
	newAdj->pi = NULL;
	newAdj->visited = 0;

	return newAdj;
}

Node* checkSurroundings(Node* current,int** grid)
{
	int j = current->x;
	int i = current->y;


	if (j != (size -1) && grid[i][j+1] != -1) //Checking to the right
	{
		current->right = 1;
	}
	if (j != 0 && grid[i][j-1] != -1) //Checking to the left
	{
		current->left = 1;
	}
	if (i != (size-1) && grid[i+1][j] != -1) //Checking below
	{
		current->down = 1;
	}
	if (i != 0 && grid[i-1][j] != -1) //Checking above
	{
		current->up = 1;
	}


	//printf(" %d  \n%d%d%d\n %d  \n",current->up,current->left,current->distance,current->right,current->down);


	return current;
}

//
//Printing out the final route
//

void printRoute(Node* destination)
{
	char* backwards = (char*) malloc(sizeof(char));
	int i = 0;


	while(destination->pi != NULL)
	{
		if(destination->teleport==1)
		{
			backwards[i] = 'T';
		}
		else
		{
			if(destination->x < destination->pi->x)
			{
				backwards[i] = 'W';
			}
			else if (destination->x > destination->pi->x)
			{
				backwards[i] = 'E';
			}

			if(destination->y < destination->pi->y)
			{
				backwards[i] = 'N';
			}
			else if (destination->y > destination->pi->y)
			{
				backwards[i] = 'S';
			}
		}
		destination = destination->pi;
		i++;

	}
	backwards[i] = '\0';

	while(i >= 0)
	{
		switch(backwards[i])
		{
			case 'N':
			case 'S':
			case 'T':
			case 'E':
			case 'W':
				printf("%c",backwards[i]);
			break;
		}
		i--;
	}
	printf("\n");

}
	
int main(int argc, char *argv[])
{
	int i;
	int j;

	//Open file frome command line argument
	FILE *in = fopen(argv[1],"r");

	//Scan in the initial size
	fscanf(in,"%d",&size);


	int** grid;
	grid = (int**)malloc(size * sizeof(int*));
	for(i = 0;i < size;i++)
	{
		grid[i] = (int*)malloc(size*sizeof(int));
	}
	//dynamically create a 2D array of size sizexsize


	//Create 1D grid to hold the start position of a teleport
	teleport** teleportGrid = (teleport**)malloc(size * size * sizeof(teleport*));



	int x1,y1,x2,y2,w = 0;

	i = 0;
	j = 0;
	while (i < size)
	{
		while(j < size)
		{
			grid[i][j] = 100000000;
			//Initialise grid to hold infinity
			j++;
		}
		i++;
		j = 0;
	}


	int ch;

	//Get the character at the start of the line
	ch = fgetc(in);


	while(ch != EOF)
	{	
		if (ch == 32)
		{
				fscanf(in,"%d %d %d %d %d",&x1,&y1,&x2,&y2,&w);
				//printf("Inputted Coords: %d %d %d %d %d\n",x1,y1,x2,y2,w);

				//Convert to normal coords.
				x1--;
				y1--;
				x2--;
				y2--;


				if (w == 0)//If a block
				{
					//Loop through the blocked off areas, setting grid to -1 where there is a block
					for(i = y1; i <= y2; i++)
					{
						for (j = x1; j <= x2; j++)
						{
							grid[i][j] = -1;
						}
					}
				}
				else
				{
					//Create new teleport from x1,y1 going to x2,y2 with weight w
					teleportGrid[x1 + (y1 * size)] = newTeleport(x2,y2,w);
				}
		}
		ch = fgetc(in);
	}
	fclose(in);

	i = 0;
	j = 0;


	//Dijkstra's

	//Create new empty heap with inital heap size 0 and capacity size*size
	minHeap* heap = createMinHeap(size * size);

	//Create source node, set inital x,y,distance and visited to 0
	Node* source = (Node*)malloc(sizeof(Node));

	source->x = 0;
	source->y = 0;
	source->distance = 0;
	source->teleport=0;
	source->pi = NULL;
	source->visited = 0;

	//Check surroundings of the source node, sets left,right,up and down to 1 if a move is allowed in that direction
	checkSurroundings(source,grid);

	//Insert the source node to the heap
	insert(heap,source);

	Node* current;

	while(!isEmpty(heap)) //heapEmpty returns 1 when empty
	{
		//Take and remove lowest priority node from heap
		current = extractMin(heap);


		//Set current node to visited
		current->visited = 1;

		int j = current->x;
		int i = current->y;
		int alt;
		int prevDist;
	
		//If the current node is the destination node print out its route and break from the loop
		if (current->x == size-1 && current->y == size-1)
		{
			printRoute(current);
			break;
		}

		//Used to check if the current node has been visited before
		grid[i][j] = 1;

		//Index for the 1d array
		int index = j + (i*size);

		if(teleportGrid[index] != NULL) //Check if there is a teleport for the current ndoe
		{
			//Set alt to the current distance + distance of the teleport
			alt = current->distance + teleportGrid[index]->distance;


			//If destination hasn't been visited then add destination to heap
			if(alt < grid[teleportGrid[index]->y][teleportGrid[index]->x] && grid[teleportGrid[index]->y][teleportGrid[index]->x] == 100000000)
			{

				Node *newAdj;
				newAdj = createNode(teleportGrid[index]->x,teleportGrid[index]->y,alt);
				newAdj = checkSurroundings(newAdj,grid);

				newAdj->pi = current;
				newAdj->teleport = 1;
				insert(heap,newAdj);
				grid[i][j] = alt;
			}
			
		}

		//Checking if there is an allowed move upwards
		if (current->up == 1)
		{
			alt = current->distance + 1;
			prevDist = 0;

			//If less distance and node hasn't been visited
			if(alt < grid[i-1][j] && grid[i-1][j] == 100000000)
			{

				Node *newAdj;
				newAdj = createNode(j,i-1,current->distance +1); //Create new node
				newAdj = checkSurroundings(newAdj,grid);

				newAdj->distance = alt;
				newAdj->pi = current;
				insert(heap,newAdj);
				grid[i-1][j] = alt;
			}
		}

		//Checking if there is an allowed move left
		if (current->left==1)
		{
			alt = current->distance + 1; 
			prevDist = 0;

			//If less distance and node hasn't been visited
			if(alt < grid[i][j-1] && grid[i][j-1] == 100000000)
			{
				Node *newAdj;
				newAdj = createNode(j-1,i,current->distance +1); //Create new node
				newAdj = checkSurroundings(newAdj,grid);


				newAdj->distance = alt;
				newAdj->pi = current;
				insert(heap,newAdj);
				grid[i][j-1] = alt;
			}		
		}		

		//Checking if there is an allowed move down
		if (current->down == 1)
		{
			alt = current->distance + 1;
			prevDist = 0;

			//If less distance and node hasn't been visited
			if(alt < grid[i+1][j] && grid[i+1][j] == 100000000)
			{
				Node *newAdj;
				newAdj = createNode(j,i+1,current->distance +1); //Create new node
				newAdj = checkSurroundings(newAdj,grid);


				newAdj->distance = alt;
				newAdj->pi = current;
				insert(heap,newAdj);
				grid[i+1][j] = alt;
			}
		}

		//Checking if there is an allowed move right
		if (current->right == 1)
		{
			alt = current->distance + 1; 
			prevDist = 0;

			//If less distance and node hasn't been visited
			if(alt < grid[i][j+1] && grid[i][j+1] == 100000000)
			{
				Node *newAdj;
				newAdj = createNode(j+1,i,current->distance + 1); //Create new node
				newAdj = checkSurroundings(newAdj,grid);

				newAdj->distance = alt;
				newAdj->pi = current;

				insert(heap,newAdj);

				grid[i][j+1] = alt;
			}
		}
	}
	return 0;
}
