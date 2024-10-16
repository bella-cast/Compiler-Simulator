#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <sstream>
#include <cstring>
//#include "dfa.h"
using namespace std;

const string STATES      = ".STATES";
const string TRANSITIONS = ".TRANSITIONS";
const string INPUT       = ".INPUT";

string DFAstring = R"(
.STATES
start
ID!
NUM!
INT!
WAIN!
IF!
ELSE!
WHILE!
PRINTLN!
RETURN!
NEW!
DELETE!
NULL!
LPAREN!
RPAREN!
LBRACE!
RBRACE!
LBRACK!
RBRACK!
BECOMES!
PLUS!
MINUS!
STAR!
SLASH!
PCT!
AMP!
COMMA!
SEMI!
LT!
GT!
LE!
GE!
NE!
EQ!
?WHITESPACE!
?COMMENT!
excl
.TRANSITIONS
start 0-9 NUM
NUM 0-9 NUM
start ( LPAREN
start ) RPAREN
start { LBRACE
start } RBRACE
start [ LBRACK
start ] RBRACK
start = BECOMES
BECOMES = EQ
start + PLUS
start - MINUS
start * STAR
start / SLASH
start & AMP
start % PCT
start , COMMA
start ; SEMI
start < LT
LT = LE
start > GT
GT = GE
start ! excl
excl = NE
start \s \t \r \n ?WHITESPACE
?WHITESPACE \s \t \r \n ?WHITESPACE
SLASH / ?COMMENT
?COMMENT \x00-\x09 \x0b \x0c \x0e-\x7f \x0B \x0C \x0E-\x7F ?COMMENT
start A-Z a-z ID
ID a-z A-Z 0-9 ID
)";

class DFA{
    vector<pair<string, bool>> states;
    vector<pair<string, pair<char,string>>> transitions; 

    public:
    pair<string,bool> initial;
    bool getAccept(string s){ 
        for(auto n : states){
            if(n.first == s) return n.second;
        }
        throw runtime_error("Invalid state!");
    }
    void addState(string s, bool accept){
        states.push_back(make_pair(s,accept));
    }
    void addTransition(string s1, char c, string s2){ 
        bool check1 = false;
        bool check2 = false;
        for(auto n: states){
            if (n.first == s1) check1 = true;
            if (n.first == s2) check2 = true;
        }
        if (!(check1 && check2)) throw runtime_error("Invalid state!");
        transitions.push_back(make_pair(s1,make_pair(c,s2)));
    }
    string getNextState(string s, char c){ 
        bool check = false;
        for(auto n: transitions){
            if (n.first == s){
                    if (n.second.first == c){
                        check = true;
                        return n.second.second;
                    }
            }
        }
        if (!(check)) return "novalidstate";
    }
};

//Helper Functions

bool isChar(string s) {
  return s.length() == 1;
}

bool isRange(string s) {
  return s.length() == 3 && s[1] == '-';
}

string squish(string s) {
  stringstream ss(s);
  string token;
  string result;
  string space = "";
  while(ss >> token) {
    result += space;
    result += token;
    space = " ";
  }
  return result;
}

int hexToNum(char c) {
  if ('0' <= c && c <= '9') {
    return c - '0';
  } else if ('a' <= c && c <= 'f') {
    return 10 + (c - 'a');
  } else if ('A' <= c && c <= 'F') {
    return 10 + (c - 'A');
  }
  // This should never happen....
  throw runtime_error("Invalid hex digit!");
}

char numToHex(int d) {
  return (d < 10 ? d + '0' : d - 10 + 'A');
}

string escape(string s) {
  string p;
  for(int i=0; i<s.length(); ++i) {
    if (s[i] == '\\' && i+1 < s.length()) {
      char c = s[i+1]; 
      i = i+1;
      if (c == 's') {
        p += ' ';            
      } else
      if (c == 'n') {
        p += '\n';            
      } else
      if (c == 'r') {
        p += '\r';            
      } else
      if (c == 't') {
        p += '\t';            
      } else
      if (c == 'x') {
        if(i+2 < s.length() && isxdigit(s[i+1]) && isxdigit(s[i+2])) {
          if (hexToNum(s[i+1]) > 8) {
            throw runtime_error(
                "Invalid escape sequence \\x"
                + string(1, s[i+1])
                + string(1, s[i+2])
                +": not in ASCII range (0x00 to 0x7F)");
          }
          char code = hexToNum(s[i+1])*16 + hexToNum(s[i+2]);
          p += code;
          i = i+2;
        } else {
          p += c;
        }
      } else
      if (isgraph(c)) {
        p += c;            
      } else {
        p += s[i];
      }
    } else {
       p += s[i];
    }
  }  
  return p;
}

