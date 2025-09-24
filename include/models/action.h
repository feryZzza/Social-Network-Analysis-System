#ifndef ACTION_H
#define ACTION_H
#include <iostream>
#include "data_structure/lin_list.h"
#include "data_structure/stack.h"
#include "data_structure/queue.h"
#include <string>
#include "models/clients.h"
#include "models/Post.h"

using namespace std;

class Client;
class Post;

class Action{//用来表示操作，用于实现模块二撤销功能
public:
    Action() {}
    bool is_add = 0;//是添加操作还是删除操作
    int index = -1;//操作的对象的位置
};

class PostAction: public Action{//帖子操作
public:
    PostAction() {}
    PostAction(bool is_add, Post*  post, int index = -1) : post(post), index(index) {this->is_add = is_add;}
    Post* post;//操作的帖子
    int index;//操作的帖子在用户帖子列表中的位置，添加操作时为-1
};

class CommentAction: public Action{//评论操作
public:
    CommentAction() {}
    CommentAction(bool is_add, string comment, int post_index, int comment_index = -1) : comment(comment), post_index(post_index), comment_index(comment_index) {this->is_add = is_add;}
    string comment;//操作的评论
    int post_index;//操作的评论所在的帖子在用户帖子列表中的位置
    int comment_index;//操作的评论在帖子评论列表中的位置，添加操作时为-1
};

class LikeAction: public Action{//点赞操作
public:
    LikeAction() {}
    LikeAction(bool is_add, int post_index) : post_index(post_index) {this->is_add = is_add;}
    int post_index;//操作的点赞所在的帖子在用户帖子列表中的位置
};

#endif