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
    virtual bool undo(){return false;};//主动从栈中弹出操作并撤销
    bool is_add = 0;//是添加操作还是删除操作
    bool used = 0;//操作是否已经被撤销过
    int index = -1;//操作的对象的位置
    Client* client = nullptr;//操作的用户
};


class LikeAction: public Action{//点赞操作
public:
    LikeAction(Post* post) : post(post) {}
    LikeAction() {}
    ~LikeAction();

    void init(Client* client,bool add){this->client = client,is_add=add;}//初始化
    bool undo() override;//主动从栈中弹出操作并撤销
    Post* post;//操作的帖子
};

#endif