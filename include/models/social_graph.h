#ifndef SOCIAL_GRAPH_H
#define SOCIAL_GRAPH_H

#include <cstddef>
#include "data_structure/lin_list.h"

struct TraversalStep {
    int vertex;
    int depth;
    TraversalStep(int v = -1, int d = 0) : vertex(v), depth(d) {}
};

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
    // DFS 搜索路径（可能不是最短），用于展示深度优先遍历结果
    bool depthFirstPath(int start, int target, LinkList<int>& path) const;
    // BFS 层级遍历
    bool breadthFirstTraversal(int start, LinkList<TraversalStep>& order) const;
    // DFS 前序遍历
    bool depthFirstTraversal(int start, LinkList<TraversalStep>& order) const;
    // 获取指定顶点的邻接表（好友索引列表）指针，如果顶点非法，返回 nullptr，用于数据保存
    const LinkList<int>* getNeighbors(int u) const;

private:
    bool validVertex(int v) const;
    bool depthFirstSearchPath(int current, int target, SeqList<bool>& visited,
                          SeqList<int>& buffer, LinkList<int>& path) const;
    void depthFirstTraversalInternal(int current, int depth, SeqList<bool>& visited,
                                     LinkList<TraversalStep>& order) const;

    SeqList<LinkList<int>>* userForm;
    std::size_t vertex_count_;
};

#endif
