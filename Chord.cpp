//Name : Jordan Jayke Sidik
//UOW ID : 5921946
#include "Chord.h"
#include "Peer.h"
using namespace std;

//Constructor
Chord::Chord()
{
    
}
//Destructor
Chord::~Chord()
{
    //clear peerVect
    for (int i = 0; i < peerVect.size(); i++)
        delete peerVect.at(i);
}

//Init function
void Chord::Init(int n)
{
    if (n < 2 || n > 31)
        exit(-1); //Silent fail
    
    cout << "Name\t: Jordan Jayke Sidik" << endl;
    cout << "UOW ID\t: 5921946" << endl;

    this->size = pow(2, n);
    this->ftSize = n;

    Peer* genesisPeer = new Peer (0, n);
    genesisPeer->setIsGen(true);
    peerVect.push_back(genesisPeer);
}

//Accessor Functions
unsigned int Chord::getFTSize()
{
    return this -> ftSize;
}

unsigned int Chord::getSize()
{
    return this -> size;
}

void Chord::Read(string filename)
{
    //Filestream opening
	ifstream infile;
	infile.open (filename.c_str());

	if (!infile)
	{
		cout << "Error opening file " << filename << " !" << endl;
		infile.close ();
		exit (-1);
	}

    string line, instruction, param;
    vector <pair <string, string>> command;

	while (infile >> instruction)
	{
		//Read remaining of line
		getline (infile, line);

		//Find position of first instance of #
		size_t comment_pos = line.find('#');

        // remove the leading space
        if(line.size() > 0)
        {
            if (isspace(line[0]))
                line = line.erase(0,1);
        }

		if (comment_pos != string::npos)
			param = line.substr(0, comment_pos);
		else
			param = line;

		//Create the pair and add to array
		pair<string, string> temp (instruction, param);
		command.push_back (temp);
	}

    //Close stream
    infile.close();

    //If first line != init, exit
    if(command.front().first != "init")
    {
        //exit silently
        exit(-1);
    }

    vector<pair<string, string>>::iterator it;

    //bool to check for 2 init lines
    bool init = false;
    vector<Peer*>::iterator vit;

    srand(time(NULL));

    for (it = command.begin(); it != command.end(); it++)
    {
        if (it->first == "init")
        {
            init = true;

            //Initialize chord
            this->Init(atoi(it->second.c_str()));
        }
        else if (it->first == "newpeer")
        {

            //Check if peer to be added is lower than current the genesis node
            if (atoi(it->second.c_str()) < peerVect[0]->getID())
            {
                peerVect[0]->setIsGen(false);
            }

            //Genesis peer -> introduce the new peer 
            Peer* genPeer = peerVect[0];
            Peer* newPeer = genPeer->NewPeer(atoi(it->second.c_str()));

            //push back to vector list and sort
            peerVect.push_back(newPeer);
            sort(peerVect.begin(), peerVect.end(), comparePeer);

            //Set genesis
            peerVect[0]->setIsGen(true);
        }
        else if (it->first == "removepeer")
        {
            //Check if peer to be removed is the genesis node
            if (atoi(it->second.c_str()) == peerVect[0]->getID())
            {
                peerVect[1]->setIsGen(true);
                peerVect[0]->setIsGen(false);
            }
            
            //Genesis peer -> remove the new peer
            Peer* genPeer = peerVect[1];
            genPeer->RemovePeer(atoi(it->second.c_str()));

            //remove from vector list
            int i = 0;
            for (; i < peerVect.size(); i++)
            {
                if(peerVect.at(i)->getID() == atoi(it->second.c_str()))
                {
                    //release pointer memory
                    delete peerVect.at(i);
                    break;
                }

            }
            
            peerVect.erase(peerVect.begin() + i);
            cout << "\nPEER " << atoi(it->second.c_str()) << " REMOVED";
        }
        else if (it->first == "store")
        {
            //handle the data storing
            Peer* genPeer = peerVect[0];
            string data = it->second;

            genPeer->Store(data);
        }
        else if (it->first == "print")
        {
            //handle the data printing
            Peer* genPeer = peerVect[0];
            genPeer->Print(atoi(it->second.c_str()));     
        }
        else if (it->first == "delete")
        {
            //handle the data deletion
            Peer* genPeer = peerVect[0];
            string data = it->second;

            genPeer->Delete(data);
        }

        //sort vector
        sort(peerVect.begin(), peerVect.end(), comparePeer);
    }
}

bool Chord::comparePeer(Peer* x, Peer* y)
{
    return (x->getID() < y->getID());
}





