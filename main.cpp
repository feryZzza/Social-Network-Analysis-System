#include <iostream>
#include "manager/core.h"
#include <string>
#include <vector> // 仅用于测试脚本管理测试数据，Core 内部不依赖 STL

using namespace std;

void printSeparator(const string& title) {
    cout << "\n=============================================" << endl;
    cout << "   " << title << endl;
    cout << "=============================================" << endl;
}

int main() {
    printSeparator("社交网络管理系统 (AVL Tree Index Test)");

    Core& core = Core::instance();

    // 1. 加载数据 (模拟系统启动)
    if (core.loadData()) {
        cout << "[系统] 成功加载历史数据。" << endl;
    } else {
        cout << "[系统] 无历史数据，初始化全新环境..." << endl;
    }

    // 如果是空环境，初始化测试数据
    if (core.getAllClients().empty()) {
        cout << "\n[Init] 正在注册基础用户群..." << endl;
        // 注意：Register参数是 (Name, ID, Password)
        // 现在我们的系统主键逻辑已经迁移到 Name 上
        core.registerClient("Alice", "id_001", "pass1");
        core.registerClient("Bob", "id_002", "pass2");
        core.registerClient("Charlie", "id_003", "pass3");
        core.registerClient("Dave", "id_004", "pass4");
        core.registerClient("Eve", "id_005", "pass5");
    }

    // --- 场景一：基于名字的快速查找测试 ---
    printSeparator("场景一：AVL树 查找功能测试");

    vector<string> names_to_find = {"Alice", "Eve", "Charlie", "NonExistentUser"};

    for (const string& name : names_to_find) {
        cout << "正在查找用户 [" << name << "] ... ";
        Client* user = core.getClientByName(name);
        
        if (user) {
            cout << "✅ 找到! (ID: " << user->ID() << ", 内存地址: " << user << ")" << endl;
        } else {
            cout << "❌ 未找到 (符合预期)" << endl;
        }
    }

    // --- 场景二：注册查重测试 ---
    printSeparator("场景二：注册查重测试");
    
    cout << "尝试注册已存在的用户名 [Alice]..." << endl;
    CoreStatus status = core.registerClient("Alice", "id_new_alice", "123");
    if (status == ERR_CLIENT_EXISTS) {
        cout << "✅ 注册失败：用户已存在 (查重逻辑生效)" << endl;
    } else {
        cout << "❌ 错误：系统允许了重名注册！" << endl;
    }

    cout << "尝试注册新用户 [Frank]..." << endl;
    status = core.registerClient("Frank", "id_006", "pass6");
    if (status == SUCCESS) {
        cout << "✅ 注册成功。" << endl;
        // 立即验证是否能查到
        if (core.getClientByName("Frank")) {
            cout << "✅ 索引同步成功：可以立即查找到 Frank。" << endl;
        } else {
            cout << "❌ 索引同步失败：无法查找到刚注册的用户。" << endl;
        }
    } else {
        cout << "❌ 注册失败 (Status: " << status << ")" << endl;
    }


    // --- 场景三：业务操作一致性测试 ---
    printSeparator("场景三：业务操作测试 (使用查找结果)");

    Client* alice = core.getClientByName("Alice");
    Client* bob = core.getClientByName("Bob");

    if (alice && bob) {
        // 1. Alice 发帖
        cout << "-> Alice 发布一条动态..." << endl;
        core.userAddPost(alice, "AVL树真好用", "查找速度飞快，再也不用遍历链表了！");
        Post* p_alice = &alice->posts.tail_ptr()->data; // 获取最新帖子
        
        cout << "   [验证] Alice 当前发帖数: " << alice->posts.size() << endl;

        // 2. Bob 查找并点赞
        cout << "-> Bob 搜索 Alice 并点赞..." << endl;
        // 模拟 Bob 通过名字找到 Alice (已经在上面做了)
        core.userLikePost(bob, p_alice);
        cout << "   [验证] 帖子点赞数: " << p_alice->likes_num() << endl;

        // 3. Bob 评论
        cout << "-> Bob 评论帖子..." << endl;
        core.userAddComment(bob, p_alice, "确实，O(logN) 比 O(N) 强多了。");
        
        // 4. 撤销测试
        cout << "-> Bob 撤销评论..." << endl;
        core.userUndo(bob);
        cout << "   [验证] 撤销后评论数: " << p_alice->comment_list.size() << " (预期减少1)" << endl;
    } else {
        cout << "❌ 严重错误：无法获取测试用户，跳过业务测试。" << endl;
    }


    // --- 场景四：哈夫曼分析回归测试 ---
    printSeparator("场景四：哈夫曼分析回归测试");
    
    Client* frank = core.getClientByName("Frank");
    if (frank) {
        core.userAddPost(frank, "测试文本", "This is a test string for Huffman coding. We expect it to be compressed.");
        Post* p_frank = &frank->posts.tail_ptr()->data;
        core.analyzePostContent(p_frank);
    }


    // --- 最终状态展示 ---
    printSeparator("最终系统状态");
    SeqList<Client>& all = core.getAllClients();
    cout << "当前系统用户总数: " << all.size() << endl;
    cout << "用户列表:" << endl;
    for(int i=0; i<all.size(); ++i) {
        cout << " - " << all[i].Name() << " (ID: " << all[i].ID() << ")" << endl;
    }

    // 保存数据
    cout << "\n>>> 正在保存数据... <<<" << endl;
    if (core.saveData()) {
        cout << "[系统] 数据保存成功。" << endl;
    } else {
        cout << "[错误] 保存失败。" << endl;
    }

    return 0;
}