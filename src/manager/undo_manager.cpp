#include "manager/undo_manager.h"
#include "models/action.h" 


int UndoManager::find_post_index(Post* p) {// 查找 Post 在注册表中的索引
    if (!p) return -1;
    for (int i = 0; i < registry.size(); ++i) {
        if (registry[i].post == p) {
            return i;
        }
    }
    return -1;
}

void UndoManager::register_action(Post* p, Action* a) {// 注册 Action 对 Post 的引用关系
    if (!p || !a) return;
    
    int index = find_post_index(p);
    
    if (index != -1) {// 找到 Post，将 Action 加入现有列表
        registry[index].actions.add(a);
    } else {
    // 未找到 Post，创建新的 PostActionPair 并加入 registry
        post_resource newPair;
        newPair.post = p;
        newPair.actions.add(a);
        registry.add(newPair);
    }
}

void UndoManager::unregister_action_self(Post* p, Action* a) {// Action 主动注销自身对 Post 的引用关系
    if (!p || !a) return;
    int pair_index = find_post_index(p);
    if (pair_index != -1) {// 找到对应的 PostActionPair
        //
        LinkList<Action*>& action_list = registry[pair_index].actions;
        
        // 在 LinkList<Action*> 中查找并移除 Action* a
        for (int i = 0; i < action_list.size(); ++i) {
            if (action_list[i] == a) {
                action_list.remove(i);
                break; // Action 只会注册一次，找到即可
            }
        }
        
        // 如果 Action 列表变空，移除整个 PostActionPair 
        if (action_list.empty()) {
            registry.remove(pair_index);
        }
    }
}

void UndoManager::notify_post_destroyed(Post* p) {
    if (!p) return;

    int pair_index = find_post_index(p);

    if (pair_index != -1) {

        LinkList<Action*>& action_list = registry[pair_index].actions;

        for (int i = 0; i < action_list.size(); i++) {
            Action* a = action_list[i];
            if (a) {
                a->invalidate(); //将 Action 内的 Post 指针置为空，防止野指针
            }
        }

        registry.remove(pair_index);
    }
}