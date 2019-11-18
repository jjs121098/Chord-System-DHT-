//Name : Jordan Jayke Sidik
//UOW ID : 5921946
#include "Chord.h"
#include "Peer.h"

//Friend function
ostream& operator<< (ostream& os, const Peer& currPeer)
{
    os << "\nDATA AT INDEX NODE " << currPeer.ID << " : ";

    list<pair<unsigned int, string>>::const_iterator lit;
    map<unsigned int, Peer*>::const_iterator fit;

    for (lit = currPeer.dataStorage.begin(); lit != currPeer.dataStorage.end(); lit++)
        os << "\n" << lit->second;

    os << "\nFINGER TABLE OF NODE " << currPeer.ID << " : " << endl;

    for(fit = currPeer.fingerTable.begin(); fit != currPeer.fingerTable.end(); fit++)
        os << fit->second->getID() << " ";

    os << endl;
    
    return os;
}

//Contructor
Peer::Peer (unsigned int ID, unsigned int ftSize)
{
    //initialize values
    this->ID = ID;
    this->ftSize = ftSize;
    this->chordSize = pow(2, ftSize);

    //Generate finger table for the peer
    genFT();
}

//Destructor
Peer::~Peer()
{
    
}

//Finger Table Function
void Peer::genFT()
{
    for(unsigned int i = 0; i < ftSize; i++)
        this->fingerTable.insert({i+1, this});
}

bool Peer::compareID(pair<unsigned int, Peer*> x, pair<unsigned int, Peer*> y)
{
    return (x.second->getID() < y.second->getID());
}

//Resolve function
Peer* Peer::Resolve(unsigned int key)
{
    //iterator to iterate through finger table
    map<unsigned int, Peer*>::iterator mit = this->fingerTable.begin();
    unsigned int counter = 1;

    if (key == this->getID())
        return this;
    //if key is bigger than current peer and peer points to genesis, resolve to genesis
    else if(key > this->getID() && this->fingerTable[1]->getIsGen())
    {
        cout << this->getID() << ">" << this->fingerTable[1]->getID();
        return (this->fingerTable[1]);
    }
    //if key is smaller than current peer but bigger than first index of FT, return current peer
    else if (key <= this->getID() && key > this->fingerTable[1]->getID())
    {
        cout << this->getID() << ">";
        return this->fingerTable[1]->Resolve(key);
    }
    //if key is smaller than current peer AND first index of finger table, jump to largest peer in FT
    else if(key < this->fingerTable[1]->getID() && key < this->getID())
    {
        if (this->fingerTable[1]->getIsGen())
        {
            cout << this->getID() << ">" << this->fingerTable[1]->getID();
            return (this->fingerTable[1]);
        }
        else
        {
            cout << this->getID() << ">";    
            (max_element(this->fingerTable.begin(), this->fingerTable.end(), compareID))->second->Resolve(key);
        }
    }
    else
    {
        for(mit = this->fingerTable.begin(); mit != this->fingerTable.end(); mit++)
        {
            cout << this->getID() << ">";
            unsigned int toCompare = mit->second->getID();        
        
            //inflate fingertable element that is smaller or equal to (in case it points to itself) current peer
            if(mit->second->getID() <= this->getID())
                toCompare += this->chordSize;
        
            if (key > toCompare && counter == ftSize)
            {
                if(mit->second->getID() == this->ID)
                {
                    cout << this->getID();
                    return mit->second;
                }
                else
                    return (mit->second->Resolve(key)); 
            }
            else if (key <= toCompare)
            {
                if(counter > 1)
                {
                    mit--;
                    return (mit->second->Resolve(key));
                }
                else
                {
                    return (mit->second);
                }
                
            }
            
            counter++;
        }
    }
    
    
}

//succ function
int Peer::succ(unsigned int i, unsigned int ID)
{
    return ((ID + (int)(pow(2, (i-1)))) % (this->chordSize));
}

//Add new peer function
Peer* Peer::NewPeer(unsigned int ID)
{
    //introduced by any peer
    Peer* nP = new Peer(ID, this->ftSize);

    //1) Peer 0 perform a lookup (Resolve for the new node) to populate the new peer's finger table
    this->populateNewFT(nP);

    //Send data request to successor peer (first index of fingertable)
    Peer* succPeer = nP->fingerTable[1];
    succPeer->dataRequest(this);

    //2) Tell the other peers about existence of new peer via finger table and update finger table
    this->updatePeersAdd(nP);

    cout << "PEER " << nP->getID() << " ADDED";

    return nP;
}

