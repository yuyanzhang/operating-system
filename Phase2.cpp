// Phase2.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "string"

#include <iostream>
#include <cstdlib> 
#include <windows.h>


using namespace std;

#define proceNum 4 //all array number
#define Runproce 2  //allowed numbers of process running
#define MemNum 10 //all memory number

//implement simulated memory for process to mount
int Memory[MemNum] = { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10 };

static int timeCounter = 0;

//PCB block statement
//ID for number of PCB enrolled or a given main key to be uniquely defined
//Data for items to hold
//*Next points to next node of PCB in queue (NULL available for tail node)
//*Before points to former node of PCB in queue (NULL available for head node)
typedef struct PCB
{
	string ID;
	int memsize;//for process simulated size
	int Runcounter;//for virtual run time
	int mounted;//for record mount place
	int startTime;//for record start time
	struct PCB *Next;
	struct PCB *Before;
}PCB;

//ReadyQueue for process going to be executed
//Head for first node
//Tail for last node

typedef struct Queue
{
	PCB *Head;
	PCB *Tail;
	int Qcounter = 0;
}Queue;

//WaitQueue for suspend nodes to hold
//Head for first node
//Tail for last node

//A variable for Qcounter
int QC = 0;

//For adding PCB, need a Head and Tail;
PCB *AllHead = NULL;
PCB *AllTail = NULL;

//For saving transferring PCB
PCB savedPCB;


bool isSamePid(string PID, PCB* Head);


void Printqueue(PCB *Header)
{

	while (Header != NULL)
	{
		printf("PCBid: ");
		cout << Header->ID << endl;
		printf("PCBdata: ");
	//	cout << Header->Data << endl;
		printf("\n");
		Header = Header->Next;
	}
}


int TerminateQueue(PCB *Header)
{

	PCB *pNext;
	if (Header == NULL)
	{
		printf("Queue is already empty\n");
		return 0;
	}
	else
	{
		while (Header->Next != NULL)
		{
			pNext = Header->Next;
			free(Header);
			Header = pNext;
		}
	}
	printf("Queue has been terminated\n");
}

//Add process into Queue
int AddQueue(PCB *Head, PCB* Tail, string ID, int startTime, int runCount, int memsize, int procemount, int Loc, int Qnum)
{


	if (Loc < 1)//case wrong input
	{

		AllHead = Head;
		AllTail = Tail;

		return Qnum;
	}

	//separate into 4 conditions
	//IF Queue is empty (Head = Tail = new process)
	//IF Location > number, add into tail
	//IF Loc == 1, add into head
	//normal condition (middle of queue)

	//empty queue
	if (Head == NULL)
	{
		PCB *p1 = (PCB *)malloc(sizeof(PCB));
		memset(p1, 0, sizeof(PCB));

		//create new PCB
		p1->Before = NULL;
		p1->Next = NULL;
	//	p1->Data = items;
		p1->ID = ID;

		//new features in Phase2
		p1->memsize = memsize;
		p1->mounted = procemount;
		p1->Runcounter = runCount;
		p1->startTime = startTime;

		//add into queue
		AllHead = p1;
		AllTail = p1;

		//increase a counter on QC
		Qnum++;
		//add PCB finished
		return Qnum;//number of PCBs in queue
	}

	//When queue is not empty but input number is bigger than number of PCBs in queue
	else if (Loc > Qnum)
	{
		//create new PCB
		PCB *p1 = (PCB *)malloc(sizeof(PCB));
		memset(p1, 0, sizeof(PCB));
	//	p1->Data = items;
		p1->ID = ID;
		//new features in Phase2
		p1->memsize = memsize;
		p1->mounted = procemount;
		p1->Runcounter = runCount;
		p1->startTime = startTime;

		//Put into queue 
		//Caution on last elements and Tail change
		Tail->Next = p1;
		p1->Before = Tail;
		p1->Next = NULL;
		Tail = p1;//need to output changed tail

		AllHead = Head;
		AllTail = Tail;

		Qnum++;
		return Qnum;
	}

	else if (Loc == 1)//the ordered location is the head of queue and there are some PCBs in queue
	{
		//create new PCB
		PCB *p1 = (PCB *)malloc(sizeof(PCB));
		memset(p1, 0, sizeof(PCB));
	//	p1->Data = items;
		p1->ID = ID;

		//new features in Phase2
		p1->memsize = memsize;
		p1->mounted = procemount;
		p1->Runcounter = runCount;
		p1->startTime = startTime;

		//Put into head of queue
		Head->Before = p1;
		p1->Next = Head;
		p1->Before = NULL;
		Head = p1;// need output the changed head

		AllHead = Head;
		AllTail = Tail;

		Qnum++;
		return Qnum;
	}
	else //normal condition, add new in the middle of queue
	{
		//create new PCB
		PCB *p1 = (PCB *)malloc(sizeof(PCB));
		memset(p1, 0, sizeof(PCB));
//		p1->Data = items;
		p1->ID = ID;

		//new features in Phase2
		p1->memsize = memsize;
		p1->mounted = procemount;
		p1->Runcounter = runCount;
		p1->startTime = startTime;

		//find location
		PCB* q1;
		PCB* q2;
		q1 = Head;
		q2 = Head;
		for (int i = 1; i < Loc; i++)//q1 points to the PCB after new Loc
		{
			q1 = q1->Next;
		}

		for (int i = 1; i < (Loc - 1); i++)//q2 points to the PCB before new Loc
		{
			q2 = q2->Next;
		}
		//put into queue
		p1->Next = q1;
		p1->Before = q1->Before;
		q1->Before = p1;
		q2->Next = p1;

		AllHead = Head;
		AllTail = Tail;

		Qnum++;
		return Qnum;
	}
}

