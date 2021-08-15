#include <iostream>
#include "FileSystem.h"
#include "Interface.h"
#include "Server.h"
#include <string>
#include <fstream>
#include <vector>
#include <utility> 
#include <stdexcept> 
#include <sstream> 
#include <variant>
using namespace std;

//Funcion que recibe un string y devuelve un vector de los tokens, o sea cada caracter.
vector<string> Tokenizer(const string &line)
{
    //Vector float donde se guardaran los tokens.
    vector<string> tokens;
    stringstream check1(line);
    string intermediate; 
    while(getline(check1, intermediate, ',')) 
    { 
        tokens.push_back(intermediate); 
    } 
    return tokens;
}

//Metodo que lee el padron pero solo lo crea como un vector de vectores, no lo carga al Filesystem. 
vector<pair<string, vector<string>>> read_csv(string filename){
   
    // Crear un vector de <string, string vector> pares para guardar el resultado.
    vector<pair<string, vector<string>>> result;

    // Crear un filestream input.
    ifstream myFile(filename);

    // Asegurarse que el archivo este abierto. 
    if(!myFile.is_open()) throw runtime_error("Could not open file");

    // Variables
    string line, colname, val;

    //Leer los nombres de las columnas.
    if(myFile.good())
    {
        // Extraer la primera linea en el archivo. 
        getline(myFile, line);

        // Crear un stringtime desde la linea.
        stringstream ss(line);

        // Extraer el nombre de cada columna.
        while(getline(ss, colname, ',')){
            // Anadir el par al resultado.
            result.push_back({colname, vector<string> {}});
        }
    }

    // Leer los datos, linea por linea.
    while(getline(myFile, line))
    {
        // Llevar el conteo del indice de la columna actual. 
        int colIdx = 0;

        while(colIdx <= 5){
            // Crea un vector de tokens para separarlo por la coma.
            vector<string> tokens = Tokenizer(line);
            // Empuja el valor al resultado. 
            result.at(colIdx).second.push_back(tokens.at(colIdx));
            
            // Incrementar el indice columna. 
            colIdx++;
        }
    }
    // Cerrar el archivo.
    myFile.close();
    return result;
}

//Metodo para buscar por cedula, el bool show es para mostrar los datos encontrados, devuelve la posicion. 
int search_Cedula(vector<pair<string, vector<string>>> csv, int cedula, bool show){
    int position;
    for(long unsigned int i = 0; i < csv.at(0).second.size(); i++){
        if(csv.at(0).second.at(i) == to_string(cedula)){
            if(show == true){
            cout << " Cedula " << cedula << " Encontrada " << endl;
            cout << " Codelec = " << csv.at(1).second.at(i);
            cout << " Vencimiento = " << csv.at(2).second.at(i);
            cout << " Nombre = " << csv.at(3).second.at(i);
            cout << " " << csv.at(4).second.at(i);
            cout << " " << csv.at(5).second.at(i) << endl;
            }
            position = i;
        };
    }
    return position;
}

//Metodo para buscar por codelec, el bool show es para mostrar los datos encontrados, devuelve la posicion. 
int search_Codelec(vector<pair<string, vector<string>>> csv, int codelec, bool show){
    int position;
    for(long unsigned int i = 0; i < csv.at(0).second.size(); i++){
        if(csv.at(1).second.at(i) == to_string(codelec)){
            if(show == true){
            cout << " Cedula = " << csv.at(0).second.at(i);
            cout << " Vencimiento = " << csv.at(2).second.at(i);
            cout << " Nombre = " << csv.at(3).second.at(i);
            cout << " " << csv.at(4).second.at(i);
            cout << " " << csv.at(5).second.at(i) << endl;
            }
            position = i;
        };
    }
    return position;
}

