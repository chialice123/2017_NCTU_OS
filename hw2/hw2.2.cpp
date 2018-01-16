#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>

using namespace std;

class process
{
private:
    int idx;
    double burst, arrival, start, waiting, turnaround;
    bool arrived, done;
public:
    //ctor
    process(){idx=0; burst=0; arrival=0; start=0; waiting=0; turnaround=0; arrived=false; done=false;};
    process(int i, double a)
        {idx=i; burst=0; arrival=a; start=0; waiting=0; turnaround=0; arrived=false; done=false;};
    //dtor
    //~process();
    double getBurst(){return burst;};
    double getWaiting(){return waiting;};
    double getTurnaround(){return turnaround;};
    double getArrival(){return arrival;}
    bool getDone(){return done;}
    bool getArrived(){return arrived;}
    int getIdx(){return idx;};
    void setStart(double);
    void setBurst(double b){burst = b;};
    void setArrived();
    void setDone();
    void setWaiting();
    void setTurnaround();
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
    waiting = start - arrival; //start time - arrival time
}

void process::setTurnaround()
{
    turnaround = waiting + burst; //waiting time + burst time
}

void process::printProcess()
{
    cout << "P[" << idx << "]" << "\t";
    cout << waiting << "\t" << "\t";
    cout << turnaround << endl;
}

void process::setStart(double st)
{
    start = st;
}

void process::setArrived()
{
    arrived = true;
}

void process::setDone()
{
    done = true;
}

int main()
{
    vector<process> p;
    int processNum;
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

    double time = 0;
    sort(p.begin(), p.end(), cmpBurst);

    int allBurst = 0;
    for (int i=0; i<p.size(); i++)
        allBurst += p[i].getBurst();

    vector<process> arrival_p;
    for (int i=0; i<p.size(); i++)
        arrival_p.push_back(p[i]);
    sort(arrival_p.begin(), arrival_p.end(), cmpArrival);

    allBurst += arrival_p[0].getArrival();
    if ((arrival_p[processNum-1].getArrival()+arrival_p[processNum-1].getBurst())>allBurst)
        allBurst = (arrival_p[processNum-1].getArrival()+arrival_p[processNum-1].getBurst());
    cout << allBurst << endl;

    while(time <= allBurst)
    {
        for (int i=0; i<p.size(); i++){
            if (p[i].getArrival()<=time)
                p[i].setArrived();
        }

        bool nothingArrived = true;
        for (int i=0; i<p.size(); i++)
        {
            if(p[i].getArrived() && (!p[i].getDone())){
                p[i].setStart(time);
                time += p[i].getBurst();
                p[i].setWaiting();
                p[i].setTurnaround();
                p[i].setDone();
                nothingArrived = false;
                break;
            }
        }
        if (nothingArrived)
            time += 1;
    }


    stable_sort(p.begin(), p.end(), less<process>());
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
