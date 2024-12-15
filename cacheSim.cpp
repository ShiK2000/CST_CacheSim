#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>

// fuck everything i am not recreating something that alredy exists

#include <vector>
#include <math.h> // literaly just for the pow function

using std::cerr;
using std::cout;
using std::endl;
using std::FILE;
using std::ifstream;
using std::string;
using std::stringstream;

//perhaps move to another file [oh but the price of editing the make file:(]
class cache // simple cache
{
	private:
	std::vector<unsigned long int > elements;
	int maxSize;
	int currSize;

	public:
	cache(int size)
	{
		// vector is doing the dirty work on its own
		// this->elements
		this->maxSize  = size;
		this->currSize = 0;
	}
	~cache() = default; 

	bool add(unsigned long int  newComer)
	{
		if(currSize < maxSize)
		{
			// we good we can add a new friend
			this->elements.push_back(newComer);
			this->currSize++;
			return true;
		}
		return false;
	}

	std::vector<unsigned long int >::iterator exists (int x)
	{
		for (auto i = this->elements.begin() ; i != this->elements.end() ; i++)
		{
			if(*i == x)
			{
				// hit!
				return i;
			}
		}
		// miss :(
		return this->elements.end();
		
	}

	bool accessed (std::vector<unsigned long int >::iterator x)
	{
		if(x == this->elements.end())
		{
			// for the if case
			return false;
		}

		int dude = *x; 
		if( elements.back() != dude)
		{
			// if he is not already last
			this->elements.erase(x);
			this->elements.push_back(dude);
		}
		return true;
	}

	unsigned long int RemoveLRU ()
	{
		unsigned long int deleted = *(this->elements.begin());
		this->elements.erase(elements.begin());
		return deleted;
	}
};






int main(int argc, char **argv)
{

	if (argc < 19)
	{
		cerr << "Not enough arguments" << endl;
		return 0;
	}

	// Get input arguments

	// File
	// Assuming it is the first argument
	char *fileString = argv[1];
	ifstream file(fileString); // input file stream
	string line;
	if (!file || !file.good())
	{
		// File doesn't exist or some other error
		cerr << "File not found" << endl;
		return 0;
	}

	unsigned MemCyc = 0, BSize = 0, L1Size = 0, L2Size = 0, L1Assoc = 0,
			 L2Assoc = 0, L1Cyc = 0, L2Cyc = 0, WrAlloc = 0;

	for (int i = 2; i < 19; i += 2)
	{
		string s(argv[i]);
		if (s == "--mem-cyc")
		{
			MemCyc = atoi(argv[i + 1]);
		}
		else if (s == "--bsize")
		{
			BSize = atoi(argv[i + 1]);
		}
		else if (s == "--l1-size")
		{
			L1Size = atoi(argv[i + 1]);
		}
		else if (s == "--l2-size")
		{
			L2Size = atoi(argv[i + 1]);
		}
		else if (s == "--l1-cyc")
		{
			L1Cyc = atoi(argv[i + 1]);
		}
		else if (s == "--l2-cyc")
		{
			L2Cyc = atoi(argv[i + 1]);
		}
		else if (s == "--l1-assoc")
		{
			L1Assoc = atoi(argv[i + 1]);
		}
		else if (s == "--l2-assoc")
		{
			L2Assoc = atoi(argv[i + 1]);
		}
		else if (s == "--wr-alloc")
		{
			WrAlloc = atoi(argv[i + 1]);
		}
		else
		{
			cerr << "Error in arguments" << endl;
			return 0;
		}
	}
	// lovely ♥
	// now for the fun shit


	cache L1(L1Size);
	//   _______________
	//  |     L1        |
	//   ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾  
	cache L2(L2Size);
	//   _________________________
	//  |           L2            |
	//   ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾  

	
	while (getline(file, line))
	{

		stringstream ss(line);
		string address;
		char operation = 0; // read (R) or write (W)
		if (!(ss >> operation >> address))
		{
			// Operation appears in an Invalid format
			cout << "Command Format error" << endl;
			return 0;
		}

		//
		// DEBUG - remove this line
		// cout << "operation: " << operation;

		string cutAddress = address.substr(2); // Removing the "0x" part of the address
		unsigned long int num = 0;
		num = strtoul(cutAddress.c_str(), NULL, 16);

		// lets goooooo

		// ok so we actually are looking only for top (size_of_num - block_size_in_log) digits
		// so the cache is represented by:
		num = num / (pow (10, BSize));
		// and out of theses we want to de-ways it (the amount of different positions in the cache is limited)
		// 0x AAAAAA BBBBB CCCCC DDDDD
		// A: set representive 
		// B: repreaters of same set (disregarded)
		// C: cacheline representative
		// D: in-block data (disregarded)

		// extract AC from num which is ABC now:
		// TODO


		// general concept:
		if(true /* L1 hit */)
		{
			// done i think
		}
		else
		{
			if (true /* L2 hit */)
			{
				// if WB1, WB to L1 and finish
			}
			else
			{
				// bring from memory
				// if WB2, WB to L2, if WB1 also WB to L1
			}
		}

	}

	double L1MissRate;
	double L2MissRate;
	double avgAccTime;

	// finally: printing
	printf("L1miss=%.03f ", L1MissRate);
	printf("L2miss=%.03f ", L2MissRate);
	printf("AccTimeAvg=%.03f\n", avgAccTime);



	return 0;
	// pray for god, any god, any one who might be listening
}
