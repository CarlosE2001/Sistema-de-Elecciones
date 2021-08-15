#include "Includes/Unit.h"

const int BLOCK_SIZE = 100;
const int BLOCKS = 5500;

/**
*@brief Clase Unit contiene un bloque de caracteres de memoria virtual, utiliza un
	sistema de almacenamiento para manejar archivos y directorios.
*/
Unit::Unit(){

	hd = new char[BLOCK_SIZE*BLOCKS];
	Unit::initUnit(BLOCKS);
	//init int que me pasan
	//mientras el int sea menor que intInicial + BLOCK_SIZE
}

/**
*@brief Método para inicializar la tabla FAT.
*@param blocks: Número de bloques FAT ubicados en la memoria virtual.
*@return -
*/
void Unit::initUnit(int blocks){

	//Update of the private variables
	createdBlocks = blocks;
	freeBlocks = blocks;  
	root_FAT = new FAT(new FAT_Block(0,-1), nullptr); //Init of the FAT
	root_FAT_Directory = nullptr;

	FAT* FAT_Aux = root_FAT; //FAT iterator

	blocks--;
	while(blocks > 0){
		FAT_Aux -> next = new FAT(new FAT_Block(0,-1), nullptr);
		FAT_Aux = FAT_Aux -> next;
		blocks--;
	}

}

/**
*@brief Método para guardar datos en la memoria virtual y actualizar la tabla FAT
	si es necesario.
*@param startingBlock: El índice del archivo en el directorio FAT.
*@param data: Dato que va hacer escrito.
*@param size: El tamaño del número de caracteres que se van a escribir.
*@return Variable tipo bool para una escritura existosa.
*/
bool Unit::saveData(FAT_Directory* startingBlock, char* data, int size){
	//Se encarga de crear los directorios en la tabla fat
	bool saved = false;
	int charIndex = 0;
	
	FAT* FB_Aux = getFatBlockByIndex(startingBlock -> dataDirectoryBlock -> startingPos);
	int startingPos = startingBlock -> dataDirectoryBlock -> startingPos;
	int blocksNeeded = ceil((double) size / BLOCK_SIZE);

	for(int i = 0; i < BLOCK_SIZE && charIndex < size; i++){
		hd[startingPos * BLOCK_SIZE + i] = data[charIndex];
		charIndex++;
	}

	blocksNeeded--;
	while(blocksNeeded > 0){
		startingPos = getSpace();
		FB_Aux -> dataBlock -> next = startingPos;
		FB_Aux = getFatBlockByIndex(startingPos);
		if(startingPos != -1){
			for(int i = 0; i < BLOCK_SIZE; i++){
				hd[startingPos * BLOCK_SIZE + i] = data[charIndex];
				charIndex++;
				
			}
		}
		blocksNeeded--;
	}
	
	startingBlock -> dataDirectoryBlock -> metaData = changeSize(startingBlock -> dataDirectoryBlock -> metaData, size);

	return true;
}


bool Unit::overwrite(int startingPos, int endingPos, string newData){
	bool ready = false;
	for(int i = startingPos; i < endingPos; i++){
		hd[i] = newData[i - startingPos];
	}
	ready = true;
	return ready;
}

