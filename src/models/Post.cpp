#include "models/Post.h"
#include "data_structure/lin_list.h"
#include "models/action.h"
#include "models/comment.h"
#include "models/message.h"

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

    CommentMassege* m = new CommentMassege(&c);
    m->init(commenter,author,this);
    author->receive_messege(m);//发送评论消息
    return;
}

void Post::receive_likes(Client* liker,bool undo){//重复点赞变为取消点赞功能，参考朋友圈
    for(int i = 0; i < likes_list.size(); i++){
        if(*likes_list[i] == *liker){//已经点过赞，取消点赞
            likes_list.remove(i);//因为点赞只会有一个，所以直接移除
            likes--;
            if(undo) return;//撤销操作不添加操作到栈中
            LikeAction* action = new LikeAction();
            action->init(liker,0,this);//初始化操作
            if(liker->a_stack_full()){
                Action* temp = liker->add_action(action);//将操作压入操作栈
                delete temp;//删除栈底操作
                temp = nullptr;
            }else{
                liker->add_action(action);//将操作压入操作栈
            }

            return;
        }
    }
    if(likes_list.add(liker)){
        likes++;
        if(undo) return;//撤销操作不添加操作到栈中
        LikeAction* action = new LikeAction();
        action->init(liker,1,this);//初始化操作
        liker->add_action(action);//将操作压入操作栈

        LikeMassege* m = new LikeMassege();
        m->init(liker,author,this);
        author->receive_messege(m);//发送点赞消息
    }
    return;
}

//重载输出
std::ostream& operator<< (std::ostream& os,Post& p) {//重载输出
    cout<<endl;
    os << "标题: " << p.title << "\n";
    os << "作者: " << p.author_name << "\n";
    os << "内容: " << p.content << "\n\n";
    os << "序号" << p.idex << "\n";
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

    //根据评论数输出评论用户,如果评论数大于5只输出前5个，形如“用户1,用户2.....用户5等6人评论过”
    LinkList<Client*> commenters;

    for(int i = 0; i < p.comment_list.size(); i++){
        Client* commenter = p.comment_list[i].get_author();
        bool found = false;
        for(int j = 0; j < commenters.size(); j++){
            if(*commenters[j] == *commenter){
                found = true;
                break;
            }
        }
        if(!found) commenters.add(commenter);
    }

    if(commenters.size() > 0){
        int limit = commenters.size() < 5 ? commenters.size() : 5;
        for(int i = 0; i < limit; i++){
            os << commenters[i]->Name();
            if(i != limit - 1) os << ", ";
        }
        if(p.likes > 5) os << "等"; 
    }
    os<< commenters.size() << "人评论过" <<p.comments_num()<<"条回复\n\n";
    os << "评论列表: " << p.comment_list << "\n";
    return os;
}