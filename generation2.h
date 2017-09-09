/*
 * File:   genertaion.h
 * Author: Zaid
 *
 * Created on October 2, 2013, 10:33 PM
 */


#ifndef GENERTAION_H
#define	GENERTAION_H

float avgU=0.6;
float utilizationB=0.75;
const int procCnt=3;
int avgTaskCnt=100;
int periodRange[]={1, 2, 4, 5, 10, 20, 40, 50, 100, 200, 400, 500, 1000};
int periodRangeSize=13;

float resCost;

struct commLink
{
    int id;
    int sender;
    int receiver;
    float cost; // Not used for now
    bool delay;   //true=there is delay
};

struct task
{
        int tID;
        int succSize;  //NOT NEEDED FOR NOW
        float c;  //wcet
        int t; // period
        float u; // utilization
        int priority;
        float totRespTime;
        vector <int> incomingLinks;
        vector <int> outgoingLinks;
        vector <int> pred; // predecessors: tasks writing to the current task
        vector <int> succ; // successors: tasks reading from the current task
        float writersC; // Not used for now
        int procAlloc;// the processor to which the task is allocated. -1 means not allocated to any core
        float offset;
        vector <int> critPath; //not used for now
};

struct processor
{
    int pID; 
    vector <int> tasks; //tasks allocated to the processor
    float u; //utilization of the core 
};

struct processorAffinity
{
    int pID;
    int affinity;
};

vector <task> tasks;              // vector of systems tasks 
vector <processor> processors;    // vector of systems processors
vector <commLink> links;         // vector of communication lonks in the system 
vector < vector<int> > graph;      //Not useful 


int node_counter=0;           
int arc_counter=0;
int assignedTaskCount=0;

void tgff();

bool generateTasks(){

    
    int totalTaskCnt=0;
    node_counter=0;
    arc_counter=0;
    int randomNum=0;
    float ftemp1=0.0,ftemp2=0.0;
    float util=avgU*procCnt;
    int temp1=0;


    srand(seed);

     

    tasks.clear();
     links.clear();
    tasks.resize(avgTaskCnt);

    for(int i=0;i<avgTaskCnt;i++)
    {
        tasks[i].succSize=0;
       
        tasks[i].tID=i;
        tasks[i].totRespTime=0;
        tasks[i].priority=tasks.size();
        tasks[i].c=0.0;
        tasks[i].t=0;
        tasks[i].u=0;
        tasks[i].offset=0.0;
        tasks[i].procAlloc=-1;
        tasks[i].offset=0;
        tasks[i].incomingLinks.clear();
        tasks[i].outgoingLinks.clear();
        tasks[i].pred.clear();
        tasks[i].succ.clear();
        tasks[i].writersC=0;
    }
    
     tgff();
      
    bool rootFound=false;


     
// Period assignment
    
    for (int n=0;n<node_counter;n++)
	{
        vector <int> allowed,allowed2;
        
            allowed.clear();
            allowed2.clear();
            if(tasks[n].pred.size()==0)
            {
                for(int k=0;k<periodRangeSize;k++)
                    allowed.push_back(periodRange[k]);
            }
            else
            {
               
                for(int k=0;k<periodRangeSize;k++)
                    allowed.push_back(periodRange[k]);
                
                for(int k=0;k<tasks[n].pred.size();k++)
                {
                    for(int i=0;i<allowed.size();i++)
                    {
                        
                        if(allowed[i]==tasks[tasks[n].pred[k]].t)
                            allowed2.push_back(allowed[i]);
                        else if(allowed[i]>tasks[tasks[n].pred[k]].t && allowed[i]%tasks[tasks[n].pred[k]].t==0)
                            allowed2.push_back(allowed[i]);
                        else if(allowed[i]<tasks[tasks[n].pred[k]].t && tasks[tasks[n].pred[k]].t%allowed[i]==0)
                            allowed2.push_back(allowed[i]);
                    }
                    
                    allowed.clear();
                    allowed=allowed2;
                    allowed2.clear();
                   
                    
                    
                }
                
            }
           
            int temp=allowed.size();
            int ran=rand()%temp;
            tasks[n].t=allowed[ran]; 
           
                
            
    }

// Utilizaiton assignment
float utilizations[node_counter];
temp1=0;
for (int i=1; i<=node_counter-1;i++)
{
	ftemp1=(float) rand()/RAND_MAX;
	ftemp2=util*pow(ftemp1,1.0/float(node_counter-i));
        utilizations[i-1]=util-ftemp2;
        if(utilizations[i-1]>0.75)
        {
            utilizations[i-1]=0.75;
            ftemp2=util-0.75;

        }

	tasks[temp1].u=utilizations[i-1];
	++temp1;
	util=ftemp2;
}
tasks[temp1].u=util;
if(tasks[temp1].u>=1)
    return false;


// WCET assignment
for (int n=0;n<node_counter;n++)
{
    tasks[n].c=tasks[n].u*tasks[n].t;
}

//succSize assignment
for(int n=0;n<node_counter;n++)
    if(tasks[n].outgoingLinks.size()>0)
    {
        randomNum=rand()%10;
		switch(randomNum)
		{
		case 0:
			tasks[n].succSize=1;
			break;
                case 1:
                case 2:
			tasks[n].succSize=4;
			break;
		case 3:
                case 4:
			tasks[n].succSize=24;
			break;
		case 5:
            tasks[n].succSize=48;
			break;
                case 6:
                case 7:
			tasks[n].succSize=128;
			break;
		case 8:
			tasks[n].succSize=256;
			break;
		case 9:
			tasks[n].succSize=512;
			break;

        }
    }


return true;
}


