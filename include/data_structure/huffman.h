#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <string>
#include <iostream>
#include "data_structure/lin_list.h"
#include "data_structure/tree.h" // 依然引用 TreeNode 定义
#include "data_structure/heap.h"

using std::string;

// 哈夫曼树节点具体存储的数据
class HuffmanData {
public:
    string charData; // 字符
    int weight;      // 权重

    HuffmanData(string c = "", int w = 0) : charData(c), weight(w) {}
    
    friend std::ostream& operator<<(std::ostream& os, const HuffmanData& d) {
        os << "[" << d.charData << ":" << d.weight << "]";
        return os;
    }
    
    bool operator<(const HuffmanData& other) const { return weight < other.weight; }
    bool operator>(const HuffmanData& other) const { return weight > other.weight; }
    bool operator==(const HuffmanData& other) const { return weight == other.weight; }
};

// 使用 tree.h 中的 TreeNode，但不继承 BinaryTree 类
typedef TreeNode<HuffmanData> HuffmanNode;

// 频率对
struct FreqPair {
    string data;
    int weight;
    FreqPair(string d = "", int w = 0) : data(d), weight(w) {}
    bool operator==(const FreqPair& other) const { return data == other.data; }
};

// 编码对
struct CodePair {
    string data;
    string code;
    CodePair(string d = "", string c = "") : data(d), code(c) {}
    bool operator==(const CodePair& other) const { return data == other.data; }
};

// --- 哈夫曼树类 (独立类) ---
class HuffmanTree {
public:
    // 构造函数：接收频率列表构建树
    HuffmanTree(SeqList<FreqPair>& frequencies);
    
    // 析构函数：需要自己实现内存清理
    ~HuffmanTree();

    // 显式禁用拷贝构造和赋值，防止 Double Free
    HuffmanTree(const HuffmanTree&) = delete;
    HuffmanTree& operator=(const HuffmanTree&) = delete;

    // 核心功能
    void generateCodes();
    string compress(const string& text);
    string decompress(const string& compressedText);
    
    // 计算带权路径长度
    double getWPL();

    // 调试功能
    void printCodes();

private:
    HuffmanNode* root;       // 树根指针 (自己管理)
    SeqList<CodePair> codes; // 存储生成的编码表

    // 递归销毁树
    void destroy(HuffmanNode* node);

    // 递归辅助生成编码
    void generateCodesRecursive(HuffmanNode* node, string currentCode);
    
    // 递归辅助计算 WPL
    void calculateWPLRecursive(HuffmanNode* node, int depth, double& wpl);

    // 辅助查找字符编码
    string getCode(const string& character);
};

// 工具函数：统计词频
void countFrequency(const string& text, SeqList<FreqPair>& outFreqs);

#endif // HUFFMAN_H