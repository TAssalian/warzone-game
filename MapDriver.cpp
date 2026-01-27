#include <iostream>
#include <crtdbg.h>
#include "map.h"
using std::string, std::cout;

int main() {

	{
		vector<string> files = {
			"artic.map",
			"bigeurope.map",
			"brasil.map",
			"europass.map",
			"austria.map",
			"eesti.map",
			"apple.map"
		};

		for (string& filename : files) {
			MapLoader m(filename);
			cout << "File: " << filename << "\n";
			if (*m.isFormatValid) {
				cout << "The format is correct and an object has been created." << "\n";
				if (m.isValid()) cout << "The structure of the object is valid." << "\n";
				else cout << "Error: " << m.getErrMsg() << "\n";
			}
			else cout << "The format is wrong and no object has been created." << "\n";

			cout << "\n";
		}
	}

	_CrtDumpMemoryLeaks();

	return 0;
}