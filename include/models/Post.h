#ifndef POST_H
#define POST_H
#include <iostream>
#include "data_structure/lin_list.h"
#include "data_structure/stack.h"
#include "data_structure/queue.h"
#include "models/clients.h"

class Client;

class Post{
public:
    Post() {}
    //Post(std::string title, std::string author, std::string content) : title(title), author(author), content(content), likes(0), comments(0) {}
    //重载输出
    friend std::ostream& operator<< (std::ostream& os,Post& p) {//重载输出
        os << "Title: " << p.title << "\n";
        os << "Author: " << p.author << "\n";
        os << "Content: " << p.content << "\n";
        os << "Likes: " << p.likes << "\n";
        os << "Comments: " << p.comments << "\n";
        os << "Comment List: " << p.comment_list << "\n";
        //os << "Like Users: " << p.like_users << "\n";
        return os;
    }
private:
    std::string title;//帖子标题
    std::string author_name;//帖子作者
    std::string content;//帖子内容
    int likes;//点赞数
    int comments;//评论数
    LinkList<std::string> comment_list; //评论列表
    Client* author; //作者指针
};
#endif