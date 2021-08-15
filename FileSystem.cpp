 #include "Includes/FileSystem.h"
/**
 * @brief Contructor un nuevo File System :: Objeto File System
 */
FileSystem::FileSystem(){
  memory = new Unit();
}

//Arreglo de numeros int[] miArreglo = [1,2,3,4,5,6,7]

/**
 * @brief Método que crea un nuevo archivo en la memoria.
 * @param user Usuario que creara el archivo.
 * @param fileName Nombre del archivo a crear.
 * @param metaData Metadata del archivo.
 * @return true si se puede/se pudo crear.
 * @return false si no se puede o no se pudo crear.
 */
bool FileSystem::create(User *user, string fileName){
  bool ready = false;

  if(memory -> searchFile(fileName) == nullptr){
    //Verificar permisos
    if(user -> permissions[0] == true){
    
      string metaData = "0," + to_string(user -> permissions[0]) + to_string(user -> permissions[1]) + to_string(user -> permissions[2]) + ",0";
      //Orden para crear el archivo en el directorio
      memory -> saveNewFile(fileName, metaData);
    } else {
      cout << " You do not have the user permission to create a file. " << endl;
    }
  }else{
    cout << "Can't create file, it already exists..." << endl;
  }
  return ready;
}

/**
 * @brief Método que abre un archivo ya existente.
 * @param user Usuario que abrirar el archivo.
 * @param fileName Nombre del archivo a abrir.
 * @param flag Flag que indica de que manera se va a abrir el archivo, w para write y r para read.
 * @return int Posicion del bloque del archivo abierto.
 */
int FileSystem::openFile(User* user, string fileName, string flag){
  FAT_Directory* block_index;
  int opening_block = -1;
  char already_open;
  //Verificacion de permisos al abrir un archivo.
  if((user -> permissions[1] == true && flag == "w") || (user -> permissions[2] == true && flag == "r")){
    // Buscar el archivo en el directorio FAT.
    block_index = memory -> searchFile(fileName);

    if(block_index == nullptr){
      cout << "The file " << fileName << " does not exist" << endl;
    }
    else{
      
      already_open = memory -> getFileFlag(block_index -> dataDirectoryBlock -> metaData);
      //Chequear la metadata para la bandera "Ya abierta", si es 1, significa qué
      //ese archivo ya se abrio y no puede ser usado en este momento.
      if (already_open == '1'){
        cout << "Access denied. The file " << fileName << " is open at the moment. Please try again later" << endl;
      }
      else{
        
        memory -> setOpenFlag(block_index -> dataDirectoryBlock, '1');
      }
    }
    opening_block = block_index -> dataDirectoryBlock -> startingPos;
  } else{
    cout << "Access denied" << endl;
  }

  return opening_block;
}

/**
 * @brief Método para cerrar un archivo abierto. 
 * @param user Usuario que cerrara el archivo.
 * @param fileName Nombre del archivo a cerrar.
 * @return true 
 * @return false 
 */
bool FileSystem::closeFile(User* user, string fileName){
  FAT_Directory* block_index;
  bool closed = false;
  
  block_index = memory -> searchFile(fileName);
  
  //Cambiar la bandera "Ya abierto" en el metadata. 
  memory -> setOpenFlag(block_index -> dataDirectoryBlock, '0');

  return closed;
}

/**
 * @brief Método que escribe en un archivo creado en la memoria.
 * @param user Usuario que escribira en el archivo.
 * @param fd Bloque inicial.
 * @param fileName Nombre del archivo a escribir.
 * @param data Datos que se escribiran.
 * @return true si se puede/se pudo escribir.
 * @return false si no se puede o no se pudo escribir.
 */
bool FileSystem::writeFile(User* user, string fileName, string data){
  bool wrote = false;
  char* dataPtr = &data[0];
  //Verificar permiso de usuario.
  if(user -> permissions[1] == true){
    FAT_Directory* FD_Aux = memory -> searchFile(fileName);
    string metaData = FD_Aux -> dataDirectoryBlock -> metaData;
    int flag = memory -> getFileFlag(metaData);
   
    if(flag == 0)
      return false;

    char* dataPtr = &data[0];
    if(memory -> memoryAvailable(strlen(dataPtr))){
      memory -> deletFAT(FD_Aux -> dataDirectoryBlock -> startingPos);
      wrote = memory -> saveData(FD_Aux, dataPtr, strlen(dataPtr));
    }
  } else{
      cout << " You do not have the user permission to write to the file. " << endl;
  }
  return wrote;

}