/**
*@brief Método para escribir al final del archivo.
*@param FAT_Directory_Index: Indice del archivo en el directorio.
*@param lastBlock: Último bloque perteneciente a un archivo.
*@param data: Dato que se va escribir en el archivo.
*@param size: El tamaño del número de caracteres que se van a escribir.
*@return Variable tipo bool para saber si la escritura fue exitosa.
*/
bool Unit::appendData(int FAT_Directory_Index, int lastBlock, char* data, int size){
	FAT_Directory* FD_Aux = getFatDirectoryBlockByIndex(FAT_Directory_Index);
	FAT* FB_Aux = getFatBlockByIndex(lastBlock);
	string metaData = FD_Aux -> dataDirectoryBlock -> metaData;
	int currentSize = getFileSize(metaData);
	int lastChar = currentSize % BLOCK_SIZE;
	int charsWrote = 0;

	int blocksNeeded = ceil((double) (size - (BLOCK_SIZE -lastChar)) / BLOCK_SIZE); //Posible error en el calculo de bloques
	for(int i = 0; i < BLOCK_SIZE - lastChar && i < strlen(data); i++){
		hd[lastBlock*BLOCK_SIZE + lastChar + i] = data[i];
		charsWrote++;
	}

	int startingPos;
	while(blocksNeeded > 0){
		startingPos = getSpace();
		FB_Aux -> dataBlock -> next = startingPos;
		FB_Aux = getFatBlockByIndex(startingPos);
		if(startingPos != -1){
			for(int i = 0; i < BLOCK_SIZE; i++){
				hd[startingPos * BLOCK_SIZE + i] = data[charsWrote];
				charsWrote++;
			}
		}
		blocksNeeded--;
	}

	FD_Aux -> dataDirectoryBlock -> metaData = changeSize(metaData, currentSize + size);

	return false;
}

/**
*@brief Método para obtener la bandera de un archivo. 
*@param metaData: metaData de un archivo.
*@return Retorna la bandera del archivo.	 
*/
char Unit::getFileFlag(string metaData){
	return metaData[0];
}

/**
*@brief Método para obtener el tamaño de un archivo.
*@param metaData: metaData de un archivo.
*@return Retorna el tamaño de un archivo.	
*/
int Unit::getFileSize(string metaData){
	string currentSize = "";
	char* meta = &metaData[0];
	
	for(int i = 6; i <= strlen(meta); i++){
		currentSize += metaData[i];
	} 
	
	return atoi(currentSize.c_str());
}

/**
*@brief Método para obtener los permisos de usuario.
*@param metaData: metaData de un archivo.
*@return Retorna los permisos de usuario.	 
*/
string Unit::getPermissions(string metaData){
	string permissions;
	for(int i = 2; i < 5 ; i++){
		permissions += metaData[i];
	}
	return permissions;
}

/**
*@brief Método para modificar el tamaño de un archivo, modificando la metadata.
*@param metaData: metaData de un archivo.
*@return Retorna la nueva metadata del archivo con el tamaño nuevo.
*/
string Unit::changeSize(string metaData, int size){
	string newMetaData = "";
	for(int i = 0; i < 6; i++){
		newMetaData += metaData[i];
	}
	newMetaData += to_string(size);
	return newMetaData;
}

/**
*@brief Método para obtener el indice de un bloque vacio en la tabla FAT.
*@return Retorna el indice del bloque de la tabla FAT.
*/
int Unit::getSpace(){
	
	FAT* F_Aux = root_FAT;
	int fatIndex = 0;
	while(F_Aux -> next != nullptr && F_Aux -> dataBlock -> status != 0){
		F_Aux = F_Aux -> next;
		fatIndex++;
	}

	if(F_Aux -> dataBlock -> status == 0){
		F_Aux -> dataBlock -> status = 1;
		return fatIndex;
	}else{
		return -1;
	}

}

/**
 * @brief Método retorna si hay espacio suficiente para escribir 
 * @param size tamaño del archivo
 * @return true suficiente espacio
 * @return false espacio insuficiente
 */
bool Unit::memoryAvailable(int size){
	FAT* F_Aux = root_FAT;
	int blocksNeeded = ceil((double) size / BLOCK_SIZE);
	int freeBlocks = 0;
	for(int i = 0; i < createdBlocks && freeBlocks < blocksNeeded; i++){
		if(F_Aux -> dataBlock -> status == 0){
			freeBlocks++;
		}
		F_Aux = F_Aux -> next;
	}
	if(freeBlocks == blocksNeeded){
		return true;
	}else{
		return false;
	}
}

