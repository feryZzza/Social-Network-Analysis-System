#include "models/social_graph.h"

#include <algorithm>
#include "data_structure/queue.h"

SocialGraph::SocialGraph(std::size_t vertexCount) : userForm(vertexCount) {}

void SocialGraph::resize(std::size_t vertexCount) {
    userForm.assign(vertexCount, {});
}

void SocialGraph::addEdge(int u, int v) {
    if (!validVertex(u) || !validVertex(v)) {
        return;
    }
    // 避免重复边
    auto &neighborsU = userForm[u];
    if (std::find(neighborsU.begin(), neighborsU.end(), v) == neighborsU.end()) {
        neighborsU.push_back(v);
    }
    auto &neighborsV = userForm[v];
    if (std::find(neighborsV.begin(), neighborsV.end(), u) == neighborsV.end()) {
        neighborsV.push_back(u);
    }
}

std::size_t SocialGraph::vertexCount() const {
    return userForm.size();
}

int SocialGraph::degree(int vertex) const {
    if (!validVertex(vertex)) {
        return 0;
    }
    return static_cast<int>(userForm[vertex].size());
}

std::vector<int> SocialGraph::degrees() const {
    std::vector<int> result;
    result.reserve(userForm.size());
    for (const auto &neighbors : userForm) {
        result.push_back(static_cast<int>(neighbors.size()));
    }
    return result;
}

int SocialGraph::indexWithMaxDegree() const {
    int maxIndex = -1;
    int maxDegree = -1;
    for (std::size_t i = 0; i < userForm.size(); ++i) {
        const int currentDegree = static_cast<int>(userForm[i].size());
        if (currentDegree > maxDegree) {
            maxDegree = currentDegree;
            maxIndex = static_cast<int>(i);
        }
    }
    return maxIndex;
}

bool SocialGraph::validVertex(int v) const {
    return v >= 0 && static_cast<std::size_t>(v) < userForm.size();
}

std::vector<int> SocialGraph::shortestPath(int start, int target) const {
    if (!validVertex(start) || !validVertex(target)) {
        return {};
    }

    std::vector<int> parent(userForm.size(), -1);
    std::vector<bool> visited(userForm.size(), false);
    LinkQueue<int> q;

    visited[start] = true;
    q.enqueue(start);

    while (!q.empty()) {
        int current = q.dequeue();
        if (current == target) {
            break;
        }
        for (int neighbor : userForm[current]) {
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

