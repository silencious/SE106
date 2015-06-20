// Name: TianJiahe
// ID: 5130379056

#include "Database.h"
#include <cstdlib>
#include <ctime>

const string charset = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
const int CHARSIZE = charset.length();

string randkey(){
    int l = rand()%4+rand()%5+rand()%5+rand()%5+1;  // from 1 to 16
    string ret = "";
    for(int i=0; i<l; i++){
        ret += charset[rand()%CHARSIZE];
    }
    return ret;
}

string randdata(){
    int l = rand()%64+rand()%65+rand()%65+rand()%65+1;  // from 1 to 256
    string ret = "";
    for(int i=0; i<l; i++){
        ret += charset[rand()%CHARSIZE];
    }
    return ret;
}

int main(){
    try{
        srand(time(NULL));
        Database db("db1", 10);
        for(int i=0; i<10; i++){
            string k = randkey();
            db.store(k, randdata(), STORE);
            db.remove(k);
        }
    }
    catch(char const* msg){
        cout << (string)msg << endl;
    }
    return 0;
}
