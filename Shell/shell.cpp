

#include "shell.h"

#include <iostream>
#include <string>
#include <vector>

//empty function to intialize shell.
Shell::Shell(string filename, int numberofblocks, int blocksize): Filesys(filename, numberofblocks, blocksize)
{
}

//It is function shows lists of file
int Shell::dir()
{
    //Vector that holds all of the valid file names in our file system
    vector<string> flist = ls();
    
    //Iterate through the vector
    for( int i=0; i< flist.size(); i++)
    {
          cout<<flist[i]<<endl;;
    }
    return 1;

}

//It is a function that helps adding file and data.
int Shell::add(string file, string buffer)
{
        if (!newfile(file))     
	{
		cout << "Cannot add file" << endl;
		return 0;
	}
	if (buffer != "")
	{       
        /*
            Split buffer into blocks, store blocks into blocks vector
            Uses Sdisk's getBlockSize()
        */
		vector<string> blocks = block(buffer, getblocksize()); 
        
        //Add blocks to the disk       
		for (int i = 0; i < blocks.size(); ++i)
		{
             //Uses Filesys's addBlock function to add block of data          
			if (!addblock(file, blocks[i]))
			{
				cout << "Error! Cannot add data!" << endl;
				return 0;
			}
		}
	}
	return 1;
}

//It is a function that delete file.
int Shell::del(string file)
{

    int block = getfirstblock(file);
 
    while(block >0)
    {
      delblock(file,block); 
      block =getfirstblock(file);     
     
    }
    rmfile(file);

    return 1;

}

//It is a function that shows the file inside.
int Shell::type(string file)
{
    /*
       Get the number of the first block belonging to the file
       Uses Filesys's getFirstBlockNum function
    */
    int block = getfirstblock(file);
    string buffer;
    while(block >0)
    {
         string t;
         //Reading the block using Filesys's readBlock function
         readblock(file, block, t);
         //Add contents of block to buffer
         buffer += t;
         //Get the next block to read Uses Filesys's nextBlock function to get the next block number
         block = nextblock(file,block);
    }
    
    cout <<"<---------------------- This is content of "<<file<<" ---------------------->"<<endl;
    for(int i=0; i<buffer.size();i++)
    {
         if(buffer[i] !='#')
         {
               cout <<buffer[i];
         }
    }
    cout <<endl;
    cout <<endl;
    cout <<"The length of " <<file<<"is "<<buffer.length()<<endl;
    cout <<"<----------------------   End of type for   "<<file<<" ---------------------->"<<endl;
    return 1;
}

//It is a function that copies file to other file.
int Shell::copy(string file1, string file2)
{
     int block = getfirstblock(file1); //Get the number of the first block belonging to file1
     if(block == -1)
     {
          cout<<"No such file"<<endl;
          return 0;
     }
     
     block=getfirstblock(file2); //Get the number of the first block belonging to file2
     if(block>=0)
     {
           cout<<"Destination File Exists"<<endl;
           return 0;
     }

    /*
        Create the second file through Filesys's newFile function
        Returns a code upon success(1)/failure(0)
    */

     newfile(file2);
     block=getfirstblock(file1);
     while(block!=0)
     {
         string buffer;
         readblock(file1, block, buffer); //Reading a block from file1
    
         addblock(file2, buffer); //Add the contents of that block (stored in b) to file2
         block=nextblock(file1,block); //Uses Filesys's nextBlock function to get the next block number
     }
  
     return 1;
}

