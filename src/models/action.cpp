#include "models/action.h"
#include "manager/undo_manager.h"
#include "models/comment.h"


void Action::init(Client* c,bool add,Post* p){//初始化操作
    this->client = c;
    is_add=add;
    index = p->get_idex();
    post = p;
}//初始化



void PostAction::clean(Client* client_context) {
    if (!post || post_node == nullptr) return;

    client_context->posts.fake_remove(post_node->data.get_idex());//从用户的帖子列表中移除该帖子 
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
            client->deletePost(post);//先假删除

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
        return true;
    }
}
void CommentAction::clean(Client* client_context) {
    if (!post || comment_node == nullptr) return; 

    client_context->receive_comment(false);//被评论数减一
    post->comment_list.fake_remove(comment_node->data.floor());//从帖子评论列表中移除该评论

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
            post->author->deleteComment(post,comment_node->data.floor());//通过楼层删除评论
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
