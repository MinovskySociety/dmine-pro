# Graph Match设计文档
    该文档主要针对目前gundam的匹配算法进行相关设计

## 使用的子图同构算法
    使用的是与DA-cand类似的子图同构匹配算法 


## 目前使用的匹配类型
    目前使用的匹配的格式主要分为以下三类：
    1.传统的计算匹配（计算出全部匹配或者k个匹配）
    2.计算(query_id,target_id)是否存在一个完整的匹配
    3.在给定pivot的情况下，计算匹配，其中每种pivot只保留一个匹配（GPAR的supp计算，GFD的supp计算，GAR match的计算均属于这一类）
    4.可只计算pattern中部分点的匹配

    

## 结构设计
![Match](/Users/apple/Desktop/buaa/code/dmine-pro/doc/match.png)

### 设计思想
    采用stl算法的设计方式，针对不同的情况用不同的方法调用，但是对于核心部分代码（_VF2)不进行修改
#### _VF2
    匹配算法的递归部分，处于所有函数最终都需要调用的部分
#### VF2_Recurive(query,target,update_callback,prune_callback,user_callback)
    仅包含query和target的VF2算法，update_callback表示人为调整candidate_set的call_back(可用该callback来实现只匹配部分点)，prune_callback表示人为对递归过程中的状态剪枝的callback,user_callback表示处理相关匹配的call_back

#### VF2_Recurive(query,target,candidate_set,match_state,update_callback,prune_callback,user_callback)
    外部已经构造好candidate_set的时候的VF2匹配，match_state为空就表示不知道匹配，否则就表示已知的部分匹配。

#### VF2_Recurive(query,target,match_state,update_callback,prune_callback,user_callback)
    仅知道部分匹配的VF2匹配

#### VF2(query,target,max_result),VF2(query,target,max_result,match_result),VF2(query,target,user_callback)
    传统的子图同构匹配算法

#### VF2(query,target,prune_callback,update_callback,user_callback)
    自定义callback的子图同构算法

#### VF2(query,target,match_vertex,user_callback)
    只匹配match_vertex内的点的子图同构算法

#### VF2(query,target,pivot,user_callback)
    对pivot去重的子图同构算法
#### VF2(query,target,query_ptr,target_ptr,user_callback)
    检查是否存在(query_ptr,target_ptr)的匹配