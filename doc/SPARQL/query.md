# 用SPARQL来查询匹配

## 例子
query.rq内容
```
PREFIX pre: <liantongdata/> 

SELECT distinct ?v1
WHERE
{   # edge
    ?v3 ?v4 "call" .
    ?v4 ?v1 "call" .
    ?v3 ?v5 "call" .
    ?v5 ?v1 "call" .
    ?v3 ?v6 "call" .
    ?v6 ?v1 "call" .
    ?v3 ?v2 "has" .
    ?v1 ?v2 "has" .
    ?v4 ?v7 "has" .
    ?v5 ?v7 "has" .
    ?v6 ?v7 "has" .
    #label
    ?v1 pre:user "has label" .
    ?v2 pre:fake_type_is_1 "has label" .
    ?v3 pre:user "has label" .
    ?v4 pre:phone "has label" .
    ?v5 pre:phone "has label" .
    ?v6 pre:phone "has label" .
    ?v7 pre:phone_head_is_130 "has label" .
    #iso
    Filter(?v1!=?v3) .
    Filter(?v4!=?v5 && ?v4!=?v6 && ?v5!=?v6) .
}
```

## 相关解释

    PREFIX:前缀，类似c++的nampspace 目前联通数据转的rdf前缀均为"\<liantongdata/\>" 

    SELECT WHERE:与SQL类似

    ?v1 ?v2...:代表需要匹配的实体节点，与pattern中的节点等价
    
    Filter():根据括号内部的条件过滤结果

## 如何将同构匹配转化为一个SPARQL查询

```
#前缀 采用联通数据必须要写
PREFIX pre: <liantongdata/> 

#SELECT:后面写的是匹配需要输出的节点，可以写多个，另外需要添加distinct来去重
SELECT distinct ?v1
WHERE
{   # 这部分与pattern的边对应 格式为 src dst label
    # 每个节点前面一定要有"?"，且最后要带一个 .
    # label写的是实际语义 可以在label.csv查看
    ?v3 ?v4 "call" .
    ?v4 ?v1 "call" .
    ?v3 ?v5 "call" .
    ?v5 ?v1 "call" .
    ?v3 ?v6 "call" .
    ?v6 ?v1 "call" .
    ?v3 ?v2 "has" .
    ?v1 ?v2 "has" .
    ?v4 ?v7 "has" .
    ?v5 ?v7 "has" .
    ?v6 ?v7 "has" .
    # 这部分写的是每个节点的label应该是什么
    # 格式： vertex pre:label "has label"
    # 其中label代表的是实际语义
    # 同样 需要末尾带一个 . 节点前要有"?"
    ?v1 pre:user "has label" .
    ?v2 pre:fake_type_is_1 "has label" .
    ?v3 pre:user "has label" .
    ?v4 pre:phone "has label" .
    ?v5 pre:phone "has label" .
    ?v6 pre:phone "has label" .
    ?v7 pre:phone_head_is_130 "has label" .
    # 这部分是限制条件 代表两两点不能重
    # 只需要保证相同label的点不重复即可
    Filter(?v1!=?v3) .
    Filter(?v4!=?v5 && ?v4!=?v6 && ?v5!=?v6) .
}
```