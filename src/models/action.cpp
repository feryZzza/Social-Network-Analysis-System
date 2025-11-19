#include "models/action.h"
#include "models/clients.h" 
#include "models/Post.h"
#include "models/comment.h"
#include "manager/undo_manager.h"

// 基类初始化
void Action::init(Client* c, bool add, Post* p) {
    this->client = c;
    is_add = add;
    post = p;
    if (p) {
        index = p->get_idex();
    }
}

// --- PostAction 实现 ---

void PostAction::clean(Client* client_context) {
    if (!post || post_node == nullptr) return;

    if (!is_add) { 
        if (post_node) {
            delete post_node; 
            post_node = nullptr;
        }
    }
}

bool PostAction::undo() {
    if (!check() || !client) {
        cout << "[撤销失败] 帖子对象已失效。" << endl;
        used = true;
        return false;
    }

    if (is_add) { 
        // 原操作是：发帖 -> 撤销逻辑：删帖
        
        int index_to_remove = -1;
        for (int i = 0; i < client->posts.size(); i++) {
            if (&client->posts[i] == post) {
                index_to_remove = i;
                break;
            }
        }

        if (index_to_remove != -1) {
            ListNode<Post>* node = client->posts.fake_remove(index_to_remove);
            UndoManager::instance().notify_post_destroyed(post);
            delete node; 
            post = nullptr;
            post_node = nullptr;
        }
        return true;

    } else {
        // 原操作是：删帖 -> 撤销逻辑：恢复帖子
        if (post_node) {
            client->posts.auto_insert(post_node);
            return true;
        }
        return false;
    }
}

// --- LikeAction 实现 ---

void LikeAction::clean(Client* client_context) {
    if (post) {
        UndoManager::instance().unregister_action_self(post, this);
    }
}

bool LikeAction::undo() {
    if (!check() || !client) {
        cout << "[撤销失败] 帖子已不存在。" << endl;
        used = true;
        return false;
    }

    LinkList<Client*>& likes_list = post->get_likes_list();

    if (is_add) {
        // 原操作：点赞 -> 撤销：取消点赞
        for (int i = 0; i < likes_list.size(); i++) {
            // 修改：比较 Name
            if (likes_list[i] && likes_list[i]->Name() == client->Name()) {
                likes_list.remove(i);
                post->decrement_likes();
                
                UndoManager::instance().unregister_action_self(post, this);
                used = true;
                return true;
            }
        }
    } else {
        // 原操作：取消点赞 -> 撤销：重新点赞
        bool exists = false;
        for(int i=0; i<likes_list.size(); ++i) {
            // 修改：比较 Name
            if(likes_list[i]->Name() == client->Name()) exists = true;
        }
        if (!exists) {
            likes_list.add(client);
            post->increment_likes();
            UndoManager::instance().unregister_action_self(post, this); 
            used = true;
            return true;
        }
    }
    return false;
}

// --- CommentAction 实现 ---

void CommentAction::clean(Client* client_context) {
    if (!post || !comment_node) return;
    
    if (!is_add) { 
        delete comment_node;
        comment_node = nullptr;
    }
}

bool CommentAction::undo() {
    if (!check() || !client) {
        cout << "[撤销失败] 帖子已不存在。" << endl;
        used = true;
        return false;
    }

    if (is_add) {
        // 原操作：发评论 -> 撤销：删评论
        int index_to_remove = -1;
        int target_floor = comment_node->data.floor();
        
        for (int i = 0; i < post->comment_list.size(); ++i) {
            if (post->comment_list[i].floor() == target_floor) {
                index_to_remove = i;
                break;
            }
        }

        if (index_to_remove != -1) {
            ListNode<Comment>* node = post->comment_list.fake_remove(index_to_remove);
            if (node) {
                client->receive_comment(false);
                
                UndoManager::instance().unregister_action_self(post, this);
                delete node; 
            }
            return true;
        }

    } else {
        // 原操作：删评论 -> 撤销：恢复评论
        if (comment_node) {
            post->comment_list.auto_insert(comment_node);
            client->receive_comment(true); 
            used = true;
            return true;
        }
    }
    return false;
}