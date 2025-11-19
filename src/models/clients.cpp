#include "models/clients.h"
#include "models/Post.h"
#include "models/action.h"
#include "models/message.h"


//现在Client只负责存储和管理自己的数据结构，具体的操作逻辑交给Core来处理
Client::~Client(){
    while(!a_stack.empty()){
        Action* a = a_stack.pop();
        a->invalidate();
        delete a;
    }
    while (!m_q.empty()){
        massege* m = m_q.dequeue();
        delete m;
    }
}

bool Client::hasFriend(Client* other) {
    if (!other) return false;
    for (int i = 0; i < friends.size(); ++i) {
        if (friends[i] == other) {
            return true;
        }
    }
    return false;
}

void Client::addFriendLink(Client* other) {
    if (!other || other == this || hasFriend(other)) return;
    friends.add(other);
}

bool Client::removeFriendLink(Client* other) {
    if (!other) return false;
    for (int i = 0; i < friends.size(); ++i) {
        if (friends[i] == other) {
            friends.remove(i);
            return true;
        }
    }
    return false;
}

std::ostream& operator<< (std::ostream& os, Client& c) {
    os << "昵称: " << c.name << "\n";
    os << "ID: " << c.id << "\n";
    os << "帖子数量: " << c.posts.size() << "\n";
    os << "帖子列表: \n" << c.posts << "\n";
    os << "好友数量: " << c.friends.size() << "\n";
    os << "好友列表: [";
    for (int i = 0; i < c.friends.size(); ++i) {
        Client* f = c.friends[i];
        if (f) {
            os << f->Name() << "(" << f->ID() << ")";
        }
        if (i < c.friends.size() - 1) os << ", ";
    }
    os << "]\n";
    return os;
}
