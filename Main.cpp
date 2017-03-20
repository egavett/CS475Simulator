#include<iostream>
#include"PCB.h"
#include"Process.h"
#include<queue>
#include<fstream>

using namespace std;

//Global variables used to calculate program statistics
int globalTime = 0;
int throughputTime;
int averageTATime;
int averageWTime;
int averageRTime;
int averageSwitchTime;
int processorUtilTIme;

//Queue to hold processes
queue<Process*> pQueue;
queue<Process*> CPUQueue[4];
queue<Process*> IOQueue;
vector<Process*> terminated;
Process* CPUs[4];

//Function for the FCFS scheduling policy
void FCFS()
{
	CPUQueue[0] = pQueue;
	int cont = 1;
	int burst = 0;
	while (cont == 1) {
		if (CPUs[0] == NULL) {
			CPUs[0] = CPUQueue[0].front();
			CPUQueue[0].pop();
			burst = CPUs[0]->myVec[CPUs[0]->currentBurst];	
			CPUs[0]->currentBurst += 1;
		}
		if (burst != 0) {
			burst -= 1;
		}
		else {
			if (CPUs[0]->currentBurst >= CPUs[0]->myVec.size()) {
				// Place process in a vector for finished processes
				terminated.push_back(CPUs[0]);
			}
			else {
				IOQueue.push(CPUs[0]);
			}
			CPUs[0] = NULL;
		}
		globalTime++;

	}
}

int main()
{
//Variables for file manipulation
	int PID;
	int nextTime;
	vector<int> timeVec;
	ifstream fileObject;
	Process *p;
//Open file to extract data
	fileObject.open("timeInfo.txt");

//Statement to check if file opened correctly
	if (fileObject.fail())
		{
			cout << "Could not open file" << endl;
		}

//While loop to read in data from file
	while (!fileObject.eof())
		{
			timeVec.clear();
			//memset(timeArray, -1, sizeof(timeArray));
			nextTime = 0;

			fileObject >> PID;
			int i = 0;
			while (fileObject >> nextTime && nextTime != -1)
				{
					timeVec.push_back(nextTime);
					i++;
				}
		
			PCB *block = new PCB();
			p = new Process(PID, 0, block, timeVec);
			pQueue.push(p);
		}

	FCFS();
	

	return 0;
}