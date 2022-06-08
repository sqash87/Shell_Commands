

#include "table.h"

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>

/*	
	This constructor creates the table object. 
	It also creates the new (empty) files flatfile 
	and indexfile in the file system on the Sdisk using diskname.
*/
Table::Table(string filename, int numberofblocks, int blocksize, string flatfile, string indexfile):Shell(filename, numberofblocks, blocksize)
{
    this->indexfile = indexfile; //Set the flat file name
    this->flatfile = flatfile; //Set the index file name
    this->newfile(flatfile); // creating flat file
    this->newfile(indexfile); // creati g index file
}

/*
	This module will read records from the input file (data.txt file), 
	add the records to the flatfile and create index records consisting of the date and block number, 
	and then add the index records to the index file.
*/
int Table::Build_Table(string input_file)
{
 
    stringstream indexstream;
	
	ifstream infile;
	infile.open(input_file); //Open the input file
	
	if (!infile.is_open()) //Could not open the file
	{
		return 0;
	}

	string s;

	
	indexstream << ".";

	while (getline(infile, s))  //Write the record to the flat file and add index record
       {
		
		string key = s.substr(0, 5); // take out first 5 bytes (our key - the date)
		
		/*
			Break the line read in into blocks
			one block back - 128 byte block size, our records are at most 120 bytes
		*/

		vector<string> blocks = block(s, getblocksize());
		
		int b = addblock(flatfile, blocks[0]); //b is the block number
		
		/*
			write the key and the block number in the
			flat file that contains the record with this key
		*/
		indexstream << key << " " << b << ".";
		
	}

	//Convert indexstream to a string, write it to buffer
	string buffer = indexstream.str();
	
	//Break the buffer (what we're going to write to the index file) into blocks
	vector<string> blocks = block(buffer, getblocksize());
	
	//Write our index file blocks to our index file
	for (int i = 0; i < blocks.size(); i++) {
		addblock(indexfile, blocks[i]);

	}
	
	return 0;
}  
 
/*
	This module accepts a key value, and searches the index file indexfile 
	for the record where the date matches the specified value. 
	IndexSearch then returns the block number key of the index record where the match occurs.
*/
int Table::IndexSearch(string value)
{
    istringstream instream; //Inpute buffer used to read from index file blocks
	int b = getfirstblock(indexfile); //Returns the first block of the index file
	string bigbuffer; //Used to hold contents of all blocks of index file
	value = "." + value + " ";
	////Error ocurred
	if (b < 0) {
		return 0;
	}
	/*
		Block does exist
		Read contents of index file, 
		store it in bigBuffer
	*/
	while (b != 0) {
		string buffer; //Buffer to hold contents of first block of index file
		readblock(indexfile, b, buffer); //Read the b block number of indexFile, store that block's contents into buffer
		b = nextblock(indexfile, b); //Get next block of index file
		bigbuffer += buffer; //Add contents of b block to our bigger buffer
	}

        
	int index = bigbuffer.find(value);
	if (index < 0)
	{
		return 0;
	}
	stringstream s(bigbuffer.substr(index + 6, 10));

	int r;
	s >> r;

	return r;
}

/*
	This module accepts a key value, 
	and searches the index file with a call to IndexSearch for the record 
	where the date matches the specified value. 
	IndexSearch returns the blocknumber of the block in the flat file 
	where the target record is located. 
	This block should then be read and the record displayed.
*/
int Table::Search(string value)
{       
{
	/*
		IndexSearch will return the block number containing the record
		with the passed in key 
	*/
	int idx = IndexSearch(value);
	if (idx == 0) {
		cout << "not existing." << endl << endl;
		return 0;
	}
	string buffer;
	int error = readblock(flatfile, idx, buffer); //Read the b block in flatFile, store it to buffer
	buffer.erase(buffer.find_last_not_of('#') + 1);
	cout << buffer << endl << endl; //Output what's in the buffer
	
	return error;
}
}
