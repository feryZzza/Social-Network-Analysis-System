#include <iostream>
#include "manager/core.h"
#include <string>
#include <vector> 

using namespace std;

// 辅助函数：打印分割线
void printSeparator(const string& title) {
    cout << "\n=============================================" << endl;
    cout << "   " << title << endl;
    cout << "=============================================" << endl;
}

// 辅助函数：打印用户收件箱
void checkInbox(Client* c) {
    if (!c) return;
    cout << "\n[查看消息] 用户: " << c->Name() << endl;
    Core::instance().userReadMessages(c);
}

int main() {
    printSeparator("社交网络管理系统 - 全功能集成测试 (Name Unique Version)");

    Core& core = Core::instance();

    // ==========================================
    // 1. 系统初始化与数据加载
    // ==========================================
    bool hasData = core.loadData();
    if (hasData) {
        cout << "[系统] 成功加载历史数据。" << endl;
    } else {
        cout << "[系统] 无历史数据，初始化全新环境..." << endl;
    }

    // 如果是空环境，注册测试用户
    // 演示：ID 不再需要固定格式，Name 必须唯一
    if (core.getAllClients().empty()) {
        cout << "\n[Init] 正在注册基础用户群..." << endl;
        core.registerClient("Alice", "alice_id_any_format", "pass1");
        core.registerClient("Bob", "bob@email.com", "pass2");     // ID 可以是邮箱格式
        core.registerClient("Charlie", "+8613800000000", "pass3"); // ID 可以是电话格式
        core.registerClient("Dave", "uid_9999", "pass4");
        core.registerClient("Eve", "just_a_random_string", "pass5");
        core.registerClient("Frank", "frank_official", "pass6"); 
        cout << "[Init] 注册完成。当前系统使用 [Name] 作为唯一索引。" << endl;
    }
    
    // 获取用户指针 (核心逻辑：通过 Name 查找)
    Client* alice = core.getClientByName("Alice");
    Client* bob = core.getClientByName("Bob");
    Client* charlie = core.getClientByName("Charlie");
    Client* dave = core.getClientByName("Dave");
    Client* eve = core.getClientByName("Eve");
    Client* frank = core.getClientByName("Frank");

    if (!alice) {
        cout << "[错误] 无法获取测试用户，请检查数据加载或注册流程。" << endl;
        return -1;
    }

    // ==========================================
    // 场景零：验证持久化 (如果是第二次运行)
    // ==========================================
    if (hasData && alice && bob) {
        printSeparator("场景零：验证数据持久化状态");
        // 检查上次保存的好友关系
        int d = core.getRelationDistance(alice, bob);
        if (d != -1) {
            cout << "✅ [验证] 好友关系图谱恢复成功 (Alice与Bob距离: " << d << ")" << endl;
        } else {
            cout << "ℹ️ [提示] Alice 和 Bob 当前无关联。" << endl;
        }
        
        // 检查上次保存的帖子
        if (alice->posts.size() > 0) {
            cout << "✅ [验证] 用户帖子数据恢复成功 (Alice有 " << alice->posts.size() << " 条帖子)。" << endl;
        }
    }

    // ==========================================
    // 场景一：发帖、评论与点赞 (互动测试)
    // ==========================================
    printSeparator("场景一：内容互动 (Post, Comment, Like)");
    
    if (alice && bob && charlie) {
        // 1. Alice 发帖
        cout << ">>> Alice 发布了一条新动态..." << endl;
        core.userAddPost(alice, "C++学习心得", "手写数据结构真是太有趣了！(才怪)");
        Post* alicePost = &alice->posts.tail_ptr()->data; // 获取刚发的帖子
        
        // 2. Bob 点赞并评论
        cout << ">>> Bob (" << bob->ID() << ") 点赞并评论了 Alice..." << endl;
        core.userLikePost(bob, alicePost);
        core.userAddComment(bob, alicePost, "确实，特别是链表指针，令人头秃。", -1); // -1 表示直接回复帖子

        // 3. Charlie 回复 Bob (楼中楼)
        // 假设 Bob 的评论是第 2 楼 (1楼是帖子本身)
        cout << ">>> Charlie (" << charlie->ID() << ") 回复了 Bob 的评论 (楼中楼)..." << endl;
        int bobFloor = alicePost->comment_list.tail_ptr()->data.floor(); // 获取Bob评论的楼层
        core.userAddComment(charlie, alicePost, "同感，Segment Fault 才是永远的朋友。", bobFloor);

        // 4. 打印帖子详情
        cout << *alicePost << endl;

        // 5. 检查 Alice 的消息通知
        checkInbox(alice);
    }

    // ==========================================
    // 场景二：撤销机制 (Undo System)
    // ==========================================
    printSeparator("场景二：后悔药测试 (Undo System)");

    if (dave && eve) {
        // 1. 测试撤销发帖
        cout << ">>> Dave 发送了一条冲动的帖子..." << endl;
        core.userAddPost(dave, "我讨厌写代码！", "毁灭吧赶紧的。");
        cout << "   当前 Dave 帖子数: " << dave->posts.size() << endl;
        
        cout << ">>> Dave 后悔了，执行撤销 (Undo)..." << endl;
        CoreStatus s1 = core.userUndo(dave);
        if (s1 == SUCCESS) cout << "✅ 撤销成功。当前帖子数: " << dave->posts.size() << " (应为0)" << endl;
        else cout << "❌ 撤销失败。" << endl;

        // 2. 测试撤销点赞
        // Eve 给 Alice 的帖子点赞
        if (alice && alice->posts.size() > 0) {
            Post* p = &alice->posts.tail_ptr()->data;
            cout << "\n>>> Eve 点赞了 Alice 的帖子..." << endl;
            core.userLikePost(eve, p);
            cout << "   当前点赞数: " << p->likes_num() << endl;

            cout << ">>> Eve 手滑了，撤销点赞..." << endl;
            core.userUndo(eve);
            cout << "✅ 撤销成功。当前点赞数: " << p->likes_num() << " (应减少1)" << endl;
        }

        // 3. 测试撤销删除评论 (高级)
        // 让 Eve 发一条评论，然后删除它，然后撤销“删除”操作（即恢复评论）
        if (alice && alice->posts.size() > 0) {
            Post* p = &alice->posts.tail_ptr()->data;
            cout << "\n>>> Eve 评论了 Alice..." << endl;
            core.userAddComment(eve, p, "测试评论恢复功能", -1);
            int commentCountBefore = p->comment_list.size();
            int floorToRemove = p->comment_list.tail_ptr()->data.floor();

            cout << ">>> Eve 删除了这条评论..." << endl;
            core.userDeleteComment(eve, p, floorToRemove);
            cout << "   当前评论数: " << p->comment_list.size() << endl;

            cout << ">>> Eve 想要恢复评论 (撤销删除操作)..." << endl;
            core.userUndo(eve); // 撤销刚才的删除
            cout << "✅ 恢复成功。当前评论数: " << p->comment_list.size() << " (应恢复)" << endl;
            
            // 再次打印确认存在
            // cout << p->comment_list << endl; 
        }
    }

    // ==========================================
    // 场景三：哈夫曼压缩分析
    // ==========================================
    printSeparator("场景三：哈夫曼压缩算法分析");
    
    if (frank) {
        string longContent = "数据结构是计算机存储、组织数据的方式。数据结构是指相互之间存在一种或多种特定关系的数据元素的集合。通常情况下，精心选择的数据结构可以带来更高的运行或者存储效率。数据结构往往同高效的检索算法和索引技术有关。";
        cout << ">>> Frank 发布了一篇长科普文..." << endl;
        core.userAddPost(frank, "什么是数据结构", longContent);
        
        Post* techPost = &frank->posts.tail_ptr()->data;
        
        // 调用核心分析功能
        core.analyzePostContent(techPost);
    }

    // ==========================================
    // 场景四：社交图谱与六度分隔
    // ==========================================
    printSeparator("场景四：社交网络连通性 (Six Degrees)");

    if (alice && bob && charlie && dave && eve) {
        cout << "构建好友链: Alice <-> Bob <-> Charlie <-> Dave" << endl;
        core.makeFriend(alice, bob);
        core.makeFriend(bob, charlie);
        core.makeFriend(charlie, dave);
        
        // 制造一个“孤岛”或者远端用户 Eve
        cout << "Eve 暂时是孤立的。" << endl;

        int dist = core.getRelationDistance(alice, dave);
        cout << "[测试] Alice 到 Dave 的距离: " << dist << " (预期: 3)" << endl;

        int distEve = core.getRelationDistance(alice, eve);
        cout << "[测试] Alice 到 Eve 的距离: " << distEve << " (预期: -1)" << endl;

        cout << "\n>>> 让 Eve 连接到网络 (Dave <-> Eve)..." << endl;
        core.makeFriend(dave, eve);
        
        int newDist = core.getRelationDistance(alice, eve);
        cout << "[测试] Alice 到 Eve 的新距离: " << newDist << " (预期: 4)" << endl;
    }

    // ==========================================
    // 场景五：排行榜系统 (集成测试)
    // ==========================================
    printSeparator("场景五：数据排行榜 (Ranking System)");

    // 1. 展示用户影响力排行 (基于好友数)
    cout << ">>> 正在生成用户影响力排行榜..." << endl;
    core.showUserRanking();

    // 2. 展示热门帖子排行 (基于点赞数)
    cout << "\n>>> 正在生成全站热门帖子排行榜..." << endl;
    core.showHotPostRanking();

    // ==========================================
    // 结束：保存数据
    // ==========================================
    printSeparator("测试结束：数据保存");
    cout << ">>> 正在将所有数据（用户、帖子、评论、关系图）写入磁盘..." << endl;
    if (core.saveData()) {
        cout << "✅ [成功] 数据已保存至 data/clients.json" << endl;
    } else {
        cout << "❌ [失败] 数据保存遇到错误。" << endl;
    }

    return 0;
}