#pragma once
#include <iostream>
#include <string>
#include "Unit.h"
#include <vector>

using namespace std;
/**
 * @brief Estructura del Usuario.
 * 
 */
struct User {
  string username; //Nombre del usuario.
  bool permissions[4]; //Permisos para manejo de archivos.
  User(string name, bool p1, bool p2, bool p3, bool p4){
    this -> username = name;
    this -> permissions[0] = p1;
    this -> permissions[1] = p2;
    this -> permissions[2] = p3;
    this -> permissions[3] = p4;
  }
};

/**
 * @brief Clase FileSystem, es la clase que maneja el sistema de archivos.
 * 
 */
class FileSystem {
private:
  
  Unit* memory; //La memoria del sistema.


public:

  FileSystem();
  bool create(User* user, string fileName);
  int openFile(User* user, string fileName, string flag);
  bool closeFile(User* user, string fileName);
  bool writeFile(User* user, string fileName, string data);
  bool append(User* user, string FileName, string data);
  bool overwriteData(User* user, string fileName, int startingPos, int finalPos, string newData);
  string readFile(User* user, string fileName);
  bool deleteFile(User* user, string FileName);
  bool checkSpace(int spaceNeeded);
  string getFileContent(User* user, string fileName);
  string getData(User* user, string fileName);
  void saveState();
  void listFiles();
  void memoryStatus();
  void HDcontents();
};