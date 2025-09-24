#ifndef COMMENT_H
#define COMMENT_H

#include <iostream>
#include <string>
#include "models/clients.h"

class Client;

class Comment {
public:
    Comment() {}
    Comment(Client* author, const std::string& content,int reply_floor = -1) 
        : author(author), content(content),comment_floor(reply_floor) {}

    void set_floor(int f){Floor = f;}
    int floor(){return Floor;}

    // 重载输出
    friend std::ostream& operator<< (std::ostream& os, const Comment& c);


private:
    Client* author; // 评论作者指针
    std::string content; // 评论内容
    int Floor=1; //评论楼层，借鉴贴吧
    int comment_floor = -1; //评论的楼层，-1表示直接评论帖子，非-1表示评论某一楼层
};






#endif