//Populate new peer's finger table
void Peer::populateNewFT(Peer* newPeer)
{
    for (unsigned int i = 0; i < ftSize; i++)
    {
        cout << "\n";

        //Resolve function
        Peer* resPeer = this->Resolve(succ(i+1, newPeer->getID()));

        //Check if returned peer is the same as the new peer.  
        //If they are, exit as the peer already exists in the chord
        if (resPeer->getID() == newPeer->getID())
        {
            newPeer = NULL;
            delete newPeer;
            break;
        }

        //Populate finger table
        if(resPeer->getID() < succ((i + 1), this->ID) && succ((i + 1),this->ID) < this->ID)
            newPeer->fingerTable[i+1] = newPeer;
        else
            newPeer->fingerTable[i+1] = resPeer;
    }

    cout << endl;
}

//Send notifications to others peers (update their finger tables)
void Peer::updatePeersAdd(Peer* newPeer)
{
    //Check last update of current peer (avoid duplicate updates)
    if (this->peerUpdate != newPeer->getID() || this->updateInfo != "ADD")
    {
        peerUpdate = newPeer->getID();
        updateInfo = "ADD";
        this->localCache.clear();

        //iterator to iterate through finger table
        map<unsigned int, Peer*>::iterator mit;

        //send out notifications
        for (mit = this->fingerTable.begin(); mit != this->fingerTable.end(); mit++)
        { 
            //if Peer to be notified != current peer
            //Notify the peer of the new peer's existence
            if(mit->second->getID() != this->getID())
                mit->second->updatePeersAdd(newPeer);

        }

        //reset iterator to update finger table
        mit = this->fingerTable.begin();

        for (mit = this->fingerTable.begin(); mit != this->fingerTable.end(); mit++)
        {
            unsigned int toCompare = mit->second->getID();
            unsigned int succValue = succ(mit->first, this->getID());
            unsigned int newPeerID = newPeer->getID();

            if(toCompare <= this->ID)
                toCompare += this->chordSize;
    
            if(succValue <= this->ID)
                succValue += this->chordSize;

            if(newPeerID <= this->ID)
                newPeerID += this->chordSize;
            
            cout << mit->second->getID() << ">";

            if (succValue <= newPeerID && newPeerID <= toCompare)
                mit->second = newPeer;
            
            cout << mit->second->getID() << endl;
        }

    }
    
}

//Remove Peer Function (send updates to peers, update finger table,  
//move data from current peer (if curr peer is to be deleted))
void Peer::RemovePeer(unsigned int toDeleteID)
{
    if (this->peerUpdate != toDeleteID || this->updateInfo != "REMOVE")
    {
        peerUpdate = toDeleteID;
        updateInfo = "REMOVE";
        this->localCache.clear();

        //iterator to iterate through finger table
        map<unsigned int, Peer*>::iterator mit;

        //1) Send out notifications
        for (mit = this->fingerTable.begin(); mit != this->fingerTable.end(); mit++)
        { 
            //if Peer to be notified != current peer
            //Notify the peer of the specific peer's removal
            if(mit->second->getID() != this->getID())
                mit->second->RemovePeer(toDeleteID);

        }

        //2) If current peer is not the peer to be removed -> Update finger table
        if (this->getID() != toDeleteID)
        {
            //iterator to iterate through finger table
            map<unsigned int, Peer*>::iterator mit;

            Peer* replacement = NULL;
            unsigned int counter = 1; 
            
            for(mit = this->fingerTable.begin(); mit != fingerTable.end(); mit++)
            {
                cout << "\n" << mit->second->getID();

                //If element is the peer to be removed
                if (mit->second->getID() == toDeleteID)
                {
                    //Resolve for new successor
                    if (replacement != NULL)
                    {
                        mit->second = replacement;
                    }    
                    else
                    {
                        cout << "\n";
                        replacement = this->Resolve(toDeleteID + 1);
                        mit->second = replacement;
                    }
                }  

                cout << ">" << mit->second->getID();

            }
        
        }
        else if (!(this->dataStorage.empty()))//2) else move data items
        {
            list<pair<unsigned int, string>>::iterator lit;

            //Search for successor, pass data items to successor
            map<unsigned int, Peer*>::iterator mit = this->fingerTable.begin();

            if (mit->second->getID() != this->getID())
            {
                for(lit = this->dataStorage.begin(); lit != this->dataStorage.end(); lit++)
                    mit->second->receiveData(lit->first, lit->second);
            }

        }

    }

}