void printSystem()
{
debug8<<seed<<endl;
debug6<<seed<<endl;
    for(int i=0;i<tasks.size();i++)
    {
        debug8<<i<<"\t"<<tasks[i].u<<"\t"<<tasks[i].t<<"\t"<<tasks[i].c<<"\t";
        debug8<<tasks[i].priority
               
                <<endl;  // ----add totRespTime;

    }

for(int i=0;i<tasks.size();i++)
        for(int j=0;j<tasks[i].outgoingLinks.size();j++)
            debug6<<links[tasks[i].outgoingLinks[j]].sender<<"->"<<links[tasks[i].outgoingLinks[j]].receiver
                    <<"\t"<<links[tasks[i].outgoingLinks[j]].cost<<endl;

  debug8<<"-------------------"<<endl;
  debug6<<"-------------------"<<endl;

}

void printRespTimes()
{
    debug7<<seed<<endl;
    debug7<<"task"<<"\t"<<"R"<<"\t"<<"T"<<"\t"<<"Offset"<<"\t"<<"priority"<<"\t"<<"allocation"<<endl;
    for(int i=0;i<tasks.size();i++)
        debug7<<i<<"\t"<<std::setprecision(3)<<tasks[i].totRespTime<<"\t"<<tasks[i].t<<"\t"<<std::setprecision(3)<<tasks[i].offset<<"\t"<<tasks[i].priority<<"\t"<<tasks[i].procAlloc<<endl;
    debug7<<"----------------------------------------"<<endl;
}

void printDelays()
{
    debug4<<seed<<endl;
    for(int i=0;i<links.size();i++)
    {
        debug4<<links[i].sender<<"->"<<links[i].receiver<<"\t"<<links[i].cost;
            if(links[i].delay==true)
                debug4<<"\t"<<"D";
        debug4<<endl;
    }
            
    debug4<<"--------------------------------"<<endl;
}

void printPrios()
{
    int level=0;
    do
    {
    for (int i=0;i<tasks.size();i++)
        if(tasks[i].priority==level)
        {
            debug5<<i<<"\t"<<tasks[i].priority<<endl;
            level++;
            break;
        }
    }
    while(level<tasks.size());
    debug5<<"--------------------------------------"<<endl;
}

void generateProcessors(){
    processors.resize(procCnt);
    for(int i=0;i<procCnt;i++)
    {
        processors[i].pID=i;
        processors[i].tasks.clear();
        processors[i].u=0;
    }
}

