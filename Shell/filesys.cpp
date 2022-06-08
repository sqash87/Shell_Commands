
#include "filesys.h"

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdio>

//It is a constructor for the filesystem. Therefore, values are initialized here.
Filesys::Filesys(string diskname, int numberofblocks, int blocksize):Sdisk(diskname,numberofblocks,blocksize)
{
   //Every file in the root has 12 characters
	//rootSize is the maximum number of files we can have
   this->rootsize = getblocksize()/12;
   
   //Every entry in FAT is 5 digits long
	//getNumberOfBlocks = # of elements in FAT
	//getNumberOfBlocks * 5 = # of bytes/characters in FAT
	//Divide (getNumberOfBlocks*5) by getBlockSize - will tell the
	//number of blocks we absolutely need
	//Give one extra block to FAT
   this->fatsize = (5*getnumberofblocks()/getblocksize())+1;
  
 
   string buffer; //Write what's in a block to the buffer so that we can read the block
   
   //getBlock returns 1 if success, 0 if failure
	//Block 0 should be a bunch of # signs
   getblock(0, buffer);
    
    
   if(buffer[1] == '#')
   {
      
      for(int i = 0; i<rootsize; ++i)
      {
       filename.push_back("xxxxxx");
   	 firstblock.push_back(0);
      }

      //Write number of first free block in FAT to fat[0]
	   //Number of first free block in FAT = fatSize + 2
	   //FAT in memory
 
      fat.push_back(fatsize+2);
      fat.push_back(-1);
     
      //Starting from block 2 to fatSize - 1, output 1s (symbolize reserved blocks)
      for(int i=0;i<fatsize; ++i)
      {
          fat.push_back(-1);
      }
      
      //Starting at fatSize + 2, we now have empty blocks in FAT
	   //From fatSize + 2 to last block in FAT (getNumberOfBlocks - 1)
      for(int i=fatsize+2;i<getnumberofblocks(); ++i)
      {
          fat.push_back(i+1);
      }
        
      //Last element in FAT should have zero to symbolize end of free list
      fat[fat.size()-1] =0;
      //Write ROOT and FAT (which are in memory) to Sdisk
      fssynch();
   }
   else
   {  

      readfs();
   }
   

}

//It is a function to close the filesystem.
int Filesys::fsclose()
{
     fssynch();
     cout<<"Filesystem is closed."<<endl;
     return 1;

}

//It is a function to synch the filesystem when it is updated.
int Filesys::fssynch()
{
   
    ostringstream ostream1; //Output buffer used to write to Sdisk
    ostream1 <<endl;
   
   //Write ROOT to ostream
    for(int i=0; i<rootsize; i++)
    {
       ostream1 << filename[i]<<" "<<firstblock[i]<<" ";
    }
    //Output buffer used to write to Sdisk
    ostringstream ostream2;
    
    //Write FAT to ostream2
    for(int i=0; i<getnumberofblocks(); i++)
    {
       ostream2<<fat[i]<<" ";
    }
 

   //Convert ostream1 to string, store it in a string called buffer1
   //Convert ostream2 to string, store it in a string called buffer2
    string buffer1 = ostream1.str();
    string buffer2 = ostream2.str();
    
    //Split our buffer1 string into blocks so we can write it to Sdisk
    vector<string> blocks1 = block(buffer1,getblocksize());
    //Split our buffer2 string into blocks so we can write it to Sdisk
    vector<string> blocks2 = block(buffer2,getblocksize());
     

   //Write blocks2 (our FAT split into blocks) to Sdisk
    putblock(1,blocks1[0]);
    for(int i =0; i<blocks2.size(); i++)
    {
         putblock(2+i, blocks2[i]); //FAT begins in block 2 of Sdisk
    }
   
    return 0;
}

//It is a function to read filesystem.
int Filesys::readfs()
{
    string buffer; //Buffer to hold ROOT
    getblock(1, buffer); //Get the ROOT, which is in block 1 of Sdisk, write to buffer
   
    istringstream istream; //Input buffer used to read from the disk
    istream.str(buffer); //Load buffer into istream, convert to string

   //Read in filename and firstBlock of each entry in ROOT
    for(int i=0; i<rootsize; i++)
    {
       string fn; //filename
       int fb; //firstblock
       istream>>fn>>fb; // readig file filename and firstblock
       filename.push_back(fn); //Add filename to vector
       firstblock.push_back(fb); //Add firstBlock to vector
    }

    string buffer2, buffer3;
    
    //Read in each block of the FAT from Sdisk
    for(int i=0; i<fatsize; i++)
    {
       getblock(i+2, buffer2); //Write block to string buffer2
       buffer3+=buffer2; //Add block contents onto buffer3
       buffer2.clear();
    }

    istringstream istream2;
    istream2.str(buffer3);//Load buffer3 into istream2, convert to string

   //Read in contents of istream2 (contents of FAT), add to fat vector
    for(int i=0; i<getblocksize(); i++)
    {
       
       istream2>>i;
       fat.push_back(i);
    }
   
    return 0;
}

// This function makes a newfile. 
// Also adds an entry for the string file in ROOT with an initial first
int Filesys::newfile(string file)
{
    

    for(int i=0; i<filename.size(); i++)
    {
        
        if(filename[i] == file)
        {
           cout<<"File exists"<<endl;
           return 0;
        }
    }

   //File does not exist - go through ROOT directory
    for(int i=0; i<filename.size(); i++)
    {
         //We have an empty space at index i of filename vector
        if(filename[i] == "xxxxxx")
        {
            filename[i] =file; //Store file's name in filename vector
            fssynch(); //Synch the filesystem
            return 1;
        }
    }
    cout<<"No space for newfile"<<endl;
    return 0;
}

