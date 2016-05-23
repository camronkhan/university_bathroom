# university_bathroom
CSC 389 Operating Systems Project 2

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
