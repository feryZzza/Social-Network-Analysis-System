#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <string>
#include <iostream>
#include "data_structure/lin_list.h"
#include "data_structure/tree.h"
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
    
    // 比较运算符，虽然我们在 NodeWrapper 里处理了比较，但保留这些是个好习惯
    bool operator<(const HuffmanData& other) const { return weight < other.weight; }
    bool operator>(const HuffmanData& other) const { return weight > other.weight; }
    bool operator==(const HuffmanData& other) const { return weight == other.weight; }
};

// 特化 TreeNode
typedef TreeNode<HuffmanData> HuffmanNode;



// 频率对 (替代 map<string, int>)
struct FreqPair {
    string data;
    int weight;
    FreqPair(string d = "", int w = 0) : data(d), weight(w) {}
    bool operator==(const FreqPair& other) const { return data == other.data; }
};

// 编码对 (替代 map<string, string>)
struct CodePair {
    string data;
    string code;
    CodePair(string d = "", string c = "") : data(d), code(c) {}
    bool operator==(const CodePair& other) const { return data == other.data; }
};

// --- 哈夫曼树类 ---
class HuffmanTree : public BinaryTree<HuffmanData> {
public:
    // 构造函数：接收频率列表构建树
    HuffmanTree(SeqList<FreqPair>& frequencies);
    
    // 析构函数：基类 BinaryTree 会自动释放节点内存
    ~HuffmanTree() = default;

    // 核心功能
    void generateCodes();
    string compress(const string& text);
    string decompress(const string& compressedText);
    
    // 原有功能：计算带权路径长度
    double getWPL();

    // 调试功能
    void printCodes();

private:
    SeqList<CodePair> codes; // 存储生成的编码表

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