// Name: TianJiahe
// ID: 5130379056

#include "Database.h"
#include <vector>
#include <cstdlib>

// keys and data are generated from the charset
const string charset = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
const int CHARSIZE = charset.length();
// randomly generate a key
string randkey(){
    int l = rand() % 4 + rand() % 5 + rand() % 5 + rand() % 5 + 1;  // from 1 to 16
    string ret = "";
    for (int i = 0; i < l; i++){
        ret += charset[rand() % CHARSIZE];
    }
    return ret;
}
// randomly generate a piece of data
string randdata(){
    int l = rand() % 64 + rand() % 65 + rand() % 65 + rand() % 65 + 1;  // from 1 to 256
    string ret = "";
    for (int i = 0; i < l; i++){
        ret += charset[rand() % CHARSIZE];
    }
    return ret;
}
// some functions to read and write integer as binary:
// from integer to binary
string itob(unsigned a){
    string ret = "";
    for (unsigned i = 0; i < INTSIZE; i++){
        ret += (char)((a >> 8 * i) & 0xFF);
    }
    return ret;
}
// from binary to integer
unsigned btoi(const string& s){
    int a = 0;
    for (unsigned i = 0; i < INTSIZE; i++){
        a += (((unsigned)s[i] & 0xFF) << 8 * i);
    }
    return a;
}
// from long int to binary
string ltob(unsigned long a){
    string ret = "";
    for (unsigned i = 0; i < LONGSIZE; i++){
        ret += (char)((a >> 8 * i) & 0xFF);
    }
    return ret;
}
// from binary to long int
unsigned long btol(const string& s){
    unsigned long a = 0;
    for (unsigned i = 0; i < LONGSIZE; i++){
        a += (((unsigned long)s[i] & 0xFF) << 8 * i);
    }
    return a;
}
// read n bytes at a time
string readn(istream& is, int n){
    string ret = "";
    for (int i = 0; i < n; i++)
        ret += is.get();
    return ret;
}

// free dynamically allocated space
void node::clear(){
    if (first != NULL){
        first->clear();
        delete first;
        for (int i = 0; i < num; i++){
            node* ptr = recs[i].ptr;
            if (ptr != NULL){
                ptr->clear();
                delete ptr;
            }
        }
    }
    if (recs != NULL)
        delete[] recs;
}

BplusTree::BplusTree(const string& name, int degree) :name(name), degree(degree), dirty(false), count(0)
{
    if (degree < MINDEG)
        throw MyException("Error with BplusTree::BplusTree(): Degree less than minimum degree");
    root = new node(degree, NULL);
    load();
}

