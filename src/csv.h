#ifndef __X_CSV_H__
#define __X_CSV_H__

#include <fstream>
#include <vector>
#include <exception>

namespace csv {

  class error: public std::exception {
  private:
    std::string msg;
  public:
    error(std::string str): msg(str) {}
    virtual ~error() {}
    virtual const char* what() const throw() {
      if (msg.size() == 0) {
        return "unexpected error";
      }
      return msg.c_str();
    }
  };

  class reader {
  private:
    char comma;
    char comment;
    int lines;
    int fields;
    std::ifstream& file;

    std::string read_line();
    std::vector<std::string> split_line();
  public:
    reader(std::ifstream& file);
    reader(std::ifstream& file, char comma, char comment);
    bool done();
    std::vector<std::vector<std::string>> read_all_records();
    std::vector<std::string> read_record();
    std::vector<std::string> read_header();
  };
}

#endif // __X_CSV_H__
