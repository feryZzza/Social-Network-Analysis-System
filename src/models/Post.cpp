#include "models/Post.h"
#include "models/action.h"

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
    return;
}

void Post::receive_likes(Client* liker){//重复点赞变为取消点赞功能，参考朋友圈
    for(int i = 0; i < likes_list.size(); i++){
        if(*likes_list[i] == *liker){//已经点过赞，取消点赞
            LikeAction* action = new LikeAction(this);
            action->init(liker,0);//初始化操作
            liker->add_action(action);//将操作压入操作栈
            likes_list.remove(i);//因为点赞只会有一个，所以直接移除
            likes--;
            return;
        }
    }
    if(likes_list.add(liker))likes++;
    return;
}
//重载输出
std::ostream& operator<< (std::ostream& os,Post& p) {//重载输出
    cout<<endl;
    os << "标题: " << p.title << "\n";
    os << "作者: " << p.author_name << "\n";
    os << "内容: " << p.content << "\n\n";
    //根据点赞数输出点赞用户,如果点赞数大于5只输出前5个，形如“用户1,用户2.....用户5等6人赞过”
    if(p.likes > 0){
        int limit = p.likes < 5 ? p.likes : 5;
        for(int i = 0; i < limit; i++){
            os << p.likes_list[i]->Name();
            if(i != limit - 1) os << ", ";
        }
        if(p.likes > 5) os << "等"; 
    }
    os<< p.likes << "人赞过";
    os << "\n";

    if(p.comments_num() > 0){
        int limit = p.comments_num() < 5 ? p.comments_num() : 5;
        for(int i = 0; i < limit; i++){
            os << p.comment_list[i].get_author()->Name();
            if(i != limit - 1) os << ", ";
        }
        if(p.likes > 5) os << "等"; 
    }
    os<< p.comments_num() << "人评论过\n\n";
    os << "评论列表: " << p.comment_list << "\n";
    return os;
}