//Data related functions
//Store data function -> Store and receiveData
void Peer::Store(string data)
{
    //Hash to obtain key
    unsigned int key = this->Hash(data);
    map<unsigned int, Peer*>::iterator it;

    Peer* destPeer = NULL;

    //Resolve to find designated peer
    if(!(this->localCache.empty()))
    {
        it = localCache.find(key);

        if(it != this->localCache.end())
            destPeer = it->second;
        else
        {
            cout << "\n"; 
            destPeer = this->Resolve(key);
            this->localCache.insert({key, destPeer});
        }
           
    }
    else
    {
        cout << "\n"; 
        destPeer = this->Resolve(key);
        this->localCache.insert({key, destPeer});
    }
       
    //Pass to designated peer
    destPeer->receiveData(key, data);
}

void Peer::receiveData(unsigned int key, string data)
{
    this->dataStorage.push_back(pair<unsigned int, string>(key, data));

    cout << "\nINSERTED " << data << " (key = " << key << ") AT " << this->getID(); 
}

//Function for new peers to request data
void Peer::dataRequest(Peer* senderPeer)
{
    list<pair<unsigned int, string>>::iterator lit, litup;
    litup = upper_bound(this->dataStorage.begin(), this->dataStorage.end(), make_pair(senderPeer->getID(), string(" ")));

    for (lit = this->dataStorage.begin(); lit != litup; lit++)
    {
        senderPeer->receiveData(lit->first, lit->second);
        dataStorage.erase(lit);
    }

}

//Print peer function
void Peer::Print(unsigned int key)
{
    map<unsigned int, Peer*>::iterator it;

    Peer* destPeer = NULL;

    //Resolve to find designated peer
    if(!(this->localCache.empty()))
    {
        it = localCache.find(key);

        if(it != this->localCache.end())
            destPeer = it->second;
        else
        {
            cout << "\n"; 
            destPeer = this->Resolve(key);
            this->localCache.insert({key, destPeer});
        }
           
    }
    else
    {
        cout << "\n"; 
        destPeer = this->Resolve(key);
        this->localCache.insert({key, destPeer});
    }

    cout << *destPeer;
}

//Delete data function -> Store and removeData
void Peer::Delete(string data)
{
    //Hash to obtain key
    unsigned int key = this->Hash(data);

    map<unsigned int, Peer*>::iterator it;

    Peer* destPeer = NULL;

    //Resolve to find designated peer
    if(!(this->localCache.empty()))
    {
        it = localCache.find(key);

        if(it != this->localCache.end())
            destPeer = it->second;
        else
        {
            cout << "\n"; 
            destPeer = this->Resolve(key);
            this->localCache.insert({key, destPeer});
        }
           
    }
    else
    {
        cout << "\n"; 
        destPeer = this->Resolve(key);
        this->localCache.insert({key, destPeer});
    }

    //Proceed to remove data
    destPeer->removeData(key, data);
}

void Peer::removeData(unsigned int key, string data)
{
    list<pair<unsigned int, string>>::iterator lit;

    lit = find(this->dataStorage.begin(), this->dataStorage.end(), make_pair(key, data));

    if (lit != this->dataStorage.end())
    {
        this->dataStorage.erase(lit);
        cout << "\nREMOVED " << data << " FROM " << this-> getID(); 
    }
          
}

//Hash function
unsigned int Peer::Hash(string data)
{
	unsigned int key = 0;

    for(int i = 0; i < data.size(); i++)
        key = ((key << 5) + key) ^ data[i];

    return (fmod(key,(pow(2, this->ftSize))));
}

//Accessor Functions
unsigned int Peer::getID()
{
    return this->ID;
}

bool Peer::getIsGen()
{
    return this->isGenesis;
}

//Mutator Functions
void Peer::setIsGen(bool status)
{
    this->isGenesis = status;
}