string unescape(string s) {
  string p;
  for(int i=0; i<s.length(); ++i) {
    char c = s[i];
    if (c == ' ') {
      p += "\\s";
    } else
    if (c == '\n') {
      p += "\\n";
    } else
    if (c == '\r') {
      p += "\\r";
    } else
    if (c == '\t') {
      p += "\\t";
    } else
    if (!isgraph(c)) {
      string hex = "\\x";
      p += hex + numToHex((unsigned char)c/16) + numToHex((unsigned char)c%16);
    } else {
      p += c;
    }
  }
  return p;
}

void check_restrict(string state, string token){
  if (state == "?WHITESPACE" || state == "?COMMENT") return;
  else if (state == "NUM"){
    signed long int d = stoul(token);
    signed long int max = 2147483647;
    if (d > max) throw runtime_error("NUM out of range");
    else if (token.length() > 1) {
      int e = token[0] - '0';
      if (e == 0) throw runtime_error("NUM has leading zeroes");
    }
  }
  else if (state == "ID"){
    if (token == "int") state = "INT";
    else if (token == "if") state = "IF";
    else if (token == "wain") state = "WAIN";
    else if (token == "else") state = "ELSE";
    else if (token == "while") state = "WHILE";
    else if (token == "println") state = "PRINTLN";
    else if (token == "return") state = "RETURN";
    else if (token == "new") state = "NEW";
    else if (token == "delete") state = "DELETE";
    else if (token == "NULL") state = "NULL";
  }
  cout << state << " " << token << endl;
}

void maxmunch(string s, DFA &dfa){ 
  string state = dfa.initial.first;
  string token = "";

  while(s.length() > 0){
    char c = s.front(); 
    string next = dfa.getNextState(state, c);
    if (next != "novalidstate"){
      token += c; 
      s.erase(0,1);
      state = next;
    } else {
      if (dfa.getAccept(state)) {
        check_restrict(state, token);
        state = dfa.initial.first;
        token = "";
      }
      else {
        throw runtime_error("invalid transition state");
        return;
      } 
    }
  }
  if (dfa.getAccept(state)) check_restrict(state, token);
  else throw runtime_error("end of input not accepted");
}

DFA DFAconstruct(istream &in) { 
  DFA dfa;
  string s;
  while(true) {
    if (!(getline(in, s))) {
      throw runtime_error
        ("Expected " + STATES + ", but found end of input.");
    }
    s = squish(s);
    if (s == STATES) {
      break;
    }
    if (!s.empty()) {
      throw runtime_error
        ("Expected " + STATES + ", but found: " + s);
    }
  }
  // Get states
  bool first = true;
  while(true) {
    if (!(in >> s)) {
      throw runtime_error
        ("Unexpected end of input while reading state set: " 
         + TRANSITIONS + "not found.");
    }
    if (s == TRANSITIONS) {
      break;
    } 
    // Process an individual state
    bool accepting = false;
    if (s.back() == '!' && s.length() > 1) {
      accepting = true;
      s.pop_back();
    }

    dfa.addState(s, accepting);
    if (first) dfa.initial = make_pair(s, accepting);
    first = false;
  }
  // Get transitions
  getline(in, s); // Skip .TRANSITIONS header
  while(true) {
    if (!(getline(in, s))) {
      break;
    }
    s = squish(s);
    if (s == INPUT) {
      break;
    } 
    string lineStr = s;
    stringstream line(lineStr);
    vector<string> lineVec;
    while(line >> s) {
      lineVec.push_back(s);
    }
    if(lineVec.empty()) {
      continue;
    }
    if (lineVec.size() < 3) {
      throw runtime_error
        ("Incomplete transition line: " + lineStr);
    }
    // Extract state information from the line
    string fromState = lineVec.front();
    string toState = lineVec.back();
    // Extract character and range information from the line
    vector<char> charVec;
    for(int i = 1; i < lineVec.size()-1; ++i) {
      string charOrRange = escape(lineVec[i]);
      if (isChar(charOrRange)) {
        char c = charOrRange[0];
        if (c < 0 || c > 127) {
          throw runtime_error
            ("Invalid (non-ASCII) character in transition line: " + lineStr + "\n"
             + "Character " + unescape(string(1,c)) + " is outside ASCII range");
        }
        charVec.push_back(c);
      } else if (isRange(charOrRange)) {
        for(char c = charOrRange[0]; charOrRange[0] <= c && c <= charOrRange[2]; ++c) {
          charVec.push_back(c);
        }
      } else {
        throw runtime_error
          ("Expected character or range, but found "
           + charOrRange + " in transition line: " + lineStr);
      }
    }
    for ( char c : charVec ) {
        dfa.addTransition(fromState, c, toState); 
    }
  }
  return dfa;
}

int main(){
  try {
    stringstream s(DFAstring);
    DFA dfa = DFAconstruct(s);
    string input;
    while(getline(cin, input)){
      if (input.size() == 0) continue;
      maxmunch(input, dfa);
    }
  } catch(runtime_error &e) {
    cerr << "ERROR: " << e.what() << "\n";
    return 1;
  }
  return 0;
}
