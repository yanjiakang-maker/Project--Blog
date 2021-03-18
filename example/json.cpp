#include<iostream>
#include <jsoncpp/json/json.h>
#include <string>

using namespace std;

int main() {
  Json::Value value;
  value["name"] = "张三";
  value["age"] = 18;
  value["score"] = 88.88;

  Json::StyledWriter writer;
  string str = writer.write(value);
  cout << str  << endl;  
  
  Json::Value value1;
  Json::Reader reader;

  reader.parse(str, value1);

  cout << value1["name"].asString() << endl;
  cout << value1["age"].asInt() << endl;
  cout << value1["score"].asFloat() << endl;


  return 0;
}
