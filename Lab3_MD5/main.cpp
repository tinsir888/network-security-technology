#include "md5.h"
// opcode_table
// print help infos.
void print_help_info(int argc,char *argv[]) {
    if (2 != argc) {
        cout << "error arguments" << endl;
    }
    cout << "usage：" << "\t" << "[-h] --help information " << endl
         << "\t" << "[-t] --test MD5 application" << endl
         << "\t" << "[-c] [file path of the file computed]" << endl
         << "\t" << "\t" <<"--compute MD5 of the given file" << endl
         << "\t" << "[-v] [file path of the file validated]" << endl
         << "\t" << "\t" <<"--validate the integrality of a given file by manual input MD5 value" << endl
         << "\t" << "[-f] [file path of the file validated] [file path of the .md5 file]" << endl
         << "\t" << "\t" <<"--validate the integrality of a given file by read MD5 value from .md5 file" << endl;
}

// print testing infos.
void print_test_info(int argc,char *argv[]) {
    if (2 != argc) {
        cout << "error arguments" << endl;
    }
    vector<string> str = {"", "a", "abc", "message digest", "abcdefghijklmnopqrstuvwxyz", "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789", "12345678901234567890123456789012345678901234567890123456789012345678901234567890"};
    MD5 md5;
    for (int i = 0; i < str.size(); ++i) { 
        md5.calc_md5_digest(str[i]);
        cout << "MD5(\"" + str[i] + "\") = " << md5.to_str() << endl;
    }
}

// calc and print md5 digest.
void print_calc_md5(int argc,char *argv[]) {
    if (argc != 3)
        cout << "error arguments" << endl;
    string file_path = argv[2];
    ifstream file_stream(file_path);
    MD5 md5;
    md5.calc_md5_digest(file_stream);
    cout << "The MD5 value of file(\"" << file_path << "\") is " << md5.to_str() << endl;
}

// input md5 digest, recalc md5 digest for given file, then compare by bit.
void print_input_md5_recalc(int argc,char *argv[]) {
    if (3 != argc) {
        cout << "error arguments" << endl;
    }
    string file_path = argv[2];
    cout << "Please input the MD5 value of file(\"" << file_path << "\")..." << endl;
    string input_md5;
    cin >> input_md5;
    cout << "The old MD5 value of file(\"" << file_path << "\") you have input is" << endl << input_md5 << endl;
    ifstream file_stream(file_path);
    MD5 md5;
    md5.calc_md5_digest(file_stream);
    string res_md5 = md5.to_str();
    cout << "The new MD5 value of file(\"" << file_path << "\") that has computed is" << endl << res_md5 << endl;
    if (!res_md5.compare(input_md5)) {
        cout << "OK! The file is integrated" << endl;
    }
    else {
        cout << "Match Error! The file has been modified!" << endl;
    }
}

// read md5 digest, recalc md5 digest for given file, then compare by bit.
void print_read_md5_recalc(int argc, char *argv[]) {
    if (4 != argc) {
        cout << "error arguments" << endl;
    }
    string file_path = argv[2];
    string md5_path = argv[3];

    ifstream md5_stream(md5_path);
    string old_md5_digest;
    md5_stream >> old_md5_digest;
    cout << "The old MD5 value of file(\"" << file_path << "\") in " << md5_path << " is " << endl << old_md5_digest << endl;

    ifstream file_stream(file_path);
    MD5 md5;
    md5.calc_md5_digest(file_stream);
    string res_md5 = md5.to_str();
    cout << "The new MD5 value of file(\"" << file_path << "\") that has computed is" << endl << res_md5 << endl;
    if (!res_md5.compare(old_md5_digest)) {
        cout << "OK! The file is integrated!" << endl;
    }
    else {
        cout << "Match Error! The file has been modified!" << endl;
    }
}

int main(int argc,char *argv[]) { //argc=num of arguments，argv[] store arguments.
    unordered_map<string, void(*)(int, char*[])> 
        opcode_table = {{"-t", print_test_info}, 
                        {"-h", print_help_info}, 
                        {"-c", print_calc_md5}, 
                        {"-v", print_input_md5_recalc},
                        {"-f", print_read_md5_recalc}};
    if (argc < 2) {
        cout << "error argument, argc = " << argc << endl;
        return -1;
    }
    string op = argv[1];
    if (opcode_table.find(op) != opcode_table.end()) {
        opcode_table[op](argc, argv);
    }
    return 0;
}