BplusTree::~BplusTree(){
    save();
    clear();
}
// insert a record to the tree
void BplusTree::insert(const rec& r){
    dirty = true;	// the tree has been modified
    count++;		// increase the count
    if (root == NULL)
        root = new node(degree, NULL);
    node* curr = root;
    vector<node*> path;	// store the path from root to the record
    int n, i;
    rec* recs;
    while (curr->first != NULL){
        recs = curr->recs;
        n = curr->num;
        for (i = 0; i < n; i++)
            if (recs[i].key > r.key)
                break;
        path.push_back(curr);
        curr = (i == 0) ? curr->first : recs[i - 1].ptr;
    }
    recs = curr->recs;
    n = curr->num;
    for (i = 0; i < n; i++)
        if (recs[i].key >= r.key)
            break;
    if (i == n){     // reach end
        recs[n] = r;
        curr->num++;
    }
    else if (recs[i].key == r.key){      // find existing key
        throw MyException("Error with BplusTree::insert(): Insert existed data");
    }
    else{       // not found
        for (int j = n; j > i; j--)
            recs[j] = recs[j - 1];
        recs[i] = r;	// insert
        curr->num++;
    }
    if (curr->num == degree - 1){    // split when the array is full
        int j;
        for (j = path.size() - 1; j >= 0; j--){
            if (!split(path[j], r.key))	// if there's no more need to split
                return;
        }
        if (j < 0 && root->num == degree - 1){	// split root
            node* src = root;
            int n = root->num;
            node* dst = new node(degree, (src->first == NULL) ? NULL : src->recs[n / 2].ptr);
            root = new node(degree, src);
            root->recs[0].key = src->recs[n / 2].key;
            root->recs[0].ptr = dst;
            root->num++;
            int s = (src->first == NULL) ? n / 2 : n / 2 + 1;
            for (int k = s; k < n; k++)
                dst->recs[k - s] = src->recs[k];
            src->num = n / 2;
            dst->num = n - s;
        }
    }
}
// fetch a record from tree
rec* BplusTree::fetch(const string& key){
    node* curr = root;
    if (curr == NULL)	// no records
        return NULL;
    int n, i;
    while (curr->first != NULL){
        rec* recs = curr->recs;
        n = curr->num;
        for (i = 0; i < n; i++)
            if (recs[i].key > key)
                break;
        curr = (i == 0) ? curr->first : recs[i - 1].ptr;
    }
    rec* recs = curr->recs;
    n = curr->num;
    for (i = 0; i < n; i++)
        if (recs[i].key == key)
            return &recs[i];	// return the address
    return NULL;	// not found
}
// remove a record from tree
void BplusTree::remove(const string& key){
    dirty = true;	// the tree has been modified
    count--;		// decrease the count
    node* curr = root;
    rec* rp = NULL;
    if (curr == NULL)
        throw MyException("Error with BplusTree::remove(): Empty tree");
    vector<node*> path;
    int n, i, j;
    while (curr->first != NULL){
        rec* recs = curr->recs;
        n = curr->num;
        for (i = 0; i < n; i++){
            if (recs[i].key == key)
                rp = &recs[i];
            if (recs[i].key > key)
                break;
        }
        path.push_back(curr);
        curr = (i == 0) ? curr->first : recs[i - 1].ptr;
    }
    rec* recs = curr->recs;
    n = curr->num;
    for (i = 0; i < n; i++)
        if (recs[i].key == key)
            break;
    if (i == n)
        throw MyException("Error with BplusTree::remove(): Nothing to be removed");
    if (i == 0 && rp != NULL)
        rp->key = recs[1].key;
    for (j = i; j < n - 1; j++)        // overwrite with the next
        recs[j] = recs[j + 1];
    curr->num--;
    if (curr->num >= (degree - 1) / 2)	// no need to borrow or merge
        return;
    for (j = path.size() - 1; j >= 0; j--){
        if (!borrow(path[j], recs[0].key)){	// fail to borrow. note: path[j] is the parent of current node
            merge(path[j], recs[0].key);	// merge with sibling
        }
        if (path[j]->num >= (degree - 1) / 2)	// no more need to borrow or merge
            return;
    }
    if (root->num == 0 && root->first != NULL){	// reach root and the root is empty
        node* temp = root;
        root = root->first;		// lower the level of tree
        delete temp;
    }
}
// functions to read and write indexfile
// use recursion to simplify the procedure
string BplusTree::readkey(ifstream& idxfile){
    string ret = "";
    char ch;
    while ((ch = idxfile.get()) != '\0')
        ret += ch;
    return ret;
}

void BplusTree::writekey(ofstream& idxfile, const string& key){
    idxfile.write(key.c_str(), key.length());
    idxfile.put('\0');
}

void BplusTree::readnode(ifstream& idxfile, node* p){
    int n = btoi(readn(idxfile, INTSIZE));
    if (n == 0)
        return;
    rec* recs = p->recs;
    if (n > 0){
        p->num = n;
        p->first = new node(degree, NULL);
        readnode(idxfile, p->first);
        for (int i = 0; i < n; i++){
            recs[i].key = readkey(idxfile);
            recs[i].ptr = new node(degree, NULL);
            readnode(idxfile, recs[i].ptr);
        }
    }
    else{
        n = -n;
        p->num = n;
        count += n;
        for (int i = 0; i < n; i++){
            recs[i].key = readkey(idxfile);
            recs[i].pos = btol(readn(idxfile, LONGSIZE));
        }
    }
}

