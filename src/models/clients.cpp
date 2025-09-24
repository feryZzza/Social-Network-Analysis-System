#include "models/clients.h"

bool Client::undo(){//撤销上一次操作
    if(a_stack.empty()) return false;
    Action a = a_stack.pop();
    //根据操作类型进行撤销
    //暂时不实现
    return true;
}

void Client::addPost(Post p){
    p.set_author(this);
    posts.add(p);
}
void Client::deletePost(int index){
    posts.fake_remove(index);
}
//重载输出
std::ostream& operator<< (std::ostream& os,Client& c) {//重载输出
    os << "昵称: " << c.name << "\n";
    os << "ID: " << c.id << "\n";
    os << "密码: " << c.password << "\n";
    os << "帖子: " << c.posts << "\n";
    return os;
}