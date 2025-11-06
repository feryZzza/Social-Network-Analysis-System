#ifndef ACTION_H
#define ACTION_H
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

class Action{//用来表示操作，用于实现模块二撤销功能
public:
    Action() {}
    virtual ~Action() {}
    virtual bool undo(){cout<<"action基类"<<endl;return false;};//主动从栈中弹出操作并撤销
    bool check();
    bool is_add = 0;//是添加操作还是删除操作
    void init(Client* client,bool add,Post* p);
    bool used = 0;//操作是否已经被撤销过
    
    Client* client = nullptr;//操作的用户
    Client* poster = nullptr;//操作涉及的帖子的作者
    int index = -1;//操作涉及帖子的位置
    Post* post = nullptr;//操作涉及的帖子
};

class PostAction: public Action{//发帖操作
public:
    PostAction(ListNode<Post>* post_node) : post_node(post_node) {}
    PostAction() {}
    ~PostAction() override;

    bool undo() override;//主动从栈中弹出操作并撤销
    //每次彻底删除帖子时调用，检查该操作的帖子是否被删除，若被删除则将post指针置为空,防止野指针
    ListNode<Post>* post_node;//操作的帖子的节点指针
};


class LikeAction: public Action{//点赞操作
public:
    LikeAction() {}
    ~LikeAction() override {};

    bool undo() override;//主动从栈中弹出操作并撤销
    //每次彻底删除帖子时调用，检查该操作的帖子是否被删除，若被删除则将post指针置为空,防止野指针
};

class CommentAction: public Action{//评论操作
public:
    CommentAction(ListNode<Comment>* comment_node) : comment_node(comment_node) {}
    CommentAction() {}
    ~CommentAction() override;

    bool undo() override;//主动从栈中弹出操作并撤销
    //每次彻底删除帖子时调用，检查该操作的帖子是否被删除，若被删除则将post指针置为空,防止野指针
    ListNode<Comment>* comment_node;//操作的评论的节点指针
};

#endif
