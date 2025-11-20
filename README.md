# 社交网络管理系统 (Social Network Analysis System)

基于 C++ 的社交网络核心后端，所有基础数据结构、JSON 读写与业务调度均为手写实现，面向教学和系统设计练习。

## 亮点
- 手写容器与内存管理：SeqList/LinkList/Stack/Queue/AVL/Heap/Huffman/Graph 全部自制，不依赖 std 容器或智能指针。
- Core-Model 分层：Core 作为唯一入口，模型保持轻量，UndoManager 保证生命周期安全，FileManager 负责多遍加载的 JSON 持久化。
- 完整撤销链路：发帖、删帖、评论、删评、点赞均可撤销，fake_remove + invalidate 机制规避悬空指针。
- 社交图谱：用自研无向图存储好友关系，支持 BFS/DFS、最短路径、度数统计与遍历导出。
- 内容分析：内置哈夫曼树对帖子内容做压缩/解压与 WPL 计算，提供分析结果字符串给 GUI。
- 排行榜：Sorter 生成用户影响力榜（好友数）与全站热帖榜（点赞数），并支持个人热帖 Top-N。

## 功能清单
- 用户：注册、按 Name 查找、好友互加/解除、关系距离查询与最短路径。
- 内容：发帖、删帖、帖内评论（含楼中楼）、点赞，消息通知队列。
- 撤销：对发帖/删帖/评论/删评/点赞的单步撤销，Fake_Stack 限制深度，节点 fake_remove 避免悬空。
- 持久化：clients/posts/comments/friend_edges 扁平 JSON，多遍加载重建指针，支持特殊字符转义。
- 排行/分析：好友数影响力榜、全站热帖榜、个人热帖榜；哈夫曼编码/压缩/解压与 WPL 输出。
- 图算法：BFS/DFS 遍历，最短路径，度数统计，孤岛检测。

## 系统特色
1) 纯手写底层：不引入 std 容器或智能指针，所有链式/顺序结构、自定义堆和搜索树都从 0 实现，方便学习指针与内存管理。  
2) Core-Model 解耦：业务逻辑集中在 Core/Manager 层，模型类保持数据载体角色，方便后期扩展 GUI 或其他入口。  
3) 撤销安全：UndoManager 维护 Action 注册表；LinkList 的 fake_remove 让节点地址稳定，真正析构时统一 invalidate，避免悬空指针。  
4) 手写 JSON 持久化：自研转义/解析器，扁平化存储对象关系，支持指针重建、点赞列表、好友关系还原。  
5) 多模块融合：哈夫曼分析、社交图谱、AVL 索引和排序器共同工作，提供分析、查询、排行等综合能力。

## 项目结构
```
.
├── CMakeLists.txt
├── main.cpp                # 场景化集成测试入口
├── data/clients.json       # 持久化文件
├── include/
│   ├── data_structure/     # SeqList, LinkList, Stack, Queue, Huffman, AVL, Heap...
│   ├── manager/            # Core, FileManager, UndoManager
│   └── models/             # Client/Post/Comment/Action/Message/SocialGraph/Sorter
└── src/                    # 以上头文件的实现
```

## 核心设计
- **Core (单例)**：处理所有业务请求、撤销入栈、消息分发、AVLT 索引重建、社交图同步。
- **UndoManager (单例)**：记录 Action 对象，监听被 fake_remove 的节点，在真实析构时批量 invalidate。
- **FileManager (单例)**：自写 JSON 解析/转义，扁平化存储 clients/posts/comments 与好友关系；多遍加载解决指针重建。
- **SocialGraph**：邻接表存好友，提供最短路径、深度/广度遍历，支持关系可视化与距离查询。
- **Sorter**：维护 SeqList 指针索引，基于自定义比较函数排序，输出排行榜数据给 CLI/GUI。

## 关键流程
- 加载：按“用户 → 帖子 → 评论 → 点赞者 → 好友边”顺序重建，保证指针稳定。
- 互动：Core 入口完成发帖/评论/点赞并推送消息，UndoManager 记录 Action，Sorter/Graph 同步更新。
- 撤销：Fake_Stack 弹出 Action 执行逆操作；已析构节点由 UndoManager invalidate 后不会被访问。
- 哈夫曼：countFrequency → HuffmanTree 构建 → generateCodes → compress/decompress → WPL 统计，可直接打印分析结果。
- 图谱：SocialGraph 维护邻接表； shortestPath 用 BFS，depthFirstPath 用 DFS，遍历接口便于 GUI 可视化。
- 排行：Sorter 初始化 SeqList 指针索引并排序，提供全局热帖、用户影响力、个人热帖 Top-N。

## 持久化策略
- 扁平格式：clients/posts/comments + friend_edges 分开保存（global_id 形如 `clientName_postIndex`），避免循环引用。
- 多遍加载：先加载用户固定内存地址，再加载帖子并链接作者，随后加载评论链接帖子/作者，最后补齐点赞者与好友边。
- 转义与容错：自写 escape/unescape，处理换行与特殊字符，尽量减少对第三方依赖。

