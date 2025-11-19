# **社交网络管理系统 (Social Network Analysis System)**

这是一个基于 C++ 实现的社交网络后端核心系统。

**项目特色**：

1. **纯手写底层架构**：不依赖 C++ 标准库容器（如 std::vector, std::map）和智能指针，所有数据结构（链表、顺序表、栈、队列）均为从零实现。  
2. **核心与模型分离**：采用 Core-Model 架构，将复杂的业务逻辑集中在管理层，模型层保持轻量。  
3. **强大的撤销 (Undo) 机制**：支持对发帖、删帖、评论、删评、点赞等操作的撤销，并解决了对象生命周期管理中的“悬空指针”问题。  
4. **自定义 JSON 持久化**：实现了手写的 JSON 解析器和生成器，支持复杂对象关系（如指针引用、多层楼中楼）的保存与重建。

## **📁 项目结构**

.  
├── CMakeLists.txt          \# CMake 构建配置  
├── main.cpp                \# 程序入口与压力测试脚本  
├── data/                   \# 数据存储目录  
│   └── clients.json        \# JSON 持久化文件  
├── include/                \# 头文件  
│   ├── data\_structure/     \# 自定义数据结构 (List, Stack, Queue, Huffman)  
│   ├── manager/            \# 管理层 (Core, FileManager, UndoManager)  
│   └── models/             \# 数据模型 (Client, Post, Comment, Action, Message)  
└── src/                    \# 源代码实现  
    ├── data\_structure/  
    ├── manager/  
    └── models/

## **🏗️ 架构设计**

系统采用了分层架构，确保逻辑清晰、职责单一。

### **1\. 核心管理层 (Manager)**

* **Core (单例)**:  
  * **系统的“大脑”**。它是外部调用（如 main.cpp 或 GUI）与内部模型交互的唯一入口。  
  * 负责处理发帖、评论、点赞等业务逻辑，管理用户撤销栈，分发消息通知。  
  * **设计意图**：将业务逻辑从数据模型中剥离，避免模型类过度膨胀。  
* **UndoManager (单例)**:  
  * **生命周期卫士**。它维护了一个注册表，记录了所有 Post 对象与引用该帖子的 Action 对象之间的关系。  
  * **防止野指针**：当一个帖子被彻底销毁时，UndoManager 会通知所有相关的 Action 将内部指针置空 (invalidate)，从而防止撤销操作访问非法内存。  
* **FileManager (单例)**:  
  * **数据持久化**。负责将内存中的对象图序列化为 JSON 字符串，并解析 JSON 还原对象。  
  * **多遍加载 (Multi-pass Loading)**：采用“先加载对象，后链接关系”的策略，解决了指针重建时的依赖问题（例如：在所有用户加载完成前，无法确立点赞关系）。

### **2\. 数据模型层 (Models)**

* **Client**: 用户实体。存储个人信息、帖子列表 (LinkList\<Post\>)、操作撤销栈 (Fake\_Stack) 和消息队列。  
* **Post**: 帖子实体。存储内容、作者指针、评论列表 (LinkList\<Comment\>) 和点赞者列表。  
* **Comment**: 评论实体。支持楼中楼逻辑（记录 reply\_floor）。  
* **Action (继承体系)**: 命令模式的实现。包括 PostAction, CommentAction, LikeAction，封装了具体的撤销逻辑。

### **3\. 基础设施层 (Data Structure)**

* **SeqList**: 动态顺序表，替代 std::vector。  
* **LinkList**: 双向/单向链表，支持 fake\_remove（假删除）和 auto\_insert（自动恢复），专为撤销机制优化。  
* **Fake\_Stack**: 基于队列实现的有限容量栈，用于限制用户的最大撤销步数（例如只保留最近 10 次操作）。  
* **LinkQueue**: 链式队列，用于消息通知。

## **🚀 编译与运行**

本项目使用 CMake 进行构建。请确保您的环境已安装 cmake 和 g++ (或 clang++)。

### **1\. 构建项目**

在项目根目录下执行以下命令：

\# 1\. 创建构建目录  
mkdir build  
cd build

\# 2\. 生成 Makefile  
cmake ..

\# 3\. 编译  
make

### **2\. 运行程序**

编译成功后，在 build 目录下运行可执行文件：

./homework

## **🧪 测试场景说明 (main.cpp)**

main.cpp 中包含了一套“终极压力测试”，用于验证系统的健壮性。当首次运行（无数据）时，会自动执行以下场景：

1. **栈溢出测试 (The Stack Overflow)**  
   * 模拟用户疯狂操作（如连续点赞/取消赞 15 次）。  
   * **验证点**：系统应仅保留最近的 10 次操作，早期的操作被安全丢弃且不内存泄漏。  
2. **“幽灵”互动测试 (The Ghost Interaction)**  
   * 模拟用户删除帖子后，其他用户（或管理员）试图对该帖子进行操作。  
   * **验证点**：验证 Action 对象在帖子被“逻辑删除”后是否依然持有有效的节点指针，以及撤销删除后内容是否能完美复原。  
3. **深度楼中楼递归 (Nested Replies)**  
   * 创建 10 层嵌套的评论回复。  
   * **验证点**：JSON 解析器能否正确处理深层依赖，正确重建每一层评论的楼层引用关系。  
4. **毁灭性冲突测试 (Destructive Conflict)**  
   * 用户 A 删除帖子，导致用户 B 对该贴的“删除评论”操作失效。  
   * **验证点**：UndoManager 是否正确通知了用户 B 的 Action 进行 invalidate，防止用户 B 撤销时程序崩溃。

## **🛠️ 关键技术细节**

### **手写 JSON 解析与扁平化存储**

为了解决对象间的循环引用和加载依赖（例如：Post 依赖 Author，Comment 依赖 Post 和 Author），FileManager 不使用嵌套结构，而是采用**扁平化**结构存储：

{  
  "clients": \[ ...所有用户... \],  
  "posts": \[ ...所有帖子 (包含 global\_id)... \],  
  "comments": \[ ...所有评论 (通过 global\_id 引用帖子)... \]  
}

加载时分为四步：

1. 加载所有 Client（内存地址固定）。  
2. 加载所有 Post，链接 Author 指针。  
3. 加载所有 Comment，链接 Post 和 Author 指针。  
4. 根据临时表，链接所有 Likers（点赞者）指针。

### **内存安全与撤销**

在不使用 std::shared\_ptr 的情况下，为了防止撤销操作访问已被 delete 的对象：

* **Fake Remove**: 链表的删除操作 fake\_remove 仅将节点从链表断开，但**不释放内存**。该节点被 Action 接管。  
* **Observer Pattern**: UndoManager 充当观察者。当一个对象被**真正**析构时，它会通知所有持有该对象引用的 Action，将其内部指针置为 nullptr。