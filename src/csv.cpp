#include <algorithm>
#include "csv.h"

namespace csv {

  const char space = ' ';
  const char tab = '\t';
  const char quote = '"';
  const char carriage = '\r';
  const char newline = '\n';

  void validateField(std::string str) {
    if (str.empty()) {
      return;
    }
    int i = str.find(quote);
    if (i >= 0) {
      throw error{"unexpected quote in bare field"};
    }
  }

  std::string trim_left(std::string str, char c) {
    auto it = find_if(str.begin(), str.end(), [c](char ch) {
      return ch != c;
    });
    str.erase(str.begin() , it);
    return str;
  }

  std::string trim_right(std::string str, char c) {
    auto it = find_if(str.rbegin(), str.rend(), [c](char ch) {
      return ch != c;
    });
    str.erase(it.base(), str.end());
    return str;
  }

  std::string trim(std::string str, char c) {
    return trim_left(trim_right(str, c), c);
  }

  reader::reader(std::ifstream& file, char comma, char comment):
    comma(comma),
    comment(comment),
    lines(0),
    fields(0),
    file(file) {}

  reader::reader(std::ifstream& file): reader(file, ',', '#') {}

  bool reader::done() {
    return file.eof();
  }

  std::vector<std::vector<std::string>> reader::read_all_records() {
    std::vector<std::vector<std::string>> vs;
    while(!done()) {
      vs.push_back(read_record());
    }
    return vs;
  }

  std::vector<std::string> reader::read_header() {
    return read_record();
  }

  std::vector<std::string> reader::read_record() {
    auto vs = split_line();
    if (fields == 0) {
      fields = vs.size();
    } else {
      if (fields != vs.size()) {
        throw error{"wrong number of fields"};
      }
    }
    return vs;
  }

  std::vector<std::string> reader::split_line() {
    std::vector<std::string> vs;
    int next = 0;
    std::string str;
    std::string line = read_line();
    while(line.size() > 0) {
      line = trim_left(line, space);
      if (line[0] == quote) {
        next = 1;
        while(1) {
          next = line.find(quote, next);
          if (next == std::string::npos) {
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
            throw error{"unexpected character after quote"};
          }
        }
      } else {
        next = line.find(comma, 0);
        if (next != 0) {
          str = next == std::string::npos ? line : line.substr(0, next);
          str = trim_right(str, space);
          validateField(str);
        } else {
          str = "";
        }
        vs.push_back(str);
        if (next == std::string::npos) {
          break;
        }
        line = line.substr(next+1);
      }
    }
    lines++;
    return vs;
  }

  std::string reader::read_line() {
    if (done()) {
      throw error{"end of file"};
    }
    std::string line;
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
