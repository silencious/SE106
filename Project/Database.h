// Name: TianJiahe
// ID: 5130379056

#include <string>
#include <iostream>
#include <fstream>

#define MINDEG 3    // minimal degree. note: if MINDEG is less than MINDEG, the program may not work correctly
#define MAXDAT 256
#define BLOCKSIZE (MAXDAT/4)	// align in datafile
#define INTSIZE sizeof(unsigned)
#define LONGSIZE sizeof(unsigned long)

using namespace std;

string randkey();
string randdata();

// struct of Exception
struct MyException : public exception{
    string s;
    MyException(const string& s) :s(s){}
    ~MyException() throw(){}
    string what() throw(){ return s; }
};

struct rec;
struct node;
class Database;

// either an internal node with pointer
// or a leaf with offset and datlen
struct rec{
    string key;
    union{  // either an internal node or a
        node* ptr;                  // points to a node
        unsigned long pos;          // position in datafile
    };
    rec(const string& key = "", node* ptr = NULL)
        :key(key), ptr(ptr){}
    rec(const string& key, unsigned long pos)
        :key(key), pos(pos){}
};
// struct of the node with an array of records
struct node{
    int num;        // number of keys, actual size of recs
    node* first;    // if internal, points to leftmost child, else, NULL, which means that if first is NULL, then ptrs are pointers to leaves
    rec* recs;      // key-node or key-pos
    node(int degree, node* p)
        :num(0), first(p){
            recs = new rec[degree - 1];
        }
    void clear();	// clear all children
};

enum FLAG{ INSERT, REPLACE, STORE };	// mode of store

class BplusTree{
    public:
        explicit BplusTree(const string& name, int degree);
        ~BplusTree();
        bool isEmpty() const{ return count == 0; }
        void insert(const rec& r);		// insert a record to the tree
        rec* fetch(const string& key);	// fetch a record from tree
        void remove(const string& key);	// remove a record from tree
        void clear();					// clear all nodes
        string randkey();				// randomly pick a key
        friend class Database;

    private:
        string name;	// file name
        int degree;     // < degree records per node
        node* root;     // root of B+ Tree
        bool dirty;     // if the index has been changed
        int count;		// number of records

        // some functions to write to & read from file
        string readkey(ifstream&);
        void writekey(ofstream&, const string&);
        void readnode(ifstream&, node*);
        void writenode(ofstream&, node*);
        void load();
        void save();

        bool split(node* p, const string& key);		// split when children is full. p is parent of the node
        bool borrow(node* p, const string& key);	// borrow from sibling when children is less than half the degree
        void merge(node* p, const string& key);		// merge siblings when cannot borrow
};

class Database{
    public:
        explicit Database(const string& name, int degree);
        ~Database();
        void open();		// open the datafile
        void close();		// close the datafile
        FLAG store(const string& key, const string& data, FLAG fl);		// store a piece of data
        string fetch(const string& key);		// fetch a piece of data
        void remove(const string& key);			// remove a piece of data
        void delfile();			// delete the datafile
        bool randremove();		// randomly remove a piece of data
        void randfetch();		// randomly fetch a piece of data
        bool randreplace();		// randomly replace a piece of data
        void rebuild();			// if the database is half empty, rebuild it to a new file

    private:
        string name;			// name of database
        BplusTree index;		// keep index in memory
        fstream datafile;		// read & write datafile
        int count;				// number of data that has been stored in the datafile
        void copyto(ofstream& newfile, node* p);
};
