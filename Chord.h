//Name : Jordan Jayke Sidik
//UOW ID : 5921946
#ifndef CHORD_H_
#define CHORD_H_

#include "Peer.h"
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <iomanip>
#include <iterator>
#include <vector>
#include <map>
#include <algorithm>
#include <random>
#include <cstdlib>
using namespace std;

class Chord
{
    private:
        //2 <= n < 32
        unsigned int size;

        //finger table size for each peer
        unsigned int ftSize;

        //Node list
        vector<Peer*> peerVect; 
    
    public:
        //Constructor
        Chord();

        //Destructor
        ~Chord();

        //Read file
        void Read(string);

        //Init chord
        void Init(int n);

        //Accessor 
        unsigned int getFTSize();
        unsigned int getSize();

        static bool comparePeer(Peer*, Peer*);
};

#endif //CHORD_H_