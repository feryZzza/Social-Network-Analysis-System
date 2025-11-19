#include "data_structure/huffman.h"
#include "data_structure/queue.h" // 引入 PriorityQueue
#include <iostream>

using namespace std;

//获取 UTF-8 字符长度
static int getUtf8CharLen(unsigned char c) {
    if ((c & 0xE0) == 0xC0) return 2;
    else if ((c & 0xF0) == 0xE0) return 3;
    else if ((c & 0xF8) == 0xF0) return 4;
    return 1;
}

//统计词频
void countFrequency(const string& text, SeqList<FreqPair>& outFreqs) {
    for (size_t i = 0; i < text.length();) {
        unsigned char c = static_cast<unsigned char>(text[i]);
        int char_len = getUtf8CharLen(c);
        
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

class Huffman_Node {
//为了节约空间资源，我们在优先队列（堆）中只存储指向 HuffmanNode 的指针，但是在堆中需要重载比较运算符才能正确排序
//所以我们还得需要再封装一层 Huffman_Node 来实现利用指针实现比较运算符重载
public:
    HuffmanNode* node;

    Huffman_Node(HuffmanNode* n = nullptr) : node(n) {}

    bool operator<(const Huffman_Node& other) const {
        if (!node) return false;
        if (!other.node) return true;
        return *node < *other.node;
    }
    
    bool operator>(const Huffman_Node& other) const {
        if (!node) return true;
        if (!other.node) return false;
        return *node > *other.node;
    }
};


HuffmanTree::HuffmanTree(SeqList<FreqPair>& frequencies) : root(nullptr), codes(256) {//利用频率表构建哈夫曼树
    if (frequencies.empty()) return;

    PriorityQueue<Huffman_Node> pq(frequencies.size() * 2 + 10);//预留空间

    for (int i = 0; i < frequencies.size(); ++i) {
        HuffmanData data(frequencies[i].data, frequencies[i].weight);
        pq.enqueue(Huffman_Node(new HuffmanNode(data)));
    }

    while (pq.size() > 1) {
        HuffmanNode* left = pq.dequeue().node;
        HuffmanNode* right = pq.dequeue().node;

        int sumWeight = left->data.weight + right->data.weight;
        HuffmanNode* parent = new HuffmanNode(HuffmanData("", sumWeight));
        
        parent->left = left;
        parent->right = right;

        pq.enqueue(Huffman_Node(parent));
    }

    if (!pq.empty()) {
        this->root = pq.dequeue().node;
    }
}

// 析构函数
HuffmanTree::~HuffmanTree() {
    destroy(root);
}

// 递归销毁树
void HuffmanTree::destroy(HuffmanNode* node) {
    if (node) {
        destroy(node->left);
        destroy(node->right);
        delete node;
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
        unsigned char c = static_cast<unsigned char>(text[i]);
        int char_len = getUtf8CharLen(c);
        
        if (i + char_len > text.length()) char_len = 1;

        string sub = text.substr(i, char_len);
        string code = getCode(sub);
        
        if(code == "") {// 未找到编码，跳过该字符
            i += char_len;
            continue;
        }
        
        compressedText += code;
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
    cout << "--- 哈夫曼编码表 ---" << endl;
    for (int i = 0; i < codes.size(); ++i) {
        cout << "'" << codes[i].data << "' : " << codes[i].code << endl;
    }
    cout << "-------------------" << endl;
}