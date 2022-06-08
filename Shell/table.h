
/*Implement the table class. All functions return 1 if successful
and 0 otherwise.*/

#ifndef TABLE_H
#define TABLE_H

#include <iostream>
#include <string>
#include <vector>
#include "shell.h"

//Table class, inherits Shell
class Table : public Shell
{
	public :
	Table(string diskname,int blocksize,int numberofblocks, string flatfile, string indexfile); // constructor

	/*
		Reads records from input file, add the records to the flatfile, create index records
		consisting of the date and block number, and then add the index records
		to the index file
	*/
	int Build_Table(string input_file);
	//Accepts a key value, and searches the index file
	int Search(string value);

	private :
	
	//The name of our flat file on our file system
	string flatfile;
	
	//The name of our index file on our file system
	string indexfile;
	
	/* 
		Accepts a key value and searches the index file for that key
		Reads in the index file, goes through all the keys to look for the
		passed in key, and it will return the block number that has that record
	*/
	int IndexSearch(string value);
};

#endif

