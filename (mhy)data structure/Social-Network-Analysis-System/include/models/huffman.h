#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <string>
#include <vector>
#include <map>
#include <queue>
#include <memory> // 用于智能指针

using namespace std;

// 哈夫曼树节点
struct HuffmanNode {
    string data; // 字符
    int weight; // 权重（频率）
    shared_ptr<HuffmanNode> left;
    shared_ptr<HuffmanNode> right;

    HuffmanNode(string d, int w) : data(d), weight(w), left(nullptr), right(nullptr) {}
};

// 用于优先队列的比较结构
struct CompareNodes {
    bool operator()(const shared_ptr<HuffmanNode>& a, const shared_ptr<HuffmanNode>& b) const {
        return a->weight > b->weight; // 最小堆
    }
};

class HuffmanTree {
public:
    // 构造函数
    HuffmanTree(const map<string, int>& frequencies);

    // 生成哈夫曼编码
    void generateCodes();

    // 压缩文本
    string compress(const string& text);

    // 解压文本
    string decompress(const string& compressedText);

    // 计算加权路径长度 (WPL)
    double getWPL();

    // 打印哈夫曼编码
    void printCodes();

private:
    shared_ptr<HuffmanNode> root;
    map<string, string> codes;

    // 递归辅助函数来生成编码
    void generateCodesRecursive(const shared_ptr<HuffmanNode>& node, const string& code);
    // 递归辅助函数来计算WPL
    void calculateWPLRecursive(const shared_ptr<HuffmanNode>& node, int depth, double& wpl);
};

// 词频统计函数
map<string, int> countFrequency(const string& text);

#endif // HUFFMAN_H