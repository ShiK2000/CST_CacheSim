#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

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
	// we no use c++11
	// ~Way() = default;

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
		for (std::vector<unsigned long>::iterator i = this->elements.begin(); i != this->elements.end(); i++)
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
		std::vector<unsigned long>::iterator i = this->elements.begin();
		for (; i != this->elements.end(); i++)
		{
			if (*i == x)
			{
				// hit!
				break;
			}
		}

		if (i == this->elements.end())
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

	bool removeSpecifically (unsigned long int fucker)
	{
		for (std::vector<unsigned long>::iterator i = this->elements.begin(); i != this->elements.end(); i++)
		{
			if( (*i) == fucker)
			{
				this->elements.erase(i);
				return true;
			}
		}
		return false;
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

	bool removeSpecifically (unsigned long int fucker)
	{
		for (auto i = sets.begin(); i != sets.end() ; i++)
		{
			if( (i->second).exists(fucker) )
			{
				(i->second).removeSpecifically(fucker);
				return true;
			}
		}
		return false;
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

	int L1miss = 0;
	int L2miss = 0;
	int L1acc = 0;
	int L2acc = 0;


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
		L1acc++;
		if (L1.exists(set1, num))
		{
			// update LRU: accessed
			L1.accessed(set1, num);
		}
		else
		{
			// L1 miss
			L1miss++;
			// we now access L2
			L2acc++;
			accTimeCounter += L2Cyc;

			if (L2.exists(set2, num))
			{
				// l2 hit
				// cache line was accessed
				L2.accessed(set2, num);

				// write allocate clause:
				if (operation != 'W' || !WrAlloc)
				{
					// we need to update the data in L1
					accTimeCounter += L1Cyc;

					// we know for a fact that the line is NOT in L1
					// we need to add the line to L1 since it was used
					if (!L1.add(set1, num))
					{
						// no space 
						L1.RemoveLRU(set1);
						L1.add(set1, num);
					}
				}
				// else:
				// 		we had a writing command but no write allocate - so we need not access L1
			}
			else
			{
				// L2 miss as well
				// failure is a habit by this point #ilovecs
				L2miss++;
				// we must access memory
				accTimeCounter += MemCyc;
				// ok but at what price
				if(operation == 'R' || !WrAlloc)
				{
					// the (changed/)line goes into L2 and then L1
					if(!L2.add(set2, num))
					{
						// no space in L2
						// kick a bitch out
						unsigned long int v = L2.RemoveLRU(set2);
						L1.removeSpecifically(v); // we don't know what set it is from so just remove him if he's there
						 
						// space was freed. add
						L2.add(set2, num);
					}
					// ok the element was added to L2
					// now L1:
					if(!L1.add(set1, num))
					{
						// no space in L1
						// kicking a bitch out again
						unsigned long int v = L1.RemoveLRU(set1);
						
						// space was freed. add
						L1.add(set1, num);
					}
				}
				//else
					// no writing back, the change is only in memory and the cache is as oblivious as a three year old :)

			}

		
		}
	}
	L1MissRate = L1miss / L1acc;
	L2MissRate = L2miss / L2acc;


	double avgAccTime;
	// we access l1 exactly once for each access we attempt
	avgAccTime = accTimeCounter/L1acc; 
	// finally: printing
	printf("L1miss=%.03f ", L1MissRate);
	printf("L2miss=%.03f ", L2MissRate);
	printf("AccTimeAvg=%.03f\n", avgAccTime);

	return 0;
	// pray for god, any god, any one who might be listening
}
