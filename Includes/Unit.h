#pragma once
#include <iostream>
#include <string.h>
#include <cmath>
#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <fstream>

using namespace std;

/**
 *Representa cada bloque de la tabla FAT. 
*/

struct FAT_Block {

  int status;
  int next; //Del archivo
  FAT_Block(int status, int next){
    this -> status = status;
    this -> next = next;
  }
  
};
/**
 * Representa la tabla FAT.
*/
struct FAT {

  FAT_Block* dataBlock;
  FAT* next;
  FAT(FAT_Block* dataBlock, FAT* next){
    this -> dataBlock = dataBlock;
    this -> next = next;
  }

};
/**
 * Representa cada bloque del directorio.
*/
struct FAT_Directory_Block {
  
  string fileName;
  int startingPos;
  string metaData;
  FAT_Directory_Block(string fileName, int initBlock, string metaData){
    this -> fileName = fileName;
    this -> startingPos = initBlock;
    this -> metaData = metaData;
  }

};
/**
 * Representa el directorio.
*/
struct FAT_Directory {

  FAT_Directory_Block* dataDirectoryBlock;
  FAT_Directory* next;
  FAT_Directory(FAT_Directory_Block* dataDirectoryBlock, FAT_Directory* next){
    this -> dataDirectoryBlock = dataDirectoryBlock;
    this -> next = next;
  }

};


class Unit {
private:

  FAT_Directory* root_FAT_Directory;
  FAT* root_FAT;
  char* hd;
  int createdBlocks;
  int freeBlocks;
  int filesCreated;

public:

  Unit();
  void initUnit(int blocks);
  int getSpace();
  bool memoryAvailable(int size);
  bool saveData(FAT_Directory* startingBlock, char* data, int size);
  bool saveNewFile(string fileName, string metaData);
  bool overwrite(int startingPos, int endingPos, string newData);
  string readMemory(int FAT_Directory_Block_Index);
  bool appendData(int FAT_Directory_Index, int lastBlock, char* data, int size);
  int getLastFileBlock(int FAT_Directory_Block_Index);
  bool deletFile(string fileName);
  bool deletFAT(int index);
  void listFiles();
  void saveFiles();
  void viewHD();
  void viewFAT();

  //Manejo de la metaData
  char getFileFlag(string metaData);
  int getFileSize(string metaData);
  string getPermissions(string metaData);
  void setOpenFlag(FAT_Directory_Block* directory_block, char status);

  string changeSize(string metaData, int newSize);


  FAT* getFatBlockByIndex(int index);
  FAT_Directory* getFatDirectoryBlockByIndex(int index);

  FAT_Directory* searchFile(string fileName);
  int searchFileIndex(string fileName);

  
  //TEST METHODS
  void FAT_TEST();
  void checkStatus();
  
};