void tgff()
{
    int pos1=0;
    char temp[4];
    int x,randomnum;
    int source,dest;
char name[20],name2[20],name3[20],name4[20];


std::string line;
    x=sprintf (name, "simple%d.tgffopt",0);
     x=sprintf (name2, "simple%d.tgff",0);
     x=sprintf (name3,"./tgff simple%d",0);
     x=sprintf (name4,"%d.eps",seed);

	std::ofstream myout(name,std::ios::out);
        std::ifstream myfile(name2,std::ios::in);
	///////////////////////////////////////////parsing the tgff file
        
       
    do
    {

        myfile.close();
        myfile.open(name2,std::ios::in);
                
        myout.close();
        myout.open(name,std::ios::out);          
                
	myout<<"seed "<<seed<<endl;
	myout<<"tg_cnt 1"<<endl;
	myout<<"task_cnt "<<avgTaskCnt<<" 1"<<endl;
	myout<<"task_degree 2 3"<<endl;
	myout<<"period_laxity 1"<<endl;
        myout<<"period_mul 1"<<endl;
	myout<<"tg_write"<<endl;
        myout<<"eps_write"<<endl;

        system (name3);
        //rename("simple0.eps",name4);

        
        node_counter=0;
	while ( !myfile.eof() )
        {
		std::getline (myfile,line);

        if(line.find("TASK t") != std::string::npos)
	  {
		node_counter++;
		//break;
	  }
	}
        
        seed++;
    }
        while(node_counter != avgTaskCnt);


        myfile.close();
        myfile.clear();
        myfile.open(name2,std::ios::in);
        myfile.seekg (0, ios_base::beg);
         tasks.resize(node_counter);
         vector <int> row;
        for(int i=0;i<node_counter;i++)
        {
             row.clear();
            for(int j=0;j<node_counter;j++)
                row.push_back(0);

            graph.push_back(row);

        }
        

        while ( !myfile.eof() )
	{
            

	getline (myfile,line);
        
		if(line.find("ARC") != std::string::npos)
	  {
		  arc_counter++;


		pos1=line.find("t0_");
		pos1=pos1+3;
		temp[0]=line[pos1];
		temp[1]='\0';
		if(line[pos1+1]!=' ')
		{
			temp[1]=line[pos1+1];
			temp[2]='\0';
		}
		source=atoi(temp);

		pos1=line.rfind("t0_");
		pos1=pos1+3;
		temp[0]=line[pos1];
		temp[1]='\0';
		if(line[pos1+1]!=' ')
		{
			temp[1]=line[pos1+1];
			temp[2]='\0';
		}

	dest=atoi(temp);

        commLink c1;
        c1.id=links.size();
        c1.sender=source;
        c1.receiver=dest;
        c1.delay=false;
        //c1.cost=(float)rand()/RAND_MAX;
        
        links.push_back(c1);

        tasks[source].outgoingLinks.push_back(c1.id);
        tasks[source].succ.push_back(dest);
        tasks[dest].incomingLinks.push_back(c1.id);
        tasks[dest].pred.push_back(source);

        graph[source][dest]=1;


        }
}
         

float totc=1.0;
float ftemp1=0.0,ftemp2=0.0;
int ls=links.size();
for (int i=1; i<=links.size()-1;i++)
{
	ftemp1=(float) rand()/RAND_MAX;
	ftemp2=totc*pow(ftemp1,1.0/float(ls-i));
	links[i-1].cost=totc-ftemp2;
	totc=ftemp2;
}
//utilizations[node_counter-1]=util;
links[ls-1].cost=totc;   


}



void assignTask(int taskID, int procID, int priority)
{
    if(tasks[taskID].procAlloc != -1)
    {
        cout<<"ASSIGN FAIL IN "<<taskID<<endl;
        exit(1);
    }
    assignedTaskCount++;
    processors[procID].tasks.push_back(taskID);
    processors[procID].u+=tasks[taskID].u;
    
    tasks[taskID].procAlloc=procID;
    tasks[taskID].priority=priority;
    
}


void unassignTask(int taskID, int procID)
{
    
    if(tasks[taskID].procAlloc != procID)
    {
        cout<<"UNASSIGN FAIL"<<endl;
        exit(1);
    }

    assignedTaskCount--;
    int loc=-1;

    for(int i=0;i<processors[procID].tasks.size();i++)
        if(processors[procID].tasks[i] == taskID)
        {
            loc=i;
            break;
        }
    if(loc!=-1)
    {
        processors[procID].tasks.erase(processors[procID].tasks.begin()+loc);
        processors[procID].u-=tasks[taskID].u;
        
        
      
    }
    tasks[taskID].procAlloc=-1;
    tasks[taskID].priority=tasks.size();
        

}

void deAllocateTasks()
{
    for(int i=0;i<tasks.size();i++)
    {
        tasks[i].priority=tasks.size();
        if(tasks[i].procAlloc!=-1)
        unassignTask(i,tasks[i].procAlloc);
    }
    
    for(int j=0;j<links.size();j++)
        links[j].delay=false;
    
    
    processors.clear();
    generateProcessors();
    assignedTaskCount=0;
}



#endif	/* GENERTAION_H */

