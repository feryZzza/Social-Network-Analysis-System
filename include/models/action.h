#ifndef ACTION_H
#define ACTION_H
#include <iostream>
#include "data_structure/lin_list.h"
#include "data_structure/stack.h"
#include "data_structure/queue.h"
#include "models/clients.h"
#include "models/Post.h"
class Action{//用来表示操作，用于实现模块二撤销功能
public:
    Action() {}
    //Client* client; //操作的用户
};
#endif