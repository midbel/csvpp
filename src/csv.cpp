#include <algorithm>
#include "csv.h"

using namespace std;

namespace csv {

  const char space = ' ';
  const char tab = '\t';
  const char quote = '"';
  const char carriage = '\r';
  const char newline = '\n';

  void validateField(string str) {
    if (str.empty()) {
      return;
    }
    int i = str.find(quote);
    if (i >= 0) {
      throw "unexpected quote in bare field";
    }
  }

  string trim_left(string str, char c) {
    auto it = find_if(str.begin(), str.end(), [c](char ch) {
      return ch != c;
    });
    str.erase(str.begin() , it);
    return str;
  }

  string trim_right(string str, char c) {
    auto it = find_if(str.rbegin(), str.rend(), [c](char ch) {
      return ch != c;
    });
    str.erase(it.base(), str.end());
    return str;
  }

  string trim(string str, char c) {
    return trim_left(trim_right(str, c), c);
  }

  reader::reader(ifstream& file, char comma, char comment):
    comma(comma),
    comment(comment),
    lines(0),
    fields(0),
    file(file) {}

  reader::reader(ifstream& file): reader(file, ',', '#') {}

  bool reader::done() {
    return file.eof();
  }

  vector<vector<string>> reader::read_all_records() {
    vector<vector<string>> vs;
    while(!done()) {
      vector<string> r = read_record();
      vs.push_back(r);
    }
    return vs;
  }

  vector<string> reader::read_header() {
    return read_record();
  }

  vector<string> reader::read_record() {
    vector<string> vs = split_line();
    if (fields == 0) {
      fields = vs.size();
    } else {
      if (fields != vs.size()) {
        throw "wrong number of fields";
      }
    }
    return vs;
  }

  vector<string> reader::split_line() {
    vector<string> vs;
    int next = 0;
    string str;
    string line = read_line();
    while(line.size() > 0) {
      line = trim_left(line, space);
      if (line[0] == quote) {
        next = 1;
        while(1) {
          next = line.find(quote, next);
          if (next == string::npos) {
            // closing quote on next line - newline char to be appended
            line += newline;
            line += trim_left(read_line(), space);
            next = 1;
            continue;
          }
          char ch = line[next+1];
          if (ch == quote) {
            line.erase(next, 1);
            next++;
          } else if (ch == comma || ch == carriage) {
            str = trim(line.substr(0, next), quote);
            vs.push_back(str);

            line = ch == carriage ? "" : line.substr(next+2);
            break;
          } else {
            throw "unexpected character after quote";
          }
        }
      } else {
        next = line.find(comma, 0);
        if (next != 0) {
          str = next == string::npos ? line : line.substr(0, next);
          str = trim_right(str, space);
          validateField(str);
        } else {
          str = "";
        }
        vs.push_back(str);
        if (next == string::npos) {
          break;
        }

        line = line.substr(next+1);
      }
    }
    lines++;
    return vs;
  }

  string reader::read_line() {
    if (done()) {
      throw "end of file";
    }
    string line;
    while(!done()) {
      getline(file, line);
      if (line.empty() || line[0] == comment) {
        continue;
      } else {
        break;
      }
    }
    return line;
  }
}
