#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath> // literaly just for the pow and log function

// fuck everything i am not recreating something that alredy exists
const int DEBUG = 1;
#include <vector>
#include <map>

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
	// TODO - change elements to vector<tuple<uli, bool>>
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
			// we good we can add a new friend  // TODO	- make sure initial validation is true
			this->elements.push_back(newComer);
			this->currSize++;
			return true;
		}
		return false;
	}

	bool exists(unsigned long int x) // TODO - verify validity before jumping to conclusions
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
				unsigned long int value = *i;
				this->elements.erase(i); // TODO - change into tuple - remember original validiy
				this->elements.push_back(value);
				return true;
			}
		}
		// no found :(
		return false;
	}

	unsigned long int RemoveLRU()
	{
		// not possible but just in case
		if (this->elements.empty())
		{
			return -1;
		}
		unsigned long int deleted = *(this->elements.begin());
		this->elements.erase(elements.begin());
		this->currSize--;
		return deleted;
	}

	bool removeSpecifically(unsigned long int fucker)
	{
		for (std::vector<unsigned long>::iterator i = this->elements.begin(); i != this->elements.end(); i++)
		{
			if ((*i) == fucker)
			{
				this->elements.erase(i);
				return true;
			}
		}
		return false;
	}

	void stats()
	{
cout << " | ";
		for (std::vector<unsigned long>::iterator i = this->elements.begin(); i != elements.end(); i++)
		{
			cout  << (*i) << " | ";
		}
	}
};

class Cache
{
private:
	std::map<unsigned long int, Way*> sets; // assuming all accessed addresses are valid, we do not need to limit or check the inputs into here
	int numSets;
	int setSize;
	int associativity;
	bool wr_alloc;

public:
	Cache(unsigned long int size, int blockSize, int assoc, int wr_alloc)
	{
		wr_alloc = wr_alloc;
		// numSets = (assoc == 0 ? size : size / (blockSize * assoc));
		// we loging, not decimaling
		setSize = pow(2, size - blockSize - assoc);
		// setSize = (assoc == 0 ? size : size / assoc);
		numSets = pow(2, assoc);
		associativity = assoc;
		sets = std::map<unsigned long int, Way *>();
	}
	~Cache()
	{
		for (std::map<unsigned long, Way *>::iterator i = sets.begin(); i != sets.end(); i++)
		{
			delete (*i).second;
		}
	}
	bool affirmSetIsIn(unsigned long int x)
	{
		for (std::map<unsigned long, Way *>::iterator i = sets.begin(); i != sets.end(); i++)
		{
			if ((*i).first == x)
			{
				return true;
			}
		}
		sets[x] = new Way(setSize);
		return sets[x] != NULL;
	}
	// bool access(unsigned long int address, bool isWrite) {}

	bool add(unsigned long int set, unsigned long int newComer)
	{
		return sets[set]->add(newComer);
	}

	bool exists(unsigned long int set, unsigned long int x)
	{
		return sets[set]->exists(x);
	}

	bool accessed(unsigned long int set, unsigned long int x)
	{
		return sets[set]->accessed(x);
	}

	unsigned long int RemoveLRU(unsigned long int set)
	{
		return sets[set]->RemoveLRU();
	}

	bool removeSpecifically(unsigned long int fucker)
	{
		for (std::map<unsigned long, Way *>::iterator i = sets.begin(); i != sets.end(); i++)
		{
			if (((*i).second)->exists(fucker))
			{
				((*i).second)->removeSpecifically(fucker);
				return true;
			}
		}
		return false;
	}

