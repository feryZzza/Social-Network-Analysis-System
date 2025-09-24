#ifndef COMMENT_H
#define COMMENT_H

#include <iostream>
#include <string>
#include "models/clients.h"

class Client;

class Comment {
public:
    Comment() {}
    Comment(Client* author, const std::string& content, int floor)
        : author(author), content(content), floor(floor) {}

    // 重载输出
    friend std::ostream& operator<< (std::ostream& os, const Comment& c);


private:
    Client* author; // 评论作者指针
    std::string content; // 评论内容
    int floor; //评论楼层，借鉴贴吧
};






#endif