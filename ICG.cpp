#include <iostream>
#include <sstream>
#include <stack>
#include <vector>
#include <map>
#include <set>
#include <cctype>
using namespace std;

int tempVarID = 0;

template <typename T>
string generateTempVar(T& symbolMap) {
    stringstream ss;
    ss << "t" << tempVarID++;
    string tempVar = ss.str();
    symbolMap[tempVar] = 0;
    return tempVar;
}

int getPrecedence(char op) {
    switch (op) {
        case '+': case '-': return 1;
        case '*': case '/': return 2;
        default: return 0;
    }
}

vector<string> convertToPostfix(const string& expr) {
    stack<char> operators;
    vector<string> result;

    for (size_t i = 0; i < expr.length(); ++i) {
        char ch = expr[i];
        if (isspace(ch)) continue;

        if (isalnum(ch)) {
            result.emplace_back(1, ch);
        } else if (ch == '(') {
            operators.push(ch);
        } else if (ch == ')') {
            while (!operators.empty() && operators.top() != '(') {
                result.push_back(string(1, operators.top()));
                operators.pop();
            }
            if (!operators.empty()) operators.pop();
        } else {
            while (!operators.empty() && getPrecedence(operators.top()) >= getPrecedence(ch)) {
                result.push_back(string(1, operators.top()));
                operators.pop();
            }
            operators.push(ch);
        }
    }

    while (!operators.empty()) {
        result.push_back(string(1, operators.top()));
        operators.pop();
    }

    return result;
}
