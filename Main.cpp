#include<iostream>
#include"PCB.h"
#include"Process.h"
#include<queue>
#include<fstream>
#include <tuple>

using namespace std;

//State of Process
enum ProcessState {STATE_NEW, STATE_READY, STATE_RUNNING, STATE_WAITING, STATE_FINISHED};

enum Scheduler {SCHEDULER_FCFS, SCHEDULER_SPN};

// Used to track clock cycles
int globalTime = 0;

//Global variables used to calculate program statistics
double throughputTime;
double averageTATime;
double averageWTime;
double averageRTime;

double averageSwitchTime;
double processorUtilTIme;

double totalWaitFCFS;
double totalTurnFCFS;

double totalWaitSPN;
double totalTurnSPN;

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

vector<std::tuple<int, Process*>> SPNCPUQueue[4];

// Simulates one cycle of wait for the IO
void IOExecute(int scheduler)
{
	if (!IOQueue.empty()) 
	{
		// Simulate waiting one clock cycle
		if (IOBurst > 0) 
		{
			IOBurst--;
		}

		// Check to see if IOBurst is 0 if so update currentBurst and add this value to the CPUQueue
		if (IOBurst == 0) 
		{
			IOQueue.front()->currentBurst += 1;

			cout << "Process " <<IOQueue.front()->ID << " returned to the ready queue." << endl;

			// Return the process to the ready Queue for the active scheduler
			if (scheduler == SCHEDULER_FCFS) 
			{
				CPUQueue[0].push(IOQueue.front());
			}
	
			else if (scheduler == SCHEDULER_SPN)
			{
				tuple<int, Process*> p = make_tuple(IOQueue.front()->myVec[IOQueue.front()->currentBurst], IOQueue.front());
				if (!SPNCPUQueue[0].empty())
				{
					int index = 0;
					for (int i = 0; i < SPNCPUQueue[0].size(); i++)
					{
						if (IOQueue.front()->myVec[IOQueue.front()->currentBurst] <= get<0>(SPNCPUQueue[0][i]))
						{
							index = i;
						}
					}
					vector<tuple<int, Process*>>::iterator it = SPNCPUQueue[0].begin();
					it += index;
					it = SPNCPUQueue[0].insert(it, p);
				}
				else
				{
					SPNCPUQueue[0].push_back(p);
				}
			}
			IOQueue.pop();

			// Reset the burst time and add it to the total turn around time
			if (!IOQueue.empty()) 
			{
				IOBurst = IOQueue.front()->myVec[IOQueue.front()->currentBurst];
				cout <<"This is an IO burst:  " << IOBurst;
				totalTurnFCFS += IOBurst;
				totalTurnSPN += IOBurst;
			}
		} 

		else if (IOBurst == -1)
		{
			// Set initial burst time
			IOBurst = IOQueue.front()->myVec[IOQueue.front()->currentBurst];
			cout << "This is an IO burst:  " << IOBurst << endl;
		}
	} 
	else
	{
		IOBurst = -1;
	}
}




// Function for the FCFS scheduling policy
void FCFS()
{
	// Local variables to keep track of loop and burst times
	int cont = 1;
	int burst = 0;

	while (cont == 1) 
	{
		cout << "The current cycle is: " << globalTime << endl;

		// Add any newly arrived processes into the ready queue
		int check = 0;
		while (check == 0) 
		{
			// Make sure pqueue still contains processes
			if (!pQueue.empty()) 
			{
				// If the arrival time is less than or equal to the global time then push this value onto the CPUQueue and remove it from the pQueue
				if (pQueue.front()->object->arrivalTime <= globalTime)
				{
					CPUQueue[0].push(pQueue.front());
					cout << "Process " << pQueue.front()->ID << " has arrived." << endl;
					pQueue.pop();
				}
				else 
				{
					// exit while loop
					check = 1;
				}
			}
			else 
			{
				// exit while loop
				check = 1;
			}
		}

		if (CPUs[0] == NULL)
		{
			if (!CPUQueue[0].empty())
			{
				// Add processes to processor
				CPUs[0] = CPUQueue[0].front();
				CPUQueue[0].pop();

				cout << "Process " << CPUs[0]->ID<< " added to the CPU." << endl;
				// Keep updated burst time
				burst = CPUs[0]->myVec[CPUs[0]->currentBurst];
				CPUs[0]->currentBurst += 1;
				totalWaitFCFS += burst;
				totalTurnFCFS += burst; 
				//cout << CPUs[0]->myVec[CPUs[0]->currentBurst] << endl;
			}
			else if (pQueue.empty()) // No more processes left, then exit
			{
				cont = 0;
				break;
			}
		}

		if (burst != 0)
		{
			burst -= 1;
		}
		else
		{
			if (CPUs[0] != NULL)
			{
				if (CPUs[0]->currentBurst >= CPUs[0]->myVec.size())
				{
					// Place process in a vector for finished processes
					terminated.push_back(CPUs[0]);
					cout << "Process " << CPUs[0]->ID << " terminated." << endl;
				}
				else
				{
					// Otherwise add them to the IO queue
					IOQueue.push(CPUs[0]);
					cout << "Process " << CPUs[0]->ID << " pushed to the IOQueue." << endl;
				}
				CPUs[0] = NULL;
			}
		}

		// Call the IOQueue to execute one cycle
		IOExecute(SCHEDULER_FCFS);

		// Time click increase
		globalTime++;	
	}
}


