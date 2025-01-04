#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>

// fuck everything i am not recreating something that alredy exists

#include <vector>
#include <map>
#include <math.h> // literaly just for the pow function

using std::cerr;
using std::cout;
using std::endl;
using std::FILE;
using std::ifstream;
using std::string;
using std::stringstream;

class Way // this is the behavior of the set
{
private:
	std::vector<unsigned long int> elements;
	int maxSize;
	int currSize;

public:
	Way(int size)
	{
		// vector is doing the dirty work on its own
		this->maxSize = size;
		this->currSize = 0;
	}
	~Way() = default;

	bool add(unsigned long int newComer)
	{
		if (currSize < maxSize)
		{
			// we good we can add a new friend
			this->elements.push_back(newComer);
			this->currSize++;
			return true;
		}
		return false;
	}

	bool exists(unsigned long int x)
	{
		for (auto i = this->elements.begin(); i != this->elements.end(); i++)
		{
			if (*i == x)
			{
				// hit!
				return true;
			}
		}
		// miss :(
		return false;
	}

	bool accessed(unsigned long int x)
	{
		// find it in the cache
		auto i = this->elements.begin();
		for (; i != this->elements.end(); i++)
		{
			if (*i == x)
			{
				// hit!
				break;
			}
		}

		if(i == this->elements.end())
		{
			// no found :(
			return false;
		}

		unsigned long int value = *i;
		this->elements.erase(i);
		this->elements.push_back(value);

		return true;
	}

	unsigned long int RemoveLRU()
	{
		unsigned long int deleted = *(this->elements.begin());
		this->elements.erase(elements.begin());
		return deleted;
	}
};

class Cache
{
private:
	std::map<unsigned long int, Way> sets; // assuming all accessed addresses are valid, we do not need to limit or check the inputs into here
	int numSets;
	int associativity;
	bool wr_alloc;

public:
	Cache(unsigned long int size, int blockSize, int assoc, int wr_alloc)
	{
		wr_alloc = wr_alloc;
		numSets = size / (blockSize * assoc);
		associativity = assoc;
		sets = std::map<unsigned long int, Way>();
	}

	bool access(unsigned long int address, bool isWrite) {}

	bool add(unsigned long int set, unsigned long int newComer)
	{
		return sets[set].add(newComer);
	}

	bool exists(unsigned long int set, unsigned long int x)
	{
		return sets[set].exists(x);
	}

	bool accessed(unsigned long int set, unsigned long int x)
	{
		return sets[set].accessed(x);
	}

	unsigned long int RemoveLRU(unsigned long int set)
	{
		return sets[set].RemoveLRU();
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

	// it's supposed to kinda be an array/vector of the cache type, with each cell corresponding to way
	// not vibing with vector<cache> as then we'll need to monitor it's size but cache[L1Size] also feels weird
	// i should start working on it when the sun is up
	//
	// cache L1(L1Size);
	//   _______________
	//  |     L1        |
	//   ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
	Cache L1 = Cache(L1Size, BSize, L1Assoc, WrAlloc);
	// cache L2(L2Size);
	//   _________________________
	//  |           L2            |
	//   ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
	Cache L2 = Cache(L2Size, BSize, L2Assoc, WrAlloc);

	double L1MissRate = 0;
	double L2MissRate = 0;
	double accTimeCounter = 0;

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
		num = num / (pow(10, BSize));
		int setSize1 = std::log(L1Assoc);
		int setSize2 = std::log(L2Assoc);

		// the indexes for accessing the elements in the Ls
		unsigned long int set1 = num % (unsigned long int)(pow(10, setSize1));
		unsigned long int set2 = num % (unsigned long int)(pow(10, setSize2));

		// TODO

		// L1 access happens always:
		accTimeCounter += L1Cyc;

		if (L1.exists(set1, num))
		{
			// done i think
			// accessed
			L1.accessed(set1, num);
			// hit L1 ++ , + count time
			// if write then dirty bit?
		}
		else
		{
			accTimeCounter += L2Cyc;
			// L1 miss
			// write allocate upon L1 miss:
			if (operation == 'W')
			{
				// writing command
				if (WrAlloc)
				{
					// yes allocate
				}
				else
				{
					// no allocate
				}
			}

			// attemting L2:
			if (L2.exists(set2, num))
			{
				// if WB1, WB to L1 and finish
			}
			else
			{
				accTimeCounter += MemCyc;
				// write allocate upon L2 miss:
				/*



				*/
				// if L2 got a new line and a line got evicted from it - we need
				// to check if L1 containg this line and if so remove it from there as well

				// bring from memory
				// if WB2, WB to L2, if WB1 also WB to L1
			}
		}
	}

	double avgAccTime;

	// finally: printing
	printf("L1miss=%.03f ", L1MissRate);
	printf("L2miss=%.03f ", L2MissRate);
	printf("AccTimeAvg=%.03f\n", avgAccTime);

	return 0;
	// pray for god, any god, any one who might be listening
}
