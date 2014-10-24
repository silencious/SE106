/*
 * File: RandomWriter.cpp
 * ----------------------
 * Name: Tian Jiahe
 * Student ID: 5130379056
 * use map<string, multiset<char> > to store data
 * so that next char is chosen by its frequence.
 */

#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include <set>
#include <queue>
#include <cstdlib>
#include <ctime>
using namespace std;

//input file name and order
void input(ifstream& fin, int& order){
    while(true){
        string file;
        cout << "Enter the source text: ";
        cin >> file;
        fin.open(file.c_str());
        if(!fin){
            cout << "Unable to open that file. Try again.\n";
            continue;
        }
        while(true){
            cout << "Enter the Markov order [0-10]: ";
            cin >> order;
            if(order < 0 || order > 10){
                cout << "The value is out of range.\n";
                continue;
            }
            break;
        }
        break;
    }
}
//converge from deque to string
string qtos(const deque<char>& q){
    string s;
    for(int i=0; i<q.size(); i++){
        s += q[i];
    }
    return s;
}
//read from file
void read(ifstream& fin, int order, map<string, multiset<char> >& data){
    char ch;
    deque<char> q;
    while(fin.get(ch)){
        if(q.size() < order){
            q.push_back(ch);
            continue;
        }
        string s = qtos(q);
        if(data.find(s) != data.end()){
            data[s].insert(ch);
        }
        else{
            data[s].insert(ch);
        }
        q.pop_front();
        q.push_back(ch);
    }
}
//find most frequent word as first seed
string mostFreq(const map<string, multiset<char> >& data){
    string tmp;
    int num=0;
    for(map<string, multiset<char> >::const_iterator it=data.begin(); it!=data.end(); it++){
        if(num < (it->second.size())){
            tmp = it->first;
            num = it->second.size();
        }
    }
    return tmp;
}
//get next char
char nextChar(const map<string, multiset<char> >& data, string seed){
    map<string, multiset<char> >::const_iterator map_it=data.find(seed);
    if(map_it==data.end())
        return '\0';
    srand(clock());             //use milisec as seed
    int r = rand() % map_it->second.size();
    multiset<char>::const_iterator set_it=map_it->second.begin();
    for(int i=0; i<r; i++)      //randomly select next char
        set_it++;
    return *set_it;
}
//get next seed
string nextSeed(string seed, char ch){  
    return seed.substr(1) + ch;
}

int main(){
    ifstream fin;
    int order;
    input(fin, order);
    map<string, multiset<char> > data;
    read(fin, order, data);

    string seed = mostFreq(data);
    cout << seed;
    int count = order;
    while(count <= 2000){
        char ch = nextChar(data, seed);
        if(ch == '\0')
            break;
        cout << ch;
        count++;
        seed = nextSeed(seed, ch);
    }
    cout << endl;
    return 0;
}
