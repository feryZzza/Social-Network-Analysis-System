#ifndef SOCIAL_GRAPH_H
#define SOCIAL_GRAPH_H

#include <vector>

// 邻接表实现的社交关系图，顶点代表用户，边代表好友/关注关系
class SocialGraph {
public:
    SocialGraph() = default;
    explicit SocialGraph(std::size_t vertexCount);

    void resize(std::size_t vertexCount);
    void addEdge(int u, int v);
    std::size_t vertexCount() const;

    int degree(int vertex) const;
    std::vector<int> degrees() const;
    int indexWithMaxDegree() const;

    std::vector<int> shortestPath(int start, int target) const;

private:
    bool validVertex(int v) const;

    std::vector<std::vector<int>> adjacency_;
};

#endif
