/* 
 * File:   schedulability.h
 * Author: admin
 *
 * Created on January 19, 2016, 3:25 PM
 */

#ifndef SCHEDULABILITY_H

#include "generation2.h"

bool sortpriority(int t1, int t2)
{
    if(tasks[t1].priority < tasks[t2].priority)
        return true;
    return false;
}

float calcInterference(int i)
{
    float ri1,ri2=0.0001;
    int procID=tasks[i].procAlloc;
    do
        {
            ri1=ri2;
            ri2=0;
            
            for(int j=0;j<processors[procID].tasks.size();j++)
            {
                int h=processors[procID].tasks[j];
                if(tasks[i].priority > tasks[h].priority)
                        ri2+=tasks[h].c*ceil(ri1/float(tasks[h].t));
            }

            if(ri2>tasks[i].t)
            {
                
                return -1;
                
            }   
        }
        while(ri2>ri1);
        ri2+=tasks[i].offset;
        return ri2;
}

void calcOffset(int i)
{
   
            float maxOffset=0.0;
            for(int j=0;j<tasks[i].incomingLinks.size();j++)
                if(links[tasks[i].incomingLinks[j]].delay==false && tasks[links[tasks[i].incomingLinks[j]].sender].procAlloc!=-1
                        && tasks[links[tasks[i].incomingLinks[j]].sender].procAlloc!=tasks[i].procAlloc)
                    maxOffset=max(maxOffset,tasks[links[tasks[i].incomingLinks[j]].sender].totRespTime);
            
            for(int j=0;j<tasks[i].outgoingLinks.size();j++)
                if(links[tasks[i].outgoingLinks[j]].delay==true && tasks[links[tasks[i].outgoingLinks[j]].receiver].procAlloc!=-1
                        && tasks[links[tasks[i].outgoingLinks[j]].receiver].procAlloc!=tasks[i].procAlloc)
                {
                    float tempval=calcInterference(links[tasks[i].outgoingLinks[j]].receiver);
                    if(tempval==-1)
                        maxOffset=tasks[i].t;
                    else
                       maxOffset=max(maxOffset,tempval); 
                }
                   // maxOffset=max(maxOffset,tasks[links[tasks[i].outgoingLinks[j]].receiver].totRespTime);
            
            tasks[i].offset=maxOffset;
            
}


bool schedAnalysis()
{
    float ri1,ri2;
    bool sched=true;
    bool cont=true;
    
    for(int i=0;i<tasks.size();i++)
        tasks[i].offset=0;
    
//    for(int i=0;i<tasks.size();i++)
//        cout<<i<<"  "<<tasks[i].procAlloc<<"  "<<tasks[i].priority<<endl;
//    cout<<"-----"<<endl;
    
    vector <float> tempR;
    for(int i=0;i<tasks.size();i++)
        tempR.push_back(0);

        
    do
    {
    
        for(int i=0;i<tasks.size();i++)
        if(tasks[i].procAlloc != -1)
        {
           
            ri2=tasks[i].c;
            int procID=tasks[i].procAlloc;

            do
            {
                ri1=ri2;
                ri2=tasks[i].c;

                for(int j=0;j<processors[procID].tasks.size();j++)
                {
                    int h=processors[procID].tasks[j];
                    if(tasks[i].priority > tasks[h].priority)
                            ri2+=tasks[h].c*ceil(ri1/float(tasks[h].t));
                }

                if(ri2>tasks[i].t)
                {

                    sched=false;
                    break;
                }   
            }
            while(ri2>ri1);
            
            tasks[i].totRespTime=ri2;
        }
        
        
        if(sched==true)
        {
            for(int i=0;i<tasks.size();i++)
                if(tasks[i].procAlloc != -1)
                {
                    calcOffset(i);
                   tasks[i].totRespTime+=tasks[i].offset;
                   if(tasks[i].totRespTime > tasks[i].t)
                   {
                       sched=false;
                       break;
                   }
                }
        }
                 
        if(sched==true)
        {
            cont=false;
            for(int i=0;i<tasks.size();i++)
            if(tasks[i].procAlloc != -1)
            {
                if(tasks[i].totRespTime != tempR[i])
                {
                    cont=true;
                    
                }
                tempR[i]=tasks[i].totRespTime;
            }
        }
    
    }
    while(sched==true && cont==true);
    
    return sched;
}





#define	SCHEDULABILITY_H



#endif	/* SCHEDULABILITY_H */

