#include "Generator.h"


Generator::Generator(){
    srand(time(NULL));
}

string Generator::generateCode(int votingTable){
    string code = "";
    int digits = countDigit(votingTable);
    for(int i = 0; i < 3 - digits; i++){
        code += "0";
    }
    code += to_string(votingTable) + "-";
    int count = 7;
    while(count > 0){
        if(count != 4){
            int randomType = rand() % 2;
            int randomChar;
            if(randomType == 0){ //Numeric Values
                int max = 57;
                int min = 48;
                randomChar = rand() % (max - min + 1) + min;
            }else{ //Alpha Values
                int min = 65;
                int max = 90;
                randomChar = rand() % (max - min + 1) + min;
            }
            code += char(randomChar);
        }else{
            code += "-";
        }
        count--;
    }
    //Saber los digitos
    //ASCII TABLE VALUES [ 48-57 ] [ 65-90 ]
    //APPEND
    return code;
}

int Generator::countDigit(long long n)
{
    int count = 0;
    while (n != 0)
    {
        n = n / 10;
        ++count;
    }
    return count;
}

//Generar un code 000-A0A-93O