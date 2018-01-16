#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>

using namespace std;

class process
{
private:
    int idx;
    double burst, arrival, start, waiting, turnaround, done;
    bool arrived;
public:
    int t1, t2;
    //ctor
    process(){idx=0; burst=0; arrival=0; start=0; waiting=0; turnaround=0; arrived=false; done=0; t1=0; t2=0;};
    process(int i, double a)
        {idx=i; burst=0; arrival=a; start=0; waiting=0; turnaround=0; arrived=false; done=0; t1=0; t2=0;};
    //dtor
    //~process();
    double getBurst(){return burst;};
    double getWaiting(){return waiting;};
    double getTurnaround(){return turnaround;};
    double getArrival(){return arrival;}
    double getDone(){return done;}
    bool getArrived(){return arrived;}
    int getIdx(){return idx;};
    void setStart(double st){start = st;};
    void setBurst(double b){burst = b;};
    void setArrival(double a){arrival = a;};
    void setArrived(){arrived = true;};
    void setDone(double t){done = t;};
    void setTime(int a, int b){t1=a; t2=b;};
    void setWaiting();
    void setTurnaround(double);
    void printProcess();
    bool operator < (const process &p) const { return idx<p.idx; };
};

bool cmpBurst(process &a, process &b)
{
    return a.getBurst() < b.getBurst();
}

bool cmpArrival(process &a, process &b)
{
    return a.getArrival() < b.getArrival();
}

void process::setWaiting()
{
    waiting += (start - arrival); //start time - arrival time
}

void process::setTurnaround(double a)
{
    turnaround = done - a; //end time + arrival time
}

void process::printProcess()
{
    cout << "P[" << idx << "]" << "\t";
    cout << waiting << "\t" << "\t";
    cout << turnaround << endl;
}

int main()
{
    vector<process> p;
    int processNum, t1, t2;
    cin >> processNum;
    for (int i=0; i<processNum; i++)
    {
        double arrival;
        cin >> arrival;
        process _p(i+1, arrival);
        p.push_back(_p);
    }
    for (int i=0; i<p.size(); i++)
    {
        double burst;
        cin >> burst;
        p[i].setBurst(burst);
    }
    cin >> t1 >> t2;

    double time = 0;
    //3 layers
    vector <process> RR1, RR2, SJF;
    //calculate all burst time
    int allBurst = 0;
    for (int i=0; i<p.size(); i++){
        allBurst += p[i].getBurst();
        p[i].setTime(t1, t2);
    }
///delete unnecessary line
    vector<process> arrival_p;
    for (int i=0; i<p.size(); i++)
        arrival_p.push_back(p[i]);
    allBurst += arrival_p[0].getArrival();
    for (int i=0; i<p.size(); i++){
        if(p[i].getArrival()+p[i].getBurst()>allBurst)
            allBurst = p[i].getArrival()+p[i].getBurst();
    }

    int cnt1=0, cnt2=0;
    process temp;
    bool runRR2=false;
    while(time<=allBurst){
        //push arrived process into RR1
        for (int i=0; i<p.size(); i++){
            if((p[i].getArrival()<=time) && (p[i].getArrived()==false)){
                RR1.push_back(p[i]);
                p[i].setArrived();
            }
        }
        //multilevel: RR1-RR2-SJF
        if (!RR1.empty() & !runRR2){ //t1
            p[RR1[0].getIdx()-1].setStart(time);
            p[RR1[0].getIdx()-1].setWaiting();
            time += 1;
            cnt1 += 1;
            p[RR1[0].getIdx()-1].setArrival(time);
            p[RR1[0].getIdx()-1].setBurst(p[RR1[0].getIdx()-1].getBurst()-1); //burst -= 1
///if done --> pop, if t1-->RR2
            if (!p[RR1[0].getIdx()-1].getBurst()){ //if done
                p[RR1[0].getIdx()-1].setDone(time);
                RR1.erase(RR1.begin());
                cnt1 = 0;
            } else if (cnt1==t1){ //if time up --> R2
                temp = RR1.front();
                RR1.erase(RR1.begin());
                RR2.push_back(temp);
                cnt1 = 0;
            }
        } else if (!RR2.empty()){ //t2
            runRR2 = true;
            p[RR2[0].getIdx()-1].setStart(time);
            p[RR2[0].getIdx()-1].setWaiting();
            time += 1;
            cnt2 += 1;
            p[RR2[0].getIdx()-1].setArrival(time);
            p[RR2[0].getIdx()-1].setBurst(p[RR2[0].getIdx()-1].getBurst()-1); //burst -= 1
///if done --> pop, if time_quantum --> SJF
            if (!p[RR2[0].getIdx()-1].getBurst()){ //if done
                p[RR2[0].getIdx()-1].setDone(time);
                RR2.erase(RR2.begin());
                cnt2 = 0;
                runRR2 = false;
            } else if (cnt2==t2){ //if time up --> R2
                temp = RR2.front();
                RR2.erase(RR2.begin());
                SJF.push_back(temp);
                cnt2 = 0;
                runRR2 = false;
            }

        } else if (!SJF.empty()){
            sort(SJF.begin(), SJF.end(), cmpBurst);
            p[SJF[0].getIdx()-1].setStart(time);
            p[SJF[0].getIdx()-1].setWaiting();
            time += p[SJF[0].getIdx()-1].getBurst();
            p[SJF[0].getIdx()-1].setBurst(0);
            p[SJF[0].getIdx()-1].setDone(time);
            SJF.erase(SJF.begin());
        } else {
            time += 1;
        }
    }

    //set turnaround time
    for (int i=0; i<arrival_p.size(); i++){
        p[arrival_p[i].getIdx()-1].setTurnaround(arrival_p[i].getArrival());
    }

    double averWait=0, averTurnaround=0;
    cout << "Process" << "\t";
    cout << "Waiting Time" << "\t";
    cout << "Turnaround Time" << endl;

    for (int i=0; i<p.size(); i++)
    {
        p[i].printProcess();
        averWait += p[i].getWaiting();
        averTurnaround += p[i].getTurnaround();
    }

    averWait /= static_cast<double>(processNum);
    averTurnaround /= static_cast<double>(processNum);

    cout << fixed << setprecision(7) << endl;
    cout << "Average waiting time: " << averWait << endl;
    cout << "Average turnaround time: " << averTurnaround << endl;
    return 0;
}