示例 JSON 结构（节选）
```json
{
  "clients": [{ "name": "Alice", "id": "alice_id", "posts": ["Alice_0"] }],
  "posts":   [{ "global_id": "Alice_0", "author": "Alice", "likers": ["Bob"] }],
  "comments":[{ "post": "Alice_0", "author": "Charlie", "reply_floor": 2 }],
  "friend_edges": [{ "a": "Alice", "b": "Bob" }]
}
```

## 数据结构速览
- **SeqList**：可扩容顺序表，替代 std::vector。
- **LinkList**：带 fake_remove/auto_insert 的链表，服务撤销与节点稳定地址。
- **Fake_Stack**：带容量上限的撤销栈，避免无界内存占用。
- **SearchTree/AVLTree**：基于名字的用户索引用于快速查找。
- **HuffmanTree**：对字符串统计词频、生成编码、压缩/解压并计算 WPL。

## 构建与运行
```bash
mkdir -p build
cd build
cmake ..
make
./homework           # 可执行文件输出在 build/ 目录
```
数据默认写入 `data/clients.json`（相对 build 目录路径为 `../data/clients.json`）。

## 集成测试场景 (main.cpp)
- 栈溢出保护：疯狂点赞/取消仅保留最近 N 步撤销，验证 Fake_Stack 容量限制与无泄漏。
- 幽灵互动：帖子被逻辑删除后其他 Action 如何被 invalidate，撤销仍可安全恢复。
- 楼中楼递归：多层评论引用与 JSON 重建的依赖顺序验证。
- 毁灭性冲突：跨用户删除/撤销交叉，UndoManager 是否正确广播失效通知。
- 哈夫曼分析：长帖压缩/解压、编码表和 WPL 计算展示。
- 社交图谱：好友链距离、孤岛用户接入，以及 BFS/DFS 遍历输出。
- 排行榜：好友数影响力榜与全局热帖榜的排序正确性。

## GUI 集成要点
- 数据入口唯一：界面层只需调用 `Core::instance()` 提供的接口 (注册/发帖/评论/点赞/撤销/好友操作/排行/图谱/Huffman)。
- 排行与展示：  
  - `getUserInfluenceRanking(topN)` 返回 Client* 顺序表，可直接渲染影响力榜。  
  - `getHotPostRanking(topN)` 与 `getTopPostsForClient(client, topN)` 提供全局与个人热帖。  
- 图谱可视化：  
  - `getRelationDistance(a, b)` 显示最短距离。  
  - `getShortestRelationPath(a, b)`/`getDepthFirstRelationPath(a, b)` 给出路径节点序列。  
  - `getBfsRelationTraversal(root)`/`getDfsRelationTraversal(root)` 可用于层级树/力导向图。  
- Huffman 分析：界面展示 `getHuffmanAnalysisResult(post)` 的编码/压缩/解压摘要。  
- 消息流：`userReadMessages(client)` 拉取并清空消息队列，可用于通知中心。

## 接口速查 (Core.h)
- 账号：`registerClient(name, id, pwd)`，`getClientByName(name)`。
- 内容：`userAddPost(user, title, content)`，`userDeletePost(user, index|Post*)`，`userAddComment(user, post, content, reply_floor)`，`userDeleteComment(user, post, floor)`，`userLikePost(user, post)`。
- 撤销：`userUndo(user)`。
- 消息：`userReadMessages(user)`。
- 社交：`makeFriend(a, b)`，`deleteFriend(a, b)`，`getRelationDistance(a, b)`。
- 图谱遍历：`getShortestRelationPath` / `getDepthFirstRelationPath` / `getBfsRelationTraversal` / `getDfsRelationTraversal`。
- 排行：`getUserInfluenceRanking(topN)`，`getHotPostRanking(topN)`，`getTopPostsForClient(client, topN)`。
- 分析：`analyzePostContent(post)`，`getHuffmanAnalysisResult(post)`。

## 拓展与优化方向
- GUI/前端：可增加“消息中心”、“关系可视化”与“哈夫曼分析”面板，提供撤销历史列表与 Top-N 榜单快速跳转。
- 性能：为 FileManager 增加简单校验/增量保存；为 Huffman/排序添加性能计时便于教学对比。
- 安全：可在 Core 层补充权限/输入校验；为 Undo/Redo 预留双向栈实现 redo。
- 数据结构实验：替换自研容器为 std 版本作对比实验，或扩展图为加权/有向图并加入最短路 (Dijkstra)。
- 观测：在 main 或 GUI 中增加日志开关，便于演示 fake_remove/invalidate 等内部行为。

## 开发者QQ
- 某不愿意透露姓名的肥宅： 2523796700
- 某不愿意透露姓名的神秘男: 2390572866

## 开发小记
- 用户唯一索引为 Name，ID 格式不再限制。
- Graph 与用户顺序表共存；重建/加载时保持指针稳定，需要按 FileManager 的多遍流程处理。
- 若添加 GUI，可直接调用 Core 的 ranking/graph/huffman 接口获取展示数据。
