#include <iostream>
#include <vector>
#include <map>
#include "../lib/json.hpp"
using json = nlohmann::json;
using namespace std;

int main() {
  // 序列化
  json js;
  vector<int> vec;
  vec.push_back(1);
  vec.push_back(2);
  vec.push_back(3);
  vec.push_back(4);
  js["list"] = vec;

  map<int, string> m;
  m.insert({1, "黄山"});
  m.insert({2, "泰山"});
  m.insert({3, "嵩山"});
  js["path"] = m;
  cout << js << endl;
  string json_str = js.dump(); // json数据对象 -序列化-> json字符串
  cout << json_str << endl;

  // 反序列化
  json buf = json::parse(json_str); 
  auto vec1 = buf["list"];
  for(const auto& i : vec1) {
    cout << i << " ";
  }
  cout << endl;
  map<int, string> map1 = buf["path"];
  for(const auto& [k, v] : map1) {
    cout << k << " " << v << endl;
  }
}
