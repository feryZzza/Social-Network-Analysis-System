#include "data_structure/huffman.h"
#include "data_structure/lin_list.h"

using namespace std;


LinkList<FrequencyPair> countFrequency(const string& text) {
    LinkList<FrequencyPair> frequencies;
    
    for (size_t i = 0; i < text.length();) {
        int char_len = 1;
        if ((text[i] & 0xE0) == 0xC0) { char_len = 2; }
        else if ((text[i] & 0xF0) == 0xE0) { char_len = 3; }
        else if ((text[i] & 0xF8) == 0xF0) { char_len = 4; }
        
        if (i + char_len > text.length()) { char_len = 1; }
        
        string char_str = text.substr(i, char_len);
        
        
        bool found = false;
        for (int j = 0; j < frequencies.size(); j++) {
            if (frequencies[j].data == char_str) {
                
                FrequencyPair pair = frequencies[j];
                pair.weight++;
                frequencies.setx(j, pair);
                
                found = true;
                break;
            }
        }
        
        
        if (!found) {
            FrequencyPair newPair;
            newPair.data = char_str;
            newPair.weight = 1;
            frequencies.add(newPair);
        }
        
        i += char_len;
    }
    return frequencies;
}


int HuffmanTree::findMinWeightNodeIndex(LinkList<HuffmanNode*>& nodeList) {
    if (nodeList.empty()) {
        return -1;
    }
    
    int minIndex = 0;
    int minWeight = nodeList[0]->weight;
    
    for (int i = 1; i < nodeList.size(); i++) {
        if (nodeList[i]->weight < minWeight) {
            minWeight = nodeList[i]->weight;
            minIndex = i;
        }
    }
    return minIndex;
}



HuffmanTree::HuffmanTree(LinkList<FrequencyPair>& frequencies) {
    LinkList<HuffmanNode*> nodeList;
    
    for (int i = 0; i < frequencies.size(); i++) {
        nodeList.add(new HuffmanNode(frequencies[i].data, frequencies[i].weight));
    }

    
    while (nodeList.size() > 1) {
        
        int index1 = findMinWeightNodeIndex(nodeList);
        HuffmanNode* left = nodeList[index1];
        nodeList.remove(index1);
        
        
        int index2 = findMinWeightNodeIndex(nodeList);
        HuffmanNode* right = nodeList[index2];
        nodeList.remove(index2);

        
        HuffmanNode* parent = new HuffmanNode("", left->weight + right->weight);
        parent->left = left;
        parent->right = right;
        nodeList.add(parent);
    }

    root = nodeList.empty() ? nullptr : nodeList[0];
}


HuffmanTree::~HuffmanTree() {
    deleteTree(root);
}


void HuffmanTree::deleteTree(HuffmanNode* node) {
    if (node == nullptr) {
        return;
    }
    deleteTree(node->left);
    deleteTree(node->right);
    delete node;
}


void HuffmanTree::generateCodesRecursive(HuffmanNode* node, const string& code) {
    if (node == nullptr) {
        return;
    }
    
    if (!node->left && !node->right) {
        CodePair newCode;
        newCode.data = node->data;
        newCode.code = code;
        codes.add(newCode);
    }
    generateCodesRecursive(node->left, code + "0");
    generateCodesRecursive(node->right, code + "1");
}


void HuffmanTree::generateCodes() {
    codes.remove(0); 
    generateCodesRecursive(root, "");
}


string HuffmanTree::compress(const string& text) {
    string compressedText = "";
    
    for (size_t i = 0; i < text.length();) {
        int char_len = 1;
        if ((text[i] & 0xE0) == 0xC0) { char_len = 2; }
        else if ((text[i] & 0xF0) == 0xE0) { char_len = 3; }
        else if ((text[i] & 0xF8) == 0xF0) { char_len = 4; }
        
        if (i + char_len > text.length()) { char_len = 1; }
        
        string char_str = text.substr(i, char_len);

        
        bool found = false;
        for (int j = 0; j < codes.size(); j++) {
            if (codes[j].data == char_str) {
                compressedText += codes[j].code;
                found = true;
                break;
            }
        }
        if (!found) {
             cerr << "Error: No code found for character: " << char_str << endl;
        }
        
        i += char_len;
    }
    return compressedText;
}



string HuffmanTree::decompress(const string& compressedText) {
    string decompressedText = "";
    HuffmanNode* currentNode = root;

    if (!currentNode) {
        return "";
    }

    for (size_t i = 0; i < compressedText.length(); i++) {
        char bit = compressedText[i];
        
        if (bit == '0') {
            currentNode = currentNode->left;
        } else if (bit == '1') {
            currentNode = currentNode->right;
        } else {
            cerr << "Error: Invalid character in compressed stream: " << bit << endl;
            return ""; 
        }

        
        if (!currentNode) {
            cerr << "Error: Invalid compressed data sequence." << endl;
            return ""; 
        }

        
        if (!currentNode->left && !currentNode->right) {
            decompressedText += currentNode->data;
            currentNode = root; 
        }
    }

    
    if (currentNode != root) {
        cerr << "Error: Compressed data is incomplete or corrupt." << endl;
        return ""; 
    }

    return decompressedText;
}


void HuffmanTree::calculateWPLRecursive(HuffmanNode* node, int depth, double& wpl) {
    if (node == nullptr) {
        return;
    }
    if (!node->left && !node->right) {
        wpl += node->weight * depth;
    }
    calculateWPLRecursive(node->left, depth + 1, wpl);
    calculateWPLRecursive(node->right, depth + 1, wpl);
}


double HuffmanTree::getWPL() {
    double wpl = 0.0;
    calculateWPLRecursive(root, 0, wpl);
    return wpl;
}


void HuffmanTree::printCodes() {
    cout << "Huffman Codes:" << endl;
    for (int i = 0; i < codes.size(); i++) {
        cout << "'" << codes[i].data << "' : " << codes[i].code << endl;
    }
}