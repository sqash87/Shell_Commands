

#include "sdisk.h"

#include <sstream>
#include <fstream>
#include <iostream>


// initialize the values
Sdisk::Sdisk(string diskname, int numberofblocks, int blocksize)
{
   this->diskname = diskname; //Set diskname
   this->numberofblocks = numberofblocks; //Set number of blocks
   this->blocksize=blocksize; //Set blocksize
 
   fstream fsfile;
   fsfile.open(this->diskname.c_str(), ios::in);
  
  
   if(fsfile.good()) 
   {
       cout<<"Disk Exists" <<endl;
      
   }
   //Build a file with numberOfBocks * blockSize number of # symbols
   else 
   {
       cout<<"Creating Disk: "<<diskname<<" "<<endl;
       fsfile.open(diskname.c_str(), ios::out);
       for(int i=0; i<this->numberofblocks*this->blocksize; i++)
       {  
            fsfile.put('#');
       }
             
   }
   return;
         
  

}
//Retreives block blockNumber from the disk and stores the data in the string buffer.

int Sdisk::getblock(int blocknumber, string& buffer)
{
    buffer.clear();
    if(blocknumber >=0 and blocknumber < numberofblocks-1) // check the condition for block number
    {
       fstream iofile;
       //Open the file - file must be c string
       iofile.open(diskname.c_str(),ios::in);
       iofile.seekg(blocknumber*blocksize); // set the position and Find the block to read innd
       
       //Get blockSize number of characters in block, push to buffer
       for(int i=0; i<blocksize; i++)
       {
             buffer +=iofile.get();    // add values to buffer
             
       }
       iofile.close(); //Close the file
       return 1;
       

    }   
    else
    {
         return 0;
     }
    
}

//Writes the string buffer to block blockNumber.
int Sdisk::putblock(int blocknumber, string buffer)
{
     
     if(blocknumber >=0 and  blocknumber < this->numberofblocks-1) // check the condition for block number
     {
       fstream iofile; //File stream object
       iofile.open(diskname.c_str(), ios::in|ios::out); //Open the file - file must be c string
       iofile.seekp(blocknumber*blocksize);    // set the position
       for(int i=0; i<buffer.length() and i<blocksize; i++)
       {
           iofile.put(buffer[i]); // put value in buffer[i] to file
       }
       iofile.close();
       return 1;
     } 
     else
     {
        return 0;
     }
     
     
}  

int Sdisk::getnumberofblocks()
{
    return this->numberofblocks; //get value for number of blocks
}


int Sdisk::getblocksize()
{
    return this->blocksize;      //get value for blocksize
}

