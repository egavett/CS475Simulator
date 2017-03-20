//Inclusion Guards
#ifndef PROCESS_H_
#define PROCESS_H_
#include<vector>
#include"PCB.h"

//Class for Process information
class Process
{
public:
//Variables to identify each unique process
	int ID;
	int priority;
	PCB *object;
	std::vector<int> myVec;
	int currentBurst = 1;
	

//No-Arg constructor, used for initialization
	Process()
	{
		 ID = 0;
		 priority = 0;
	}

//Arg constructor,used most when creating new processes
	Process(int newID, int newPriority, PCB* newObject, std::vector<int> newVec)
	{
		ID = newID;
		priority = newPriority;
		object = newObject;
		
		for (int i = 0; i < newVec.size(); i++)
		{
			myVec.push_back(newVec[i]);
		}

		newObject->arrivalTime = myVec[0];
		newObject->cycleNeeded = 0;
		newObject->cycleSoFar = 0;
		newObject->turnAround = 0;

	}
};



#endif // ! PROCESS_H_

