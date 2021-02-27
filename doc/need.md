# GPAR各方面需要改进的部分

## 效率
### 运行时间
edge=3 6172s
### 需要做的改进
#### 匹配部分
1.LargeGraph缺少InVertexBegin(edge_label,src_label) OutVertexBegin(edge_label,dst_label)接口(UpdateCandidateSet时使用)，CountInVertex,CountOutVertex,CountInVertex(vertex_label),CountOutVertex(vertex_label)接口(InitCandidateSet时使用)，InEdgeBegin(edge_label,src_ptr),OutEdgeBegin(edge_label,dst_ptr)接口(IsJoinable时使用)
基于目前的图的话 在VertexBegin不需要做src_label
InVertexBegin(edge_label),OutVertexBegin(edge_label),CountInVertex(edge_label),CountOutVertex(edge_label)
结构:label做key
CountInVertex,CountOutVertex,
InEdgeBegin(edge_label,src_ptr),OutEdgeBegin(edge_label,dst_ptr)
结构：vertex_ptr做key，然后label做第二个key
2.DetermineMatchOrder部分可以提前处理好邻接节点
3.InitCandidateSet部分可以适当优化一下写法
#### 挖掘部分
目前拓展-去重过程比较暴力 参考其他论文是否有很好的解决办法（目前未知）
#### 其他部分
boost iso的缩点因为写法比较差在大图跑的比较慢 有时间尝试优化（优先级比较低）

## 效果
### 挖掘结果分析
1.visit web这种边在Top30种就出现一条 表示visit web这个信息其实无法有效区分
2.conf=1的pattern就两个 筛出的fake user个数分别为1与15，对应的pattern分别为"若user1与phone3互相发短信，且phone3给user4打电话，则user1为fake"以及"若phone3给user1打电话，且phone3与user4互相发过短信，那么user1一定是fake"
3.其他pattern的conf都在0.94以上，筛出的user(fake+not fake)个数大概在100-150这个区间，且大多数pattern筛出的user数量相同，推测大概率有很大重合
4.Top30的pattern中出现了"若user1 xxx,user2 xxx user2为fake，那么user1为fake"的pattern,分别为“若phone3给user1发短信，且user2给phone3打电话，若user2为fake，则user1为fake”以及“若phone3给user1发短信，且phone3给user2打电话，若user2为fake，则user1为fake”，筛出的fake user分别为135以及136，推测重合度很大
5.观察了中间过程中生成conf=0的pattern,发现以下特点：
5.1  存在pattern"若phone3给user1打电话以及发短信，那么user1一定不是fake"以及"若phone3给user1发短信且user1给phone3打电话，那么user1一定不是fake"
5.2  存在pattern"若user1给phone3打电话/发短信，phone3给user4发过短信并且user4也给phone3打电话（根据5.1说明user4肯定不是fake），则user1一定不是fake"，"若user1给phone3打电话，phone3给user4以及user5发过短信，则user1一定不是fake"

6.观察了中间过程中没match的pattern,发现以下特点：
6.1 存在pattern"若phone3给user1发短信且user1给phone3打了两次电话"，"若phone3给user1打电话以及发短信，且user4给phone3打电话"，"若phone3给user4发短信且user4给phone3打电话，并且phone3给user1打电话"，"若phone3给user1打电话以及发短信，且user1给phone3打电话"
### 需要改进的部分
1.跟原来的数据相比 部分数据展开不全，pattern表示的语义有限，可以考虑展开部分属性重新挖掘
1.1 "visit web"语义上，若web只有label的话，在访问网站的行为上，无法挖掘出很好的结果
1.2 "call"或者"send"语义上，目前的pattern只强调次数，不强调动作的其他性质（通话时长，开始时间等等），无法挖掘出更加有用的信息
2.在目前的图上，“visit web”边效果又差又费时间
3.目前挖掘的TopK Pattern的Supp值不是特别高（100-150，期望应该在300+）
4.多重边有一定用处，是否应该去掉？

## 工程
### 需要改进部分
1.对于pattern部分的输出以及信息统计最好单独写一个.txt文件
2.配置在bddev上
3.多机

## 其他
1.考虑换一种思路拓展：对于每个GPAR打分，每次选择分数最大的GPAR进行拓展，将拓展的结果加入到队列中。
1.1 打分标准应该是什么？
1.2 结束的条件是什么？已经匹配的GPAR个数？
2.确定好需要挖掘的GPAR的形式，根据形式的特点重新定义Supp,Conf以及挖掘算法
2.1 形式大概要什么样？
2.2 期望的边有多少？
2.3 特点是什么？
