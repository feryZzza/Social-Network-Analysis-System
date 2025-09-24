#include "models/Post.h"

void Post::set_author(Client* a){
    author = a;
    author_name = a->Name();
}

void Post::addComment(Comment &c,Client* commenter){
    int idex = this->floor+1;
    c.set_floor(idex);
    if(comment_list.add(c)){//添加评论成功
        this->floor++;
        commenter->receive_comment(1);
    }
    comments++;
}
//重载输出
std::ostream& operator<< (std::ostream& os,Post& p) {//重载输出
    cout<<endl;
    os << "标题: " << p.title << "\n";
    os << "作者: " << p.author_name << "\n";
    os << "内容: " << p.content << "\n";
    os << "点赞数: " << p.likes << "\n";
    os << "评论数: " << p.comments << "\n";
    os << "评论列表: " << p.comment_list << "\n";
    return os;
}