/**
*@brief Método para guardar un nuevo archivo.
*@param filename: nombre del nuevo archivo.
*@param metaData: metadata del nuevo archivo.
*@return Variable tipo bool que indica si se guardó el archivo correctamente.
*/
bool Unit::saveNewFile(string fileName, string metaData){

	bool saved = false;

	FAT_Directory_Block* FDB = new FAT_Directory_Block(fileName, -1, metaData);

	FAT_Directory* FD_Aux = root_FAT_Directory;
	if(FD_Aux == nullptr){

		root_FAT_Directory = new FAT_Directory(FDB, nullptr);
		saved = true;

	}else{

		while(FD_Aux -> next != nullptr){

			FD_Aux = FD_Aux -> next;

		}

		FD_Aux -> next = new FAT_Directory(FDB, nullptr);
		saved = true;

	}


	int nextEmptyPos = 0;
	if(root_FAT -> dataBlock -> status == 0){

		FDB -> startingPos = nextEmptyPos;
		root_FAT -> dataBlock -> status = 1;

	}else{

		FAT* F_Aux = root_FAT -> next;
		nextEmptyPos += BLOCK_SIZE;
		while(F_Aux -> next != nullptr && F_Aux -> dataBlock -> status == 1){
			
			F_Aux = F_Aux -> next;
			nextEmptyPos += BLOCK_SIZE;

		}

		if(F_Aux -> dataBlock -> status != 1){

			FDB -> startingPos = nextEmptyPos / BLOCK_SIZE;
			F_Aux -> dataBlock -> status = 1;

		}else {

			F_Aux -> next = new FAT(new FAT_Block(0,-1), nullptr);
			FDB -> startingPos = (nextEmptyPos + BLOCK_SIZE) / BLOCK_SIZE;
			F_Aux -> next -> dataBlock -> status = 1;
			createdBlocks++;

		}

	}

	return saved;

}

/**
*@brief Método para leer un archivo.
*@param FAT_Directory_Block_Index: Indice del directorio del archivo a leer.
*@return Varible tipo string con los caracteres del archivo a leído.
*/
string Unit::readMemory(int FAT_Directory_Block_Index){

	string stream = "";
	int charsRead = 0;

	FAT_Directory* blockToRead = getFatDirectoryBlockByIndex(FAT_Directory_Block_Index);

	string fileName = blockToRead -> dataDirectoryBlock -> fileName;
	int startPos = blockToRead -> dataDirectoryBlock -> startingPos;
	string metaData = blockToRead -> dataDirectoryBlock -> metaData;
	int fileSize = getFileSize(metaData); //Buscar la forma de separar el string para obtener el size


	FAT* F_Aux = getFatBlockByIndex(startPos);
	do
	{
		int nextStartingPos = F_Aux -> dataBlock -> next;
		if(nextStartingPos != -1){
			F_Aux = getFatBlockByIndex(nextStartingPos);
		}

		for(int i = startPos*BLOCK_SIZE; i < startPos*BLOCK_SIZE + BLOCK_SIZE && charsRead < fileSize; i++){
			stream += hd[i];
			charsRead++;
		}
		startPos = nextStartingPos;
	} while (startPos != -1);
	
	return stream;
}

/** 
 * @brief Método para recuperar el Fat Block según el index.
 * @param index: Indice para buscar el Fat Block de la lista Fat.
 * @return Bloque Fat de la posicion segun el index. 
*/
FAT* Unit::getFatBlockByIndex(int index){
	FAT* F_Aux = root_FAT;
	for(int i = 0; i < index; i++){
		F_Aux = F_Aux -> next;
	}
	return F_Aux; //Si pasó si... voy
}

/**
 * @brief Metodo para recuperar el Fat Directory Block de la posicion según el Index.
 * @param index: Indice para buscar el Fat Directory Block de la lista Fat Directory.
 * @return Bloque Fat Directory de la posicion segun el index.
*/
FAT_Directory* Unit::getFatDirectoryBlockByIndex(int index){
	FAT_Directory* FD_Aux = root_FAT_Directory;
	for(int i = 0; i < index; i++){
		FD_Aux = FD_Aux -> next;
	}
	return FD_Aux;
}