//Metodo para cargar el csv como archivo al Filesystem.
void csvLoad(vector<pair<string, vector<string>>> csv, FileSystem* fs, User* DBA){
    // Crear un filestream input.
    ifstream myFile("Padron_Prueba.csv");
    string total = " ";
    string line; 
    //Leer cada linea y adjuntarla al total. 
    while(getline(myFile, line)){
        total = total + line + "\n";
    }
    //Crear archivo
    fs -> create(DBA, "PruebaCSV");
    //Abrir archivo.
    int fcsv = fs -> openFile(DBA, "PruebaCSV", "w");
    //Escribir archivo con el padron.
    fs -> writeFile(DBA, fcsv, "PruebaCSV", total);
    //Leer archivo.
    fs -> readFile(DBA, "PruebaCSV");
}

int main() {
  // Interface interface;
  //Server* server = new Server();
  FileSystem* fs = new FileSystem();

  User* DBA = new User("sudo", true, true, true, true);
  /**
  //Create a file
  fs -> create(DBA, "Prueba0");
  fs -> create(DBA, "Prueba1");
  
  //Message received from client
  //server -> run();
  //string message = server -> getMessage();

  string message = "Datos de prueba para el segundo archivo";

  //Open a file
  int fd = fs -> openFile(DBA, "Prueba0", "w");
  int fd1 = fs -> openFile(DBA, "Prueba1", "w");
  //Write a file
  fs -> writeFile(DBA, fd, "Prueba0", "Datos de prueba para el archivo prueba0");
  //Check the FAT table
  fs -> writeFile(DBA, fd1, "Prueba1", message);
  
  //Close File A
  fs -> closeFile(DBA, "Prueba1");
  
  //Append a file
  fs -> append(DBA, fd, "Prueba0", " Informacion para la siguiente parte que corresponde al metodo de append");
  fs -> memoryStatus();
  //Close File B
  fs -> closeFile(DBA, "Prueba0");
  //Open files
  fs -> openFile(DBA, "Prueba0", "r");
  fd1 =  fs -> openFile(DBA, "Prueba1", "r");
  //Read a file
  fs -> readFile(DBA, "Prueba0");
  fs -> readFile(DBA, "Prueba1");
  
  // View hard drive
  fs -> HDcontents();

  cout << "\n\n<<<<<PRIOR DELETE>>>>>" << endl;

  //Check the tables
  fs -> memoryStatus();

  cout << "<<<<<PRIOR DELETE>>>>>\n\n" << endl;

  //Delete a file
  fs -> deleteFile(DBA, "Prueba0");
  //Reopen the file
  fd = fs -> openFile(DBA, "Prueba1", "r");

  //Re read the existing file to make sure it wasn't damaged
  fs -> readFile(DBA, "Prueba1");

  // Close file
  fd = fs -> closeFile(DBA, "Prueba1");
  
  cout << "<<<<<POS DELETE>>>>>" << endl;

  //Check the tables
  fs -> memoryStatus();

  cout << "<<<<<POS DELETE>>>>>" << endl;
  
  fs -> saveState();
   **/

  //Cargar el csv.
  csvLoad(prueba, fs, DBA);

   /**
    //Toda esta parte es el ejemplo de cargar el csv pero no al filesystem. 
    vector<pair<string, vector<string>>> prueba = read_csv("Padron_Prueba.csv");
    cout << "** Mostrar las primeras 10 entradas" << endl;
for(int i = 0; i<10; i++){
    cout << i << ". Cedula = " << prueba.at(0).second.at(i);
    cout << " Codelec = " << prueba.at(1).second.at(i);
    cout << " Vencimiento = " << prueba.at(2).second.at(i);
    cout << " Nombre = " << prueba.at(3).second.at(i);
    cout << " " << prueba.at(4).second.at(i);
    cout << " " << prueba.at(5).second.at(i) << endl;
    }
    cout << " " << endl;
    cout << "Tamano del padron = " << prueba.at(0).second.size() + 1 << endl;
    cout << "**Buscar cedula 101031136." << endl;
    cout << "Posicion = " << search_Cedula(prueba, 101031136, false) << endl;
    cout << "**Buscar codelec 301027." << endl;
    search_Codelec(prueba, 301027, true);
    **/
}