#ifndef FILESYS_H
#define FILESYS_H

#include <iostream>
#include <string>
#include <vector>
#include "sdisk.h"
#include <sstream>
#include <cstdio>

using namespace std;

class Filesys: public Sdisk
{
	public :
	Filesys(string diskname, int numberofblocks, int blocksize);
	int fsclose();
	int fssynch();
	int newfile(string file);
	int rmfile(string file);
	int getfirstblock(string file);
	int addblock(string file, string block);
	int delblock(string file, int blocknumber);
	int readblock(string file, int blocknumber, string& buffer);
	int writeblock(string file, int blocknumber, string buffer);
	int nextblock(string file, int blocknumber);
	bool checkblock(string file, int blocknumber);
	vector<string> block(string buffer, int b);
        vector<string> ls();

	private :
	int rootsize;           // maximum number of entries in ROOT
	int fatsize;            // number of blocks occupied by FAT
	int readfs();
	vector<string> filename;   // filenames in ROOT
	vector<int> firstblock; // firstblocks in ROOT
	vector<int> fat;             // FAT
};

#endif
