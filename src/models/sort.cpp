#include "models/sort.h"
#include "models/clients.h"
#include "models/Post.h"
#include <utility> // for std::swap

// --- 自定义工具函数 ---
template <typename T>
void mySwap(T& a, T& b) {
    T temp = a;
    a = b;
    b = temp;
}

int myMin(int a, int b) {
    return a < b ? a : b;
}

// --- 排序算法实现 ---

// 插入排序（降序）
template <typename T>
void insertionSort(SeqList<T>& arr, bool (*count)(const T& a, const T& b)) {
    int n = arr.size(); // 修正: getSize() -> size()
    for (int i = 1; i < n; i++) {
        T key = arr[i];
        int j = i - 1;
        while (j >= 0 && count(key, arr[j])) { 
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

// 快速排序（降序）
template <typename T>
int partition(SeqList<T>& arr, int left, int right, bool (*count)(const T& a, const T& b)) {
    T pivot = arr[right];
    int i = left - 1;
    for (int j = left; j <= right - 1; j++) {
        if (count(arr[j], pivot)) {
            i++;
            mySwap(arr[i], arr[j]);
        }
    }
    mySwap(arr[i + 1], arr[right]);
    return i + 1;
}

template <typename T>
void quickSort(SeqList<T>& arr, int left, int right, bool (*count)(const T& a, const T& b)) {
    if (left < right) {
        int pivotIdx = partition(arr, left, right, count);
        quickSort(arr, left, pivotIdx - 1, count);
        quickSort(arr, pivotIdx + 1, right, count);
    }
}

// 堆排序（降序）
template <typename T>
void heapify(SeqList<T>& arr, int n, int i, bool (*count)(const T& a, const T& b)) {
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < n && count(arr[left], arr[largest])) largest = left;
    if (right < n && count(arr[right], arr[largest])) largest = right;

    if (largest != i) {
        mySwap(arr[i], arr[largest]);
        heapify(arr, n, largest, count);
    }
}

template <typename T>
void heapSort(SeqList<T>& arr, bool (*count)(const T& a, const T& b)) {
    int n = arr.size();
    for (int i = n / 2 - 1; i >= 0; i--) {
        heapify(arr, n, i, count);
    }
    for (int i = n - 1; i > 0; i--) {
        mySwap(arr[0], arr[i]);
        heapify(arr, i, 0, count);
    }
}

// 自适应排序
const int SMALL_SCALE = 100;   
const int LARGE_SCALE = 10000; 

template <typename T>
void ShiyingSort(SeqList<T>& arr, bool (*count)(const T& a, const T& b)) {
    int n = arr.size(); // 修正: getSize() -> size()
    if (n <= 1) return;

    if (n <= SMALL_SCALE) {
        // std::cout << "(使用插入排序)" << std::endl;
        insertionSort(arr, count);
    }
    else if (n <= LARGE_SCALE) {
        // std::cout << "(使用快速排序)" << std::endl;
        quickSort(arr, 0, n - 1, count);
    }
    else {
        // std::cout << "(使用堆排序)" << std::endl;
        heapSort(arr, count);
    }
}

// --- 比较函数 ---

// 用户排序：按好友数降序，ID升序
bool compareHaoyou(const Client& a, const Client& b) {
    if (a.get_friends_num() != b.get_friends_num()) { // 修正: 使用 getter
        return a.get_friends_num() > b.get_friends_num(); 
    }
    return a.ID() < b.ID(); // 修正: 使用 ID() 方法
}

// 帖子排序：按点赞数降序，ID升序
bool comparePost(const Post& a, const Post& b) {
    if (a.likes_num() != b.likes_num()) { // 修正: 使用 likes_num() 方法
        return a.likes_num() > b.likes_num(); 
    }
    return a.get_idex() < b.get_idex(); // 修正: 使用 get_idex() 方法
}

// --- 排行榜生成函数 ---

// 1. 用户影响力排行榜
void haoyoushu(SeqList<Client>& clients) {
    if (clients.size() == 0) {
        std::cout << "没有用户数据可生成排行榜" << std::endl;
        return;
    }

    // 1. 对用户列表排序
    ShiyingSort(clients, compareHaoyou);

    std::cout << "\n=== 用户影响力排行榜（按好友数） ===" << std::endl;
    int displayUserCnt = myMin(10, clients.size());
    
    for (int i = 0; i < displayUserCnt; i++) {
        Client& user = clients[i]; // 修正: client -> Client
        std::cout << i + 1 << ". 用户ID: " << user.ID()
            << ", 昵称: " << user.Name()
            << ", 好友数: " << user.get_friends_num()
            << ", 发布帖子数: " << user.posts.size() << std::endl;

        // 2. 处理用户的热门帖子
        // 修正: user.posts 是 LinkList，不能直接传给 ShiyingSort (需要 SeqList)
        // 解决方案: 将帖子复制到临时的 SeqList 中进行排序
        if (user.posts.size() > 0) {
            SeqList<Post> tempPosts(user.posts.size() + 10);
            for(int k = 0; k < user.posts.size(); k++) {
                tempPosts.add(user.posts[k]);
            }
            
            ShiyingSort(tempPosts, comparePost);

            int displayPostCnt = myMin(3, tempPosts.size());
            if (displayPostCnt > 0) {
                std::cout << "   个人热门帖子：" << std::endl;
                for (int j = 0; j < displayPostCnt; j++) {
                    Post& post = tempPosts[j];
                    std::cout << "     " << j + 1 << ". 帖子ID: " << post.get_idex()
                        << ", 标题: " << post.get_title()
                        << ", 点赞数: " << post.likes_num() << std::endl;
                }
            }
        }
    }

    if (clients.size() > 10) {
        std::cout << "... 还有 " << clients.size() - 10 << " 名用户未显示" << std::endl;
    }
}

// 2. 全局热门帖子排行榜
void dianzanshu(SeqList<Client>& users) { // 修正: client -> Client
    if (users.size() == 0) {
        std::cout << "没有用户数据，无法生成热门帖子排行榜" << std::endl;
        return;
    }

    // 1. 统计总贴数以初始化列表
    int totalPosts = 0;
    for(int i = 0; i < users.size(); i++) {
        totalPosts += users[i].posts.size();
    }

    if (totalPosts == 0) {
        std::cout << "没有帖子数据可生成排行榜" << std::endl;
        return;
    }

    // 2. 收集所有帖子
    SeqList<Post> allPosts(totalPosts + 10);
    for (int i = 0; i < users.size(); i++) {
        Client& user = users[i];
        for (int j = 0; j < user.posts.size(); j++) {
            allPosts.add(user.posts[j]); 
        }
    }

    // 3. 排序
    ShiyingSort(allPosts, comparePost);

    // 4. 展示
    std::cout << "\n=== 全局热门帖子排行榜（按点赞数） ===" << std::endl;
    int displayPostCnt = myMin(10, allPosts.size());
    for (int i = 0; i < displayPostCnt; i++) {
        Post& post = allPosts[i];
        std::string authorName = post.author ? post.author->Name() : "未知";
        
        std::cout << i + 1 << ". 帖子ID: " << post.get_idex()
            << ", 作者: " << authorName
            << ", 标题: " << post.get_title()
            << ", 点赞数: " << post.likes_num() << std::endl;
    }

    if (allPosts.size() > 10) {
        std::cout << "... 还有 " << allPosts.size() - 10 << " 个帖子未显示" << std::endl;
    }
}