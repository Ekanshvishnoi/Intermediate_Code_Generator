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


vector<string> createTAC(const vector<string>& postfix, map<string, int>& table) {
    stack<string> stk;
    vector<string> tac;

    for (const auto& token : postfix) {
        if (isalnum(token[0]) && token.length() == 1) {
            stk.push(token);
        } else {
            string right = stk.top(); stk.pop();
            string left = stk.top(); stk.pop();
            string temp = generateTempVar(table);
            tac.push_back(temp + " = " + left + " " + token + " " + right);
            stk.push(temp);
        }
    }
    return tac;
}

void applyConstantFolding(vector<string>& tacList) {
    for (auto& stmt : tacList) {
        stringstream ss(stmt);
        string lhs, eq, left, op, right;
        ss >> lhs >> eq >> left >> op >> right;

        if (isdigit(left[0]) && isdigit(right[0])) {
            int a = stoi(left), b = stoi(right), res = 0;
            if (op == "+") res = a + b;
            else if (op == "-") res = a - b;
            else if (op == "*") res = a * b;
            else if (op == "/") res = b != 0 ? a / b : 0;
            stmt = lhs + " = " + to_string(res);
        } else if ((left == "0" && op == "*") || (right == "0" && op == "*")) {
            stmt = lhs + " = 0";
        }
    }
}

void eliminateDeadCode(vector<string>& tacList) {
    set<string> used;
    for (const auto& stmt : tacList) {
        stringstream ss(stmt);
        string lhs, eq, left, op, right;
        ss >> lhs >> eq >> left;
        if (ss >> op >> right) {
            if (!isdigit(left[0])) used.insert(left);
            if (!isdigit(right[0])) used.insert(right);
        } else {
            if (!isdigit(left[0])) used.insert(left);
        }
    }

    vector<string> optimized;
    for (const auto& stmt : tacList) {
        string lhs = stmt.substr(0, stmt.find('=') - 1);
        if (used.count(lhs) || lhs == "result") {
            optimized.push_back(stmt);
        } else {
            cout << "Eliminated Dead Code: " << stmt << endl;
        }
    }

    tacList = optimized;
}
