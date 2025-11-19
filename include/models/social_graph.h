#ifndef SOCIAL_GRAPH_H
#define SOCIAL_GRAPH_H

#include <cstddef>
#include "data_structure/lin_list.h"

class SocialGraph {
public:
    // 构造空图
    SocialGraph();
    // 以顶点数量初始化图，初始无边
    SocialGraph(std::size_t vertexCount);
    ~SocialGraph();
    SocialGraph(const SocialGraph&) = delete;
    SocialGraph& operator=(const SocialGraph&) = delete;

    // 调整顶点数量并清空边
    void resize(std::size_t vertexCount);
    // 添加无向边（好友关系）
    void addEdge(int u, int v);
    // 移除无向边
    void removeEdge(int u, int v);
    // 获取顶点数量
    std::size_t vertexCount() const;
    // 获取指定顶点的度
    int degree(int vertex) const;
    // 返回度数最大的顶点索引
    int indexWithMaxDegree() const;
    // BFS 计算最短路径，结果写入 path，成功返回 true
    bool shortestPath(int start, int target, LinkList<int>& path) const;
    // 获取指定顶点的邻接表（好友索引列表）指针，如果顶点非法，返回 nullptr，用于数据保存
    const LinkList<int>* getNeighbors(int u) const;

private:
    bool validVertex(int v) const;

    SeqList<LinkList<int>>* userForm;
    std::size_t vertex_count_;
};

#endif