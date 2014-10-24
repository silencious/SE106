/**
* Homework 2: Using Template
* Assigned: September 13
* Due: September 23 24:00
**/

//
// Student ID: 5130379056
// Name: Tian Jiahe
//

#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <string>
#include <stdexcept>

#include <iostream>

using namespace std;

template <typename T>
class Vector {
private:
	// Add private variables and methods here
	size_t _size;
	T* _storage;

public:
	// Do NOT modify following interfaces
	Vector();
	Vector(int size, const T& val = T());
	Vector(const Vector& x);
	~Vector();
	Vector<T>& operator=(const Vector<T>& x);
	int size() const;
	bool empty() const;
	T& operator [](int pos);
	void resize(size_t n, T val = T());
	void push_back(const T& val);
	void sort();
};

// (default constructor)
// Constructs an empty container, with no elements.
template <typename T>
Vector<T>::Vector() {
    _size = 0;
	_storage = NULL;
}

// (fill constructor)
// Constructs a container with n elements.
// Each element is a copy of val.
template <typename T>
Vector<T>::Vector(int size, const T& val) {
    _size = size;
    _storage = new T[size];
    for(int i=0; i<size; i++)
        _storage[i] = val;
}

// (copy constructor)
// Constructs a container with a copy of each of the
// elements in x, in the same order.
template <typename T>
Vector<T>::Vector(const Vector& x) {
	_size = x.size();
    _storage = new T[_size];
    for(int i=0; i<_size; i++)
        _storage[i] = x._storage[i];
}

// (destructor)
// Destroys the container object.
template <typename T>
Vector<T>::~Vector() {
	delete[] _storage;
}

// operator=
// Assigns new contents to the container, replacing its
// current contents, and modifying its size accordingly.
template <typename T>
Vector<T>& Vector<T>::operator=(const Vector<T>& x) {
	delete[] _storage;
    _size = x.size();
    _storage = new T[_size];
    for(int i=0; i<_size; i++)
        _storage[i] = x._storage[i];
    return *this;
}

// size
// Returns the number of elements in the vector.
// This is the number of actual objects held in the
// vector, which is not necessarily equal to its
// storage capacity.
template <typename T>
int Vector<T>::size() const {
	return _size;
}

// empty
// Returns whether the vector is empty
template <typename T>
bool Vector<T>::empty() const {
	return _size==0;
}

// operator[]
// Returns a reference to the element at position n
// in the vector container.
// if the requested position is out of range, the
// behavior is undefined
template <typename T>
T& Vector<T>::operator[](int pos) {
	if(pos>=_size || _storage==NULL)
        throw out_of_range("");
    return _storage[pos];
}

// resize
// Resizes the container so that it contains n elements.
// If n is smaller than the current container size, the 
// content is reduced to its first n elements, removing 
// those beyond (and destroying them).
// If n is greater than the current container size, the 
// content is expanded by inserting at the end as many 
// elements as needed to reach a size of n. If val is 
// specified, the new elements are initialized as copies 
// of val, otherwise, they are value-initialized.
template <typename T>
void Vector<T>::resize(size_t n, T val) {
    T* dst = new T[n];
    if(n<_size){
        for(int i=0; i<n; i++)
            dst[i] = _storage[i];
    }
    else{
        for(int i=0; i<_size; i++)
            dst[i] = _storage[i];
        for(int i=_size; i<n; i++)
            dst[i] = val;
    }
    delete[] _storage;
    _size = n;
    _storage = dst;
}

// push_back
// Adds a new element at the end of the vector, after its
// current last element. The content of val is copied (or
// moved) to the new element.
template <typename T>
void Vector<T>::push_back(const T& val) {
	resize(_size+1, val);
}

// a general sort
// Sort the vector, output the result in std out
// with monotonically increasing order in one line
// for instance: 
//     your original vector is 3 2 4 5
//     your output after sorting is 2 3 4 5
// you shouldn't change the order of oringinal vector
template <typename T>
void Vector<T>::sort() {
	T* cpy = new T[_size];
    for(int i=0; i<_size; i++)
        cpy[i] = _storage[i];
    for(int i=0; i<_size; i++){
        T tmp = cpy[i];
        for(int j=i; j<_size; j++){
            if(tmp>cpy[j]){
                tmp = cpy[j];
                cpy[j] = cpy[i];
                cpy[i] = tmp;
            }
        }
        cout << tmp << ' ';
    }
    cout<<endl;
    delete[] cpy;
}

// specialization of string
// Sort the string vector in lexicographical increasing order 
// from tail to head!
// for instance:
//      your original vector is "abc" "cba" "bcaa"
//      your output after sorting is "bcaa" "cba" "abc"
// you shouldn't change the order of oringinal vector
template <>
void Vector<string>::sort() {
	string* cpy = new string[_size];
    for(int i=0; i<_size; i++)
        cpy[i] = _storage[i];
    for(int i=0; i<_size; i++){
        string tmp1 = cpy[i];
        for(int j=i; j<_size; j++){
            int l1 = tmp1.length()-1;
            string tmp2 = cpy[j];
            int l2 = tmp2.length()-1;
            while(tmp1[l1]==tmp2[l2] && l1>0 && l2>0){
                l1--;
                l2--;
            }
            if(tmp1[l1]>tmp2[l2]){
                tmp1 = tmp2;
                cpy[j] = cpy[i];
                cpy[i] = tmp1;
            }                
        }
        cout << tmp1 << ' ';
    }
    cout<<endl;
	delete[] cpy;
}


#endif

