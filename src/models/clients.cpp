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

std::ostream& operator<< (std::ostream& os, Client& c) {
    os << "昵称: " << c.name << "\n";
    os << "ID: " << c.id << "\n";
    os << "帖子数量: " << c.posts.size() << "\n";
    os << "帖子列表: \n" << c.posts << "\n";
    return os;
}