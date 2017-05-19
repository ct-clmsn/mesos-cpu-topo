#include <string>
#include <regex>
#include <iostream>

int main(int argc, char **argv) {
  const std::string STR = "Node 0 MemTotal:       16699204 kB\n";
  auto spos = STR.find(":");
  std::regex e("[0-9]+");
  std::smatch m;
  auto membytesstr = STR.substr(spos);
  std::cout << std::regex_search(membytesstr, m, e) << std::endl;

  for(size_t i = 0; i < m.size(); i++) {
    std::cout << m[i].str() << std::endl;
  }


}