/**
 * @brief Metodo para recuperar el ultimo bloque del archivo.
 * @param FAT_Directory_Index: indice del Fat Directory que se busca.
 * @return el ultimo bloque del archivo.
 */
int Unit::getLastFileBlock(int FAT_Directory_Index){
	FAT_Directory* FD_Aux = getFatDirectoryBlockByIndex(FAT_Directory_Index);
	int start = FD_Aux -> dataDirectoryBlock -> startingPos;
	FAT* FB_Aux = getFatBlockByIndex(start);
	int lastBlock = start;
	while(FB_Aux -> dataBlock -> next != -1){
		lastBlock = FB_Aux -> dataBlock -> next;
		FB_Aux = getFatBlockByIndex(FB_Aux -> dataBlock -> next);
	}
	return lastBlock; 
}

/**
*@brief Método para eliminar un archivo.
*@param fileName: Indica el nombre del archivo a eliminar.
*@return varieble para verificar si se borró el archivo con éxito.
*/
bool Unit::deletFile(string fileName){

	//Bool para verificar si se puede/se pudo borrar.
	bool delet = false;
	bool first = false;
	//Root del FAT Directory.
	FAT_Directory* FD_Aux = root_FAT_Directory;
	
	//Verificar si esta vacia, si esta vacia, entonces no hay nada que se pueda borrar.
	if(FD_Aux == nullptr){
		cout << "No existen archivos." << endl;

	}else{
		//Verificar si es el primero de la lista. Caso especial.
		if(FD_Aux -> dataDirectoryBlock -> fileName == fileName){
			root_FAT_Directory = FD_Aux -> next;
			//delete FD_Aux;
			delet = true;
			first = true;
		}else{
			//Moverse atraves del FAT Directory.
			//Si llego a un nullptr o ya se encontro, significa que ya no queda que buscar mas.
			while(FD_Aux -> next != nullptr && delet == false){
				//Verificar si es el archivo que estoy buscando.
				if(FD_Aux -> next -> dataDirectoryBlock -> fileName == fileName){
				//Si es, toca borrarlo.
				delet = true;
				}else{
				//Si no es el que estoy buscando, pasar al siguiente. 
				FD_Aux = FD_Aux -> next;
				}
			}
		}
	}
	
	if (delet == true){
		
		//Asignar el index a la posicion inicial del archivo.
		int index;
		if(first == true){
			index = FD_Aux -> dataDirectoryBlock -> startingPos;
		}else{
			index = FD_Aux -> next -> dataDirectoryBlock -> startingPos;
			
		}
		FAT* F_Aux;
		while(index != -1){
			//Root de la tabla FAT.
			F_Aux = root_FAT;
			for (int i = 0; i < index; i++){
				//Pasar al siguiente.
				F_Aux = F_Aux -> next;
			}
			index = F_Aux -> dataBlock -> next; 
			F_Aux -> dataBlock -> status = 0; //Cambiar el status a 0.
			F_Aux -> dataBlock -> next = -1; //Cambiar el siguiente a -1
		}
		//Fat directory
		if (first == false){
			FAT_Directory* FD_Delet = FD_Aux -> next; //El FD que se va a borrar.
			FD_Aux -> next = FD_Aux -> next -> next;//Cambiar el siguiente al mas mas siguiente, o sea, saltarse el que se va a borrar.
			delete FD_Delet; //Borrar el que se iba a borrar.
		}
	}
	return delet;
}

