#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <string>
#include <iostream>
#include "data_structure/lin_list.h"

using namespace std;

struct HuffmanNode {
    string data;
    int weight;
    HuffmanNode* left;
    HuffmanNode* right;

    HuffmanNode(string d, int w) : data(d), weight(w), left(nullptr), right(nullptr) {}
};

struct FrequencyPair {
    string data;
    int weight;
    
    bool operator==(const FrequencyPair& other) const {
        return data == other.data;
    }
};

struct CodePair {
    string data;
    string code;
    
    bool operator==(const CodePair& other) const {
        return data == other.data;
    }
};


class HuffmanTree {
public:
    
    HuffmanTree(LinkList<FrequencyPair>& frequencies);
    
    
    ~HuffmanTree();

    void generateCodes();
    string compress(const string& text);
    string decompress(const string& compressedText);
    double getWPL();
    void printCodes();

private:
    HuffmanNode* root;
    LinkList<CodePair> codes; 

    
    void generateCodesRecursive(HuffmanNode* node, const string& code);
    void calculateWPLRecursive(HuffmanNode* node, int depth, double& wpl);
    
    
    void deleteTree(HuffmanNode* node);
    
    
    int findMinWeightNodeIndex(LinkList<HuffmanNode*>& nodeList);
};


LinkList<FrequencyPair> countFrequency(const string& text);

#endif