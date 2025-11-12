#include "models/sort.h"

//  自定义工具函数 
template <typename T>
void mySwap(T& a, T& b) {
    T temp = std::move(a);
    a = std::move(b);
    b = std::move(temp);
}

int myMin(int a, int b) {
    return a < b ? a : b;
}

// 排序算法实现 
// 插入排序（降序）- 小规模数据
template <typename T>
void insertionSort(SeqList<T>& arr, bool (*count)(const T& a, const T& b)) {
    int n = arr.getSize();
    for (int i = 1; i < n; i++) {
        T key = std::move(arr[i]);
        int j = i - 1;
        while (j >= 0 && count(key, arr[j])) {
            arr[j + 1] = std::move(arr[j]);
            j--;
        }
        arr[j + 1] = std::move(key);
    }
}

// 快速排序（降序）- 中等规模数据
template <typename T>
int partition(SeqList<T>& arr, int left, int right, bool (*count)(const T& a, const T& b)) {
    T pivot = std::move(arr[right]);
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

// 堆排序（降序）- 大规模数据
template <typename T>
void heapify(SeqList<T>& arr, int n, int i, bool (*count)(const T& a, const T& b)) {
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < n && count(arr[left], arr[largest])) largest = left;
    if (right < n&& count(arr[right], arr[largest])) largest = right;

    if (largest != i) {
        mySwap(arr[i], arr[largest]);
        heapify(arr, n, largest, count);
    }
}

template <typename T>
void heapSort(SeqList<T>& arr, bool (*count)(const T& a, const T& b)) {
    int n = arr.getSize();
    for (int i = n / 2 - 1; i >= 0; i--) {
        heapify(arr, n, i, count);
    }
    for (int i = n - 1; i > 0; i--) {
        mySwap(arr[0], arr[i]);
        heapify(arr, i, 0, count);
    }
}

// 自适应排序（按数据量选择算法）
const int SMALL_SCALE = 100;   // 小规模阈值
const int LARGE_SCALE = 10000; // 大规模阈值

template <typename T>
void ShiyingSort(SeqList<T>& arr, bool (*count)(const T& a, const T& b)) {
    int n = arr.getSize();
    if (n <= 1) return;

    if (n <= SMALL_SCALE) {
        std::cout << "(使用插入排序，数据量: " << n << ")" << std::endl;
        insertionSort(arr, count);
    }
    else if (n <= LARGE_SCALE) {
        std::cout << "(使用快速排序，数据量: " << n << ")" << std::endl;
        quickSort(arr, 0, n - 1, count);
    }
    else {
        std::cout << "(使用堆排序，数据量: " << n << ")" << std::endl;
        heapSort(arr, count);
    }
}

//  比较函数（核心排序规则） 
// 用户排序：按好友数降序（影响力）
bool compareHaoyou(const client& a, const client& b) {
    return a.friendCount > b.friendCount;
}

// 帖子排序：按点赞数降序（热门程度）
bool comparePost(const Post& a, const Post& b) {
    return a.likes > b.likes;
}

// 排行榜生成函数（核心功能） 
// 1. 用户影响力排行榜（按好友数）+ 每个用户的个人热门帖子
void haoyoushu(SeqList<client>& clients) {
    if (clients.getSize() == 0) {
        std::cout << "没有用户数据可生成排行榜" << std::endl;
        return;
    }

    // 步骤1：按好友数排序用户（影响力排行）
    ShiyingSort(clients, compareHaoyou);

    // 步骤2：对每个用户的帖子按点赞数排序（个人热门帖子）
    for (int i = 0; i < clients.getSize(); i++) {
        ShiyingSort(clients[i].posts, comparePost);
    }

    // 步骤3：展示结果（用户Top10 + 每个用户的Top3帖子）
    std::cout << "\n=== 用户影响力排行榜（按好友数） ===" << std::endl;
    int displayUserCnt = myMin(10, clients.getSize());
    for (int i = 0; i < displayUserCnt; i++) {
        client& user = clients[i];
        std::cout << i + 1 << ". 用户ID: " << user.id
            << ", 好友数: " << user.friendCount
            << ", 发布帖子数: " << user.posts.getSize() << std::endl;

        // 显示该用户的Top3热门帖子
        int displayPostCnt = myMin(3, user.posts.getSize());
        if (displayPostCnt > 0) {
            std::cout << "   个人热门帖子：" << std::endl;
            for (int j = 0; j < displayPostCnt; j++) {
                Post& post = user.posts[j];
                std::cout << "     " << j + 1 << ". 帖子ID: " << post.id
                    << ", 点赞数: " << post.likes << std::endl;
            }
        }
    }

    if (clients.getSize() > 10) {
        std::cout << "... 还有 " << clients.getSize() - 10 << " 名用户未显示" << std::endl;
    }
}

// 2. 全局热门帖子排行榜（从所有用户的帖子中收集，按点赞数）
void dianzanshu(SeqList<client>& users) {
    if (users.getSize() == 0) {
        std::cout << "没有用户数据，无法生成热门帖子排行榜" << std::endl;
        return;
    }

    // 步骤1：收集所有用户的所有帖子（全局帖子池）
    SeqList<Post> allPosts;
    for (int i = 0; i < users.getSize(); i++) {
        client& user = users[i];
        for (int j = 0; j < user.posts.getSize(); j++) {
            allPosts.add(user.posts[j]); // 拷贝帖子到全局池
        }
    }

    if (allPosts.getSize() == 0) {
        std::cout << "没有帖子数据可生成排行榜" << std::endl;
        return;
    }

    // 步骤2：按点赞数排序全局帖子
    ShiyingSort(allPosts, comparePost);

    // 步骤3：展示结果（全局Top10帖子，显示所属用户）
    std::cout << "\n=== 全局热门帖子排行榜（按点赞数） ===" << std::endl;
    int displayPostCnt = myMin(10, allPosts.getSize());
    for (int i = 0; i < displayPostCnt; i++) {
        Post& post = allPosts[i];
        std::cout << i + 1 << ". 帖子ID: " << post.id
            << ", 所属用户: " << post.userId
            << ", 点赞数: " << post.likes << std::endl;
    }

    if (allPosts.getSize() > 10) {
        std::cout << "... 还有 " << allPosts.getSize() - 10 << " 个帖子未显示" << std::endl;
    }
}
