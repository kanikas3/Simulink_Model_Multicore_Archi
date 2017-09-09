/* 
 * File:   main.cpp
 * Author: admin
 *
 * Created on January 18, 2016, 1:37 PM
 */

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <utility>
#include <string>
#include <stdio.h>
#include <time.h>
#include <vector>
#include <set>

using namespace std;

std::ofstream debug4("delays.txt",std::ios::out);
std::ofstream debug5("results.txt",std::ios::out);
std::ofstream debug6("links.txt",std::ios::out);
std::ofstream debug7("RTA.txt",std::ios::out);
std::ofstream debug8("tasks.txt",std::ios::out);


time_t seed;
int numSystems=100;

#include"generation2.h"
#include"schedulability.h"

void algo();

/*
 * 
 */
int main(int argc, char** argv) {
    
   
    seed=time(NULL);
    //seed=1481219055;
    cout << "Seed = " << seed << endl;
    int numSched=0;
    int numDelays=0;
    int numLinks=0;
    time_t clk1=clock();
    
    for(int i=0;i<numSystems;i++)
    {
    
        generateProcessors();
        while(generateTasks()==false);
        printSystem();
        
        algo();

        bool sched=true;
        for(int i=0;i<tasks.size();i++)
            if(tasks[i].procAlloc==-1)
                sched=false;

        if(sched && schedAnalysis()==true)
        {
           cout<<"sched"<<endl;
           printRespTimes();
           printDelays();
           numSched++;
           
           for(int j=0;j<links.size();j++)
           {
               numLinks++;
               if(links[j].delay==true)
                   numDelays++;
               
           }
        }
        else
            cout<<"NOT sched"<<endl;
       
        deAllocateTasks();
    }
    
    debug5<<"time="<<float(clock()-clk1)/float(CLOCKS_PER_SEC)<<endl;
    debug5<<"at average u of "<<avgU<<" scheduled "<<numSched<<" out of "<<numSystems<<endl;
    debug5<<"Delays: "<<numDelays<<"/"<<numLinks<<"   "<<float(numDelays)/float(numLinks)<<endl;

    
    cout<<"at average u of "<<avgU<<" scheduled "<<numSched<<" out of "<<numSystems<<endl;
    cout<<"Delays: "<<numDelays<<"/"<<numLinks<<"   "<<float(numDelays)/float(numLinks)<<endl;
    
     
     
    
    return 0;
}

// populate the task array with tasks numbered 0 to avgTaskCnt 
void fillTaskArray ( int array[] )
{
  for ( int i = 0 ; i < avgTaskCnt ; i++ ) 
     array[i] = i ;
}

// if task is allocated return true, else false
bool isAllocated ( int task ) 
{
  if ( tasks.at(task).procAlloc != -1 )
     return true;
  else
     return false;
} 

// sort the input task set by period
void sortByPeriod ( int array[] )
{
  int swap ;
  for ( int i = 0 ; i < ( avgTaskCnt - 1 ) ; i++ )
  {
     for ( int j = 0 ; j < ( avgTaskCnt - i - 1 ) ; j++ ) 
     {
        if ( tasks.at(array[j]).t > tasks.at(array[j+1]).t )
        {
           swap = array[j];
           array[j] = array[j+1];
           array[j+1] = swap;
        } 
     }
  }
}

// Insert the task on the core with highest priority task having shortest deadline first
// return true if schedulable, else false
bool tryTask ( int task , int core ) 
{
  bool sched = true ;

  // determine priority
  int priority = 0 ;
  for ( int i = 0 ; i < processors.at(core).tasks.size() ; i++ ) 
     if ( tasks[processors.at(core).tasks.at(i)].t <= tasks[task].t )
       ++priority;

  // increase ( rather decrease ) priority of tasks with higher period
  for ( int i = 0 ; i < processors.at(core).tasks.size() ; i++ )
     if ( tasks[processors.at(core).tasks.at(i)].t > tasks.at(task).t )
        tasks[processors.at(core).tasks.at(i)].priority++;

  // assign the task to core
  assignTask ( task , core , priority ) ;
  if ( !schedAnalysis() ) 
  {

     cout << "Introducing RT delay" << endl;
     // introduce RT delay blocks
     for ( int j = 0 ; j < tasks[task].incomingLinks.size() ; j++ ) 
        if ( tasks[links[tasks[task].incomingLinks[j]].sender].procAlloc!=-1 &&
             tasks[links[tasks[task].incomingLinks[j]].sender].procAlloc!=tasks[task].procAlloc )
           links[tasks[task].incomingLinks[j]].delay=true;
     for ( int j = 0 ; j < tasks[task].outgoingLinks.size() ; j++ )
        if ( tasks[links[tasks[task].outgoingLinks[j]].receiver].procAlloc!=-1
             && tasks[links[tasks[task].outgoingLinks[j]].receiver].procAlloc!=tasks[task].procAlloc )
           links[tasks[task].outgoingLinks[j]].delay=true; 

     if ( !schedAnalysis() ) 
     {
        sched = false; 
        // reset RT delay blocks
        for ( int j = 0 ; j < tasks[task].incomingLinks.size() ; j++ ) 
           links[tasks[task].incomingLinks[j]].delay=false;
        for ( int j = 0 ; j < tasks[task].outgoingLinks.size() ; j++ )
           links[tasks[task].outgoingLinks[j]].delay=false;

        // reset priority of remaining task
        for ( int i = 0 ; i < processors.at(core).tasks.size() ; i++ )
           if ( tasks[processors.at(core).tasks.at(i)].t > tasks.at(task).t )
             tasks[processors.at(core).tasks.at(i)].priority--;

        unassignTask ( task , core ) ;
     }
  }

  if ( !sched ) 
    cout << "unable to assign task " << task << " on core " << core << endl;

  return sched;
}

