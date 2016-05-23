/*===========================================================================================

NAME: bathroom.c

AUTHOR: Camron Khan
DATE: 5/4/16

General rules:
	(1) Only people of the same gender may be in the bathroom at the same time
	(2) A maximum of 5 people can be in the bathroom at once
	(3) If a person cannot access the bathroom, they must wait in a queue
	(4) There is no maximum to the length of the queue
	(5) Each user is randomly assigned a use length (measured in number of cycles) between 1 and 3
	(6) Each user is given a unique ID, gender, and number of uses
		- Ex 1:  W(10,2)  =>  A woman with an ID 10 who must use the bathroom for 2 cycles
		- Ex 2:  M(1,5)   =>  A man with an ID 1 who must use the bathroom for 5 cycles

Rules for entering the bathroom:
	(1) Queued people may enter the bathroom before new requestors
	(2) Adjacent queued people of the same gender may enter on the same cycle if stalls are available
	(3) New requestors may enter bathroom directly if queue is empty prior to cycle
	(4) New requestors may enter bathroom directly if queue is emptied during current cycle
	(5) New requestors must enter the queue if queue is not empty
		- Ex 1:  Person of different gender is already in queue
		- Ex 2:  Person of same gender is in queue but cannot enter bathroom due to no vacancy

Rules for exiting the bathroom:
	(1) On each cycle decrement the remaining number of required uses by one
	(2) If any user's required uses changes from one to zero, remove the user

===========================================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Variables
int cycles = 0;				// Counts number of current cycles
int userId = 0;				// Unique identifier
int occupants = 0;			// Counts number of occupants in bathroom
int womanCount = 0;			// Counts number of women in bathroom
int manCount = 0;			// Counts number of men in bathroom
int bathroomMutex = 1;		// Bathroom mutex
int manMutex = 1;			// Man mutex
int womanMutex = 1;			// Woman mutex
int stalls[5];				// Array of available stalls

// Define person struct
typedef struct {
	int id;			// Person's identifier
	char gender;	// 'W' == woman; 'M' == man
	int usage;		// Number of cycles required in bathroom (between 1 and 5)
} person;


/*====================================
  Queue (Linked List Implementation)
====================================*/

// Declare node structure
struct node
{
	person requestor;		// Store person struct
	struct node* next;		// Store next node
};
struct node* head = NULL;	// Pointer to head
struct node* tail = NULL;	// Pointer to tail

// Add person to queue
void enqueue(person x)
{
	// Create a node and use temp to point to new node
	struct node* temp = (struct node*)malloc(sizeof(struct node));	
	temp->requestor = x;					// Assign person
	temp->next = NULL;						// Set next node to NULL
	if (head == NULL && tail == NULL)		// If queue is empty...
	{
		head = tail = temp;					// Set head and tail to point new node location
		return;								// (i.e., the location pointed to by temp)
	}
	tail->next = temp;						// If not empty, set next of present node to point to new node
	tail = temp;							// Set tail to point to new node
}

// Return first person in queue
person dequeue()
{
	struct node* temp = head;				// Assign head pointer to temp (to be freed later)
	person req;								// Declare person to be returned
	if (head == NULL) return;				// If head is NULL, do nothing
	if (head == tail)						// If only one person is in queue...
	{
		req = temp->requestor;				// Get person
		head = tail = NULL;					// Reset head and tail to NULL
	}
	else									// Otherwise, there are more than one people in queue...
	{
		req = temp->requestor;				// Get person at head
		head = head->next;					// Update head to next person
	}
	free(temp);								// Release formerly head node
	return req;								// Return person
}

// Return the gender of the first person in the queue
char get_first_in_queue_gender()
{
	int len = get_queue_length();					// Get length of queue
	if (len > 0) return head->requestor.gender;		// If there is at least one person in the queue, return the first person's gender
	else return '?';								// Otherwise, something went wrong -> return '?'
}

// Return the length of the queue
int get_queue_length()
{
	int len = 0;					// Initilzie length counter
	struct node* temp = head;		// Temp will iterate beginning at head
	while (temp != NULL)			// While temp is not NULL...
	{
		len++;						// Increase the length counter
		temp = temp->next;			// Iterate to next node
	}
	return len;
}

// Print length of queue to console
void display_queue_length()
{
	int len = get_queue_length();			// Get length
	printf("Queue Length: %d\n", len);		// Print length
}

// Print number of women waiting in queue
void display_queue_women()
{
	int w = 0;										// Initialize counter
	struct node* temp = head;						// Temp will iterate beginning at head
	while (temp != NULL)							// While temp is not NULL...
	{
		if (temp->requestor.gender == 'W') w++;		// If the person gender is woman (i.e., 'W'), increment counter
		temp = temp->next;							// Iterate to next node
	}
	printf("Women Waiting: %d\n", w);				// Print results
}

