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
    printSeparator("社交网络管理系统 (Social Graph Test)");

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
        core.registerClient("Alice", "id_001", "pass1");
        core.registerClient("Bob", "id_002", "pass2");
        core.registerClient("Charlie", "id_003", "pass3");
        core.registerClient("Dave", "id_004", "pass4");
        core.registerClient("Eve", "id_005", "pass5");
        core.registerClient("Frank", "id_006", "pass6"); 
    }
    
    // 获取用户指针
    Client* alice = core.getClientByName("Alice");
    Client* bob = core.getClientByName("Bob");
    Client* charlie = core.getClientByName("Charlie");
    Client* dave = core.getClientByName("Dave");
    Client* eve = core.getClientByName("Eve");

    // --- 场景一：建立社交网络 ---
    printSeparator("场景一：建立社交网络 (加好友)");
    
    if (alice && bob && charlie && dave && eve) {
        cout << "构建社交网络..." << endl;
        // 构建链式关系: Alice --friend--> Bob --friend--> Charlie --friend--> Dave
        // Eve 是孤立的
        
        if (core.makeFriend(alice, bob) == SUCCESS)
            cout << " + [Alice] 和 [Bob] 成为好友" << endl;
        
        if (core.makeFriend(bob, charlie) == SUCCESS)
            cout << " + [Bob] 和 [Charlie] 成为好友" << endl;
        
        if (core.makeFriend(charlie, dave) == SUCCESS)
            cout << " + [Charlie] 和 [Dave] 成为好友" << endl;

        // 验证距离
        int d1 = core.getRelationDistance(alice, bob);
        cout << "\n[验证] Alice 和 Bob 的距离: " << d1 << " (预期: 1)" << endl;

        int d2 = core.getRelationDistance(alice, charlie);
        cout << "[验证] Alice 和 Charlie 的距离: " << d2 << " (预期: 2)" << endl;

        int d3 = core.getRelationDistance(alice, dave);
        cout << "[验证] Alice 和 Dave 的距离: " << d3 << " (预期: 3)" << endl;
    } else {
        cout << "❌ 无法获取所有测试用户，跳过测试。" << endl;
    }

    // --- 场景二：删除好友关系 ---
    printSeparator("场景二：删除好友关系 (断开链接)");
    
    if (alice && bob && charlie) {
        cout << "尝试删除 [Bob] 和 [Charlie] 的好友关系..." << endl;
        CoreStatus status = core.deleteFriend(bob, charlie);
        
        if (status == SUCCESS) {
            cout << "✅ 删除成功。" << endl;
            
            // 验证直接关系
            int d_direct = core.getRelationDistance(bob, charlie);
            cout << "   -> Bob 和 Charlie 的距离: " << d_direct << " (预期: -1, 不连通)" << endl;
            
            // 验证间接关系 (链路是否断裂)
            // 原本 Alice -> Bob -> Charlie -> Dave
            // 现在 Bob -> Charlie 断了，所以 Alice -> Dave 也应该断了
            int d_chain = core.getRelationDistance(alice, dave);
            cout << "   -> Alice 和 Dave 的距离: " << d_chain << " (预期: -1, 链路断裂)" << endl;
            
        } else {
            cout << "❌ 删除失败 (Status: " << status << ")" << endl;
        }
    }

    // --- 场景三：重新连接与环路 ---
    printSeparator("场景三：重新连接 (多路径)");
    
    if (alice && charlie && eve) {
        cout << "建立新关系: [Alice] --friend--> [Eve] --friend--> [Charlie]" << endl;
        core.makeFriend(alice, eve);
        core.makeFriend(eve, charlie);
        
        // 现在 Alice 可以通过 Eve 到达 Charlie (距离2)
        // Alice -> Eve -> Charlie -> Dave (距离3)
        
        int d_new = core.getRelationDistance(alice, dave);
        cout << "\n[验证] Alice 和 Dave 的新距离: " << d_new << " (预期: 3, 路径: Alice->Eve->Charlie->Dave)" << endl;
        
        if (d_new == 3) {
            cout << "✅ 社交图谱路径计算正确 (BFS 找到了绕行路径)。" << endl;
        } else {
            cout << "❌ 路径计算错误。" << endl;
        }
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