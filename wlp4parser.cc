#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <sstream>
#include <cstring>
#include "wlp4data.h"
//#include "wlp4data.cc"
using namespace std;

const int INT_MIN = -2147483648;

class Rule{
    public:
    string LHS;
    vector<string> RHS;
    Rule(istream &in){
        string line;
        in >> LHS;
        while(in >> line) {
            if (line != ".EMPTY") RHS.push_back(line);
        }
    }
};

class Tree {
  public:
  pair<string, string> data;
  vector<Tree*> children;

  Tree(string s1, string s2){
    data = make_pair(s1,s2);
  }
  void addchild(Tree* tree){
    children.push_back(tree);
  }
  void print(ostream &out = cout) {
    if (data.second == "") data.second = ".EMPTY";
    cout << data.first << " " << data.second << endl;
    int size = children.size();
    for(int i = 0; i < size; ++i){
      children[i]->print();
    }
  }
  ~Tree() {
    for (auto& c : children) {
      delete c;
    }
  }
};

vector<Rule> cfg;

vector<int> states;

vector<Tree*> treestack;

vector<pair<string, string>> Input;
int INDEX = 0;

const string CFG = ".CFG";
const string TR = ".TRANSITIONS";
const string RD = ".REDUCTIONS";

class DFA{
    public:
    vector<pair<pair<int, string>, int>> transitions;
    vector<pair<pair<int, int>, string>> reductions;

    int getTransition(int state, string LHS){
      for (auto n : transitions){
        if ((state == n.first.first) && (LHS == n.first.second)){
          return n.second;
        }
      }
      return INT_MIN;
    }

    int getReduction(int state, string symbol){
      for (auto n: reductions){
        if((state == n.first.first) && (symbol == n.second)){
          return n.first.second;
        }
      }
      return INT_MIN;
    }
};

DFA dfa;

void getDATA(istream &in){
  string s;
  while(true) {
    if (!(getline(in, s))) {
      throw runtime_error
        ("Expected " + CFG + ", but found end of input.");
    }
    if (s == CFG) {
      break;
    }
    if (!s.empty()) {
      throw runtime_error
        ("Expected " + CFG + ", but found: " + s);
    }
  }
  // Get  WLP4 rules
  while(true) {
    if (!(getline(in, s))) {
      throw runtime_error
        ("Unexpected end of input while reading CFG set: " 
         + TR + "not found."); 
    }
    if (s == TR) {
      break;
    } 
    //Get rules
    istringstream line{s};
    cfg.push_back(Rule(line));
  }
  //Transitions
  while(true) {
    if (!(getline(in, s))) {
      break;
    }
    if (s == RD) {
      break;
    }
    int state0;
    string symbol;
    int state1;
    stringstream line(s);
    line >> state0 >> symbol >> state1;
    dfa.transitions.push_back(make_pair(make_pair(state0, symbol),state1));
    
  }
  //Reductions
  while(true) {
    if (!(getline(in, s))) {
      break;
    }
    int state0;
    int symbol;
    string rulenum;
    stringstream line1(s);
    line1 >> state0 >> symbol >> rulenum;
    dfa.reductions.push_back(make_pair(make_pair(state0, symbol),rulenum));
  }
}

void getINPUT(){
  string s;
  string kind;
  string lexeme;
  Input.push_back(make_pair("BOF", "BOF"));
  while(true){
    if (!(getline(cin, s))){
      Input.push_back(make_pair("EOF","EOF"));
      break;
    }
    istringstream ss{s};
    ss >> kind >> lexeme;
    Input.push_back(make_pair(kind, lexeme));
  }
}

string RHScreate(Rule r){
  string s;
  for(int i = 0; i < r.RHS.size(); ++i){
    s += r.RHS[i];
    s += " ";
  }
  return s;
}

void reducestates(Rule r){
  int len = r.RHS.size();
  if (len > states.size()) throw runtime_error("Invalid state stack");
  for(int i = 0; i < len; ++i){
    states.pop_back();
  }
  int state = states.back();
  string rLHS = r.LHS;
  int newstate = dfa.getTransition(state, rLHS);
  if (newstate != INT_MIN) states.push_back(newstate);
  else throw runtime_error("No transition");
}

void reducetrees(Rule r){
  string rRHS = RHScreate(r);
  Tree *T = new Tree(r.LHS, rRHS);
  int len = r.RHS.size();
  int index = treestack.size() - len; //want leftmost first, so we find the index
  if(index < 0 ) throw runtime_error("Invalid tree stack");
  for(int i = 0; i < len; ++i){
    Tree* TT = treestack[index + i];
    T->children.push_back(TT);
  }
  for(int i = 0; i < len; ++i){
    treestack.pop_back();
  }
  treestack.push_back(T);
}

void shift(pair<string, string> r){
  string s1 = r.first;
  string s2 = r.second;
  Tree *T = new Tree(s1, s2);
  treestack.push_back(T);
  int newstate = dfa.getTransition(states[states.size() - 1], s1);
  if (newstate != INT_MIN) states.push_back(newstate);
  else throw runtime_error("No transition");
}

void beginparse(){
  for (auto i : Input){
    int current_state = states[states.size() -1];
    string s1 = i.first;
    string s2 = i.second;
    int newrule = dfa.getReduction(current_state, s1);
    while(newrule != INT_MIN){
      reducetrees(cfg[newrule]);
      reducestates(cfg[newrule]);
      current_state = states[states.size() -1];
      newrule = dfa.getReduction(current_state, s1);
    }
    shift(i);
    if(i.first == "EOF") {
      reducetrees(cfg[0]);
      break;
    }
  }
}

int main(){
    try{
        states.push_back(0);
        stringstream s(WLP4_COMBINED);
        getDATA(s);
        getINPUT();
        beginparse();
        treestack[0]->print();
        int size = treestack.size();
        for(int i = 0; i < size; ++i){
          delete treestack[i];
        }
    }
    catch(runtime_error &e) {
        cerr << "ERROR: " << e.what() << "\n";
        int size = treestack.size();
        for(int i = 0; i < size; ++i){
          delete treestack[i];
        }
        return 1;
    }
    return 0;
}