// check the affinity of the task with each processor
void checkProcessorAffinity ( int task , processorAffinity *array ) 
{
   // reset the array 
   for ( int i = 0 ; i < procCnt ; i++ ) 
   {
      array[i].pID = i ;
      array[i].affinity = 0 ;
   }

   for ( int i = 0 ; i < procCnt ; i++ ) 
   {
      for ( int j = 0 ; j < processors[i].tasks.size() ; j++ ) 
      {
          bool flag = false;
 
          for ( int k = 0 ; k < tasks[task].pred.size() ; k++ ) 
          {
              if ( processors[i].tasks[j] == tasks[task].pred[k] )
              {
                 array[i].affinity++;
                 flag = true;
                 break;
              }
          }

          if ( !flag ) 
          {
             for ( int k = 0 ; k < tasks[task].succ.size() ; k++ ) 
             {
                if ( processors[i].tasks[j] == tasks[task].succ[k] )
                { 
                   array[i].affinity++;
                   break;
                }
             }
          }
      } // for each task 
   } // for each processor 

   // sort by affinity
   processorAffinity swap;
   for ( int i = 0 ; i < (procCnt - 1 ) ; i++ )
   {
       for ( int j = 0 ; j < (procCnt - i - 1 ) ; j++ )
       {
           if ( array[j].affinity < array[j+1].affinity )
           {
              swap = array[j];
              array[j] = array[j+1];
              array[j+1] = swap;
           }
       }
   }
   
} 

void algo()
{
  int *taskArray ;
  processorAffinity *affinityArr ;

  taskArray = new int[avgTaskCnt] ;
  affinityArr = new processorAffinity[procCnt];

  fillTaskArray ( taskArray ) ;
  sortByPeriod ( taskArray ) ;

  bool sched ; 
  int  core, nextCore = 0 ;

  // for each task in the sorted list
  for ( int i = 0 ; i < avgTaskCnt ; i++ ) 
  {
     // skip if already allocated
     if ( isAllocated ( taskArray[i] ) )
     {
        cout << " Task " << taskArray[i] << " is already allocated" << endl ;
        continue ; 
     }

     //cout << " Trying to schedule " << taskArray[i] << endl ;
     sched = false;

     checkProcessorAffinity ( taskArray[i] , affinityArr ) ; 

     for ( int j = 0 ; j < procCnt ; j++ ) 
     {
        cout << " Affine core = " << affinityArr[j].pID << " affinity = " << affinityArr[j].affinity << endl;

        // redistribute
        if ( j == 0 && affinityArr[j].affinity == 0 && processors[nextCore].u <= utilizationB ) 
        {
           cout << " Trying to schedule " << taskArray[i] << " on core " << nextCore << endl ;
           if (sched = tryTask ( taskArray[i] ,  nextCore ))
           {
              core = nextCore ;
              cout << "Task " << taskArray[i] << " was scheduled on core " <<  nextCore << endl ;
              break;
           }
        }
 
        if ( processors[affinityArr[j].pID].u > utilizationB )
           continue;

        cout << " Trying to schedule " << taskArray[i] << " on core " << affinityArr[j].pID << endl ;
        if (sched = tryTask ( taskArray[i] ,  affinityArr[j].pID))
        {
           core = affinityArr[j].pID ;
           cout << "Task " << taskArray[i] << " was scheduled on core " <<  affinityArr[j].pID << endl ;
           break;
        }
        else
           cout << "Task " << taskArray[i] << " was scheduled on not core " << affinityArr[j].pID << endl ;
     }
  
     if ( sched ) 
     {
        // try the children
        nextCore = (core + 1 ) == procCnt ? 0 : core + 1 ;
        for ( int j = 0 ; j < tasks[taskArray[i]].succ.size() ; j++ )
            if (!isAllocated(tasks[taskArray[i]].succ[j]))
               if (tryTask ( tasks[taskArray[i]].succ[j] , core ))
                  cout << " Task " << tasks[taskArray[i]].succ[j] << " was scheduled on core " << core << endl;
               else
                  cout << " Task " << tasks[taskArray[i]].succ[j] << " was not scheduled on core " << core << endl;
     } 
  }

  // retry the unallocated tasks and try to bruteforce on each core without considering upper bound
  for ( int i = 0 ; i < avgTaskCnt ; i++ )
  {
      if (!isAllocated(taskArray[i]))
      {
         cout << " Retrying task " << taskArray[i] << endl;
         for ( int j = 0 ; j < procCnt ; j++ )
         {
             if (tryTask(taskArray[i],j))
             {
                cout << " Task " << taskArray[i] << " was scheduled on core " << j << endl ;
                break;
             }
         }
      }
  } 
}