void SPN()
{
	// Local variables to keep track of loop and burst times
	int cont = 1;
	int burst = 0;

	while (cont == 1)
	{
		cout << "The current cycle is: " << globalTime << endl;
		int check = 0;
		while (check == 0)
		{
			// Make sure pqueue still contains processes
			if (!pQueue.empty())
			{
				// If the arrival time is less than or equal to the global time then push this value onto the CPUQueue and remove it from the pQueue
				if (pQueue.front()->object->arrivalTime <= globalTime)
				{
					tuple<int, Process*> p = make_tuple(pQueue.front()->myVec[pQueue.front()->currentBurst], pQueue.front());

					if (!SPNCPUQueue[0].empty()) 
					{
						int index = 0;
						for (int i = 0; i < SPNCPUQueue[0].size(); i++) 
						{
							if (pQueue.front()->myVec[pQueue.front()->currentBurst] <= get<0>(SPNCPUQueue[0][i])) 
							{
								index = i;
							}
						}
						vector<tuple<int, Process*>>::iterator it = SPNCPUQueue[0].begin();
						it += index;

						it = SPNCPUQueue[0].insert(it, p);
					}

					else 
					{
						SPNCPUQueue[0].push_back(p);
					}

					cout << "Process " << pQueue.front()->ID << " has arrived." << endl;
					pQueue.pop();

					cout << "max burst time: " << get<0>(SPNCPUQueue[0].front()) << endl;

				}
				else
				{
					// exit while loop
					check = 1;
				}


				cout << "Process " << pQueue.front()->ID << " has arrived." << endl;
				pQueue.pop();

				cout << "max burst time: " << get<0>(SPNCPUQueue[0].front()) << endl;	
			}
		}

		if (CPUs[0] == NULL)
		{
			// Check to see if SPN has no processes
			if (!SPNCPUQueue[0].empty())
			{
				// Add processes to processor
				CPUs[0] = get<1>(SPNCPUQueue[0].front());
				SPNCPUQueue[0].erase(SPNCPUQueue[0].begin());

				cout << "Process " << CPUs[0]->ID << " added to the CPU." << endl;

				// Keep updated burst time and add it to the total wait time for processes
				burst = CPUs[0]->myVec[CPUs[0]->currentBurst];
				CPUs[0]->currentBurst += 1;
				totalWaitSPN += burst;
				totalTurnSPN += burst;
				//cout << CPUs[0]->myVec[CPUs[0]->currentBurst] << endl;
			}

			// No more processes left, then exit
			else if (pQueue.empty())
			{
				cont = 0;
				break;
			}
		}

		if (burst != 0)
		{
			burst -= 1;
		}
		else
		{
			if (CPUs[0] != NULL)
			{
				if (CPUs[0]->currentBurst >= CPUs[0]->myVec.size())
				{
					// Place process in a vector for finished processes
					terminated.push_back(CPUs[0]);
					cout << "Process " << CPUs[0]->ID << " terminated." << endl;
				}
				else
				{
					// Otherwise add them to the IO queue
					IOQueue.push(CPUs[0]);
					cout << "Process " << CPUs[0]->ID << " pushed to the IOQueue." << endl;
				}
				CPUs[0] = NULL;
			}
		}

		// Call the IOQueue to execute one cycle
		IOExecute(SCHEDULER_SPN);

		// Time click increase
		globalTime++;
	}
}

int main()
{
// Variables for file manipulation
	int PID;
	int nextTime;
	vector<int> timeVec;
	ifstream fileObject;
	Process *p;

// Open file to extract data
	fileObject.open("timeInfo2.txt");

// Statement to check if file opened correctly
	if (fileObject.fail())
	{
		cout << "Could not open file" << endl;
	}

// While loop to read in data from file
	while (!fileObject.eof())
	{
		// Clear the vector after each iteration so we can place new data in
		timeVec.clear();

		//memset(timeArray, -1, sizeof(timeArray));
		nextTime = 0;

		// Get the Process ID
		fileObject >> PID;

		int i = 0;
			 
		// While there is data to read and we don't reach the end of a line, add the data to the vector
		while (fileObject >> nextTime && nextTime != -1)
		{
			timeVec.push_back(nextTime);
			i++;
		}
		
		// Create a new process and add it to the queue
		PCB *block = new PCB();
		p = new Process(PID, 0, block, timeVec);
		pQueue.push(p);
	}

// Call first come first server function
	FCFS();
  //SPN();


////////Uncomment when ready to run/////////

	// Statement to assign and display statistics for FCFS
		averageWTime = totalWaitFCFS / terminated.size();
		averageTATime = totalTurnFCFS / terminated.size();
		cout << "Average wait time for FCFS: " << averageWTime << endl;
		cout << "Average turnaround time for FCFS: " << averageTATime << endl;
/*
	// Statement to assign and display statistics for SPN
		averageWTime = totalWaitSPN / terminated.size();
		averageTATime = totalTurnSPN / terminated.size();
		cout << "Average wait time for SPN: " << averageWTime << endl;
		cout << "Average turnaround time for SPN: " << averageTATime << endl;
*/

	return 0;
}