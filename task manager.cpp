#include <iostream>
#include <ctime>
#include <fstream>
#include <vector>

using namespace std;

const int MaxDay=31+10;
const int MaxMonth=12+10;
const int CountMonth=12;
const int CountDay[]={0,31,28,31,30,31,30,31,31,30,31,30,31};

class Clock{
public:
    Clock(){ }
    Clock(int hour,int minute){ this->hour=hour; this->minute=minute; }
    void setHour(int hour){ this->hour=hour; }
    void setMinute(int minute){ this->minute=minute; }
    int getHour(){ return hour; }
    int getMinute(){ return minute; }
    bool operator < (Clock clk){
        if (hour==clk.getHour())
            return (minute<clk.getMinute());
        return (hour<clk.getHour());
    }
    bool operator > (Clock clk){
        if (hour==clk.getHour())
            return (minute>clk.getMinute());
        return (hour>clk.getHour());
    }
    Clock operator - (Clock clk){ return Clock(hour-clk.hour-(minute<clk.minute),minute-clk.minute+60*(minute<clk.minute)); }
    Clock operator + (Clock clk){ return Clock(hour+clk.hour+(minute+clk.minute)/60,(minute+clk.minute)%60); }
    void operator ++ (){ ++minute; hour+=(minute/60); minute%=60; }
    friend istream& operator >> (istream& input,Clock& clk);
    friend ostream& operator << (ostream& output,Clock& clk);
private:
    int hour;
    int minute;
};
istream& operator >> (istream& input,Clock& clk){ input >> clk.hour; input.ignore(1); input >> clk.minute; return input; }
ostream& operator << (ostream& output,Clock& clk){ output << (clk.hour<10 ? "0" : "") << clk.hour << ":" << (clk.minute<10 ? "0" : "") << clk.minute; return output; }

class Date{
public:
    Date(){ }
    Date(int day,int month){ this->month=month; this->day=day; }
    void setMonth(int month){ this->month=month; }
    void setDay(int day){ this->day=day; }
    int getMonth(){ return month; }
    int getDay(){ return day; }
    bool operator == (Date dt){ return (day==dt.day and month==dt.month); }
    friend istream& operator >> (istream& input,Date& dt);
    friend ostream& operator << (ostream& output,Date& dt);
private:
    int day;
    int month;
};
istream& operator >> (istream& input,Date& dt){ input >> dt.day; input.ignore(1); input >> dt.month; return input; }
ostream& operator << (ostream& output,Date& dt){ output << (dt.day<10 ? "0" : "") << dt.day << "-" << (dt.month<10 ? "0" : "") << dt.month; return output; }

class Time{
public:
    Time(){ }
    Time(Date dt,Clock clk){ this->dt=dt; this->clk=clk; }
    void setDate(Date dt){ this->dt=dt; }
    void setClock(Clock clk){ this->clk=clk; }
    Date getDate(){ return dt; }
    Clock getClock(){ return clk; }
    friend istream& operator >> (istream& input,Time& tme);
    friend ostream& operator << (ostream& output,Time& tme);
private:
    Date dt;
    Clock clk;
};
istream& operator >> (istream& input,Time& tme){ input >> tme.dt >> tme.clk; return input; }
ostream& operator << (ostream& output,Time& tme){ output << tme.dt << " " << tme.clk; return output; }

class Period{
public:
    Period(){ }
    Period(Clock strt,Clock end){ this->strt=strt; this->end=end; }
    void setStart(Clock strt){ this->strt=strt; }
    void setEnd(Clock end){ this->end=end; }
    Clock getStart(){ return strt; }
    Clock getEnd(){ return end; }
    Clock perLength(){ return end-strt; }
    void operator ++ (){ ++strt; ++end; }
    friend istream& operator >> (istream& input,Period& prd);
    friend ostream& operator << (ostream& output,Period& prd);
private:
    Clock strt;
    Clock end;
};
istream& operator >> (istream& input,Period& prd){ input >> prd.strt >> prd.end; return input; }
ostream& operator << (ostream& output,Period& prd){ output << prd.strt << " " << prd.end; return output; }

