#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <string>
#include <fstream>
#include <set>
using namespace std;

//////////// HELPER //////////////////

const int INSTANCE_READ_MODE = 1;
const int SOLUTION_READ_MODE = 2;

void checkInputConstraint(bool validConstraint, int lineNumber, string failMsg) {
  if (!validConstraint) {
    cerr << "Instance Error (" << lineNumber << "): " << failMsg << endl;
    exit(2);
  }
}

void checkSolutionConstraint(bool validConstraint, string failMsg) {
  if (!validConstraint) {
    #ifdef VERBOSE
      cout << 0 << "|" << failMsg << endl;
    #else
      cout << 0 << "|Wrong Answer" << endl;
    #endif
    exit(0);
  }
}

void checkConstraint(int mode, bool validConstraint, int lineNumber, string failMsg) {
  if (mode == INSTANCE_READ_MODE) {
    checkInputConstraint(validConstraint, lineNumber, failMsg);
  } else if (mode == SOLUTION_READ_MODE) {
    checkSolutionConstraint(validConstraint, failMsg);
  }
}

vector<string> tokenize(string s) {
  vector<string> tokens;
  stringstream ss(s);
  string tmp;

  while (ss >> tmp) {
    tokens.push_back(tmp);
  }

  return tokens;
}

int parseInt(int mode, string x, int lineNumber = -1) {
  checkConstraint(mode, x.length() > 0, lineNumber, "Expected integer, got empty string");
  
  int sign = 1;
  int ret = 0;

  if (x[0] == '-') {
    sign = -1;
    x = x.substr(1);
  }

  checkConstraint(mode, x.length() > 0, lineNumber, "Expected integer, got non-integer string");

  for (char ch : x) {
    checkConstraint(mode, '0' <= ch && ch <= '9', lineNumber, "Expected integer, got non-integer string");
    ret = 10 * ret + (ch - '0');
  }

  return ret * sign;
}

string intToString(int x) {
  stringstream ss;
  ss << x;

  string ret;
  ss >> ret;

  return ret;
}

////////////// END OF HELPER //////////////////

class Solution {
  public:
    int getNumVertex() {
      return numVertex;
    }

    int getCoverSize() {
      return coverSize;
    }

    vector<int> getVertexCover() {
      return vertexCover;
    }

    void readFromStream(ifstream &is) {
      coverSize = -1;
      numVertex = -1;
      vertexCover.clear();

      string line;
      while (getline(is, line)) {
        vector<string> tokens = tokenize(line);
        
        if (tokens.empty() || tokens[0] == "c") {
          continue;
        }

        if (tokens[0] == "s") {
          checkSolutionConstraint(coverSize == -1, "Multiple header in solution");
          checkSolutionConstraint(tokens.size() == 4, "Header must be 4 tokens <s vc numVertex numInVertexCover>");
          checkSolutionConstraint(tokens[1] == "vc", "Second header token must be vc");

          numVertex = parseInt(SOLUTION_READ_MODE, tokens[2]);
          coverSize = parseInt(SOLUTION_READ_MODE, tokens[3]);

          continue;
        }

        checkSolutionConstraint(coverSize != -1, "Vertex specification before header");
        checkSolutionConstraint(tokens.size() == 1, "Vertex line must consist of 1 token");
        
        int u = parseInt(SOLUTION_READ_MODE, tokens[0]);
        vertexCover.push_back(u);
      }

      checkSolutionConstraint(coverSize != -1, "No header found");
      checkSolutionConstraint(coverSize == (int)vertexCover.size(), "Different size of vertex cover with stated in header");
    }

    void write(ostream &stream) {
      stream << "s vc " << numVertex << " " << coverSize << endl;
      for (int v : vertexCover) {
        cout << v << endl;
      }
    }

  private:
    int numVertex;
    int coverSize;
    vector<int> vertexCover;
};

class ProblemInstance {
  public:
    int getNumVertex() {
      return numVertex;
    }

    int getNumEdge() {
      return numEdge;
    }