void BplusTree::writenode(ofstream& idxfile, node* p){
    int n = p->num;
    rec* recs = p->recs;
    if (p->first != NULL){
        idxfile.write(itob(n).c_str(), INTSIZE);
        writenode(idxfile, p->first);
        for (int i = 0; i < n; i++){
            writekey(idxfile, recs[i].key);
            writenode(idxfile, recs[i].ptr);
        }
    }
    else{
        idxfile.write(itob(-n).c_str(), INTSIZE);
        for (int i = 0; i < n; i++){
            writekey(idxfile, recs[i].key);
            idxfile.write(ltob(recs[i].pos).c_str(), LONGSIZE);
        }
    }
}

void BplusTree::load(){
    ifstream idxfile((name + ".idx").c_str(), fstream::binary);
    if (idxfile){
        readnode(idxfile, root);
    }
}

void BplusTree::save(){
    if (!dirty)
        return;
    ofstream idxfile((name + ".idx").c_str(), fstream::trunc | fstream::binary);
    writenode(idxfile, root);
}
// split a node
bool BplusTree::split(node* p, const string& key){
    rec* recs = p->recs;
    int n = p->num;
    int i;
    for (i = 0; i < n; i++)
        if (recs[i].key > key)
            break;
    for (int j = n; j > i; j--)
        recs[j] = recs[j - 1];
    p->num++;	// number of children increased

    node* src = (i == 0) ? p->first : recs[i - 1].ptr;
    n = src->num;
    if (n != degree - 1)
        throw MyException("Error with BplusTree::split()");
    recs[i].key = src->recs[n / 2].key;
    node* dst = new node(degree, (src->first == NULL) ? NULL : src->recs[n / 2].ptr);
    recs[i].ptr = dst;
    int s = (src->first == NULL) ? n / 2 : n / 2 + 1;	// handle internal and leaf nodes differently
    for (int j = s; j < n; j++)
        dst->recs[j - s] = src->recs[j];
    src->num = n / 2;
    dst->num = n - s;
    return (p->num == degree - 1);  // if there should be more split
}
// borrow from sibling
bool BplusTree::borrow(node* p, const string& key){
    rec* recs = p->recs;
    int n = p->num;
    int i;
    for (i = 0; i < n; i++)
        if (recs[i].key > key)
            break;
    // decide where to borrow from & to
    node* src, *dst;
    if (i == 0){
        dst = p->first;
        src = recs[0].ptr;
    }
    else if (i == n){
        dst = recs[n - 1].ptr;
        src = (i > 1) ? recs[n - 2].ptr : p->first;
        i--;        // now i stands for where the key needs to be updated;
    }
    else{
        dst = recs[i - 1].ptr;
        src = recs[i].ptr;
        if (src->num < (degree - 1) / 2){
            src = (i>1) ? recs[i - 2].ptr : p->first;
            i--;
        }
    }
    if (src->num < (degree - 1) / 2)   // failed to borrow
        return false;

    int j;
    if (src->first == NULL){     // borrow between leaves
        if (dst->recs[0].key < src->recs[0].key){
            dst->recs[dst->num] = src->recs[0];
            for (j = 0; j < src->num - 1; j++)
                src->recs[j] = src->recs[j + 1];
        }
        else{
            for (j = dst->num; j > 0; j--)
                dst->recs[j] = dst->recs[j - 1];
            dst->recs[0] = src->recs[src->num - 1];
        }
    }
    else{       // borrow between internal nodes
        node* curr;
        if (dst->recs[0].key < src->recs[0].key){
            curr = src->first;
            while (curr->first != NULL)
                curr = curr->first;
            dst->recs[dst->num].key = curr->recs[0].key;
            dst->recs[dst->num].ptr = src->first;
            src->first = src->recs[0].ptr;
            for (j = 0; j < src->num - 1; j++)
                src->recs[j] = src->recs[j + 1];
        }
        else{
            for (j = dst->num; j > 0; j--)
                dst->recs[j] = dst->recs[j - 1];
            curr = dst->first;
            while (curr->first != NULL)
                curr = curr->first;
            dst->recs[0].key = curr->recs[0].key;
            dst->recs[0].ptr = dst->first;
            dst->first = src->recs[src->num - 1].ptr;
        }
    }
    dst->num++;		// adjust the number of children
    src->num--;
    // update key of the record
    node* curr = recs[i].ptr;
    while (curr->first != NULL)
        curr = curr->first;
    recs[i].key = curr->recs[0].key;

    return true;
}
// merge with sibling
void BplusTree::merge(node* p, const string& key){
    rec* recs = p->recs;
    int n = p->num;
    int i, j;
    for (i = 0; i < n; i++)
        if (recs[i].key > key)
            break;
    node* src, *dst;
    if (i == 0){			// the first children
        dst = p->first;
        src = recs[0].ptr;
    }
    else if (i == n){	// the last children
        dst = (i > 1) ? recs[n - 2].ptr : p->first;
        src = recs[n - 1].ptr;
    }
    else{				// merge with next child
        dst = recs[i - 1].ptr;
        src = recs[i].ptr;
    }
    if (src->first != NULL){	// merge internal nodes
        dst->recs[dst->num].key = (i == n) ? recs[n - 1].key : recs[i].key;
        dst->recs[dst->num].ptr = src->first;
        dst->num++;
    }
    for (j = 0; j < src->num; j++)
        dst->recs[j + dst->num] = src->recs[j];
    dst->num += src->num;
    if (dst->num == degree - 1)
        throw MyException("Error with BplusTree::merge()");
    delete src;
    for (j = i; j < n - 1; j++)
        recs[j] = recs[j + 1];
    p->num--;
}
// recursively clear all nodes
void BplusTree::clear(){
    if (root != NULL){
        root->clear();
        delete root;
        root = NULL;
        dirty = true;
    }
}
// randomly pick a key
string BplusTree::randkey(){
    node* curr = root;
    if (curr == NULL)
        return "";
    while (curr->first != NULL){
        int i = rand() % (curr->num + 1);
        curr = (i == 0) ? curr->first : curr->recs[i - 1].ptr;
    }
    if (curr->num == 0)
        return "";
    int i = rand() % curr->num;
    return curr->recs[i].key;
}