	void stats()
	{
		for (std::map<unsigned long, Way *>::iterator i = sets.begin(); i != sets.end(); i++)
		{
			cout << (*i).first << " ~";
			(*i).second->stats();
			cout << endl;
		}
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

	// clearly till here we good

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

		string cutAddress = address.substr(2); // Removing the "0x" part of the address
cout << "curr string: " << cutAddress ;
		unsigned long int num = 0;
		num = strtoul(cutAddress.c_str(), NULL, 16);
cout << " aka " << num ;
		// lets goooooo

		// ok so we actually are looking only for top (size_of_num - block_size_in_log) digits
		// so the cache is represented by:
		num = num / (pow(2, BSize)); // removing BSize last bits is equal to dividing by 2^Bsize
cout << " that is entered as " << num << endl;
		// the indexes for accessing the elements in the Ls
		unsigned long int set1 = num % (unsigned long int)(pow(2, L1Assoc));
		unsigned long int set2 = num % (unsigned long int)(pow(2, L2Assoc));

		// if the sets aren't defined int the cache yes we should add them
		// 	this is done automatically in the practical situation but i am trying to save us some
		// 	space by NOT hacing thousands of arrays hanging around.
		L1.affirmSetIsIn(set1);
		L2.affirmSetIsIn(set2);

		// L1 access happens always:
		accTimeCounter += L1Cyc;
		L1acc++;
		if (L1.exists(set1, num))
		{
			// update LRU: accessed
			L1.accessed(set1, num);

			// if the operation is write, the matching element in L2 (that must exists cuz inclusivity) is no longer valid
			// since we don't have write through we do not update the data.
			// TODO
		}
		else
		{
			// L1 miss
			L1miss++;
			// we now access L2
			L2acc++;
			accTimeCounter += L2Cyc;

			if (L2.exists(set2, num))
			{cout << "l2 hit meow [there is one impostor among us]" << endl;
				// l2 hit
				// cache line was accessed
				L2.accessed(set2, num);

				// write allocate clause:
				if (operation != 'w' || WrAlloc) // anything but write in no write allocate
				{
					// we need to update the data in L1
					accTimeCounter += L1Cyc;

					// we know for a fact that the line is NOT in L1
					if (!L1.add(set1, num))
					{
						// no space
						L1.RemoveLRU(set1);
						L1.add(set1, num);
					}
					// we added to L1 so inclusivity is restored♥
					
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
				if (operation == 'r' || WrAlloc)
				{
					// only skipping this if this is a write command with no write allocate

					// the (changed/)line goes into L2 and then L1
					if (!L2.add(set2, num))
					{
						// no space in L2
						// kick a bitch out
						unsigned long int v = L2.RemoveLRU(set2);
						L1.removeSpecifically(v); // we don't know what set it is from so just remove him if he's there

						// space was freed. add
						L2.add(set2, num);
					}
					// ok the element was added to L2
					// and also to L1
					if (!L1.add(set1, num))
					{
						// no space 
						// kick a bitch out
						unsigned long int v = L1.RemoveLRU(set1);
						L1.removeSpecifically(v); // we don't know what set it is from so just remove him if he's there
						// L2 is allowed to have stuff L1 doesn't have, so we can leave it at that

						// space was freed. add
						L1.add(set1, num);
					}
				}
				// else
				//  no writing back, the change is only in memory and the cache is as oblivious as a three year old :)
			}
		}

		if (DEBUG)
		{
			// std::cout << "l1: " << L1miss << " L2: " << L2miss << std::endl;
			cout << "L1: " << endl;
			L1.stats();
			cout << endl
				 << endl << "L2: " << endl;
			L2.stats();
			cout <<  endl
				 << "------------------------------------------------------"<< endl;
			cout << endl
				 << endl;
		}
	}
	L1MissRate = (double)L1miss / L1acc;
	L2MissRate = (double)L2miss / L2acc;
	cout << "ok so L1miss is " << L1miss << " and L2 iss is " << L2miss << endl;
	cout << "L1acc " << L1acc << " and 2 " << L2acc << endl;  
	double avgAccTime;
	// we access l1 exactly once for each access we attempt
	avgAccTime = accTimeCounter / L1acc;
	// finally: printing
	printf("L1miss=%.03f ", L1MissRate);
	printf("L2miss=%.03f ", L2MissRate);
	printf("AccTimeAvg=%.03f\n", avgAccTime);

	return 0;
	// pray for god, any god, any one who might be listening
}
