#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <numeric>
#include <filesystem>

using namespace std;

struct Edge {
    int start, end;
    double attribute;
};

struct Vertex {
    double attribute;
};

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <input_file> <output_file>" << endl;
        return 1;
    }

    ifstream inputFile(argv[1]);
    if (!inputFile.is_open()) {
        cerr << "Error opening input file: " << argv[1] << endl;
        cerr << "Current working directory: " << endl;
        system("cd");
        cerr << "File exists check: " << (filesystem::exists(argv[1]) ? "Yes" : "No") << endl;
        cerr << "Full path: " << filesystem::absolute(argv[1]) << endl;
        return 1;
    }

    ofstream outputFile(argv[2]);
    if (!outputFile.is_open()) {
        cerr << "Error opening output file: " << argv[2] << endl;
        cerr << "Current working directory: ";
        system("cd");
        return 1;
    }

    int NV, NE;
    string firstLine;
    getline(inputFile, firstLine);
    cout << "First line: '" << firstLine << "'" << endl;
    
    istringstream iss(firstLine);
    if (!(iss >> NV >> NE)) {
        cerr << "Error reading NV and NE from input file" << endl;
        cerr << "NV should be number of vertices, NE should be number of edges" << endl;
        cerr << "First line should contain two numbers separated by space" << endl;
        return 1;
    }
    inputFile.ignore(); // Ignore the newline after the first line

    vector<Edge> edges(NE);
    vector<Vertex> vertices(NV);

    // Инициализация атрибутов
    for(auto& vertex : vertices) {
        vertex.attribute = 0.0;
    }
    for(auto& edge : edges) {
        edge.attribute = 0.0;
    }

    // Сначала обработаем правила для рёбер
    for (int i = NV; i < NV + NE; ++i) {
        string rule;
        getline(inputFile, rule);
        cout << "Rule " << i + 1 << ": " << rule << endl;
        istringstream ruleStream(rule);

        int edgeIndex = i - NV;
        if (isdigit(rule[0]) || rule[0] == '.') {
            ruleStream >> edges[edgeIndex].attribute;
            cout << "Edge " << edgeIndex + 1 << " value: " << edges[edgeIndex].attribute << endl;
        } else if (rule[0] == 'e') {
            int sourceEdgeIndex;
            ruleStream.ignore(1); // Ignore 'e'
            ruleStream >> sourceEdgeIndex;
            edges[edgeIndex].attribute = edges[sourceEdgeIndex - 1].attribute;
            cout << "Edge " << edgeIndex + 1 << " copying from edge " << sourceEdgeIndex << endl;
        }
    }

    // Затем правила для вершин
    inputFile.seekg(0); // Вернуться в начало файла
    string line;
    getline(inputFile, line); // Пропустить первую строку
    for (int i = 0; i < NE; i++) {
        getline(inputFile, line); // Пропустить строки с рёбрами
    }

    for (int i = 0; i < NV; ++i) {
        string rule;
        getline(inputFile, rule);
        cout << "Rule " << i + 1 << ": " << rule << endl;
        istringstream ruleStream(rule);

        if (isdigit(rule[0]) || rule[0] == '.') {
            ruleStream >> vertices[i].attribute;
            cout << "Vertex " << i + 1 << " value: " << vertices[i].attribute << endl;
        } else if (rule[0] == 'v') {
            int vertexIndex;
            ruleStream.ignore(1); // Ignore 'v'
            ruleStream >> vertexIndex;
            vertices[i].attribute = vertices[vertexIndex - 1].attribute;
            cout << "Vertex " << i + 1 << " copying from vertex " << vertexIndex << endl;
        } else if (rule == "min") {
            vector<double> edgeAttributes;
            for (const auto& edge : edges) {
                if (edge.end == i + 1) {
                    edgeAttributes.push_back(edge.attribute);
                    cout << "Found incoming edge with value: " << edge.attribute << endl;
                }
            }
            if (!edgeAttributes.empty()) {
                vertices[i].attribute = *min_element(edgeAttributes.begin(), edgeAttributes.end());
            }
            cout << "Vertex " << i + 1 << " min value: " << vertices[i].attribute << endl;
        }
    }

    // Output results
    for (const auto& vertex : vertices) {
        outputFile << vertex.attribute << endl;
    }
    for (const auto& edge : edges) {
        outputFile << edge.attribute << endl;
    }

    inputFile.close();
    outputFile.close();

    return 0;
}
