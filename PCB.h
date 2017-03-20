//Inclusion Guards
#ifndef PCB_H_
#define PCB_H_

//Class for PCB information
class PCB
{
public:
//Variables needed for calculations
	int arrivalTime;
	int cycleNeeded;
	int cycleSoFar;
	int waitTime;
	int turnAround;

//No-Arg constructor, used for initialization
	PCB()
	{
		 arrivalTime = 0;
		 cycleNeeded = 0;
		 cycleSoFar = 0;
		 turnAround = 0;
		
	}

//Arg constructor, we will use this the most when creating PCB objects
	PCB(int newarrivalTime, int newcycleNeeded, int newcycleSoFar, int newturnAround)
	{
		arrivalTime = newarrivalTime;
		cycleNeeded = newcycleNeeded;
		cycleSoFar = newcycleSoFar;
		turnAround = newturnAround;

	}
};
#endif
