#include "data_structure/huffman.h"
#include <iostream>
#include <vector>

using namespace std;

// 词频统计函数实现
map<string, int> countFrequency(const string& text) {
    map<string, int> frequencies;
    for (size_t i = 0; i < text.length();) {
        int char_len = 1;
        // 判断UTF-8字符字节数
        if ((text[i] & 0xE0) == 0xC0) { // 2字节
            char_len = 2;
        } else if ((text[i] & 0xF0) == 0xE0) { // 3字节
            char_len = 3;
        } else if ((text[i] & 0xF8) == 0xF0) { // 4字节
            char_len = 4;
        }
        // 确保不会超出字符串范围
        if (i + char_len > text.length()) {
            char_len = 1;
        }
        frequencies[text.substr(i, char_len)]++;
        i += char_len;
    }
    return frequencies;
}


// HuffmanTree 构造函数
HuffmanTree::HuffmanTree(const map<string, int>& frequencies) {
    priority_queue<shared_ptr<HuffmanNode>, vector<shared_ptr<HuffmanNode>>, CompareNodes> pq;

    // 1. 创建叶子节点并加入优先队列
    for (auto const& [key, val] : frequencies) {
        pq.push(make_shared<HuffmanNode>(key, val));
    }

    // 2. 构建哈夫曼树
    while (pq.size() > 1) {
        // 取出权重最小的两个节点
        shared_ptr<HuffmanNode> left = pq.top();
        pq.pop();
        shared_ptr<HuffmanNode> right = pq.top();
        pq.pop();

        // 创建新的内部节点
        auto parent = make_shared<HuffmanNode>("", left->weight + right->weight);
        parent->left = left;
        parent->right = right;
        pq.push(parent);
    }

    root = pq.top();
}

// 递归生成哈夫曼编码
void HuffmanTree::generateCodesRecursive(const shared_ptr<HuffmanNode>& node, const string& code) {
    if (!node) {
        return;
    }
    // 如果是叶子节点，存储编码
    if (!node->left && !node->right) {
        codes[node->data] = code;
    }
    generateCodesRecursive(node->left, code + "0");
    generateCodesRecursive(node->right, code + "1");
}

// 公有接口：生成编码
void HuffmanTree::generateCodes() {
    generateCodesRecursive(root, "");
}

// 压缩
string HuffmanTree::compress(const string& text) {
    string compressedText = "";
    for (size_t i = 0; i < text.length();) {
        int char_len = 1;
        if ((text[i] & 0xE0) == 0xC0) {
            char_len = 2;
        } else if ((text[i] & 0xF0) == 0xE0) {
            char_len = 3;
        } else if ((text[i] & 0xF8) == 0xF0) {
            char_len = 4;
        }
        if (i + char_len > text.length()) {
            char_len = 1;
        }
        compressedText += codes[text.substr(i, char_len)];
        i += char_len;
    }
    return compressedText;
}


// 解压 (包含健壮性纠正)
string HuffmanTree::decompress(const string& compressedText) {
    string decompressedText = "";
    auto currentNode = root;

    // 健壮性检查：如果哈夫曼树为空
    if (!currentNode) {
        return "";
    }

    for (char bit : compressedText) {
        if (bit == '0') {
            currentNode = currentNode->left;
        } else if (bit == '1') { // 明确只处理 '0' 和 '1'
            currentNode = currentNode->right;
        } else {
            // 遇到无效字符，说明编码已损坏
            std::cerr << "Error: Invalid character in compressed stream: " << bit << std::endl;
            return ""; // 返回空字符串或抛出异常
        }

        // 健壮性检查：如果编码导致遍历到空指针（说明编码序列错误）
        if (!currentNode) {
            std::cerr << "Error: Invalid compressed data sequence." << std::endl;
            return ""; // 返回空字符串或抛出异常
        }

        // 到达叶子节点
        if (!currentNode->left && !currentNode->right) {
            decompressedText += currentNode->data;
            currentNode = root; // 重置回根节点
        }
    }

    // 【关键纠正】
    // 循环结束后，必须检查 currentNode 是否回到了根节点。
    // 如果没有，说明最后的比特序列是不完整的。
    if (currentNode != root) {
        std::cerr << "Error: Compressed data is incomplete or corrupt." << std::endl;
        return ""; // 返回空字符串，表示解压失败
    }

    return decompressedText;
}


// 递归计算WPL
void HuffmanTree::calculateWPLRecursive(const shared_ptr<HuffmanNode>& node, int depth, double& wpl) {
    if (!node) {
        return;
    }
    if (!node->left && !node->right) {
        wpl += node->weight * depth;
    }
    calculateWPLRecursive(node->left, depth + 1, wpl);
    calculateWPLRecursive(node->right, depth + 1, wpl);
}

// 公有接口：获取WPL
double HuffmanTree::getWPL() {
    double wpl = 0.0;
    calculateWPLRecursive(root, 0, wpl);
    return wpl;
}

// 打印哈夫曼编码
void HuffmanTree::printCodes() {
    cout << "Huffman Codes:" << endl;
    for (auto const& [key, val] : codes) {
        cout << "'" << key << "' : " << val << endl;
    }
}