class Task{
public:
    Task(){ title="NONE"; dt=Date(0,0); prd=Period(Clock(0,0),Clock(0,0)); }
    Task(string title,Date dt,Period prd){ this->title=title; this->dt=dt; this->prd=prd; }
    void setTitle(string title){ this->title=title; }
    void setPeriod(Period prd){ this->prd=prd; }
    void setDate(Date dt){ this->dt=dt; }
    string getTitle(){ return title; }
    Period getPeriod(){ return prd; }
    Date getDate(){ return dt; }
    friend ostream& operator << (ostream& output,Task& task);
private:
    string title;
    Date dt;
    Period prd;
};
ostream& operator << (ostream& output,Task& task){ output << task.title << endl << task.dt << endl << task.prd; return output; }

class Day{
public:
    Day(){ tasks.clear(); }
    void freePrcs(Clock clk){
        for (int i=tasks.size()-1;i>=0;--i)
            if (tasks[i].getPeriod().getStart()<clk and clk<tasks[i].getPeriod().getEnd())
                tasks.erase(tasks.begin()+i);
    }
    void addTask(Task atk){
        if (tasks.empty() or tasks.back().getPeriod().getEnd()<atk.getPeriod().getEnd()){
            tasks.push_back(atk);
            return;
        }
        for (int i=tasks.size()-1;i>=0;--i)
            if (atk.getPeriod().getEnd()<tasks[i].getPeriod().getEnd()){
                tasks.insert(tasks.begin()+i,atk);
                return;
            }
    }
    Day findSchedule(){
        Day dsch;
        if (tasks.empty())
            return dsch;
        dsch.addTask(tasks[0]);
        Task last=tasks[0];
        for (int i=1;i<tasks.size();++i)
            if (last.getPeriod().getEnd()<tasks[i].getPeriod().getStart()){
                dsch.addTask(tasks[i]);
                last=tasks[i];
            }
        return dsch;
    }
    pair<Period,int> findPeriod(Clock org,Clock len){
        Period prd(org,org+len);
        pair<Period,int> ans=make_pair(prd,tasks.size());
        while(prd.getEnd()<Clock(24,00)){
            int cnt=0;
            for (int i=0;i<tasks.size();++i){
                if (prd.getStart()<tasks[i].getPeriod().getStart() and tasks[i].getPeriod().getStart()<prd.getEnd())
                    ++cnt;
                else if (tasks[i].getPeriod().getStart()<prd.getStart() and prd.getStart()<tasks[i].getPeriod().getEnd())
                    ++cnt;
            }
            if (ans.second>cnt)
                ans=make_pair(prd,cnt);
            ++prd;
        }
        return ans;
    }
    void writeFile(ostream& output){
        for (int i=0;i<tasks.size();++i)
            output.write(reinterpret_cast<char*> (&tasks[i]),sizeof(Task));
    }
    friend ostream& operator << (ostream& output,Day& day);
private:
    vector <Task> tasks;
};
ostream& operator << (ostream& output,Day& day){ for (int i=0;i<day.tasks.size();++i) output << day.tasks[i] << endl; return output; }

class Month{
public:
    void setIndex(int index){ this->index=index; }
    pair<Date,Period> findPeriod(Time now,Clock len){
        pair<Period,int> ans=day[now.getDate().getDay()].findPeriod(now.getClock(),len);
        Date dt=now.getDate();
        for (int i=now.getDate().getDay()+1;i<=CountDay[index];++i){
            pair<Period,int> ppi=day[i].findPeriod(Clock(0,0),len);
            if (ans.second<ppi.second)
                ans=ppi;
        }
        return make_pair(dt,ans.first);
    }
    void writeFile(ostream& output){
        for (int i=1;i<=CountDay[index];++i)
            day[i].writeFile(output);
    }
    Day& getDay(int ind){ return day[ind]; }
    friend ostream& operator << (ostream& output,Month& mon);
private:
    int index;
    Day day[MaxDay];
};
ostream& operator << (ostream& output,Month& mon){ for (int i=1;i<=CountDay[mon.index];++i) output << mon.day[i]; return output; }

