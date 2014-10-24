/*
 * File: WordLadder.cpp
 * --------------------
 * Name: Tian Jiahe
 * Student ID: 5130379056
 * This file is the starter project for the word ladder problem.
 * 
 * Use WordLadder class to store data and execute find operation.
 * Use a deque to represent ladders, first-in-first-out.
 * Use set to help determine whether a word is in the ladder.
 */


#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include <algorithm>
#include <set>
#include "lexicon.h"
using namespace std;

class WordLadder{
public:
    WordLadder(string, string);
    void find_ladder();

private:
    string new_word(const vector<string>&);

    deque< vector<string> > ladders;
    set<string> word_set;
    Lexicon dict;
    string start;
    string dest;
    int len;
};

WordLadder::WordLadder(string s, string d)
    :dict("EnglishWords.dat"), start(s), dest(d), len(s.length())
{
    word_set.insert(start);
}

//generate a new word and put into set
//if fail, return empty string
string WordLadder::new_word(const vector<string>&v){
    string word = v[v.size()-1];
    for(int i=0; i<len; i++){
        for(char c='a'; c<='z'; c++){
            string tmp = word;
            if(c==word[i])
                continue;
            tmp[i] = c;
            if(word_set.find(tmp) == word_set.end() && 
                dict.contains(tmp)){
                    word_set.insert(tmp);
                    return tmp;
            }
        }
    }
    return "";
}

//find the shortest ladder. traverse all cases. 
void WordLadder::find_ladder(){
    if(len != dest.length())
        return;
    if(!dict.contains(start) || !dict.contains(dest))
        return;
    if(start==dest){
        cout << "Found ladder: " << start << ' ' << dest;
        return;
    }
    ladders.push_back(vector<string>(1,start));
    while(true){
        vector<string> v = ladders[0];
        ladders.pop_front();
        string tmp;
        while(true){
            tmp = new_word(v);
            if(tmp=="")
                break;
            vector<string> vcopy(v);
            vcopy.push_back(tmp);
            if(tmp!=dest)
                ladders.push_back(vcopy);
            else{
                cout << "Found ladder: ";
                int n=vcopy.size();
                for(int i=0; i<n; i++)
                    cout << vcopy[i] << ' ';
                return;
            }
        }
        if(ladders.size()==0)
            return;
    }
}

//the main input and output module
int main() {
    while(true){
        string start, dest;
        cout << "Enter start word (RETURN to quit): ";
        getline(cin, start);        
        if(start == "")
            break;
        cout << "Enter destination word: ";
        getline(cin, dest);
        if(dest == ""){
            cout << endl;
            continue;
        }
        WordLadder ladder(start, dest);
        ladder.find_ladder();
        cout << endl;
    }
    cout << "Goodbye!\n";
    return 0;
}
