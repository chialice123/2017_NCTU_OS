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
    //ctor
    process(){idx=0; burst=0; arrival=0; start=0; waiting=0; turnaround=0; arrived=false; done=0;};
    process(int i, double a)
        {idx=i; burst=0; arrival=a; start=0; waiting=0; turnaround=0; arrived=false; done=0;};
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
    int allBurst = 0;
    for (int i=0; i<p.size(); i++)
        allBurst += p[i].getBurst();

    vector<process> arrived_p;
    vector<process> arrival_p;
    for (int i=0; i<p.size(); i++)
        arrival_p.push_back(p[i]);
    allBurst += arrival_p[0].getArrival();
    for (int i=0; i<p.size(); i++){
        if(p[i].getArrival()+p[i].getBurst()>allBurst)
            allBurst = p[i].getArrival()+p[i].getBurst();
    }

    while(time<=allBurst){
        //set arrived process array
        for (int i=0; i<p.size(); i++){
            if ((p[i].getArrival()<=time) && (p[i].getArrived()==false)){
                arrived_p.push_back(p[i]);
                p[i].setArrived();
            }
        }
        //sort arrived process array
        sort(arrived_p.begin(), arrived_p.end(), cmpBurst);
        //sort arrival time of all process
        sort(arrival_p.begin(), arrival_p.end(), cmpBurst);
        if (!arrived_p.empty()){
            p[arrived_p[0].getIdx()-1].setStart(time);
            p[arrived_p[0].getIdx()-1].setWaiting();
            time += 1;
            p[arrived_p[0].getIdx()-1].setArrival(time);
            p[arrived_p[0].getIdx()-1].setBurst(p[arrived_p[0].getIdx()-1].getBurst()-1); //burst -= 1
            if (!p[arrived_p[0].getIdx()-1].getBurst()){
                p[arrived_p[0].getIdx()-1].setDone(time);
                arrived_p.erase(arrived_p.begin());
            }
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
