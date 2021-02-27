## GUNDAM todo
按照优先级排序
4.EdgePtr，AttributePtr内部迭代器生存周期问题(有待解决)
5.移动构造
6.匹配为Label相等情况下 考虑相关的优化策略
7.csvgraph读100个人要26s 具体原因不明
## Dmine todo
2.命令行参数解析（找个还行的库）
3.log的输出方式(glog)
1.multi-query实现（阅读相关论文，复现并嵌入相关应用）
## GAR todo
0.加入MLLiteral (基础数据结构部分已经完成，等待XML-RPC框架完成写update以及satisfy)
2.GAR discover 实现(基于GFD的办法先弄一个)
3.multi-query实现（阅读相关论文，复现并嵌入相关应用）
## others
2.联通数据一些分析
    基于GPAR的相关分析
    2.1 小图数据下成团的判定条件（精确到呼入/呼出，需要添加信息/web）
    2.2 某些pattern存在着从该pattern拓展出的pattern supp均为0的情况 抓出来看看具体有哪些
    2.3 分析一些匹配耗时比较大的Pattern，寻找原因，并抽象出可靠的剪枝策略
    基于GAR/GFD的分析
    2.4 根据现实情况构造出一些GAR/GFD 采用Match得到相关思路
    2.5 用discover先挖出一些GAR/GFD看看效果
3.query-driven ER相关论文
4.维护项目相关文档跟代码（GUNDAM,GPAR,GAR)
5.并行化（接到GRAPE，短期内不考虑执行) 
