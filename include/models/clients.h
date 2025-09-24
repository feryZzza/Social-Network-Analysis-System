#ifndef STACK_H
#define STACK_H
#include<iostream>
#include<string>
#include"models/Post.h"
#include"data_structure/lin_list.h"
#include"data_structure/stack.h"
#include"data_structure/queue.h"

using namespace std;

class Post;

class Client{
public:
    Client() {}
    Client(string name, string id, string password) : name(name), id(id), password(password) {}
private:
    string name;//用户昵称
    string id;//用户ID
    string password;//用户密码
    //帖子列表
    LinkList<Post> posts; //用户发布的帖子列表
    
};
#endif