#include "models/social_graph.h"

#include "data_structure/queue.h"

SocialGraph::SocialGraph() : userForm(nullptr), vertex_count_(0) {}

SocialGraph::SocialGraph(std::size_t vertexCount) : userForm(nullptr), vertex_count_(0) {
    resize(vertexCount);
}

SocialGraph::~SocialGraph() {
    delete userForm;
    userForm = nullptr;
}

void SocialGraph::resize(std::size_t vertexCount) {
    delete userForm;
    userForm = nullptr;
    vertex_count_ = vertexCount;
    if (vertexCount == 0) {
        return;
    }
    userForm = new SeqList<LinkList<int>>(static_cast<int>(vertexCount), 
                                          static_cast<int>(vertexCount));
}

void SocialGraph::addEdge(int u, int v) {
    if (!validVertex(u) || !validVertex(v) || userForm == nullptr) {
        return;
    }

    LinkList<int>& neighborsU = (*userForm)[u];
    bool exists = false;
    for (int i = 0; i < neighborsU.size(); ++i) {
        if (neighborsU[i] == v) {
            exists = true;
            break;
        }
    }
    if (!exists) {
        neighborsU.add(v);
    }

    LinkList<int>& neighborsV = (*userForm)[v];
    exists = false;
    for (int i = 0; i < neighborsV.size(); ++i) {
        if (neighborsV[i] == u) {
            exists = true;
            break;
        }
    }
    if (!exists) {
        neighborsV.add(u);
    }
}

void SocialGraph::removeEdge(int u, int v) {
    if (!validVertex(u) || !validVertex(v) || userForm == nullptr) {
        return;
    }
    LinkList<int>& neighborsU = (*userForm)[u];
    for (int i = 0; i < neighborsU.size(); ++i) {
        if (neighborsU[i] == v) {
            neighborsU.remove(i);
            break; 
        }
    }
    LinkList<int>& neighborsV = (*userForm)[v];
    for (int i = 0; i < neighborsV.size(); ++i) {
        if (neighborsV[i] == u) {
            neighborsV.remove(i);
            break;
        }
    }
}

std::size_t SocialGraph::vertexCount() const {
    return vertex_count_;
}

int SocialGraph::degree(int vertex) const {
    if (!validVertex(vertex) || userForm == nullptr) {
        return 0;
    }
    return (*userForm)[vertex].size();
}

int SocialGraph::indexWithMaxDegree() const {
    if (userForm == nullptr || vertex_count_ == 0) {
        return -1;
    }

    int maxIndex = -1;
    int maxDegree = -1;
    for (int i = 0; i < static_cast<int>(vertex_count_); ++i) {
        int currentDegree = (*userForm)[i].size();
        if (currentDegree > maxDegree) {
            maxDegree = currentDegree;
            maxIndex = i;
        }
    }
    return maxIndex;
}

bool SocialGraph::shortestPath(int start, int target, LinkList<int>& path) const {
    if (!validVertex(start) || !validVertex(target) || userForm == nullptr) {
        return false;
    }

    while (!path.empty()) {
        path.remove(0);
    }

    const int count = static_cast<int>(vertex_count_);
    SeqList<int> father(count, count);
    SeqList<bool> visited(count, count);
    for (int i = 0; i < count; ++i) {
        father.setx(i, -1);
        visited.setx(i, false);
    }

    LinkQueue<int> q;
    visited.setx(start, true);
    q.enqueue(start);

    while (!q.empty()) {
        int current = q.dequeue();
        if (current == target) {
            break;
        }
        LinkList<int>& neighbors = (*userForm)[current];
        for (int i = 0; i < neighbors.size(); ++i) {
            int neighbor = neighbors[i];
            bool visitedFlag;
            visited.getx(neighbor, visitedFlag);
            if (!visitedFlag) {
                visited.setx(neighbor, true);
                father.setx(neighbor, current);
                q.enqueue(neighbor);
            }
        }
    }

    bool targetVisited;
    visited.getx(target, targetVisited);
    if (!targetVisited) {
        return false;
    }

    SeqList<int> reversed(count);
    for (int v = target; v != -1; ) {
        reversed.add(v);
        int fatherValue;
        father.getx(v, fatherValue);
        v = fatherValue;
    }

    for (int i = reversed.size() - 1; i >= 0; --i) {
        path.add(reversed[i]);
    }
    return true;
}

bool SocialGraph::validVertex(int v) const {
    return v >= 0 && static_cast<std::size_t>(v) < vertex_count_;
}

const LinkList<int>* SocialGraph::getNeighbors(int u) const {
    if (!validVertex(u) || userForm == nullptr) {
        return nullptr;
    }
    return &(*userForm)[u];
}