bool isSamePid(string PID, PCB* Head)
{
	int i;
	PCB* ptr;
	ptr = Head;
	if (ptr == NULL)//the queue is empty
	{
		return false;
	}
	else if (ptr->Next == NULL)//only one PCB in queue
	{
		if (PID == ptr->ID)
		{
			printf("The process ID has already been used! Please try another one!\n");
			return true;
		}
		else
			return false;
	}
	else//More than one PCB is in queue
	{
		while (ptr != NULL)
		{
			if (PID == ptr->ID)
			{
				printf("The process ID has already been used! Please try another one!\n");
				return true;
			}
			ptr = ptr->Next;
		}
		return false;
	}
}

int DeleteQueue(PCB *Head, PCB *Tail, string DID, int Qnum)
{
	//Five conditions
	//0. Empty queue
	//1. Head is to be deleted
	//2. Normal Delete (middle PCBs)
	//3. Tail is to be deleted
	//4. given ID not found

	PCB* ptr = Head;
	//Case empty queue

	if (ptr == NULL)
	{
	//	printf("Queue is empty!\n");
		AllHead = Head;
		AllTail = Tail;
		return Qnum;
	}

	//find if first PCB is to be deleted
	else if (ptr->ID == DID)
	{
	//	printf("Target PCB found in Head\n");

		//save current process
		savedPCB.ID = ptr->ID;
		savedPCB.memsize = ptr->memsize;
		savedPCB.mounted = ptr->mounted;
		savedPCB.Runcounter = ptr->Runcounter;
		savedPCB.startTime = ptr->startTime;

		PCB* afterPCB = ptr->Next;
		//special case: only one PCB in line
		if (afterPCB == NULL)
		{
			AllHead = NULL;
			AllTail = NULL;
			free(ptr);
			Qnum--;
	//		printf("Target deleted\n");
	//		printf("Caution! No PCB in queue at moment\n");
			return Qnum;
		}
		afterPCB->Before = NULL;//delete links with first PCB
		AllHead = ptr->Next;//second PCB would be head
		AllTail = Tail;
		free(ptr);//release first PCB
		Qnum--;
	//	printf("Target deleted\n");
		return Qnum;
	}
	else//other three cases
	{
		ptr = ptr->Next;
		while (ptr != NULL)
		{

			if (ptr->ID == DID)//find target PCB
			{
				//save current process
				savedPCB.ID = ptr->ID;
				savedPCB.memsize = ptr->memsize;
				savedPCB.mounted = ptr->mounted;
				savedPCB.Runcounter = ptr->Runcounter;
				savedPCB.startTime = ptr->startTime;

				if (ptr->Next == NULL)//Tail is to be deleted
				{
		//			printf("Target PCB found in Tail\n");
					PCB* beforePCB = ptr->Before;
					AllTail = ptr->Before;
					AllHead = Head;
					beforePCB->Next = NULL;
					free(ptr);
		//			printf("Target Deleted\n");
					Qnum--;
					return Qnum;
				}
				else//normal condition
				{
			//		printf("Target PCB found in middle\n");
					AllHead = Head;
					AllTail = Tail;
					PCB* beforePCB = ptr->Before;
					beforePCB->Next = ptr->Next;
					PCB* afterPCB = ptr->Next;
					afterPCB->Before = ptr->Before;
					free(ptr);
			//		printf("Target Deleted\n");
					Qnum--;
					return Qnum;
				}
			}
			ptr = ptr->Next;
			/**
			printf("Given ID not found! Please try again\n");
			AllHead = Head;
			AllTail = Tail;
			return Qnum;*/
		}
	//	printf("Given ID not found! Please try again\n");
		AllHead = Head;
		AllTail = Tail;
		return Qnum;
	}
}


