//Name : Jordan Jayke Sidik
//UOW ID : 5921946
#include "Chord.h"
#include "Peer.h"
#include <chrono>
#include <unistd.h>

int main(int argc, char* argv[])
{
    //Check if argument less than 2 (no filename passed)
	if (argc != 2)
	{
		cout << "Invalid number of arguments!" << endl
			 << "Please pass in file name!" << endl;

		exit (-1);
	}

    Chord c;
    c.Read(argv[1]);
}
