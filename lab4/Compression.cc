//Name: TianJiahe
//ID: 5130379056

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <vector>
#include <map>
using namespace std;

const static int MAXLEN = 8*sizeof(int);
const static int INTSIZE = sizeof(int);

struct node{
    char ch;
    bool real;      //if node is really a char or an empty node
    int freq;
    node* lp;
    node* rp;
    node(char c = EOF, bool b = false, int f = 0, node* left = NULL, node* right = NULL)
        :ch(c), real(b), freq(f), lp(left), rp(right){}
    void clear(){
        if(lp){lp->clear(); delete lp;}
        if(rp){rp->clear(); delete rp;}
    }
};

//print error message
void error(const string& msg){
    cerr << msg << endl;
    exit(1);
}

//from integer to binary
string itob(unsigned int a){
    string ret = "";
    for(int i=0; i<INTSIZE; i++){
        ret += (char)((a >> 8*i) & 0xFF);
    }
    return ret;
}

//from binary to integer
unsigned int btoi(const string& s){
    unsigned int a = 0;
    for(int i=0; i<INTSIZE; i++){
        a += (((unsigned int)s[i] & 0xFF) << 8*i);
    }
    return a;
}

//string(of '0' and '1') to binary
string stob(const string& s){
    int a = 0;
    for(int i=0; i<MAXLEN; i++){
        a += ((int)(s[i]-'0') << i);
    }
    return itob(a);
}

//binary to string(of '0' and '1')
string btos(const string& s){
    string ret = "";
    int a = btoi(s);
    for(int i=0; i<MAXLEN; i++){
        ret += (((a>>i) & 0x1) + '0');
    }
    return ret;
}

//read n bytes at a time
string readn(istream& is, int n){
    string str = "";
    char ch;
    for(int i=0; i<n; i++){
        if(!is.get(ch))
            break;
        str+=ch;
    }
    return str;
}

//write n bytes at a time
void writen(ostream& os, const string& s, int n){
    for(int i=0; i<n; i++){
        if(!os.put(s[i]))
            return;
    }
}

//sift up from i, maintain the heap
void siftup(vector<node*>& heap, int i){
    while(i != 0){
        int up = (i-1)/2;
        if(heap[i]->freq < heap[up]->freq){
            swap(heap[up], heap[i]);
            i = up;
        }
        else{
            break;
        }
    }
}

//sift down from i, maintain the heap
void siftdown(vector<node*>& heap, int i){
    int n = heap.size();
    while(i < n){
        int down = 2*i+1;
        if(down >= n) break;
        if(down == n-1){
            if(heap[i]->freq > heap[down]->freq)
                swap(heap[i], heap[down]);
            break;
        }
        if((heap[i]->freq < heap[down]->freq) && (heap[i]->freq < heap[down+1]->freq))
            break;
        if(heap[down]->freq < heap[down+1]->freq){
            swap(heap[i], heap[down]);
            i = down;
        }
        else{
            swap(heap[i], heap[down+1]);
            i = down+1;
        }
    }
}

//delete min from a heap and return it
node* deletemin(vector<node*>& heap){
    int n = heap.size();
    swap(heap[0], heap[n-1]);
    node* ret = heap[n-1];
    heap.pop_back();
    siftdown(heap, 0);
    return ret;
}

//insert an element to the heap, maintain the heap
void insert(vector<node*>& heap, node* p){
    heap.push_back(p);
    int n = heap.size();
    siftup(heap, n-1);
}

class HuffmanTree{
    public:
        enum MODE{COMPRESS, DECOMPRESS};
        explicit HuffmanTree(vector<node*>&, int, MODE);
        ~HuffmanTree();
        void encode(ifstream&, ofstream&);
        void decode(ifstream&, ofstream&);

    private:
        node* root;
        unsigned int total;
        map<char, string> code;
        map<string, char> icode;    //inverse code;

        void reachdown(node*, const string&, MODE);
};

//compress mode constructor
HuffmanTree::HuffmanTree(vector<node*>& heap, int t, MODE m) :root(new node()), total(t)
{
    int n = heap.size();
    if(n == 0) return;      //nothing in this file
    //start building the tree
    if(n == 1){
        root->rp = deletemin(heap);
        if(m == COMPRESS)
            code.insert(pair<char, string>(root->rp->ch, "1"));
        else
            icode.insert(pair<string, char>("1", root->rp->ch));
        return;
    }
    for(int i=0; i<n-1; i++){
        node* l = deletemin(heap);
        node* r = deletemin(heap);
        root = new node(EOF, false, l->freq + r->freq, l, r);
        insert(heap, root);
    }
    reachdown(root, "", m);
}

HuffmanTree::~HuffmanTree(){
    int n = code.size();
    if(n == 0 || root == NULL) return;
    root->clear();
    delete root;
}

