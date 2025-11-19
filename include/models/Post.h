#ifndef POST_H
#define POST_H
#include <iostream>
#include "data_structure/lin_list.h"
#include "models/comment.h"

using namespace std;

class Client;

class Post {
public:
    Post() {}
    Post(string title, string content) : title(title), content(content), likes(0) {}
    
    // --- 数据访问 ---
    string get_title() const { return title; }
    string get_content() const { return content; }
    int get_floor() const { return floor; }
    void set_floor(int f) { floor = f; }
    
    int get_idex() const { return idex; }
    void set_idex(int i) { idex = i; }

    Client* author = nullptr;
    void set_author(Client* a) { author = a; }
    
    // --- 点赞管理 ---
    LinkList<Client*>& get_likes_list() { return likes_list; }
    int likes_num() const { return likes; }
    void increment_likes() { likes++; }
    void decrement_likes() { likes--; }

    // --- 评论管理 ---
    LinkList<Comment> comment_list;
    int comments_num() { return comment_list.size(); }

    // 重载
    friend std::ostream& operator<< (std::ostream& os, Post& p);
    bool operator==(const Post& other) const { return this->idex == other.idex; }
    bool operator<(const Post& other) const { return this->idex < other.idex; }
    bool operator>(const Post& other) const { return this->idex > other.idex; }
    bool operator<=(const Post& other) const { return this->idex <= other.idex; }
    bool operator>=(const Post& other) const { return this->idex >= other.idex; }

private:
    int floor = 1;
    string title;
    string content;
    int likes = 0;
    int idex = 0;
    LinkList<Client*> likes_list; 
};
#endif