/**
*@brief Método para eliminar solo los bloques fat de un archivo.
*@param index: indica el indice donde empieza el bloque fat que se quiere eliminar.
*@return variable para verificar si se borraron los bloques con exito.
*/
bool Unit::deletFAT(int index){
	FAT* F_Aux;
	int nextIndex = index;
	while(nextIndex != -1){
		F_Aux = getFatBlockByIndex(nextIndex);
		nextIndex = F_Aux -> dataBlock -> next;
		F_Aux -> dataBlock -> next = -1;
		F_Aux -> dataBlock -> status = 0;
	}
	getFatBlockByIndex(index) -> dataBlock -> status = 1; //Primer bloque siempre tiene que aparecer reservado
	return true;
}

/**
*@brief Método para mostrar las tablas FAT y el directorio FAT.
*/
void Unit::checkStatus(){
	listFiles();
	viewFAT();
}

/**
 * @brief Método para ver el estado de la tabla FAT
 */
void Unit::viewFAT(){
	cout << "---------------\n" << "Status de la Tabla FAT" << endl;
	FAT* F_Aux = root_FAT;
	if(F_Aux == nullptr){
		cout << "Tabla FAT vacía" << endl;
	}else{
		int count = 0;
		cout << "Index: " << count << "\tPos: " << count * BLOCK_SIZE << "\t-" << "[" << F_Aux -> dataBlock -> status << " | " << F_Aux -> dataBlock -> next << "]" << endl;
		while(F_Aux -> next != nullptr){

			F_Aux = F_Aux -> next;
			count++;
			cout << "Index: " << count << "\tPos:" << count * BLOCK_SIZE << "\t-" << "[" << F_Aux -> dataBlock -> status << " | " << F_Aux -> dataBlock -> next << "]" << endl;
		
		}
	}

	cout << "--------FIN DE STATUS-------" << endl;
}

/**
 * @brief Método para imprimir los nombres de los archivos existentes.
*/
void Unit::listFiles(){
	cout << "---------------\n" << "Status del Directorio FAT" << endl;
	FAT_Directory* FD_Aux = root_FAT_Directory;
	if(FD_Aux == nullptr){
		cout << "No hay archivos creados..." << endl;
	}else{
		cout << ">>" << FD_Aux -> dataDirectoryBlock -> fileName << " | " << FD_Aux -> dataDirectoryBlock -> startingPos << " | " << FD_Aux -> dataDirectoryBlock -> metaData << endl;
		while(FD_Aux ->next != nullptr){
			FD_Aux = FD_Aux -> next;
			cout << ">>" << FD_Aux -> dataDirectoryBlock -> fileName << " | " << FD_Aux -> dataDirectoryBlock -> startingPos << " | " << FD_Aux -> dataDirectoryBlock -> metaData << endl;
		}
	}
}

void Unit::FAT_TEST(){
	FAT* FAT_Aux = root_FAT;
	for(int i = 0; i < createdBlocks; i++){
		cout << FAT_Aux -> dataBlock -> status << " | " << FAT_Aux -> dataBlock -> next << endl;
		FAT_Aux = FAT_Aux -> next; 
	}
}