// member functions of Database
Database::Database(const string& name, int degree) :name(name), index(name, degree), count(index.count)
{
    open();
}
Database::~Database(){
    datafile.close();
    if (index.isEmpty()){
        datafile.open((name + ".dat").c_str(), fstream::trunc);
        datafile.close();
    }
}
// open the database file
void Database::open(){
    datafile.open((name + ".dat").c_str(), fstream::in | fstream::out | fstream::binary | fstream::ate);
    if (!datafile){	// not existed
        datafile.open((name + ".dat").c_str(), fstream::app);	// creat the file
        datafile.close();
        datafile.open((name + ".dat").c_str(), fstream::in | fstream::out | fstream::binary | fstream::ate);
    }
}
// close the database file
void Database::close(){
    datafile.close();
}
// store a piece of data
FLAG Database::store(const string& key, const string& data, FLAG fl){
    rec* rp = index.fetch(key);
    unsigned long pos;
    unsigned datlen = data.length();
    unsigned size = (datlen + BLOCKSIZE - 1) / BLOCKSIZE*BLOCKSIZE;
    string chunk = data;
    for (unsigned i = datlen; i < size; i++)	// align to BLOCKSIZE
        chunk += ' ';

    if (rp == NULL){		// not exist
        if (fl == INSERT || fl == STORE){	// insert to database
            datafile.seekp(0, fstream::end);
            pos = datafile.tellp();
            index.insert(rec(key, pos));
            datafile.write(itob(size).c_str(), INTSIZE);
            datafile.write(itob(datlen).c_str(), INTSIZE);
            datafile.write(chunk.c_str(), size);
            if (index.count > count)		// modify the max count of data
                count = index.count;
            return INSERT;
        }
        else
            throw MyException("Error with BplusTree::replace(): Nothing to be replaced");
    }
    else{		// exist
        if (fl == REPLACE || fl == STORE){
            datafile.seekg(rp->pos);
            unsigned capacity = btoi(readn(datafile, INTSIZE));
            if (capacity >= size){
                datafile.seekp(rp->pos + INTSIZE);
            }
            else{
                datafile.seekp(0, fstream::end);
                rp->pos = datafile.tellp();
                datafile.write(itob(size).c_str(), INTSIZE);
            }
            datafile.write(itob(datlen).c_str(), INTSIZE);
            datafile.write(chunk.c_str(), size);
            return REPLACE;
        }
        else
            throw MyException("Error with Database::store(): Insert existed data");
    }
}
// fetch and return a piece of data
string Database::fetch(const string& key){
    rec* rp = index.fetch(key);
    if (rp == NULL)
        return "";
    datafile.seekg(rp->pos + INTSIZE);
    unsigned int datlen = btoi(readn(datafile, INTSIZE));
    return readn(datafile, datlen);
}
// remove a piece of data
void Database::remove(const string& key){
    index.remove(key);
    if (index.count > 100000 && index.count < count / 2){	// if the datafile is half empty, rebuild it
        count = index.count;
        rebuild();
    }
    return;
}
// delete the datafile
void Database::delfile(){
    index.clear();
    if (system(("rm " + name + ".dat ").c_str()));
}
// randomly remove a piece of data
bool Database::randremove(){
    string key = index.randkey();
    if (key == "")
        return false;
    remove(key);
    if (index.fetch(key) == NULL)
        return true;
    throw MyException("Error with Database::randremove().");
}
// randomly fetch a piece of data
void Database::randfetch(){
    string key = index.randkey();
    if (key == "")
        return;
    string data = fetch(key);
}
// randomly replace a piece of data
bool Database::randreplace(){
    string key = index.randkey();
    if (key == "")
        return true;
    string data = randdata();
    store(key, data, REPLACE);
    return data == fetch(key);
}
// recursively copy a node to a newfile, used to rebuild database
void Database::copyto(ofstream& newfile, node* p){
    int n = p->num;
    unsigned long pos;
    if (p->first == NULL){
        for (int i = 0; i < n; i++){
            datafile.seekg(p->recs[i].pos);
            unsigned size = btoi(readn(datafile, INTSIZE));
            datafile.seekg(-INTSIZE, fstream::cur);
            pos = newfile.tellp();
            p->recs[i].pos = pos;
            newfile.write(readn(datafile, size + 2 * INTSIZE).c_str(), size + 2 * INTSIZE);
        }
    }
    else{
        copyto(newfile, p->first);
        for (int i = 0; i < n; i++)
            copyto(newfile, p->recs[i].ptr);
    }
}
// rebuild database
void Database::rebuild(){
    cout << "Rebuilding Database.\n";
    ofstream newfile;
    newfile.open((name + ".bak").c_str(), fstream::binary | fstream::trunc);
    if (index.root != NULL){
        copyto(newfile, index.root);
        datafile.close();
        if (system(("mv " + name + ".bak " + name + ".dat").c_str()));
        datafile.open((name + ".dat").c_str(), fstream::in | fstream::out | fstream::binary | fstream::ate);
    }
    else{
        datafile.close();
        datafile.open((name + ".dat").c_str(), fstream::in | fstream::out | fstream::binary | fstream::trunc);
    }
}
