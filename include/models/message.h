#ifndef MESSEGE_H
#define MESSEGE_H
#include <iostream>
#include "data_structure/lin_list.h"
#include "data_structure/stack.h"
#include "data_structure/queue.h"
#include <string>
#include "models/clients.h"
#include "models/Post.h"
#include "models/comment.h"

using namespace std;

class Client;
class Comment;
class Post;

class massege{//用来表示消息
public:
    massege() {}
    virtual ~massege() {}
    virtual bool show(){cout<<"massege基类"<<endl;return false;};//显示消息
    void init(Client* sender,Client* receiver,Post* p = nullptr){
        this->sender = sender;
        this->receiver = receiver;
        this->post = p;
    }
    
    Client* sender = nullptr;//消息发送者
    Client* receiver = nullptr;//消息接收者
    Post* post = nullptr;//消息涉及的帖子
};

class CommentMassege: public massege{//评论消息
public:
    CommentMassege(Comment* c) : comment(c) {}
    CommentMassege() {}
    ~CommentMassege() = default;
    
    bool show() override;//显示消息
    Comment* comment = nullptr;//评论
};

class LikeMassege: public massege{//点赞消息
public:
    LikeMassege() {}
    ~LikeMassege() = default;
    
    bool show() override;//显示消息
    
};



#endif
