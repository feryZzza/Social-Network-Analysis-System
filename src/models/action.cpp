#include "models/action.h"
#include "manager/undo_manager.h"
#include "models/comment.h"
#include "models/clients.h" // 确保包含 clients.h 来访问 posts 成员


void Action::init(Client* c,bool add,Post* p){//初始化操作
    this->client = c;
    is_add=add;
    index = p->get_idex();
    post = p;
}//初始化


void PostAction::clean(Client* client_context) {
    if (!post || post_node == nullptr) return;

    // 查找 post 的实际索引
    int index_to_remove = -1;
    for(int i = 0; i < client_context->posts.size(); ++i) {
        if(&client_context->posts[i] == post) {
            index_to_remove = i;
            break;
        }
    }

    if (index_to_remove != -1) {
        // 从用户的帖子列表中移除该帖子 (假删除，将节点从链表中断开，但不删除内存)
        client_context->posts.fake_remove(index_to_remove); 
    }
    
    UndoManager::instance().notify_post_destroyed(post); //通知所有引用该 Post 的 Action，使其无效化

    delete post_node;
    post_node = nullptr;
}

bool PostAction::undo() {//主动从栈中弹出发帖操作并撤销
    if(!check()){
        cout<<"该操作涉及的帖子已被删除，无法撤销"<<endl;
        used = true;
        return false;
    }else{
        if(is_add){//发帖操作，撤销即彻底删帖 (is_add=1)
            //这里不能调用Client的删帖函数，因为会产生新的操作，导致死循环
            int index_to_remove = -1;
            for(int i = 0; i < client->posts.size(); i++){
                if(&client->posts[i] == post){
                    index_to_remove = i;
                    break;
                }
            }
            
            if (index_to_remove != -1) {
                client->posts.fake_remove(index_to_remove); 
            }
            UndoManager::instance().notify_post_destroyed(post);//通知所有引用该 Post 的 Action，使其无效化
            delete post_node;//删除该帖子
            post_node = nullptr;
        }else{//删帖操作，撤销即恢复帖子
            client->posts.auto_insert(post_node);//将帖子按序插入帖子列表
        }
        used = true;
        return true;
    }
}

bool LikeAction::undo() {//主动从栈中弹出点赞操作并撤销
    //点赞变为取消点赞，取消点赞变为点赞，只需调用receive_likes函数即可
    if(!check()){//说明原操作的帖子已经被彻底删除，无法撤销，这时调用撤回会提示错误，并选择是否删除栈中的该操作
        cout<<"该次点赞涉及的帖子已被删除，无法撤销"<<endl;
        post = nullptr;
        used = true;
        return false;
    }else{
        post->receive_likes(client,true);
        UndoManager::instance().unregister_action_self(post, this);//主动注销自身对帖子的引用关系
        used = true;
        return true;
    }
}
void CommentAction::clean(Client* client_context) {
    if (!post || comment_node == nullptr) return; 

    client_context->receive_comment(false);//被评论数减一
    
    // 查找评论的实际索引
    int index_to_remove = -1;
    for(int i = 0; i < post->comment_list.size(); ++i) {
        if(post->comment_list[i].floor() == comment_node->data.floor()) {
            index_to_remove = i;
            break;
        }
    }
    
    if (index_to_remove != -1) {
        post->comment_list.fake_remove(index_to_remove);//从帖子评论列表中移除该评论
    }

    delete comment_node;
    comment_node = nullptr;
}

bool CommentAction::undo() {//主动从栈中弹出评论操作并撤销
    if(!check()){
        cout<<"该评论涉及的帖子已被删除，无法撤销"<<endl;
        used = true;
        return false;
    }else{
        if(is_add){//评论操作，撤销即删评论
            //这里不能调用Client的删评论函数，因为会产生新的操作，导致死循环
            int index_to_remove = -1;
            for(int i = 0; i < post->comment_list.size(); ++i) {
                if(post->comment_list[i].floor() == comment_node->data.floor()) {
                    index_to_remove = i;
                    break;
                }
            }
            if (index_to_remove != -1) {
                post->comment_list.fake_remove(index_to_remove);
                post->author->receive_comment(0);//被评论数减一
            }
            
            UndoManager::instance().unregister_action_self(post, this); // 注销对 Post 的引用
            delete comment_node;
            comment_node = nullptr;
            
            used = true;
            return true;
        }else{//删评论操作，撤销即恢复评论
            post->comment_list.auto_insert(comment_node);//将评论按序插入评论列表
            client->receive_comment(1);//被评论数加一
            used = true;
        }
        return true;
    }
}