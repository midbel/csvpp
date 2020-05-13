#include <fstream>
#include <iostream>
#include "csv.h"

using namespace std;

int main(int argc, char** argv) {
  if (argc != 2) {
    cerr << "invalid number of arguments" << endl;
    return 1;
  }

  ifstream in(argv[1]);
  if (!in) {
    cerr << "fail to open file " << argv[1] << endl;
    return 2;
  }

  csv::reader r = csv::reader(in);
  for (int i = 1; !r.done(); i++) {
      cout << "record: " << i << endl;
      for (string str: r.read_record()) {
        cout << "-> " << str << endl;
      }
      cout << "---" << endl;
  }
  in.close();
}