// Print number of men waiting in queue
void display_queue_men()
{
	int m = 0;										// Initialize counter
	struct node* temp = head;						// Temp will iterate beginning at head
	while (temp != NULL)							// While temp is not NULL...
	{
		if (temp->requestor.gender == 'M') m++;		// If the person gender is man (i.e., 'M'), increment counter
		temp = temp->next;							// Iterate to next node
	}
	printf("Men Waiting: %d\n", m);					// Print results
}

// Display the distribution of women and men in the queue
void display_queue_distribution()
{	
	printf("Queue Distribution: ");		// Print title
	struct node* temp = head;			// Temp will iterate beginning at head
	while (temp != NULL)				// While temp is not NULL...
	{
		// Print person's gender, ID, and required length of use in Gender(ID,Uses) format
		printf("%c(%d,%d) ", temp->requestor.gender, temp->requestor.id, temp->requestor.usage);
		temp = temp->next;				// Iterate to next node
	}
	printf("\n");						// Print line break
}


/*================
  App Functions
================*/

// Decrement mutex variable
void down(int *ref)
{
	(*ref)--;
}

// Increment mutex variable
void up(int *ref)
{
	(*ref)++;
}

// Generate a person who needs to use the bathroom
person generate_person(char gen)
{
	person p;						// Declare the person
	p.id = ++userId;				// Get unique ID from global pool
	p.gender = gen;					// Assign gender
	p.usage = rand() % 3 + 1;		// Randomly assign a required number of cycles in bathroom
	return p;						// Return the person
}

// Initilize value of each stall to zero
void initialize_stalls()
{
	int i;
	int len = sizeof(stalls) / sizeof(stalls[0]);
	for (i = 0; i < len; i++)
	{
		stalls[i] = 0;
	}
}

// Assign user's required number of uses to next available stall
void enter_stall(int use)
{
	int i;
	int len = sizeof(stalls) / sizeof(stalls[0]);
	for (i = 0; i < len; i++)
	{
		if (stalls[i] == 0)
		{
			stalls[i] = use;
			break;
		}
	}
}

// Allow woman to enter the bathroom
void woman_enters_bathroom(person w)
{
	down(&womanMutex);			// Unlock woman mutex
	down(&bathroomMutex);		// Unlock bathroom mutex
	womanCount++;				// Increment number of women in bathroom
	occupants++;				// Increment total occupants in bathroom
	enter_stall(w.usage);		// Assign user's required number of uses to next available stall
	up(&bathroomMutex);			// Lock bathroom mutex
}

// Allow man to enter the bathroom
void man_enters_bathroom(person m)
{
	down(&manMutex);			// Unlock man mutex
	down(&bathroomMutex);		// Unlock bathroom mutex
	manCount++;					// Increment number of men in bathroom
	occupants++;				// Increment total occupants in bathroom
	enter_stall(m.usage);		// Assign user's required number of uses to next available stall
	up(&bathroomMutex);			// Lock bathroom mutex
}

// Woman requests to use bathroom
void woman_wants_to_enter()
{
	// Generate a woman
	person woman = generate_person('W');
	
	// Get queue length
	int queLength = get_queue_length();

	// If the bathroom is not full and there are no men present, then enter the bathroom
	if (occupants < 5 && manMutex == 1  && queLength == 0)
	{
		woman_enters_bathroom(woman);
		printf("A woman (%d,%d) has entered the bathroom.\n", woman.id, woman.usage);
	}
	// Otherwise, add the woman to the queue
	else
	{
		enqueue(woman);
		printf("A woman (%d,%d) has entered the queue.\n", woman.id, woman.usage);
	}
}

// Man requests to use the bathroom
void man_wants_to_enter()
{
	// Generate a man
	person man = generate_person('M');
	
	// Get queue length
	int queLength = get_queue_length();

	// If the bathroom is not full and there are no women present, then enter the bathroom
	if (occupants < 5 && womanMutex == 1 && queLength == 0)
	{
		man_enters_bathroom(man);
		printf("A man (%d,%d) has entered the bathroom.\n", man.id, man.usage);
	}
	// Otherwise, add the man to the queue
	else
	{
		enqueue(man);
		printf("A man (%d,%d) has entered the queue.\n", man.id, man.usage);
	}
}

