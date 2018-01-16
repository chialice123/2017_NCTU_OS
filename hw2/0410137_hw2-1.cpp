#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>

using namespace std;

class process
{
private:
    int idx;
    double burst, arrival, start, waiting, turnaround;
public:
    //ctor
    process();
    process(int, double);
    //dtor
    //~process();
    double getBurst(){return burst;};
    double getWaiting(){return waiting;};
    double getTurnaround(){return turnaround;};
    int getIdx(){return idx;};
    void setStart(double);
    void setWaiting();
    void setTurnaround();
    void printProcess();
};

process::process()
{
    idx = 0;
    burst = 0;
    arrival = 0;
    start = 0;
}

process::process(int i, double b)
{
    idx = i;
    burst = b;
    arrival = 0;
    start = 0;
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


int main()
{
    vector<process> p;
    int processNum;
    cin >> processNum;
    for (int i=0; i<processNum; i++)
    {
        double burst;
        cin >> burst;
        process _p(i+1, burst);
        p.push_back(_p);
    }

    double time = 0;
    for (int i=0; i<p.size(); i++)
    {
        p[i].setStart(time);
        time += p[i].getBurst();
        p[i].setWaiting();
        p[i].setTurnaround();
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
