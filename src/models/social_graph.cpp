#include "models/social_graph.h"

#include <algorithm>
#include "data_structure/queue.h"

SocialGraph::SocialGraph(std::size_t vertexCount) : adjacency_(vertexCount) {}

void SocialGraph::resize(std::size_t vertexCount) {
    adjacency_.assign(vertexCount, {});
}

void SocialGraph::addEdge(int u, int v) {
    if (!validVertex(u) || !validVertex(v)) {
        return;
    }
    // 避免重复边
    auto &neighborsU = adjacency_[u];
    if (std::find(neighborsU.begin(), neighborsU.end(), v) == neighborsU.end()) {
        neighborsU.push_back(v);
    }
    auto &neighborsV = adjacency_[v];
    if (std::find(neighborsV.begin(), neighborsV.end(), u) == neighborsV.end()) {
        neighborsV.push_back(u);
    }
}

std::size_t SocialGraph::vertexCount() const {
    return adjacency_.size();
}

int SocialGraph::degree(int vertex) const {
    if (!validVertex(vertex)) {
        return 0;
    }
    return static_cast<int>(adjacency_[vertex].size());
}

std::vector<int> SocialGraph::degrees() const {
    std::vector<int> result;
    result.reserve(adjacency_.size());
    for (const auto &neighbors : adjacency_) {
        result.push_back(static_cast<int>(neighbors.size()));
    }
    return result;
}

int SocialGraph::indexWithMaxDegree() const {
    int maxIndex = -1;
    int maxDegree = -1;
    for (std::size_t i = 0; i < adjacency_.size(); ++i) {
        const int currentDegree = static_cast<int>(adjacency_[i].size());
        if (currentDegree > maxDegree) {
            maxDegree = currentDegree;
            maxIndex = static_cast<int>(i);
        }
    }
    return maxIndex;
}

std::vector<int> SocialGraph::shortestPath(int start, int target) const {
    if (!validVertex(start) || !validVertex(target)) {
        return {};
    }

    std::vector<int> parent(adjacency_.size(), -1);
    std::vector<bool> visited(adjacency_.size(), false);
    LinkQueue<int> q;

    visited[start] = true;
    q.enqueue(start);

    while (!q.empty()) {
        int current = q.dequeue();
        if (current == target) {
            break;
        }
        for (int neighbor : adjacency_[current]) {
            if (!visited[neighbor]) {
                visited[neighbor] = true;
                parent[neighbor] = current;
                q.enqueue(neighbor);
            }
        }
    }

    if (!visited[target]) {
        return {};
    }

    std::vector<int> path;
    for (int v = target; v != -1; v = parent[v]) {
        path.push_back(v);
    }
    std::reverse(path.begin(), path.end());
    return path;
}

bool SocialGraph::validVertex(int v) const {
    return v >= 0 && static_cast<std::size_t>(v) < adjacency_.size();
}
