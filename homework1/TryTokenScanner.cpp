//ID: 5130379056
//Name: Tian Jiahe

#include "TryTokenScanner.h"

#include "tokenscanner.h"

using namespace std;

// Part I
Vector<string> TryTokenScanner::allTokens(string text) {
  Vector<string> tokens;
  TokenScanner scanner(text);
  while (scanner.hasMoreTokens()) {
    string token = scanner.nextToken();
    tokens.push_back(token);
  }
  return tokens;
}

// Part II
Vector<string> TryTokenScanner::noWhitespaces(string text) {
  Vector<string> tokens;
  TokenScanner scanner(text);
  scanner.ignoreWhitespace();
  while (scanner.hasMoreTokens()) {
    string token = scanner.nextToken();
    tokens.push_back(token);
  }
  return tokens;
}

// Part III
Vector<string> TryTokenScanner::noComments(string text) {
  Vector<string> tokens;
  TokenScanner scanner(text);
  scanner.ignoreComments();
  while (scanner.hasMoreTokens()) {
    string token = scanner.nextToken();
    tokens.push_back(token);
  }
  return tokens;
}

