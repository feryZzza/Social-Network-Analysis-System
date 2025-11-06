#ifndef SOCIAL_GRAPH_H
#define SOCIAL_GRAPH_H

#include <vector>

class SocialGraph {
public:

    SocialGraph() = default;
    // 构造函数，仅有顶点，无边情况
    SocialGraph(std::size_t vertexCount);

    // 调整顶点数量
    void resize(std::size_t vertexCount);

    // 在顶点 u 与 v 之间添加无向边，即好友关系
    void addEdge(int u, int v);

    // 查询顶点数量
    std::size_t vertexCount() const;

    // 查询指定顶点的度，即好友数
    int degree(int vertex) const;

    // 查询每个顶点的度数数组
   std::vector<int> degrees() const;

    // 找到度最大的顶点索引
   int indexWithMaxDegree() const;

    // 使用BFS计算最短路径顶点序列
    std::vector<int> shortestPath(int start, int target) const;

private:
    // 检查顶点索引是否有效
    bool validVertex(int v) const;

    std::vector<std::vector<int>> userForm;
};

#endif
