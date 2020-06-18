#ifndef __X_CSV_H__
#define __X_CSV_H__

#include <fstream>
#include <vector>
#include <string>

using namespace std;

namespace csv {
  class reader {
  private:
    char comma;
    char comment;
    int lines;
    int fields;
    ifstream& file;

    string read_line();
    vector<string> split_line();
  public:
    reader(ifstream& file);
    reader(ifstream& file, char comma, char comment);
    bool done();
    vector<vector<string>> read_all_records();
    vector<string> read_record();
    vector<string> read_header();
  };
}

#endif // __X_CSV_H__
