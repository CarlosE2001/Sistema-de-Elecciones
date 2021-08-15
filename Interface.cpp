#include "Interface.h"

Interface::Interface(){
    fs = new FileSystem();
    Interface::runInterface();
}

int Interface::runInterface(){
    int opt = 0;
    string username;
    string fileName;
    string metaData;
    string flag;
    string data;
    bool deleted;
    bool write;
    bool read;
    bool all;
    int fd = -1;

    cout << "File System for Elections (FSE)\n@BugBusters 2021\n\n" << endl;
    cout << "Username >>";
    cin >> username;
    cout << "Write? (1 o 0) >>";
    cin >> write;
    cout << "Read? (1 o 0) >>";
    cin >> read;
    cout << "All? (1 o 0) >>";
    cin >> all;

    User* user = new User();
    user -> username = username;
    user -> permissions[0] = write;
    user -> permissions[1] = read;
    user -> permissions[2] = all;

    do{
        
        cout << "1.Create a file\n2.Delete a file\n3.Write on a file\n4.Append on a file\n5.Read a file\n6.List files\n7.Quit" << endl;
        cout << user -> username << "$>>";
        cin >> opt;
        
        cin>>opt;
        bool exit = true;
        while(exit)
        {
        if(cin.fail())
        {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(),’\n’);
        cout<<“You have entered wrong input”<<endl;
        cin>>opt;
        }
        if(!cin.fail())
        exit = false;
        }

        fileName = metaData = data = "";
        switch (opt)
        {
        case 1:
            cout << "FileName for the new file? >>";
            cin >> fileName;
            cout << "Meta data >>";
            cin >> metaData;
            fs -> create(user, fileName);
            break;

        case 2:
            fs -> listFiles();
            cout << "File to be deleted? >>";
            cin >> fileName;
            deleted = fs -> deleteFile(user, fileName);
            cout << "Deleted: " << deleted << endl;
            break;
            
        case 3:
            fs -> listFiles();
            cout << "File name? >>";
            cin >> fileName;
            fd = fs -> openFile(user, fileName, "w");
            cout << "Enter the data to be written >>";
            cin.ignore();
            getline(cin, data);
            cout << data << endl;
            fs -> writeFile(user, fd, fileName, data);
            fs -> closeFile(user, fileName);
            break;

        case 4:
            fs -> listFiles();
            cout << "File name? >>";
            cin >> fileName;
            fd = fs -> openFile(user, fileName, "w");
            cout << "Enter the data to be append >>";
            cin >> data;
            fs -> append(user, fd, fileName, data);
            fs -> closeFile(user, fileName);
        
            break;

        case 5:
            fs -> listFiles();
            cout << "File name? >>";
            cin >> fileName;
            fd = fs -> openFile(user, fileName, "r");
            fs -> readFile(user, fileName);
            fs -> closeFile(user, fileName);
            break;
        
        case 6:
            cout << "File Directory" << endl;
            fs -> listFiles();
            break;

        case 7:
            fs -> memoryStatus();
            break;

        case 8:
            cout << "Exiting the File System for Elections" << endl;
            break;

        }

    }while(opt != 0 && opt != 8);
   

    return 0;
}