//It is a function to remove the file. 
//Also, removes the entry file from ROOT if the file is empty (first block is 0)

int Filesys::rmfile(string file)
{
   	for (int i = 0; i < filename.size(); i++)
        {
		if (filename[i] == file) 
                {
                     
		     if (firstblock[i] != 0) 
                     {
			   cout << "File is not empty" << endl;
			   return 0;
		     }
                     else
                     {
                       
			filename[i] ="xxxxxx";
                        firstblock[i] = 0;
                     }
                 }
    
	}

    fssynch();
    return 1;
    

}

//It is a function to get first block.
int Filesys::getfirstblock(string file)
{
   for(int i=0; i<filename.size(); i++)
   {
       if(filename[i]==file)
       {
           return firstblock[i];
       }
   }
   return -1;
}

//It is a function to add blocks to the disk.
int Filesys::addblock(string file, string block)
{
   
   int allocate=fat[0];
   if(allocate==0)
   {
       cout<<"No sapce on disk"<<endl;
       return 0;
   }
   
   fat[0]=fat[fat[0]];
   fat[allocate]=0;
   
   int block1 = getfirstblock(file);
   
   if(block1==0)
   {
      for(int i=0; i<rootsize;++i)
      {
         if(filename[i]==file)
         {
            firstblock[i]=allocate; //Get the first empty block in fat
            fssynch();
            putblock(allocate, block);
            return allocate;
         }
      }
   }
   else
   {
      int first = block1;
      while(fat[first]!=0)
      { 
             first=fat[first];  //Update the free list in the fat 
           
       }
      fat[first]=allocate;
      
       fssynch();
       putblock(allocate, block);
       return allocate;
   }
   
  
   fssynch();
   return 1;
}

//It is a function to check the block.
bool Filesys::checkblock(string file, int blocknumber)
{
  int block = getfirstblock(file);

  while(block!=0)
  {
     if(block == blocknumber)
     {
        return true;
     } 
     block=fat[block];
  }

  return false;
}

//It is a function to read a block.
int Filesys::readblock(string file, int blocknumber, string& buffer)
{
   if(checkblock(file,blocknumber))
   {
      getblock(blocknumber, buffer);
      return 1;
   }
   else
   {
      cout<<"No can do"<<endl;
      return 0;
   }
}

//It is a function to write the block.
int Filesys::writeblock(string file, int blocknumber, string buffer)
{
   if(checkblock(file,blocknumber))
   {
      putblock(blocknumber, buffer);
      return 1;
   }
   else
   {
      cout<<"No can do"<<endl;
      return 0;
   }
}

//It is a function for the next block.
int Filesys::nextblock(string file, int blocknumber)
{
   if(checkblock(file,blocknumber))
   {
     
      return fat[blocknumber];
   }
   else
   {
      cout<<"No can do"<<endl;
      return 0;
   }
}

//It is a function to delete the block from the disk.
int Filesys::delblock(string file, int blocknumber)
{
  
   if(not checkblock(file,blocknumber))
   {
      cout<<"No can do"<<endl;
      return 0;
   }

   int block =getfirstblock(file);
   if(block == blocknumber)
   {
      for(int i=0; i<filename.size(); ++i)
      {
         if(filename[i] ==file)
         {
            firstblock[i]=fat[blocknumber];
         }
      }
   }
   else
   {
      while(fat[block]!=0 and fat[block] != blocknumber)
      {
         block=fat[block];
      }
      fat[block] = fat[blocknumber];
   }
   fat[blocknumber] = fat[0];
   fat[0]=blocknumber;
   fssynch();
   cout<<"Deleting the block is complete."<<endl;
   return 0;
}

//Breaks an ostringstream buffer into blockSized blocks.
vector<string> Filesys::block(string buffer, int b)
{
	vector<string> blocks;
	int numberofblocks = 0;
   //If buffer length is divisibly by blockSize
	//We can fill n blocks completely
	if (buffer.length() % b == 0)
	{
		numberofblocks = buffer.length()/b;
	}
   //Buffer length not divisible by blockSize
	//We cannot fill all blocks completely - last one will not be completely full
   else 
   {
		numberofblocks = buffer.length()/b +1;
	}

	string tempblock;
   //Split buffer into blocks

	for (int i = 0; i < numberofblocks; i++)
	{
      //i = 0: position = 0, read blockSize characters
		//i = 1: position = blockSize, read blockSize characters
		//Jump blockSize characters and reading blockSize characters at a time
		tempblock = buffer.substr(b*i, b);
		blocks.push_back(tempblock); //Add block substring to blocks vector
	}

   //Index of last block in vector
	int lastblock = blocks.size() - 1;
   //Fill in extra space of last block
	for (int i = blocks[lastblock].length(); i < b; i++)
	{
		blocks[lastblock] += "#";
                
	}

	return blocks;
}

vector<string> Filesys::ls()
{ 
    vector<string> flist;
    for (int i=0; i<filename.size(); i++)
    {
        if (filename[i] != "xxxxxx")
        {
            flist.push_back(filename[i]);
        }
    }
    return flist;
}

 



