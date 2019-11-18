//Name : Jordan Jayke Sidik
//UOW ID : 5921946
#ifndef PEER_H_
#define PEER_H_

#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <iomanip>
#include <iterator>
#include <map>
#include <list>
#include <algorithm>
#include <string>
using namespace std;

class Peer
{
    private:
        //Node id
        unsigned int ID;

        //Chord Size
        unsigned int chordSize;

        //local set of known peers (finger table)
        map <unsigned int, Peer*> fingerTable;

        //Finger table and size of table (get from chord)
        unsigned int ftSize;

        //lowest node
        bool isGenesis = false;

        //Flags for updating to avoid duplicate calls
        unsigned int peerUpdate = 0;
        string updateInfo = "";

        //Data
        list<pair<unsigned int, string>> dataStorage;
        map<unsigned int, Peer*> localCache;
        

    public:
        friend ostream& operator<< (ostream& os, const Peer& currPeer);

        //Constructor
        Peer(unsigned int, unsigned int);

        //Destructor
        ~Peer();

        //Finger table generation 
        void genFT();

        //Basic Functions
        Peer* NewPeer(unsigned int);
        void RemovePeer(unsigned int);
        Peer* Resolve(unsigned int);

        //Functions to update peers (for adding of peers)
        void updatePeersAdd(Peer*);

        //Populate finger table for new peers
        void populateNewFT(Peer*);

        //Hash Function 
        unsigned int Hash(string);

        //Other functions
        int succ(unsigned int, unsigned int ID);
        static bool compareID(pair<unsigned int, Peer*>, pair<unsigned int, Peer*>);

        //Accessor functions
        unsigned int getID();
        bool getIsGen();

        //Mutator Function
        void setIsGen(bool);

        //Data item Functions
        void Store (string);
        void Delete (string);
        void Print (unsigned int);
        void dataRequest(Peer*);
        void receiveData(unsigned int, string);
        void removeData(unsigned int, string);
};

#endif //PEER_H_