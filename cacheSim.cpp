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

class Cacheline
{
private:
	unsigned long int block;
	bool valildity;

public:
	Cacheline(unsigned long int x)
	{
		block = x;
		valildity = true;
	}

	bool operator==(const Cacheline &b) const
	{
		return this->block == b.block && this->valildity == b.valildity;
	}
	void invalidate()
	{
		this->valildity = false;
	}
	void revalidate()
	{
		this->valildity = true;
	}
	unsigned long int value()
	{
		return block;
	}
	bool validity()
	{
		return valildity;
	}
};

class Way 
{
private:
	std::vector<Cacheline *> elements;
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
	~Way()
	{
		for (std::vector<Cacheline *>::iterator i = elements.begin(); i != elements.end(); i++)
		{
			delete (*i);
		}
	}

	bool add(unsigned long int newComer)
	{
		// first let's check if it is there just invalidly
		Cacheline iv(newComer);
		iv.invalidate();
		for (std::vector<Cacheline *>::iterator i = this->elements.begin(); i != this->elements.end(); i++)
		{
			if (*(*i) == iv)
			{
				// yup
				// updating this one
				(*i)->revalidate();
				return true;
			}
		}
		// else way, we need to add him forcibly
		if (currSize < maxSize)
		{
			this->elements.push_back(new Cacheline(newComer));
			this->currSize++;
			return true;
		}
		return false;
	}

	bool exists(unsigned long int x) 
	{
		for (std::vector<Cacheline *>::iterator i = this->elements.begin(); i != this->elements.end(); i++)
		{
			if (*(*i) == Cacheline(x))
			{
				// hit!
				// its there and its VALID
				return true;
			}
		}
		// miss :(
		return false;
	}

	bool existdInvalidOrValid(unsigned long int x)
	{
		Cacheline inv(x);
		inv.invalidate();
		for (std::vector<Cacheline *>::iterator i = this->elements.begin(); i != this->elements.end(); i++)
		{
			if (*(*i) == Cacheline(x) || **i == inv)
			{
				// hit!
				// its there
				return true;
			}
		}
		// miss :(
		return false;
	}

	void invalidate(unsigned long int x)
	{
		for (std::vector<Cacheline *>::iterator i = this->elements.begin(); i != this->elements.end(); i++)
		{
			if (*(*i) == Cacheline(x))
			{
				(*i)->invalidate();
			}
		}
	}

	bool accessed(unsigned long int x)
	{
		// find it in the cache
		std::vector<Cacheline *>::iterator i = this->elements.begin();
		for (; i != this->elements.end(); i++)
		{
			if (**i == Cacheline(x))
			{
				// hit!
				Cacheline *value = *i;
				this->elements.erase(i); // since it is a basic pointer it doesn't get deleted, and i am holding him do we don't lost him.
				this->elements.push_back(value);
				return true;
			}
		}
		// no found :(
		return false;
	}

	Cacheline RemoveLRU()
	{
		// not possible but just in case
		if (this->elements.empty())
		{
			return -1;
		}

		Cacheline *deleted = *(this->elements.begin());
		unsigned long int rip = deleted->value();
		Cacheline ri(rip);
		if (!deleted->validity())
		{
			ri.invalidate();
		}
		delete deleted; // bye bye bye
		this->elements.erase(elements.begin());
		this->currSize--;
		return ri;
	}

	bool removeSpecifically(unsigned long int fucker)
	{
		for (std::vector<Cacheline *>::iterator i = this->elements.begin(); i != this->elements.end(); i++)
		{
			if ((*i)->value() == fucker)
			{
				delete *i;
				this->elements.erase(i);
				this->currSize--;
				return true;
			}
		}
		return false;
	}

	void stats()
	{
		cout << " | ";
		for (std::vector<Cacheline *>::iterator i = this->elements.begin(); i != elements.end(); i++)
		{
			cout << (*i)->value() << " v: " << (*i)->validity() << " | ";
		}
	}
};

