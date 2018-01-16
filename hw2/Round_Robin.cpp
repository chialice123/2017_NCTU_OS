#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include <queue>
#define pb push_back
#define MAX_N 100
#define PAUSE {fgetc(stdin);}
using namespace std;
typedef vector<int> vi;
template <typename T>
string num_to_str(T num)
{
     ostringstream ss;
     ss << num;
     return ss.str();
}
string trick(int process_num)
{
    return "P["+num_to_str(process_num)+"]";
}
struct one_process
{
    int arrival_time, process_id, burst_time, waiting_time, ta_time;
};
int main()
{
    fstream fptr;
    fptr.open("RR.txt");

    vector<one_process> process;
    int some,tcase,wt=0,tt=0,cnt=0,time_quantum=0;//,total_tt=0,cnt=0;
    int data_in [MAX_N] ;
    bool get_tcase=0,get_burst=0;
    printf("Enter time quantum: ");
    scanf("%d",&time_quantum);
    while(fptr)
    {
        fptr>>data_in[cnt];
        cnt++;
    }
    //initilaize data storage
    process.resize(data_in[0]);
    tcase=data_in[0];
    cnt=0;
    //fetch the data from buffer data_in since the latter cause undefined behavior
    for(int i=1;i<=tcase*2;i++)
    {
        process[cnt];
        if(i<=tcase)
        {
            process[cnt].arrival_time=data_in[i];
            process[cnt].process_id=cnt; //no+1
            process[cnt].waiting_time=0;
            process[cnt].ta_time=0;
            cnt++;//count up for one data
            if(cnt==tcase)
            {
                get_burst=1;
                cnt=0;
            }
        }
        else
        {
            process[cnt].burst_time=data_in[i];
            cnt++;
        }
    }
    //initilaize calculation, find the first process to execute and make the waiting queue
    int time_el=0,task_el=0,cur_pid=0; //time elapsed, time elapsed for a certain task
    queue<one_process> ready_queue;
    for(int i=0;i<process.size();i++)
    {
        if(process[i].arrival_time==0)
        {
            process[i].waiting_time=0;
            cur_pid=i;
            break;
        }
    }
    /*for(int i=0;i<process.size();i++)
        cout<<"ar "<<process[i].arrival_time<<" bu "<<process[i].burst_time<<endl;*/
    //start to do Round Robin
    while(1)
    {
        //for the rest of the non-executing process, just imcrement their waiting time
        time_el++;
        process[cur_pid].burst_time--;
        printf("\nTime el %d find job %d, with burst %d \n",time_el,cur_pid+1,process[cur_pid].burst_time);
        for(int i=0;i<process.size();i++)//for the rest of the non-executing process, just imcrement their waiting time
        {
            if(time_el>process[i].arrival_time&&process[i].burst_time>0&&i!=cur_pid)
            {
                process[i].waiting_time++;
                cout<<"pid "<<i+1<<" timeup by 1";
            }
        }
        //Searcing for new process which has just arrived in the ready queue
        for(int i=0;i<process.size();i++)
        {
            //push the arrived process first, if happens concurrently
            if(time_el==process[i].arrival_time&&time_el)
            {
                ready_queue.push(process[i]);
                printf("  Push %d into the queue \n",i+1);
            }
        }

        //current executing process, so burst time--

        cout<<endl;
        if(process[cur_pid].burst_time==0)//if certain job has been done, than context switiching to the job on top of queue
        {
            tcase--;
            int old=cur_pid;
            process[cur_pid].ta_time=time_el-process[cur_pid].arrival_time;
            if(ready_queue.size())//in case of further access cause std::bad_alloc
            {
                cur_pid=ready_queue.front().process_id;
                ready_queue.pop();
            }
            printf("Pid %d has done, switch to %d\n",old+1,cur_pid+1);
        }
        else if(time_el&&((time_el)%time_quantum==0)) //context switching
        {
            if(process[cur_pid].burst_time)
            {
                ready_queue.push(process[cur_pid]);
            }
            printf("  Push %d into the queue ver2\n",cur_pid+1);
            if(ready_queue.size())//in case of further access cause std::bad_alloc
            {
                cur_pid=ready_queue.front().process_id;
                ready_queue.pop();
            }
            printf("Time qty is up, switch to process: %d \n",cur_pid+1);
        }
        queue<one_process> tmp(ready_queue);
        //printf("Ready Queue: ");
        while(tmp.size())
        {
            int jjjj=tmp.front().process_id;
            cout<<jjjj+1<<" ";
            tmp.pop();
        }
        if(tcase==0) //in case of further access cause std::bad_alloc
            break;
        //PAUSE;

    }
    cout<<"Process     Waiting Time     Turnaround Time"<<endl;
    for(int i=0;i<process.size();i++)
        cout<<setw(12)<<left<<trick(i+1)<<setw(17)<<left<<process[i].waiting_time<<setw(14)<<left<<process[i].ta_time<<endl;

    int total_wt=0,total_tt=0;
    for(int i=0;i<process.size();i++)
    {
        total_wt+=process[i].waiting_time;
        total_tt+=process[i].ta_time;
    }

    printf("Average waiting time: %lf\n",total_wt/(double)process.size());
    printf("Average turnaround time: %lf\n",total_tt/(double)process.size());
    fptr.close();
	return 0;
}
