# Dmine_Pro

## 概述
    Dmine_Pro是基于GPAR的挖掘算法Dmine进行优化与修改的程序，其优化部分包括：
    1.使用了目前业界运行效率较高的DAF(DPISO)算法作为匹配框架，相比于VF2算法大大提升了匹配效率
    2.结合Dmine中GPAR生成的特点，采用了拓展图相关理论的剪枝以及基于GPAR-DFSCode的去重方法用于减少无用GPAR的个数从而提高效率。
    3.考虑到实际数据的特点，使用了一套筛选方法来删除部分无用的搜索空间以提高搜索效率的同时保留了大部分有用好的结果。

## 参数说明
    Dmine_pro的参数分为两个部分，一个是运行参数（写在Yaml文件中的参数），另外一个是程序的相关算法需要的一些可不自己设置而采用默认配置的参数（在gpar_artifical.h中）

### 运行参数
    默认配置文件为./dmine_pro.yaml
    yaml配置文件格式如下：

    NodeFile: _v的csv
    EdgeFile: _e的csv
    OutputDir: TopK的GPAR的输出目录
    NodeXLabel: q(x,y)的x
    NodeYLabel: q(x,y)的y
    EdgeQLabel: q(x,y)的q
    SuppRLimit: 若supp_r<SuppRLimit,则不进入TopK计算以及拓展
    Radius: GPAR的最大边数
    TopK: TopK GPAR的数目

    yaml文件中可选择配置的内容：
    Root:   自定义初始的GPAR，可以有多个
     -
      name: 名字
      NodeFile:点文件
      EdgeFile:边文件
      XID:x的ID
      YID:y的ID

### 默认配置参数
    相关默认配置参数以及说明如下所示
#### similiar_limit
    相似程度的一个阈值，常规的相似阈值可以为0.75-0.85左右，如果不想采用筛选方法，将其设置为1即可
#### eps
    double判断的误差，一般在1e-6～1e-10都是合理的
#### y_label_count_1
    若已知Data Graph中y_label只有一个的话，可以将其设置为true
    若该值为true,则可以在supp_R的计算中进行加速（对于supp_Q,查看其q_label的出点是否存在y_label的点即可）
#### using_prune
    是否采用基于拓展图剪枝的方法
#### L_size
    基于拓展图的剪枝需要维护一个supp_Q=0以及supp_R小于阈值的GPAR列表，考虑到随着挖掘深度的增加，剪枝的费时也会逐渐增长，若不限制这个列表的数量，则会出现剪枝时间>该GPAR在Data Graph上的匹配时间的情况，因此设定一个L_size用于限制他们的大小
    L_size的计算可以参考以下公式
    （L_size)<=（该轮的无用的GPAR的数量/该轮的GPAR生成数量）*（无用GPAR在Data Graph上的期望时间）/(两个pattern的期望匹配时间)
    一般来说，在同一种Data Graph下，该轮的无用的GPAR的数量/该轮的GPAR生成数量 基本在某一范围内浮动，比如在联通数据图下，这个比值在0.95左右
    无用GPAR在Data Graph上的期望时间）/(两个pattern的期望匹配时间) 可通过不采用剪枝的方法对结果进行统计

#### erase_edge_label，erase_vertex_label
    若无法对数据图进行处理，则可通过这两个参数设定不想在GPAR中出现的点的label和边的label的集合

#### using_dfs_code
    若采用基于GPAR-DFSCode对GPAR进行去重的话，设置为true,否则采用两两匹配的去重方法
    基于GPAR-DFSCode的去重方法为了保证一定的效率，会存在去重不干净的情况，因此如果每轮的数量特别的少（比如就几十个），可以采用两两匹配的方法
#### using_selectk
    若采用Dmine_Pro的筛选方法，将其设置为true
#### select_k
    采用Dmine_Pro的筛选方法时，每轮最多留下的GPAR数量

#### using_manual_check
    若需要人工筛选一些结构不合法的GPAR时，将其设置为true

#### manual_check
    人工检验GPAR合法性的函数
    

