#include "models/clients.h"
#include "data_structure/lin_list.h"
#include "manager/undo_manager.h"
#include "models/Post.h"
#include "models/action.h"

bool Client::undo(){//撤销上一次操作
    if(a_stack.empty()){
        // cout<<"没有操作可以撤销"<<endl; // 移到 main.cpp 中提示
        return false;
    }
    Action* a = a_stack.pop();
    if (a->undo()) { // 确保 undo 成功
        delete a;//删除操作
        return true;
    } else {
        // 撤销失败，可能操作已失效
        delete a; // 仍然要删除操作
        return false;
    }
}

void Client::addPost(Post &p){//发帖
    p.set_author(this);
    p.set_idex(++post_time);
    posts.add(p);
    ListNode<Post>* p_pointer = posts.tail_ptr();//获取新发帖的节点指针

    PostAction* action = new PostAction(p_pointer);//创建发帖操作
    action->init(this,1,&p_pointer->data);//初始化操作

    UndoManager::instance().register_action(action->post, action);//注册操作与帖子的引用关系

    if(a_stack.full()){
        cout<<"操作栈已满，删除最早的操作"<<endl;
        Action* temp = add_action(action);//将操作压入操作栈，并获取溢出的操作指针
        temp->clean(this);//清理资源，防止野指针
        delete temp;//删除栈底操作
        temp = nullptr;
    }else{
        add_action(action);//将操作压入操作栈
    }
}

void Client::deletePost(int index){//因为有撤销功能，删除帖子只是将其从列表中移除，并不真正删除，并且将删除操作压入操作栈
    ListNode<Post>* node = posts.fake_remove(index);//假删除，同时记录被删除节点的指针
    
    // 如果节点为空（比如索引无效），则不执行任何操作
    if (!node) {
        cout << "[错误] 尝试删除一个无效的帖子索引。" << endl;
        return;
    }

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

void Client::deletePost(Post* post){//因为有撤销功能，删除帖子只是将其从列表中移除，并不真正删除，并且将删除操作_压入操作K
    if (!post) return; // 安全检查
    for(int i = 0; i < posts.size(); i++){//寻找该帖子
        if(&posts[i] == post){
            ListNode<Post>* node = posts.fake_remove(i);//假删除，同时记录被删除节点的指针
            
            if (!node) return; // fake_remove 失败

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
    if (!post || !comment) return; // 安全检查
    for(int i = 0; i < post->comment_list.size(); i++){
        if(&post->comment_list[i] == comment){
            ListNode<Comment>* node = post->comment_list.fake_remove(i);//假删除，同时记录被删除节点的指针
            
            if (!node) return; // fake_remove 失败

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
    if (!post) return; // 安全检查
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
    if (!post) return; // 安全检查
    post->addComment(comment,this);
    CommentAction* action = new CommentAction(post->comment_list.tail_ptr());//获取新评论的节点指针
    action->init(this,1,post);//初始化操作
    UndoManager::instance().register_action(post, action);
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
    if (!post) return; // 安全检查
    post->receive_likes(this,undo);
    return;
}
//重载输出
std::ostream& operator<< (std::ostream& os,Client& c) {//重载输出
    os << "昵称: " << c.name << "\n";
    os << "ID: " << c.id << "\n";
    os << "密码: " << c.password << "\n";
    os << "帖子列表: \n" << c.posts << "\n";
    return os;
}

void Client::read_messege(){
    if(m_q.empty()){
        cout<<"没有新消息"<<endl;
        return;
    }
    while(!m_q.empty()){
        massege* m = m_q.dequeue();
        m->show();
        delete m;//删除消息
    }
    return;
}

Client::~Client(){
    while(!a_stack.empty()){
        Action* a = a_stack.pop();
        a->invalidate();//清理资源
        delete a;//删除操作
        a = nullptr;
    }
    while (!m_q.empty()){
        massege* m = m_q.dequeue();
        delete m;//删除消息
    }
}