class Cache
{
private:
	std::map<unsigned long int, Way *> sets; // assuming all accessed addresses are valid, we do not need to limit or check the inputs into here
	int numSets;
	int waySize;
	// int associativity;
	bool wr_alloc;

public:
	Cache(unsigned long int size, int blockSize, int assoc, int wr_alloc)
	{
		wr_alloc = wr_alloc;
		// we loging, not decimaling
		numSets = pow(2, size - blockSize - assoc); // we have no control over it but it is kept by the % operation
		waySize = pow(2, assoc);
		// associativity = assoc;
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
		sets[x] = new Way(waySize);
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

	bool existsInvOrVal(unsigned long int set, unsigned long int x)
	{
		return sets[set]->existdInvalidOrValid(x);
	}

	bool accessed(unsigned long int set, unsigned long int x)
	{
		return sets[set]->accessed(x);
	}

	Cacheline RemoveLRU(unsigned long int set)
	{
		return sets[set]->RemoveLRU();
	}

	bool removeSpecifically(unsigned long int fucker)
	{
		for (std::map<unsigned long, Way *>::iterator i = sets.begin(); i != sets.end(); i++)
		{
			if (((*i).second)->existdInvalidOrValid(fucker))  // when removing it can be either valid or invalid we don't care
			{
				((*i).second)->removeSpecifically(fucker);

				return true;
			}
		}
		return false;
	}

	void invalidate(unsigned long int set, unsigned long int x)
	{
		sets[set]->invalidate(x);
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
		// cout << "curr string: " << cutAddress;
		unsigned long int num = 0; 
		num = strtoul(cutAddress.c_str(), NULL, 16);
		// lets goooooo

		// ok so we actually are looking only for top (size_of_num - block_size_in_log) digits
		// so the cache is represented by:
		num = num / (pow(2, BSize)); // removing BSize last bits is equal to dividing by 2^Bsize

		// the indexes for accessing the elements in the Ls
		//pow(2, size - blockSize - assoc)
		unsigned long int set1 = num % (unsigned long int)(pow(2, L1Size - L1Assoc - BSize));
		unsigned long int set2 = num % (unsigned long int)(pow(2, L2Size - L2Assoc - BSize));
		if (DEBUG)
			cout << "set1: " << set1 << " and set 2: " << set2 << endl;
		// if the sets aren't defined int the cache yes we should add them
		// 	this is done automatically in the practical situation but i am trying to save us some
		// 	space by NOT having thousands of arrays hanging around.
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

			if (operation == 'w')
			{
				L2.invalidate(set2, num);
				L2.accessed(set2, num);
			}
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
				if (operation != 'w' || WrAlloc) // anything but write in no write allocate
				{
					// we need to update the data in L1
					// accTimeCounter += L1Cyc;

					// we know for a fact that the line is NOT in L1
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
				if (operation == 'r' || WrAlloc)
				{
					// only skipping this if this is a write command with no write allocate
					// the (changed/)line goes into L2 and then L1

					if (!L2.add(set2, num))
					{
						// no space in L2
						// kick a bitch out
						unsigned long int v = L2.RemoveLRU(set2).value();
						// L1.invalidate(v % (unsigned long int)(pow(2, L1Assoc)))
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
						Cacheline v = L1.RemoveLRU(set1);
						// L2 is allowed to have stuff L1 doesn't have, so we can leave it at that

						// if that element was modified, we'd have to update his data in L2, meaning it was accessed in L2
						if (!v.validity())
						{
							// there was a modification.
							// the update is done in background, but the elements was accessed
							// since it's a random element,we don't know it's set but we can calculate it:
							unsigned long int set2too = v.value() % (unsigned long int)(pow(2, L2Assoc));
							L2.accessed(set2too, v.value());
						}
						// space was freed. add
						L1.add(set1, num);
					}
				}
				if (operation == 'w')
				{
					// finaly, write:
					// writing only in L1
					L1.invalidate(set1, num);
				}
				// else
				//  no writing back, the change is only in memory and the cache is as oblivious as a three year old :)
			}
		}

		if (DEBUG) // honeslty the whole this is great but my debugging functions fucking SLAYED i deserve extra point for it
		{
			// std::cout << "l1: " << L1miss << " L2: " << L2miss << std::endl;
			cout << "L1: " << endl;
			L1.stats();
			cout << endl
				 << endl
				 << "L2: " << endl;
			L2.stats();
			cout << endl
				 << "------------------------------------------------------" << endl;
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
// i almost hope this will get flagged so someone will read my shitposting