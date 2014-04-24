#include <string>
#include <iostream>

#define BUFFER_SIZE 4096

using namespace std;

void replaceAll(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}


int main(int argc, char** argv)
{
	char buffer[BUFFER_SIZE];
	
	if (argc != 2)
	{
		cerr << "toh reads text from stdin and outputs a C header file containing a string constant" << endl;
		cerr << "    that contains the input from stdin." << endl;
		cerr << "Syntax: toh <variable name>" << endl;
		return 1;
	}

	cout << "/* This file was generated by toh, do not edit */" << endl;

	cout << "std::string " << argv[1] << " = ";

	while (!cin.eof())
	{
		cout << endl;

		// read line
		cin.getline(buffer, BUFFER_SIZE);
		string line(buffer);

		// escape quotes
		replaceAll(line, "\\", "\\\\");
		replaceAll(line, "\"", "\\\"");

		cout << "   \"" << line << "\\n\"";
	}
	cout << ";" << endl;
}
