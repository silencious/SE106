// Name: TianJiahe
// ID: 5130379056

#include "Database.h"
#include <cstdlib>
#include <ctime>
#include <sstream>


// part1, test corretness
void testCorrectness(Database& db, int nrec){
    cout << "PartI: Correctness.\n";
    cout << "Testing correctness.\nRandomly store and fetch " << nrec << " records,\nRandomly remove 1 record and replace 1 record per 13 records.\n";
    for (int i = 0; i < nrec; i++){
        try{
            string key = randkey(), data = randdata();
            db.store(key, data, STORE);
            if (data != db.fetch(key))
                throw(MyException("Correctness test failed with storing or fetching."));
            if (i % 13 == 0){
                db.randremove();
                if (!db.randreplace())
                    throw(MyException("Correctness test failed with removing"));
            }
        }
        catch (MyException& e){
            cerr << e.what() << endl;
            exit(1);
        }
    }
    cout << "Test complete correctly.\n";
    cout << "Deleting old file.\n\n";
    db.delfile();
}
// part2, test performance
void testPerformance(Database& db, int nrec){
    time_t cl1, cl2;

    cout << "PartII: Performance.\n";
    cout << "Stage1: Storing " << nrec << " records.\n";
    cl1 = clock();
    for (int i = 0; i < nrec; i++){
        try{
            string key = randkey();
            string data = randdata();
            db.store(key, data, STORE);
        }
        catch (MyException& e){
            cerr << e.what() << endl;
        }
    }
    cl2 = clock();
    cout << "Takes " << (float)(cl2 - cl1) / CLOCKS_PER_SEC << " seconds.\n\n";

    cout << "Stage2: Fetching " << nrec << " records.\n";
    cl1 = clock();
    for (int i = 0; i < nrec; i++)
        try{
            db.randfetch();
        }
    catch (MyException& e){
        cerr << e.what() << endl;
    }
    cl2 = clock();
    cout << "Takes " << (float)(cl2 - cl1) / CLOCKS_PER_SEC << " seconds.\n\n";

    cout << "Stage3: Complex test.\nRandomly read " << 5 * nrec << " records,\nRandomly remove 1 record per 37 records,\nRandomly store and fetch 1 record per 11 records,\nRandomly replace 1 record per 17 records.\n";
    cl1 = clock();
    for (int i = 0; i < 5 * nrec; i++){
        try{
            db.randfetch();
            if (i % 37 == 0)
                if (!db.randremove())
                    throw MyException("Complex test failed with removing.\n");
            if (i % 11 == 0){
                string key = randkey(), data = randdata();
                db.store(key, data, STORE);
                if (data != db.fetch(key)){
                    throw MyException("Complex test failed with fetching.\n");
                }
            }
            if (i % 17 == 0){
                if (!db.randreplace())
                    throw(MyException("Complex test failed with removing."));
            }
        }
        catch (MyException& e){
            cerr << e.what() << endl;
        }
    }
    cl2 = clock();
    cout << "Takes " << (float)(cl2 - cl1) / CLOCKS_PER_SEC << " seconds.\n\n";

    cout << "Stage4: Removing all records.\nFetch 10 records for each removal.\n";
    cl1 = clock();
    while (true){
        try{
            if (!db.randremove())
                break;
            for (int i = 0; i < 10; i++)
                db.randfetch();
        }
        catch (MyException& e){
            cerr << e.what() << endl;
        }
    }
    cl2 = clock();
    cout << "Takes " << (float)(cl2 - cl1) / CLOCKS_PER_SEC << " seconds.\n\n";

}

int main(int argc, char* argv[]){
    if (argc < 3){
        cerr << "Usage: " << argv[0] << " [-name] [-nrec]" << endl;
        exit(1);
    }
    string name = argv[1];
    int nrec = atoi(argv[2]);
    srand(time(NULL));
    Database db(name, 16);
    testCorrectness(db, nrec);
    testPerformance(db, nrec);

    return 0;
}
