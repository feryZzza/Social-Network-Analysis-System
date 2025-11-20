#ifndef SORT_H
#define SORT_H
#include <iostream>
#include "data_structure/lin_list.h"
#include "models/clients.h"
#include "models/Post.h"

// --- Sorter 类 ---
// 职责：管理索引顺序表，提供排行榜生成功能
// "haoyoushu" (好友数排行) 和 "dianzanshu" (点赞数排行) 为对外接口
class Sorter {
public:
    Sorter();
    ~Sorter();

    // 禁用拷贝
    Sorter(const Sorter&) = delete;
    Sorter& operator=(const Sorter&) = delete;
    
    // 生成用户影响力排行榜 (基于好友数)
    void haoyoushu(SeqList<Client>& clients);

    // 生成全局热门帖子排行榜 (基于点赞数)
    void dianzanshu(SeqList<Client>& users);

private:

    SeqList<Client*>* clientIndex = nullptr;
    SeqList<Post*>* postIndex = nullptr;
    
    // 初始化索引表
    void initClientIndices(SeqList<Client>& source);
    void initPostIndices(LinkList<Post>& source);
    void initGlobalPostIndices(SeqList<Client>& users);

    // 排序，使用函数指针作为比较依据
    void sortClientIndices(bool (*compare)(const Client&, const Client&));
    void sortPostIndices(bool (*compare)(const Post&, const Post&));
};

#endif