/*
	Process people's request to use the bathroom according to the following rules:
	(1) Queued people may enter the bathroom before new requestors
	(2) Adjacent queued people of the same gender may enter on the same cycle if stalls are available
	(3) New requestors may enter bathroom directly if queue is empty prior to cycle
	(4) New requestors may enter bathroom directly if queue is emptied during current cycle
	(5) New requestors must enter the queue if queue is not empty
		- Ex 1:  Person of different gender is already in queue
		- Ex 2:  Person of same gender is in queue but cannot enter bathroom due to no vacancy
*/
void acquire_bathroom()
{
	// If people are waiting for the bathroom (i.e., queue exists) and bathroom is not full
	int queLength = get_queue_length();
	if (queLength > 0 && occupants < 5)
	{
		// If first person matches gender currently in bathroom, enter bathroom
		char firstGender = get_first_in_queue_gender();
		if (manMutex == 1 && firstGender == 'W')
		{
			// Allow adjacent queued women to enter the bathroom on the same cycle if stalls are available
			do {
				person queuedWoman = dequeue();
				woman_enters_bathroom(queuedWoman);
				printf("A woman (%d,%d) has moved from the queue to the bathroom.\n", queuedWoman.id, queuedWoman.usage);
				queLength = get_queue_length();
				firstGender = get_first_in_queue_gender();
			} while (occupants < 5 && queLength > 0 && firstGender == 'W');
		}
		else if (womanMutex == 1 && firstGender == 'M')
		{
			// Allow adjacent queued men to enter the bathroom on the same cycle if stalls are available
			do {
				person queuedMan = dequeue();
				man_enters_bathroom(queuedMan);
				printf("A man (%d,%d) has moved from the queue to the bathroom.\n", queuedMan.id, queuedMan.usage);
				queLength = get_queue_length();
				firstGender = get_first_in_queue_gender();
			} while (occupants < 5 && queLength > 0 && firstGender == 'M');
		}
	}

	// Generate pseudorandom number
	int selector = rand() % 2;

	// Determine if a woman or man wants to enter
	if (selector == 0) woman_wants_to_enter();
	else man_wants_to_enter();
}

// Remove woman from bathroom
void woman_leaves()
{
	down(&bathroomMutex);							// Unlock bathroom mutex
	womanCount--;									// Decrement number of women in bathroom
	occupants--;									// Decrement total number of occupants
	up(&bathroomMutex);								// Lock bathroom mutex
	up(&womanMutex);								// Lock woman mutex
	printf("A woman has exited the bathroom.\n");	// Print notification
}

// Remove man from bathroom
void man_leaves()
{
	down(&bathroomMutex);							// Unlock bathroom mutex
	manCount--;										// Decrement number of men in bathroom
	occupants--;									// Decrement total number of occupants
	up(&bathroomMutex);								// Lock bathroom mutex
	up(&manMutex);									// Lock man mutex
	printf("A man has exited the bathroom.\n");		// Print notification
}

/*
	Releases people from the bathroom according to the following rules:
	(1) On each cycle decrement the remaining number of required uses by one
	(2) If any user's required uses changes from one to zero, remove the user
*/
void release_bathroom()
{
	if (occupants > 0)
	{
		int i;
		int len = sizeof(stalls) / sizeof(stalls[0]);
		for (i = 0; i < len; i++)
		{
			if (stalls[i] > 1) stalls[i]--;
			else if (stalls[i] == 1)
			{
				stalls[i]--;
				if (womanCount > 0 && manCount == 0) woman_leaves();
				else if (manCount > 0 && womanCount == 0) man_leaves();
			}
		}
	}
}

// Print stall usage
void display_stalls()
{
	int i;
	for (i = 0; i < 5; i++)
	{
		printf("Stall %d: %d\n", i, stalls[i]);
	}
	printf("\n");
}

// Print bathroom stats
void display_bathroom_stats()
{
	/*
	printf("\n***** Mutexes *****\n");
	printf("Bathroom Mutex: %d\n", bathroomMutex);
	printf("Woman Mutex: %d\n", womanMutex);
	printf("Man Mutex: %d\n", manMutex);
	*/
	printf("\n***** Occupancy *****\n");
	printf("Bathroom Occupants: %d\n", occupants);
	printf("Women Using Bathroom: %d\n", womanCount);
	printf("Men Using Bathroom: %d\n", manCount);
	printf("\n***** Queue *****\n");
	display_queue_length();
	display_queue_women();
	display_queue_men();
	display_queue_distribution();
	printf("\n***** Remaining Stall Uses *****\n");
	display_stalls();
}


/*====================================
  Main Program
====================================*/

int main(int argc, char *argv[])
{
	// User input
	int usrInpt;

	// Set seed variable for pseudorandom number generation
	srand(time(NULL));

	// Welcome user
	printf("\nWelcome to the University bathroom!\n");


	// App engine
	do
	{
		// Prompt user for input
		printf("\nEnter the desired number of cycles or 0 to quit: ");
		scanf("%d", &usrInpt);

		// Run program for requested number of cycles
		int cycleCounter;
		for (cycleCounter = 0; cycleCounter < usrInpt; cycleCounter++)
		{
			// Display cycle number
			printf("\n============================\n");
			printf("          Cycle %d", ++cycles);
			printf("\n============================\n");
			printf("\n***** Activity *****\n");

			// Consumer
			release_bathroom();

			// Producer
			acquire_bathroom();
			
			// Stats
			display_bathroom_stats();
		}
	} while (usrInpt != 0);

	// Inform user program is ending
	printf("\nThe bathroom is now closed. Goodbye.\n\n");

	// Exit
	return 0;
}