void HuffmanTree::reachdown(node* p, const string& s, MODE m){
    if(p->real){
        if(m == COMPRESS)
            code.insert(pair<char, string>(p->ch, s));
        else
            icode.insert(pair<string, char>(s, p->ch));
        return;
    }
    if(p->lp) reachdown(p->lp, s+'0', m);
    if(p->rp) reachdown(p->rp, s+'1', m);
}



void HuffmanTree::encode(ifstream& ifs, ofstream& ofs){
    if(total == 0) return;
    string str, buf = "";
    char ch;
    int len = 0;
    while(ifs.get(ch)){
        str = code[ch];
        len += str.length();
        buf += str;
        while(len >= MAXLEN){
            str = buf.substr(0, MAXLEN);
            buf.erase(0, MAXLEN);
            writen(ofs, stob(str), INTSIZE);
            len -= MAXLEN;
        }
    }
    for(int i=0; i<MAXLEN-len; i++) buf += '1';
    writen(ofs, stob(buf), INTSIZE);
}

void HuffmanTree::decode(ifstream& ifs, ofstream& ofs){
    if(total == 0) return;
    int i = 0, len = 0;
    unsigned int cnt = 0;
    string str = "";
    while(cnt < total){
        str += btos(readn(ifs, INTSIZE));
        len += MAXLEN;
        while(i < len){
            string sub = str.substr(0, i+1);
            if(icode.find(sub) != icode.end()){
                ofs.put(icode[sub]);
                str.erase(0, i+1);
                len -= (i+1);
                i = 0;
                if(++cnt == total) break;
            }
            else
                i++;
        }
    }
}

//count the frequency of char
void count(ifstream& ifs, vector<node*>& heap, int& total){
    map<char, int> charset;
    char ch;
    int n = 0;
    map<char, int>::iterator it;
    while(ifs.get(ch)){
        total++;
        it = charset.find(ch);
        if(it == charset.end()){
            charset.insert(pair<char, int>(ch, 1));
            n++;
        }
        else{
            it->second++;
        }
    }
    heap.reserve(n);
    it = charset.begin();
    for(int i=0; i<n; i++, it++)
        insert(heap, new node(it->first, true, it->second));
}

void compress(const string& inputFilename, const string& outputFilename){
    ifstream ifs(inputFilename.c_str(), ios::in | ios::binary);
    ofstream ofs(outputFilename.c_str(), ios::out | ios::binary);
    if(!ifs || !ofs) error("Cannot find input or output file, try again.");
    vector<node*> heap;
    int total = 0;
    count(ifs, heap, total);
    int n = heap.size();
    //header
    writen(ofs, itob(total), INTSIZE);
    writen(ofs, itob(n), INTSIZE);
    for(int i=0; i<n; i++){
        node* tmp = heap[i];
        ofs.put(tmp->ch);
        writen(ofs, itob(tmp->freq), INTSIZE);
    }

    HuffmanTree huff(heap, total, HuffmanTree::COMPRESS);

    ifs.clear();
    ifs.sync();
    ifs.seekg(0, ios::beg);

    huff.encode(ifs, ofs);
    ofs.close();
    ifs.close();
};

void decompress(const string& inputFilename, const string& outputFilename){
    ifstream ifs(inputFilename.c_str(), ios::in | ios::binary);
    ofstream ofs(outputFilename.c_str(), ios::out | ios::binary);
    if(!ifs || !ofs) error("Cannot find input or output file, try again.");
    //header
    int total = btoi(readn(ifs, INTSIZE));
    int n = btoi(readn(ifs, INTSIZE));
    vector<node*> heap(n);
    for(int i=0; i<n; i++)
        heap[i] = new node(ifs.get(), true, btoi(readn(ifs, INTSIZE)));
    HuffmanTree huff(heap, total, HuffmanTree::DECOMPRESS);

    huff.decode(ifs, ofs);
    ofs.close();
    ifs.close();
}

void usage(string prog){
    cerr << "Usage: " << endl
        << "    " << prog << "[-d] input_file output_file" << endl;
    exit(2);
}

int main(int argc, char* argv[]){
    int i;
    string inputFilename, outputFilename;
    bool isDecompress = false;
    for(i = 1; i < argc; i++){
        if(argv[i] == string("-d"))isDecompress = true;
        else if(inputFilename == "")inputFilename = argv[i];
        else if(outputFilename == "")outputFilename = argv[i];
        else usage(argv[0]);
    }
    if(outputFilename == "")usage(argv[0]);
    if(inputFilename == outputFilename){
        if(system(("cp " + inputFilename + " " + inputFilename + ".tmp").c_str()) != 0) error("Unknown error1");
        if(isDecompress)decompress(inputFilename + ".tmp", outputFilename);
        else compress(inputFilename + ".tmp", outputFilename);
        if(system(("rm " + inputFilename + ".tmp").c_str()) != 0) error("Unknown error2");
    }
    else{
        if(isDecompress)decompress(inputFilename, outputFilename);
        else compress(inputFilename, outputFilename);
    }
    return 0;
}
