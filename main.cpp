#include <iostream>
#include "manager/core.h"

using namespace std;

int main() {
    cout << "=== 社交网络管理系统 (Core版 - 压力测试) ===" << endl;

    Core& core = Core::instance();

    // 1. 尝试加载数据
    if (core.loadData()) {
        cout << "[系统] 成功加载历史数据。" << endl;
    } else {
        cout << "[系统] 无历史数据，系统初始化..." << endl;
    }

    // 2. 如果数据为空，生成复杂的挑战性测试数据
    if (core.getAllClients().empty()) {
        cout << "\n>>> 开始生成复杂测试场景 <<<" << endl;
        
        // --- 阶段 A: 批量注册用户 ---
        cout << "1. 注册用户群..." << endl;
        core.registerClient("张伟(技术宅)", "zhangwei", "pass1");
        core.registerClient("李静(生活派)", "lijing", "pass2");
        core.registerClient("王磊(杠精)", "wanglei", "pass3");
        core.registerClient("刘洋(吃货)", "liuyang", "pass4");
        core.registerClient("陈秀英(广场舞)", "chenxiuying", "pass5");

        Client* zw = core.getClientById("zhangwei");
        Client* lj = core.getClientById("lijing");
        Client* wl = core.getClientById("wanglei");
        Client* ly = core.getClientById("liuyang");
        Client* cxy = core.getClientById("chenxiuying");

        // --- 阶段 B: 复杂互动场景 ---
        
        // [场景一] 热门技术贴：测试多层评论和大量点赞
        cout << "\n[场景一] 张伟发布 C++ 教程 (多层楼中楼测试)..." << endl;
        core.userAddPost(zw, "深度解析 C++ 指针与引用", "指针是 C++ 的灵魂，也是新手的噩梦...");
        Post* p_tech = &zw->posts[0];

        // 众人点赞
        core.userLikePost(lj, p_tech);
        core.userLikePost(wl, p_tech);
        core.userLikePost(ly, p_tech);
        core.userLikePost(cxy, p_tech);

        // 楼中楼互动
        core.userAddComment(lj, p_tech, "太深奥了，大佬带带我！"); // 2楼
        core.userAddComment(wl, p_tech, "这有什么难的？有手就行。", 2); // 3楼，王磊回复2楼(李静)
        core.userAddComment(ly, p_tech, "楼上别站着说话不腰疼。", 3); // 4楼，刘洋回复3楼(王磊)
        core.userAddComment(zw, p_tech, "大家理性讨论，别吵架。", -1); // 5楼，张伟回复帖子本身

        // [场景二] 撤销操作测试：王磊的“反复横跳”
        cout << "\n[场景二] 王磊发表不当言论并撤回 (Action 撤销测试)..." << endl;
        core.userAddPost(cxy, "今晚广场舞集合", "老地方，不见不散！");
        Post* p_dance = &cxy->posts[0];

        // 王磊发表负面评论
        core.userAddComment(wl, p_dance, "吵死了，能不能别跳了！");
        cout << "   -> 王磊已评论: 吵死了..." << endl;
        
        // 王磊后悔了，执行撤销
        core.userUndo(wl);
        cout << "   -> [撤销] 王磊撤回了上一条评论。" << endl;

        // 王磊重新评论
        core.userAddComment(wl, p_dance, "阿姨们注意身体，支持！");
        cout << "   -> 王磊重新评论: 支持..." << endl;

        // [场景三] 删帖恢复测试：李静的误操作
        cout << "\n[场景三] 李静删帖后恢复 (节点内存管理测试)..." << endl;
        core.userAddPost(lj, "心情不好", "想把账号注销了...");
        Post* p_mood = &lj->posts[0];
        
        // 张伟安慰点赞
        core.userLikePost(zw, p_mood);
        
        // 李静删除帖子
        core.userDeletePost(lj, p_mood);
        cout << "   -> 李静删除了帖子。" << endl;
        
        // 此时 p_mood 指针在链表中已失效，但在 Action 中被捕获
        // 李静撤销删除
        core.userUndo(lj); 
        cout << "   -> [撤销] 李静恢复了帖子。" << endl;
        
        // 验证恢复：陈秀英可以继续评论
        // 注意：需重新获取指针，因为链表可能重排(虽然auto_insert通常原位，但安全起见)
        p_mood = &lj->posts[0]; 
        core.userAddComment(cxy, p_mood, "妹子别想不开，来跳舞！");

        // [场景四] 逻辑反转测试：刘洋的点赞开关
        cout << "\n[场景四] 刘洋的点赞开关 (重复操作逻辑测试)..." << endl;
        core.userLikePost(ly, p_dance); 
        cout << "   -> 刘洋点赞了广场舞贴。" << endl;
        
        core.userLikePost(ly, p_dance); // 再次调用，应触发取消点赞
        cout << "   -> 刘洋再次点击，取消了赞。" << endl;
        
        core.userUndo(ly); // 撤销“取消赞”操作 -> 变回“已赞”
        cout << "   -> [撤销] 刘洋撤销了操作，状态应恢复为已赞。" << endl;

        // [场景五] 消息队列检查
        cout << "\n[场景五] 检查张伟的消息通知..." << endl;
        // 张伟应该收到：
        // 1. 4个人的点赞 (针对技术贴)
        // 2. 1条回复 (针对技术贴)
        // 3. 李静删帖恢复后的相关消息(如果有)
        core.userReadMessages(zw);
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