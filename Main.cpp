#include<iostream>
#include"PCB.h"
#include"Process.h"
#include<queue>
#include<fstream>

using namespace std;

enum ProcessState
{
	STATE_NEW, STATE_READY, STATE_RUNNING, STATE_WAITING, STATE_FINISHED
};

// Used to track clock cycles
int globalTime = 0;

//Global variables used to calculate program statistics
int throughputTime;
int averageTATime;
int averageWTime;
int averageRTime;
int averageSwitchTime;
int processorUtilTIme;

// Multiprocessor Ready Queue
queue<Process*> pQueue;
// Array of Ready Queues: one for each processor
queue<Process*> CPUQueue[4];
// Queue of processes using the IO device
queue<Process*> IOQueue;	
int IOBurst = -1;

// Vector of processes that have finished executing
vector<Process*> terminated;
// Represent processes: pointers to each processes bing 'executed'
Process* CPUs[4];

// Simulates one cycle of wait for the IO 
void IOExecute()
{
	if (!IOQueue.empty()) {
		// Simulate waiting one clock cycle
		IOBurst--;
		if (IOBurst == 0) {
			IOQueue.front()->currentBurst += 1;

			// Return the process to the ready Queu
			CPUQueue[0].push(IOQueue.front());
			IOQueue.pop();

			// Reset the burst time
			IOBurst = IOQueue.front()->myVec[IOQueue.front()->currentBurst];
		} else if (IOBurst == -1) {
			// Set initial burst time
			IOBurst = IOQueue.front()->myVec[IOQueue.front()->currentBurst];
		}
	}
}

//Function for the FCFS scheduling policy
void FCFS()
{
	CPUQueue[0] = pQueue;
	int burst = 0;
	int cont = 1;
	while (cont == 1) {
		if (CPUs[0] == NULL) {
			CPUs[0] = CPUQueue[0].front();
			CPUQueue[0].pop();
			burst = CPUs[0]->myVec[CPUs[0]->currentBurst];	
			CPUs[0]->currentBurst += 1;
		}

		if (burst != 0) {
			burst -= 1;
		} else {
			if (CPUs[0]->currentBurst >= CPUs[0]->myVec.size()) {
				// Place process in a vector for finished processes
				terminated.push_back(CPUs[0]);
			}
			else {
				IOQueue.push(CPUs[0]);
			}
			CPUs[0] = NULL;
		}

		// Call the IOQueue to execute one cycle
		IOExecute();

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