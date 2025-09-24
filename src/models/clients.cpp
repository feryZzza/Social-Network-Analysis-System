#include "models/clients.h"
#include "models/Post.h"

bool Client::undo(){//撤销上一次操作
    if(a_stack.empty()){
        cout<<"没有操作可以撤销"<<endl;
        return false;
    }
    Action* a = a_stack.pop();
    a->undo();
    return true;
}

void Client::addPost(Post p){
    p.set_author(this);
    posts.add(p);
}
void Client::deletePost(int index){//因为有撤销功能，删除帖子只是将其从列表中移除，并不真正删除，并且将删除操作压入操作栈
    posts.fake_remove(index);
    return;
}

void Client::addComment(Post* post, Comment &comment){
    post->addComment(comment,this);
    return;
}

void Client::like(Post* post,bool undo){//点赞帖子
    post->receive_likes(this,undo);
    return;
}
//重载输出
std::ostream& operator<< (std::ostream& os,Client& c) {//重载输出
    os << "昵称: " << c.name << "\n";
    os << "ID: " << c.id << "\n";
    os << "密码: " << c.password << "\n";
    os << "帖子: " << c.posts << "\n";
    return os;
}