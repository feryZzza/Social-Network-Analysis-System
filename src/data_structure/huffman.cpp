#include "data_structure/huffman.h"
#include "data_structure/queue.h" // 引入 PriorityQueue
#include <iostream>

using namespace std;

// --- 辅助函数：统计词频 ---
void countFrequency(const string& text, SeqList<FreqPair>& outFreqs) {
    for (size_t i = 0; i < text.length();) {
        int char_len = 1;
        // 判断 UTF-8 字符字节数
        unsigned char c = static_cast<unsigned char>(text[i]);
        if ((c & 0xE0) == 0xC0) char_len = 2;
        else if ((c & 0xF0) == 0xE0) char_len = 3;
        else if ((c & 0xF8) == 0xF0) char_len = 4;
        
        // 边界检查
        if (i + char_len > text.length()) char_len = 1;

        string sub = text.substr(i, char_len);
        bool found = false;
        // 在线性表中查找
        for (int k = 0; k < outFreqs.size(); ++k) {
            if (outFreqs[k].data == sub) {
                outFreqs[k].weight++;
                found = true;
                break;
            }
        }
        // 未找到则添加
        if (!found && !outFreqs.full()) {
            outFreqs.add(FreqPair(sub, 1));
        }
        i += char_len;
    }
}

// --- 包装器：用于在通用堆中比较节点指针 ---
struct NodeWrapper {
    HuffmanNode* node;

    NodeWrapper(HuffmanNode* n = nullptr) : node(n) {}

    // 重载 < 运算符，用于 MinHeap (PriorityQueue 默认是 MinHeap)
    bool operator<(const NodeWrapper& other) const {
        if (!node) return false;
        if (!other.node) return true;
        return *node < *other.node;
    }
    
    bool operator>(const NodeWrapper& other) const {
        if (!node) return true;
        if (!other.node) return false;
        return *node > *other.node;
    }
};

// --- HuffmanTree 实现 ---

HuffmanTree::HuffmanTree(SeqList<FreqPair>& frequencies) : codes(256) {
    // 使用 PriorityQueue 替代 MinHeap
    PriorityQueue<NodeWrapper> pq(frequencies.size() + 10);

    // 1. 初始化堆
    for (int i = 0; i < frequencies.size(); ++i) {
        HuffmanData data(frequencies[i].data, frequencies[i].weight);
        // enqueue 替代 push
        pq.enqueue(NodeWrapper(new HuffmanNode(data)));
    }

    // 2. 构建哈夫曼树
    while (pq.size() > 1) {
        // dequeue 替代 pop
        HuffmanNode* left = pq.dequeue().node;
        HuffmanNode* right = pq.dequeue().node;

        int sumWeight = left->data.weight + right->data.weight;
        HuffmanNode* parent = new HuffmanNode(HuffmanData("", sumWeight));
        
        parent->left = left;
        parent->right = right;

        pq.enqueue(NodeWrapper(parent));
    }

    // 3. 设置树根
    if (!pq.empty()) {
        this->root = pq.dequeue().node;
    } else {
        this->root = nullptr;
    }
}

void HuffmanTree::generateCodesRecursive(HuffmanNode* node, string currentCode) {
    if (!node) return;

    if (!node->left && !node->right) {
        if (codes.full()) return;
        codes.add(CodePair(node->data.charData, currentCode));
    }

    generateCodesRecursive(node->left, currentCode + "0");
    generateCodesRecursive(node->right, currentCode + "1");
}

void HuffmanTree::generateCodes() {
    if (!root) return;
    
    if (!root->left && !root->right) {
        codes.add(CodePair(root->data.charData, "0"));
        return;
    }
    
    generateCodesRecursive(root, "");
}

// 计算 WPL
void HuffmanTree::calculateWPLRecursive(HuffmanNode* node, int depth, double& wpl) {
    if (!node) return;

    if (!node->left && !node->right) {
        wpl += node->data.weight * depth;
    }

    calculateWPLRecursive(node->left, depth + 1, wpl);
    calculateWPLRecursive(node->right, depth + 1, wpl);
}

double HuffmanTree::getWPL() {
    double wpl = 0.0;
    calculateWPLRecursive(root, 0, wpl);
    return wpl;
}

string HuffmanTree::getCode(const string& character) {
    for (int i = 0; i < codes.size(); ++i) {
        if (codes[i].data == character) {
            return codes[i].code;
        }
    }
    return "";
}

string HuffmanTree::compress(const string& text) {
    string compressedText = "";
    for (size_t i = 0; i < text.length();) {
        int char_len = 1;
        unsigned char c = static_cast<unsigned char>(text[i]);
        if ((c & 0xE0) == 0xC0) char_len = 2;
        else if ((c & 0xF0) == 0xE0) char_len = 3;
        else if ((c & 0xF8) == 0xF0) char_len = 4;
        
        if (i + char_len > text.length()) char_len = 1;

        string sub = text.substr(i, char_len);
        compressedText += getCode(sub);
        
        i += char_len;
    }
    return compressedText;
}

string HuffmanTree::decompress(const string& compressedText) {
    if (!root) return "";
    
    string decompressedText = "";
    HuffmanNode* currentNode = root;
    
    for (char bit : compressedText) {
        if (bit == '0') {
            if (currentNode->left) currentNode = currentNode->left;
        } else {
            if (currentNode->right) currentNode = currentNode->right;
        }

        if (!currentNode->left && !currentNode->right) {
            decompressedText += currentNode->data.charData;
            currentNode = root; 
        }
    }
    return decompressedText;
}

void HuffmanTree::printCodes() {
    cout << "Huffman Codes:" << endl;
    for (int i = 0; i < codes.size(); ++i) {
        cout << "'" << codes[i].data << "' : " << codes[i].code << endl;
    }
}