    vector<pair<int, int>> getEdgeList() {
      return edgeList;
    }
  
    void readFromStream(ifstream &stream) {
      numVertex = -1;
      numEdge = -1;
      edgeList.clear();

      string line;
      int lineNum = 0;

      while (getline(stream, line)) {
        lineNum++;
        vector<string> tokens = tokenize(line);
        
        if (tokens.empty() || tokens[0] == "c") {
          continue;
        }

        if (tokens[0] == "p") {
          checkInputConstraint(numVertex == -1, lineNum, "Multiple header");
          checkInputConstraint(tokens.size() == 4, lineNum, "Header not consisting of 4 tokens <p td numVertex numEdge>");
          checkInputConstraint(tokens[0] == "p", lineNum, "First header token must be \"p\"");
          checkInputConstraint(tokens[1] == "td", lineNum, "Second header token must be \"td\"");
          
          numVertex = parseInt(INSTANCE_READ_MODE, tokens[2], lineNum);
          numEdge = parseInt(INSTANCE_READ_MODE, tokens[3], lineNum);

          checkInputConstraint(numVertex > 0, lineNum, "numVertex must be positive");
          checkInputConstraint(numEdge > 0, lineNum, "numEdge must be positive");

          continue;
        }

        checkInputConstraint(numVertex != -1, lineNum, "Edge specification before header");
        checkInputConstraint(tokens.size() == 2, lineNum, "Edge must consists of two vertices");

        int u = parseInt(INSTANCE_READ_MODE, tokens[0], lineNum);
        int v = parseInt(INSTANCE_READ_MODE, tokens[1], lineNum);

        checkInputConstraint(1 <= u && u <= numVertex, lineNum, "First vertex out of range [1, numVertex]");
        checkInputConstraint(1 <= v && v <= numVertex, lineNum, "Second vertex out of range [1, numVertex]");

        edgeList.push_back({u, v});
      }

      checkInputConstraint(numVertex != -1, -1, "No header found");
      checkInputConstraint((int)edgeList.size() == numEdge, -1, "Edge number differs from actual");
    }

    int validate(Solution solution) {
      vector<int> vertexCover = solution.getVertexCover();
      set<int> cover(vertexCover.begin(), vertexCover.end());

      checkSolutionConstraint(solution.getNumVertex() == getNumVertex(), "Number of vertex in graph differs");
      for (int v : cover) {
        checkSolutionConstraint(1 <= v && v <= numVertex, "Vertex out of range [1, numVertex]");
      }

      int edgeNum = 0;
      for (auto edge : edgeList) {
        edgeNum++;
        checkSolutionConstraint(cover.count(edge.first) || cover.count(edge.second), 
                        intToString(edgeNum) + "-th edge is not covered");
      }

      return 1;
    }

    void write(ostream &stream) {
      stream << "p td " << numVertex << " " << numEdge << endl;
      for (auto edge : edgeList) {
        stream << edge.first << " " << edge.second << endl;
      }
    }

  private:
   int numVertex;
   int numEdge;
   vector<pair<int, int>> edgeList;
};

ProblemInstance problemInstance;
Solution judgeSolution, userSolution;

int main(int argc, char **argv) {
  if (argc < 3) {
    printf("Usage: %s instance_input solution_output [instance_output]\n", argv[0]);
    return 0;
  }
  
  ifstream instanceInputStream(argv[1]);
  ifstream userOutputStream(argv[2]);

  if (!userOutputStream) {
    // No output
    checkSolutionConstraint(false, "No output produced by user"); 
  }

  problemInstance.readFromStream(instanceInputStream);
  userSolution.readFromStream(userOutputStream);

  if (argc >= 4) {
    ifstream instanceOutputStream(argv[3]);
    judgeSolution.readFromStream(instanceOutputStream);
  }

  int valid = problemInstance.validate(userSolution);

  int result = valid;
  if (argc >= 4) {
    result &= (userSolution.getCoverSize() <= judgeSolution.getCoverSize());
  }

  cout << result << "|SUCCESS\n";

  return 0;
}