class Year{
public:
    Year(){ for (int i=1;i<=12;++i) mon[i].setIndex(i); }
    void writeFile(ostream& output){
        for (int i=1;i<=CountMonth;++i)
            mon[i].writeFile(output);
    }
    Month& getMonth(int ind){ return mon[ind]; }
    friend ostream& operator << (ostream& output,Year& year);
private:
    Month mon[MaxMonth];
};
ostream& operator << (ostream& output,Year& year){ for (int i=1;i<=CountMonth;++i) output << year.mon[i]; return output; }

class TaskManager{
public:
    Time getNow(){
        time_t t0=time(0);
        tm* now=localtime(&t0);
        return Time(Date(now->tm_mday,(now->tm_mon)+1),Clock(now->tm_hour,now->tm_min));
    }
    Year& getYear(){ return year; }
private:
    Year year;
};

int main(){
    TaskManager tskmng;
    ifstream fin("Tasks.dat",ios::binary);
    // while (fin.good()){
    //     Task tsk;
    //     fin.read(reinterpret_cast<char*>(&tsk),sizeof(Task));
    //     if (tsk.getDate()==Date(0,0))
    //         continue;
    //     tskmng.getYear().getMonth(tsk.getDate().getMonth()).getDay(tsk.getDate().getDay()).addTask(tsk);
    // }
    fin.close();
    string type;
    while (true){
        cin >> type;
        if (type=="now"){
            Time now=tskmng.getNow();
            cout << now << endl;
        }
        else if (type=="today"){
            Time now=tskmng.getNow();
            Day day=tskmng.getYear().getMonth(now.getDate().getMonth()).getDay(now.getDate().getDay());
            cout << day;
        }
        else if (type=="month"){
            Time now=tskmng.getNow();
            Month mon=tskmng.getYear().getMonth(now.getDate().getMonth());
            cout << mon;
        }
        else if (type=="year"){
            Year yr=tskmng.getYear();
            cout << yr;
        }
        else if (type=="free"){
            Time tf;
            cin >> tf;
            tskmng.getYear().getMonth(tf.getDate().getMonth()).getDay(tf.getDate().getDay()).freePrcs(tf.getClock());
            // remove("Tasks.dat");
            // ofstream fout.open("Tasks.dat",ios::binary);
            // tskmng.getYear().writeFile(fout);
            // fout.close();
        }
        else if (type=="schedule"){
            Date ds;
            cin >> ds;
            Day dsch=tskmng.getYear().getMonth(ds.getMonth()).getDay(ds.getDay()).findSchedule();
            cout << dsch;
        }
        else if (type=="find"){
            Clock cf;
            Time now=tskmng.getNow();
            cin >> cf;
            pair<Date,Period> pdp=tskmng.getYear().getMonth(now.getDate().getMonth()).findPeriod(now,cf);
            cout << pdp.first << endl << pdp.second << endl;
        }
        else if (type=="add"){
            string s;
            string title;
            Time strt,end;
            cin >> s >> s;
            cin.ignore(1);
            getline(cin,title);
            cin >> s >> s >> strt;
            cin >> s >> s >> end;
            Task tsk(title,strt.getDate(),Period(strt.getClock(),end.getClock()));
            tskmng.getYear().getMonth(strt.getDate().getMonth()).getDay(strt.getDate().getDay()).addTask(tsk);
            cin >> s;
            // ofstream fout("Tasks.dat",ios::binary|ios::app);
            // fout.write(reinterpret_cast<char*>(&tsk),sizeof(Task));
            // fout.close();
        }
        else if (type=="exit")
            exit(0);
        else
            cout << "INVALID INPUT" << endl;
    }
}