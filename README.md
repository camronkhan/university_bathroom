# university_bathroom

##The Problem
Suppose that a university wants to show off how progressive it is and ends its long standing practice of gender-segregated bathrooms on campus. However, as a concession to propriety, it makes a policy that when a woman is in the bathroom only other women may enter, but not men, and vice versa. On the door of every bathroom there will be a sign with a sliding marker that will indicate one of three possible states it is currently in:
- Empty
- Women Present
- Men Present

##General Rules:
- Only people of the same gender may be in the bathroom at the same time
- A maximum of 5 people can be in the bathroom at once
- If a person cannot access the bathroom, they must wait in a queue
- There is no maximum to the length of the queue
- Each user is randomly assigned a use length (measured in number of cycles) between 1 and 3
- Each user is given a unique ID, gender, and number of uses
  - Ex 1:  W(10,2)  =>  A woman with an ID 10 who must use the bathroom for 2 cycles
  - Ex 2:  M(1,5)   =>  A man with an ID 1 who must use the bathroom for 5 cycles

##Rules for Entering the Bathroom:
- Queued people may enter the bathroom before new requestors
- Adjacent queued people of the same gender may enter on the same cycle if stalls are available
- New requestors may enter bathroom directly if queue is empty prior to cycle
- New requestors may enter bathroom directly if queue is emptied during current cycle
- New requestors must enter the queue if queue is not empty
	- Ex 1:  Person of different gender is already in queue
	- Ex 2:  Person of same gender is in queue but cannot enter bathroom due to no vacancy

##Rules for Exiting the Bathroom:
- On each cycle decrement the remaining number of required uses by one
- If any user's required uses changes from one to zero, remove the user