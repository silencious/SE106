// Name: Tian Jiahe
// Student ID: 5130379056

#include <iostream>
#include <ctime>
#include <cstdlib>
using namespace std;

const int SIZE = 100;

inline void swap(int& a, int& b){
    int c = a;
    a = b;
    b = c;
}

void inplaceMerge(int a[SIZE], int low, int mid, int high){
    int i=low, j=mid+1;
    while(i<=mid && j<=high){
        if(a[i]>a[j]){
            for(int k=j; k>i; k--)
                swap(a[k], a[k-1]);
            i++;
            j++;
            mid++;
        }
        else{
            i++;
        }    
    }
}

void inplaceMergeSort(int a[SIZE], int low, int high){
    if(high == low)
        return;
    if(high - low == 1){
        if(a[low]>a[high])
            swap(a[low], a[high]);
        return;
    }
    int mid = low + (high - low) / 2;
    inplaceMergeSort(a, low, mid);
    inplaceMergeSort(a, mid + 1, high);
    inplaceMerge(a, low, mid, high);
}

void merge(int a[SIZE], int low, int mid, int high){
    int i=low, j=mid+1, k=0;
    int* b = new int[high-low+1];
    while(i<=mid && j<=high){
        if(a[i]<=a[j])
            b[k++] = a[i++];
        else
            b[k++] = a[j++];
    }
    while(i<=mid)
        b[k++] = a[i++];
    while(j<=high)
        b[k++] = a[j++];
    for(i=low; i<=high; i++)
        a[i] = b[i-low];
    delete[] b;
}

void mergeSort(int a[SIZE], int low, int high){
    if(high == low)
        return;
    if(high - low == 1){
        if(a[low]>a[high])
            swap(a[low], a[high]);
        return;
    }
    int mid = low + (high - low) / 2;
    mergeSort(a, low, mid);
    mergeSort(a, mid + 1, high);
    merge(a, low, mid, high);
}

void generate(int a[SIZE]){
    for(int i=0; i<SIZE; i++)
        a[i] = rand() % 1000;
}

int main(){
    srand(time(0));
    int a[SIZE];
    generate(a);
    cout << "Before sort:\n";
    for(int i=0; i<SIZE; i++)
        cout << a[i] << ' ';
    cout << "\nAfter merge sort:(classic)\n";
    mergeSort(a, 0, SIZE-1);
    for(int i=0; i<SIZE; i++)
        cout << a[i] << ' ';
    cout << endl;

    generate(a);
    cout << "Before sort:\n";
    for(int i=0; i<SIZE; i++)
        cout << a[i] << ' ';
    cout << "\nAfter merge sort:(inplace)\n";
    inplaceMergeSort(a, 0, SIZE-1);
    for(int i=0; i<SIZE; i++)
        cout << a[i] << ' ';
    cout << endl;
    return 0;
}
