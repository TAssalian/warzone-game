#include <iostream>

// Raises error for Mac users if #ifdef is not present
#ifdef _MSC_VER
#include <crtdbg.h>
#endif

#include "Map.h"
using std::string, std::cout;

int run_map_driver() {
  {
    vector<string> files = {
        "data/maps/artic.map",   "data/maps/bigeurope.map",
        "data/maps/brasil.map",  "data/maps/europass.map",
        "data/maps/austria.map", "data/maps/eesti.map",
        "data/maps/apple.map",   "data/maps/apple_valid.map"};

    for (string &filename : files) {
      MapLoader m(filename);
      cout << "File: " << filename << "\n";
      if (*m.isFormatValid) {
        cout << "The format is correct and an object has been created." << "\n";
        if (m.isValid())
          cout << "The structure of the object is valid." << "\n";
        else
          cout << "Error: " << m.getErrMsg() << "\n";
      } else
        cout << "The format is wrong and no object has been created." << "\n";

      cout << "\n";
    }
  }

#ifdef _MSC_VER
  _CrtDumpMemoryLeaks();
#endif

  return 0;
}

int main() {
  run_map_driver();
  return 0;
}
