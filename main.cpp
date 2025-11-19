#include <iostream>
#include "manager/core.h"
#include <string>

using namespace std;

int main() {
    cout << "=== 社交网络管理系统 (Core版 - 终极压力测试) ===" << endl;

    Core& core = Core::instance();

    // 1. 尝试加载数据
    if (core.loadData()) {
        cout << "[系统] 成功加载历史数据。" << endl;
    } else {
        cout << "[系统] 无历史数据，系统初始化..." << endl;
    }

    // 2. 如果数据为空，生成复杂的挑战性测试数据
    if (core.getAllClients().empty()) {
        cout << "\n>>> ⚠️ 开始执行终极压力测试场景 ⚠️ <<<" << endl;
        
        // --- 阶段 A: 批量注册用户 ---
        cout << "\n[Init] 正在注册测试用户群..." << endl;
        core.registerClient("Admin(管理员)", "admin", "root");
        core.registerClient("Spammer(刷屏狂)", "spammer", "123456");
        core.registerClient("Ghost(幽灵)", "ghost", "boo");
        core.registerClient("DeepDiver(潜水员)", "diver", "deep");
        core.registerClient("Victim(受害者)", "victim", "help");

        Client* admin = core.getClientById("admin");
        Client* spammer = core.getClientById("spammer");
        Client* ghost = core.getClientById("ghost");
        Client* diver = core.getClientById("diver");
        Client* victim = core.getClientById("victim");

        // --- 场景一：操作栈溢出测试 (The Stack Overflow) ---
        // 测试目的：Fake_Stack 默认大小为 10，连续操作超过 10 次，最早的操作应被丢弃且不泄露内存。
        cout << "\n[场景一] 操作栈溢出测试 (The Stack Overflow)..." << endl;
        core.userAddPost(victim, "测试点赞贴", "请大家疯狂点赞！");
        Post* p_overflow = &victim->posts[0];

        cout << "   -> Spammer 开始疯狂点赞/取消赞 (执行 15 次)..." << endl;
        for(int i=0; i<15; ++i) {
            // 奇数次是点赞，偶数次是取消赞
            core.userLikePost(spammer, p_overflow); 
        }
        cout << "   -> Spammer 当前操作栈大小: " << spammer->action_num() << " (预期: 10)" << endl;
        
        cout << "   -> Spammer 尝试全部撤销..." << endl;
        int undo_count = 0;
        while(core.userUndo(spammer) == SUCCESS) {
            undo_count++;
        }
        cout << "   -> 成功撤销次数: " << undo_count << " (预期: 10, 前5次操作已丢失)" << endl;


        // --- 场景二：“幽灵”互动测试 (The Ghost Interaction) ---
        // 测试目的：删帖后，Post对象从链表移除但未销毁。测试此时对该Post的操作处理，以及恢复后的状态。
        cout << "\n[场景二] “幽灵”互动测试 (Delete & Interact & Restore)..." << endl;
        core.userAddPost(ghost, "我即将消失", "你看得见我吗？");
        Post* p_ghost = &ghost->posts[0]; // 获取指针

        // 1. Ghost 删除帖子
        core.userDeletePost(ghost, p_ghost);
        cout << "   -> Ghost 删除了帖子。" << endl;

        // 2. Admin 试图评论一个已经被删的帖子 (模拟持有旧指针的情况)
        // 注意：在逻辑上这应该被允许（对象还在Action里活着），或者被禁止。
        // 现在的Core实现允许对 valid 指针操作。
        core.userAddComment(admin, p_ghost, "我利用系统漏洞看到了这条被删的帖子！");
        cout << "   -> Admin 对“被删”帖子进行了评论 (存储在悬空对象中)。" << endl;

        // 3. Ghost 撤销删除 (复活帖子)
        core.userUndo(ghost);
        cout << "   -> [撤销] Ghost 恢复了帖子。" << endl;
        
        // 4. 验证 Admin 的评论是否随帖子一起归来
        cout << "   -> 检查恢复后的帖子内容..." << endl;
        // 重新获取指针（虽然 auto_insert 应该保持原指针有效，但从列表读最稳妥）
        if(ghost->posts.size() > 0) {
            cout << ghost->posts[0]; 
        }


        // --- 场景三：深度楼中楼递归 (Stairway to Heaven) ---
        // 测试目的：测试极深的回复链，以及 JSON 序列化/反序列化是否能处理这种依赖。
        cout << "\n[场景三] 深度楼中楼递归 (Nested Replies)..." << endl;
        core.userAddPost(diver, "深海挑战", "我们将潜入第 10 层梦境。");
        Post* p_deep = &diver->posts[0];
        
        // 创建 10 层回复
        // Floor 2-11
        int current_target_floor = -1; // 先回复帖子
        for(int i=1; i<=10; ++i) {
            string content = "潜入第 " + to_string(i) + " 层";
            if(i > 1) content += " (回复第 " + to_string(current_target_floor) + " 楼)";
            
            // 每个人都回复上一楼
            Client* current_user = (i % 2 == 0) ? admin : diver;
            core.userAddComment(current_user, p_deep, content, current_target_floor);
            
            // 获取刚刚发的楼层号 (它是最新的楼层)
            current_target_floor = p_deep->get_floor(); 
        }
        cout << "   -> 已创建 10 层嵌套回复。" << endl;


        // --- 场景四：毁灭性冲突测试 (Destructive Conflict) ---
        // 测试目的：用户 A 删帖，导致用户 B 针对该贴的操作失效（Invalidated）。
        cout << "\n[场景四] 毁灭性冲突测试 (Invalidation Chain)..." << endl;
        core.userAddPost(victim, "求助贴", "如果不删帖，请帮帮我。");
        Post* p_conflict = &victim->posts[0];

        // 1. Admin 评论
        core.userAddComment(admin, p_conflict, "已收到反馈。");
        cout << "   -> Admin 评论了帖子。" << endl;

        // 2. Admin 删除自己的评论
        core.userDeleteComment(admin, p_conflict, 2); // 删除2楼
        cout << "   -> Admin 删除了自己的评论 (Action 入栈)。" << endl;

        // 3. Victim 突然删除整个帖子！
        core.userDeletePost(victim, p_conflict);
        cout << "   -> Victim 删除了整个帖子！" << endl;
        cout << "   -> (系统应通知 UndoManager 销毁所有相关引用...)" << endl;

        // 4. Admin 试图撤销“删除评论”
        // 预期：失败，因为帖子都没了，评论无法恢复挂载。
        cout << "   -> Admin 试图撤销“删除评论”操作..." << endl;
        CoreStatus status = core.userUndo(admin);
        if (status == ERR_ACTION_INVALID || status == SUCCESS) { // SUCCESS 是指Undo调用成功，但内部可能print失败
             // 我们看控制台输出
        }

        // 5. Victim 撤销“删帖”
        core.userUndo(victim);
        cout << "   -> [撤销] Victim 恢复了帖子。" << endl;

        // 6. Admin 再次试图撤销“删除评论”
        // 预期：仍然失败。因为在第3步删帖时，Admin 栈里的 Action 已经被 invalidate 了。
        // 即使帖子回来了，之前的引用关系已经被切断（为了安全）。
        cout << "   -> Admin 再次试图撤销..." << endl;
        core.userUndo(admin);
    }

    // 3. 最终状态展示
    cout << "\n=============================" << endl;
    cout << "=== 最终系统全景视图 ===" << endl;
    cout << "=============================" << endl;
    SeqList<Client>& clients = core.getAllClients();
    for(int i=0; i<clients.size(); ++i) {
        cout << clients[i] << endl;
    }

    // 4. 持久化测试
    cout << "\n>>> 正在保存数据到 JSON... <<<" << endl;
    if (core.saveData()) {
        cout << "[系统] 数据保存成功！请检查 clients.json 文件。" << endl;
    } else {
        cout << "[错误] 保存失败。" << endl;
    }

    return 0;
}