#include "models/clients.h"
#include "data_structure/lin_list.h"
#include "models/Post.h"
#include "models/action.h"

bool Client::undo(){//撤销上一次操作
    if(a_stack.empty()){
        cout<<"没有操作可以撤销"<<endl;
        return false;
    }
    Action* a = a_stack.pop();
    a->undo();
    cout<<"操作栈大小"<<a_stack.size()<<endl;
    return true;
}

void Client::addPost(Post p){//发帖
    p.set_author(this);
    p.set_idex(++post_time);
    posts.add(p);
    ListNode<Post>* p_pointer = posts.tail_ptr();//获取新发帖的节点指针
    safe_posts.add(&posts[posts.size()-1]);//将新发的帖子加入安全列表

    PostAction* action = new PostAction(p_pointer);//创建发帖操作
    action->init(this,1,&p_pointer->data);//初始化操作

    if(a_stack.full()){
        cout<<"操作栈已满，删除最早的操作"<<endl;
        Action* temp = add_action(action);//将操作压入操作栈
        delete temp;//删除栈底操作
        temp = nullptr;
    }else{
        add_action(action);//将操作压入操作栈
    }
}

void Client::deletePost(int index){//因为有撤销功能，删除帖子只是将其从列表中移除，并不真正删除，并且将删除操作压入操作栈
    ListNode<Post>* node = posts.fake_remove(index);//假删除，同时记录被删除节点的指针

    PostAction* action = new PostAction(node);//创建发帖操作
    action->init(this,0,&node->data);//初始化操作

    if(a_stack.full()){
        Action* temp = add_action(action);//将操作压入操作栈
        delete temp;//删除栈底操作
        temp = nullptr;
    }else{
        add_action(action);//将操作压入操作栈
    }
    return;
}

void Client::deletePost(Post* post){//因为有撤销功能，删除帖子只是将其从列表中移除，并不真正删除，并且将删除操作压入操作栈
    for(int i = 0; i < posts.size(); i++){//寻找该帖子
        if(&posts[i] == post){
            ListNode<Post>* node = posts.fake_remove(i);//假删除，同时记录被删除节点的指针
            PostAction* action = new PostAction(node);//创建发帖操作
            action->init(this,0,&node->data);//初始化操作
            
            if(a_stack.full()){
                Action* temp = add_action(action);//将操作压入操作栈
                delete temp;//删除栈底操作
                temp = nullptr;
            }else{
                add_action(action);//将操作压入操作栈
            }
            return;
        }
    }
    cout<<"未找到该帖子"<<endl;
    return;
}

void Client::deleteComment(Post* post,Comment* comment){//删评论
    for(int i = 0; i < post->comment_list.size(); i++){
        if(&post->comment_list[i] == comment){
            ListNode<Comment>* node = post->comment_list.fake_remove(i);//假删除，同时记录被删除节点的指针
            CommentAction* action = new CommentAction(node);//创建评论操作
            action->init(this,0,post);//初始化操作
            
            if(a_stack.full()){
                Action* temp = add_action(action);//将操作压入操作栈
                delete temp;//删除栈底操作
                temp = nullptr;
            }else{
                add_action(action);//将操作压入操作栈
            }
            this->receive_comment(0);//被评论数减一
            return;
        }
    }
    cout<<"未找到该评论"<<endl;
    return;
}

void Client::deleteComment(Post* post,int floor){//删评论
    for(int i = 0; i < post->comment_list.size(); i++){
        if(post->comment_list[i].floor() == floor){
            deleteComment(post,&post->comment_list[i]);
            return;
        }
    }
    cout<<"未找到该评论"<<endl;
    return;
}



void Client::addComment(Post* post, Comment &comment){
    post->addComment(comment,this);
    CommentAction* action = new CommentAction(post->comment_list.tail_ptr());//获取新评论的节点指针
    action->init(this,1,post);//初始化操作
    if(a_stack.full()){
        Action* temp = add_action(action);//将操作压入操作栈
        delete temp;//删除栈底操作
        temp = nullptr;
    }else{
        add_action(action);//将操作压入操作栈
    }
    return;
}

void Client::like(Post* post,bool undo){//点赞帖子
    post->receive_likes(this,undo);
    return;
}
//重载输出
std::ostream& operator<< (std::ostream& os,Client& c) {//重载输出
    os << "昵称: " << c.name << "\n";
    os << "ID: " << c.id << "\n";
    os << "密码: " << c.password << "\n";
    os << "帖子: " << c.posts << "\n";
    return os;
}

bool Client::undo_safe_check(Post *post){//检查撤销操作涉及的帖子是否被删除，防止野指针
    for(int i = 0; i < safe_posts.size(); i++){
        if(safe_posts[i]->get_idex() == post->get_idex()) return true;//找到相同序号说明帖子还在
    }
    return false;
}

void Client::undo_safe_update(Post* p){//每次彻底删除帖子时调用，删去彻底释放的指针，若有则将post指针置为空,防止野指针
    for(int i = 0; i < safe_posts.size(); i++){
        if(safe_posts[i]->get_idex() == p->get_idex()){//找到相同序号说明帖子还在
            safe_posts.remove(i);//更新安全列表
            break;
        }
    }
    return;
}