/**
 * @brief Método que adjunta datos a un archivo ya existente.
 * @param user Usuario que adjuntara al archivo.
 * @param startingPos Bloque inicial.
 * @param fileName Nombre del archivo a adjuntar.
 * @param data Datos a adjuntar.
 * @return true si se puede/se pudo adjuntar.
 * @return false si no se puede o no se pudo adjuntar.
 */
bool FileSystem::append(User* user, string fileName, string data){
  if(!user -> permissions[0] && !user -> permissions[2])
    return false;
  bool wrote = false;
  char* dataPtr = &data[0];
  int FAT_Index = memory -> searchFileIndex(fileName);
  int lastBlock = memory -> getLastFileBlock(FAT_Index);
  wrote = memory -> appendData(FAT_Index, lastBlock, dataPtr, strlen(dataPtr));
  return wrote;
}

bool FileSystem::overwriteData(User* user, string fileName, int startingPos, int endingPos, string newData){
  bool dataModified = false;
  if(user -> permissions[1]){
    FAT_Directory* FD_Aux = memory -> searchFile(fileName);
    if(FD_Aux != nullptr){
      memory -> overwrite(startingPos, endingPos, newData);
    }else{
      cout << "No \"" << fileName << "\" file was found" << endl;
    }
  }else{
    cout << "User: " << user -> username << " access denied" << endl;
  }
  return dataModified;
}

/**
 * @brief Método que lee un archivo ya existente.
 * @param user Usuario que leera el archivo.
 * @param fileName Nombre del archivo a leer.
 * @return true si se puede/se pudo leer.
 * @return false si no se puede o no se pudo leer.
 */
string FileSystem::readFile(User* user, string fileName){
  string data;

  //Verificación de permiso de usuario
  if(user -> permissions[2] == true){
    int FDB_Index = memory -> searchFileIndex(fileName);
    data = memory -> readMemory(FDB_Index);
    cout << "\n-----------------------\nDATA\n-----------------------\n";
    cout << data << endl;
    cout << "\n-----------------------\nDATA\n-----------------------\n";
  } else{
      cout << " You do not have the user permission to read this file. " << endl;
  }
  return data;
}

/**
 * @brief Método que borra un archivo ya existente.
 * 
 * @param user Usuario que borrara el archivo.
 * @param fileName Nombre del archivo a borrar.
 * @return true si se puede/se pudo borrar.
 * @return false si no se puede o no se pudo borrar.
 */
bool FileSystem::deleteFile(User* user, string fileName){
  bool delit = false;
  //Verificar permisos

  if(!user -> permissions[0]){
    return false;

  }

  //Orden para borrar el archivo en el directorio
  memory -> deletFile(fileName);
  return delit;
}

bool FileSystem::checkSpace(int spaceNeeded){
  bool space;
  space = memory -> memoryAvailable(spaceNeeded);
  return space;
}

string FileSystem::getFileContent(User* user, string fileName){
  string data;

  //Verificación de permiso de usuario
  if(user -> permissions[2] == true){
    int FDB_Index = memory -> searchFileIndex(fileName);
    data = memory -> readMemory(FDB_Index);
  } else{
      cout << " You do not have the user permission to read this file. " << endl;
  }
  return data;
}



/**
 * @brief Este método imprime el directorio de archivos 
 */
void FileSystem::listFiles(){
  memory -> listFiles();
}



/**
 * @brief Este método guarda el directorio de archivos 
 */
void FileSystem::saveState(){
  memory -> saveFiles();
}


/**
 * @brief Método que muestra el estado de la memoria, basicamente muestra la tabla FAT 
 * y la tabla de directorios FAT.
 */
void FileSystem::memoryStatus(){
  memory -> checkStatus();
}

/**
 * @brief Method to view the contents of the hard drive
 */
void FileSystem::HDcontents(){
  memory -> viewHD();
}


/**
 * @brief Método que lee un archivo ya existente y retorna sus datos.
 * @param user Usuario que leera el archivo.
 * @param fileName Nombre del archivo a leer.
 * @return los datos leidos.
 */
string FileSystem::getData(User* user, string fileName){
  bool read = false;
  string data = "";

  //Verificación de permiso de usuario
  if(user -> permissions[2] == true){
    int FDB_Index = memory -> searchFileIndex(fileName);
    data = memory -> readMemory(FDB_Index);
  } else{
      cout << " You do not have the user permission to read this file. " << endl;
  }
  return data;
}