/**
 * @brief Método para imprimir los nombres de los archivos existentes.
*/
void Unit::saveFiles(){
	ofstream myfile ("savedata.txt");
  	if (myfile.is_open())
  	{
	myfile << "---------------\n" << "Status del Directorio FAT" << "\n";
	FAT_Directory* FD_Aux = root_FAT_Directory;
	if(FD_Aux == nullptr){
		myfile << "No hay archivos creados..." << "\n";
	}else{
		myfile << ">>" << FD_Aux -> dataDirectoryBlock -> fileName << " | " << FD_Aux -> dataDirectoryBlock -> startingPos << " | " << FD_Aux -> dataDirectoryBlock -> metaData << "\n";
		while(FD_Aux ->next != nullptr){
			FD_Aux = FD_Aux -> next;
			myfile << ">>" << FD_Aux -> dataDirectoryBlock -> fileName << " | " << FD_Aux -> dataDirectoryBlock -> startingPos << " | " << FD_Aux -> dataDirectoryBlock -> metaData << "\n";
		}
	}
	FAT* F_Aux = root_FAT;
	myfile << "---------------\n" << "Status de la Tabla FAT" << endl;
	if(F_Aux == nullptr){
		myfile << "Tabla FAT vacía" << "\n";
	}else{
		int count = 0;
		myfile << "Index: " << count << "\tPos: " << count * BLOCK_SIZE << "\t-" << "[" << F_Aux -> dataBlock -> status << " | " << F_Aux -> dataBlock -> next << "]" << "\n";
		while(F_Aux -> next != nullptr){

			F_Aux = F_Aux -> next;
			count++;
			myfile << "Index: " << count << "\tPos:" << count * BLOCK_SIZE << "\t-" << "[" << F_Aux -> dataBlock -> status << " | " << F_Aux -> dataBlock -> next << "]" << "\n";
		
		}
	}

	myfile << "--------FIN DE STATUS-------" << "\n";
    myfile.close();
  	}
  		else {
	cout << "Unable to open file" << endl;
    }
}


/**
*@brief Método para buscar un archivo en el directorio FAT
*@param fileName: nombre del archivo a buscar.
*@return el índice del bloque del directorio FAT que contiene el archivo.
Si no se encuentra, se devuelve -1.
*/
FAT_Directory* Unit::searchFile(string fileName){
	FAT_Directory* FD_Aux = root_FAT_Directory;
	if(FD_Aux != nullptr){
		if(FD_Aux -> dataDirectoryBlock -> fileName == fileName){
			return FD_Aux;
		}
		while(FD_Aux -> next != nullptr && FD_Aux -> dataDirectoryBlock -> fileName != fileName){
			FD_Aux = FD_Aux -> next;
		}

		if(FD_Aux != nullptr && FD_Aux -> dataDirectoryBlock -> fileName == fileName){
			return FD_Aux;
		}else{
			return nullptr;
		}
	}else{
		return nullptr;
	}
}

/**
*@brief Método para buscar el indice asociado a un archivo en el directorio.
*@param fileName: nombre del archivo a buscar.
*@return Indice asociado al archivo.
*/
int Unit::searchFileIndex(string fileName){
	FAT_Directory* FD_Aux = root_FAT_Directory;
	int index = -1;

	if(FD_Aux -> dataDirectoryBlock -> fileName == fileName){
		index = 0;
	}else{
		int count = 0;
		while(FD_Aux -> next != nullptr && FD_Aux -> dataDirectoryBlock -> fileName != fileName){
			FD_Aux = FD_Aux -> next;
			count++;
		}
		if(FD_Aux -> next == nullptr && FD_Aux -> dataDirectoryBlock -> fileName == fileName){
			index = count;
		}
	}

	return index;
}

/**
*@brief Método para colocar el estado de la bandera de un archivo.
*@param directory_block: Bloque del directorio donde se encuentra el archivo.
*@param status: Indica el estado en el que se encuentra el archivo, 0 indica que el archivo no está abierto, 1 está em escritura, 2 está abierto en modo lectura.
*@return -
*/
void Unit::setOpenFlag(FAT_Directory_Block* directory_block, char status){
	directory_block -> metaData[0] = status;
}

/**
 * @brief Method to view the contents of the hard drive
 */
void Unit::viewHD(){
	int size = BLOCK_SIZE * BLOCKS;

	cout << "-------------------\nHARD DRIVE CONTENTS\n-------------------" << endl;
	int lineCount = 0;
	for(int i = 0; i < size; i++){

		if(lineCount == 0)
			cout << "\t";
		
		if(hd[i] == 0 || hd[i] == '\0' || hd[i] == '\n' || hd[i] == '\r'){
			cout << "X";
		}
		else{
			cout << hd[i];
		}

		lineCount++;

		if(lineCount == BLOCK_SIZE){
			lineCount = 0;
			cout << endl;
		}

	}
	cout << "" << endl;
}