//processList is a pointer for an array, holding all incoming process
void Loop()//mei chuan jin lai
{
	//initialize new queue
	Queue ReadyQueue;
	Queue RunQueue;
	ReadyQueue.Head = NULL;
	ReadyQueue.Tail = NULL;
	RunQueue.Head = NULL;
	RunQueue.Tail = NULL;

	//For test
	PCB processList[proceNum];

	processList[0].ID = "0001";
	processList[0].memsize = 1;
	processList[0].mounted = 1;
	processList[0].Runcounter = 2;
	processList[0].startTime = 0;

	processList[1].ID = "0002";
	processList[1].memsize = 1;
	processList[1].mounted = 1;
	processList[1].Runcounter = 3;
	processList[1].startTime = 1;

	processList[2].ID = "0003";
	processList[2].memsize = 1;
	processList[2].mounted = 1;
	processList[2].Runcounter = 10;
	processList[2].startTime = 0;

	processList[3].ID = "0004";
	processList[3].memsize = 1;
	processList[3].mounted = 1;
	processList[3].Runcounter = 4;
	processList[3].startTime = 0;

	//test end

	

	//start looping
	while (1)
	{
		//adding new process into queue
		int i;
		int queueNum;
		for (i = 0; i < proceNum; i++)
		{
			if (processList[i].startTime == timeCounter)
			{
				ReadyQueue.Qcounter = AddQueue(ReadyQueue.Head, ReadyQueue.Tail, processList[i].ID, processList[i].startTime, processList[i].Runcounter, processList[i].memsize, processList[i].mounted, 11, ReadyQueue.Qcounter);
				queueNum = ReadyQueue.Qcounter;
				ReadyQueue.Head = AllHead;
				ReadyQueue.Tail = AllTail;
				printf("Added a new process in time: %d\n", i);
			//	printf("Add process id: %s\n", processList[i].ID);
				cout << "Add process ID:" << processList[i].ID << endl;
				printf("Currently %d processes in queue\n", queueNum);
			}
		}

		//check if there are empty place for process to run
		while (RunQueue.Qcounter < Runproce && ReadyQueue.Head!= NULL)//当ReadyQueue里面没有东西时候也需要判断
		{

			//there are places for process to run
			PCB* ptr = ReadyQueue.Head;//first process in ReadyQueue is going to run
			ReadyQueue.Qcounter = DeleteQueue(ReadyQueue.Head, ReadyQueue.Tail, ptr->ID, ReadyQueue.Qcounter);
			ReadyQueue.Head = AllHead;
			ReadyQueue.Tail = AllTail;
			RunQueue.Qcounter = AddQueue(RunQueue.Head, RunQueue.Tail, savedPCB.ID, savedPCB.startTime, savedPCB.Runcounter, savedPCB.memsize, savedPCB.mounted, 11, RunQueue.Qcounter);
			RunQueue.Head = AllHead;
			RunQueue.Tail = AllTail;

			//特殊情况 不够那么多进程
			if (ReadyQueue.Head == NULL)
			{
				break;
			}
		}

		//aging every process in RunQueue
		PCB* ptr = RunQueue.Head;
		while (ptr != NULL)
		{
			ptr->Runcounter--;
			//when a process finish job
			if (ptr->Runcounter <= 0)
			{
				//特殊情况 当一个进程运行完毕后会被删除
				//这时不能指向next 不然会出现问题
				//ptr需要再指向下一个
				PCB* ptr2 = ptr->Next;
				RunQueue.Qcounter = DeleteQueue(RunQueue.Head, RunQueue.Tail, ptr->ID, RunQueue.Qcounter);
				RunQueue.Head = AllHead;
				RunQueue.Tail = AllTail;
				ptr = ptr2;
			}
			else
				ptr = ptr->Next;
		}

		printf("System time: %d\n", timeCounter);
		timeCounter++;

//		getchar();
//		getchar();

		//final situation
		if (ReadyQueue.Head == NULL && RunQueue.Head == NULL)
		{
			printf("All processes finished their job\n");
			break;
		}

		//模拟运行时间
		Sleep(2000);
	}

}

int _tmain(int argc, _TCHAR* argv[])
{
	PCB pcblist[proceNum];

	pcblist[1].ID = '0001';
	pcblist[1].memsize = 1;
	pcblist[1].mounted = 1;
	pcblist[1].Runcounter = 2;
	pcblist[1].startTime = 0;

	pcblist[1].ID = '0002';
	pcblist[1].memsize = 1;
	pcblist[1].mounted = 1;
	pcblist[1].Runcounter = 3;
	pcblist[1].startTime = 1;

	//模拟运行时间
	Sleep(2000);

	Loop();

	getchar();
	return 0;
}

