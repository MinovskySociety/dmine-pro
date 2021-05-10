目前main分支还在逐步更新，稳定版本已移步到dev1.0分支

# dmine_pro使用方法
    运行方法:
    dmine_pro [yaml配置文件路径]
    若不写yaml配置文件，默认配置文件为./dmine_pro.yaml
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


# dmine_origin使用方法
    运行方法:
    dmine_origin [yaml配置文件路径]
    若不写yaml配置文件，默认配置文件为./dmine_origin.yaml
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

# gar_match使用方法:
    运行方法:
    ./gar_match.exe 或者 ./gar_match.exe 配置文件路径
    若不写配置文件路径，默认该配置文件位于config文件夹内且名字为gar_match_config.txt
    配置文件内容共7行，内容如下：    
    gar的pattern .v的csv文件路径
    gar的pattern .e的csv文件路径
    gar的literal  X的csv文件路径
    gar的literal  Y的csv文件路径
    datagraph    .v的csv文件路径
    datagraph    .e的csv文件路径
    输出结果的文件路径

# gar_match_using_yaml使用方法
    运行方法:
    ./gar_match_using_yaml 或者 ./gar_match_using_yaml yaml文件路径
    若不写yaml文件路径，默认该yaml文件与gar_match_using_yaml在同一目录下，且文件名为gar_match_config.yaml
    yaml文件内容如下：
    Graph:  
    Name: 图名字
    Dir: 图所在文件夹
    VertexFile:
        - 点文件csv
    EdgeFile:
        - 边文件csv

    GAR:
        Name: GAR名字
        Dir: GAR所在目录
        VertexFile: 点文件csv
        EdgeFile: 边文件csv
        LiteralX: LiteralX文件csv
        LiteralY: LiteralY文件csv

    ResultDir: 匹配结果存储目录
    所有匹配结果写在一个csv，名字为Name_match.csv，格式如下：
    match_id,query id1,query id2,....
    1,target id1,target id2.....
    2,target id1,target id2.....

# Literal文件格式
    以GAR为例，其literal.csv的格式如下所示：
    type,x_id,x_attr,y_id,y_attr,edge_label,c
    Edge,1,,5,,2,
    其中第一行是表头，每一行代表一个literal
    type表示该literal的类型，目前支持的类型有"Attribute","Edge","Constant","Variable"
    其中每个类型代表的意义可参考论文
    x_id表示literal中x对应的id
    x_attr表示的是x节点对应的属性的key，其只会在"Attribute","Constant","Variable"出现，相当于x.A的A
    y_id表示"Edge"，"Variable" Literal中 y的id
    y_attr表示"Variable" Literal中 y.B的B
    edge_label表示"Edge" Literal中加边的label
    c表示的是"Constant" Literal中的c，其格式为value;类型 比如可以写成3.1415926;double或者
    Tom;string

# gar_match_with_pivot使用方法:
    运行方法:
    ./gar_match_with_pivot.exe 或者 ./gar_match_with_pivot.exe 配置文件路径
    若不写配置文件路径，默认该配置文件位于config文件夹内且名字为gar_match_with_pivot_config.txt
    配置文件内容共9行，内容如下：    
    gar的pattern .v的csv文件路径
    gar的pattern .e的csv文件路径
    gar的literal  X的csv文件路径
    gar的literal  Y的csv文件路径
    datagraph    .v的csv文件路径
    datagraph    .e的csv文件路径
    输出结果的文件路径
    pivot的数量
    全部的pivot，用空格隔开

# gar_chase使用方法
    运行方法:
    gar_chase [yaml配置文件路径]    
    若不写yaml配置文件，默认配置文件为./gar_chase.yaml
    yaml文件内容如下：
    Graph:  
    Name: 图名字
    Dir: 图所在文件夹
    VertexFile:
        - 点文件csv
    EdgeFile:
        - 边文件csv

    GAR:
       -
        Name: GAR名字
        Dir: GAR所在目录
        VertexFile: 点文件csv
        EdgeFile: 边文件csv
        LiteralX: LiteralX文件csv
        LiteralY: LiteralY文件csv
       -
       .....

    OutputGraph:
        Name: 输出图名字
        Dir: 输出图路径
        VertexFile: 点文件名称
        EdgeFile: 边文件名称

# tgraph_discover
    简介: 时序图关联规则(TGR)的发现算法(TGRD)
    运行方法:
    ./tgraph_discover discover.yaml配置文件路径 [time_window.yaml配置文件路径]
    discover.yaml配置文件格式如下：
    Graph:
      Name: 时序图名字
      Dir: 时序图所在文件夹
      VertexFile: 点文件csv
      EdgeFile: 边文件csv
    OutputGraph:
      Name: 发现的TGR名字
      Dir: 发现的Top-K个TGR所在文件夹
    Settings:
      NodeXLabel: q(x,y)的x
      NodeYLabel: q(x,y)的y
      EdgeQLabel: q(x,y)的q
      EdgeQHasOrder: 预测边是否有次序
      SuppRLimit: 若supp_r<SuppRLimit，则不进入TopK计算以及拓展
      TopK: TopK TGR的数目
      Radius: TGR的最大边数
      EdgeFrequency: 边扩展时，增加的边标签需要满足的最小频率
      ThreadNum: 使用的线程数量

    若不写time_window.yaml配置文件，默认采用一个时序图分区
    time_window.yaml配置文件格式如下：
    TimeWindowNumber: 按照给定时间窗口数目划分(优先考虑)
    TimeWindowSize: 按照给定时间窗口大小划分
    UsePartition: 是否使用时间窗口对时序图进行划分

# tgraph_apply使用方法
    简介: 基于时序图关联规则的事件预测算法(TGREP)
    运行方法:
    mpirun -n ProcessNum ./tgraph_apply apply.yaml配置文件路径 [time_window.yaml配置文件路径]
    apply.yaml文件内容如下：
    Graph:  
      Name: 时序图名字
      Dir: 时序图所在文件夹
      VertexFile: 点文件csv
      EdgeFile: 边文件csv

    TGR:
    - Name: TGR名字
      Dir: TGR所在目录
      VertexFile: 点文件csv
      EdgeFile: 边文件csv
      LiteralX: LiteralX文件csv，此处为空
      LiteralY: LiteralY文件csv，包含预测边

    OutputGraph: 匹配结果存储目录
      Name: 匹配结果文件名字
      Dir: 匹配结果所在目录
    所有匹配结果写在一个csv，名字为Name_match.csv，格式如下：
    match_id,query id1,query id2,....
    1,target id1,target id2.....
    2,target id1,target id2.....

# mgm使用方法
    简介: 基于MCTS的GPAR挖掘算法
    运行方法:
    ./bin/app/mgm mgm.yaml
    若不写yaml配置文件路径，默认配置文件为./mgm.yaml
    yaml配置文件格式如下：

    Graph:
        Name: 输入图数据的名称
        Dir: 输入图数据路径
        VertexFile: 输入图数据的点文件
        EdgeFile: 输入图数据的边文件

    MGM:
        Supp: 支持度阈值
        Conf: 置信度阈值
        EdgeSize: 边集规模阈值
        TopK: conf Top-K GPAR数量
        SrcNodeLabel: 预测边的源点标签
        DstNodeLabel: 预测边的目的点标签
        EdgeLabel: 预测边标签

    OutputGraph:
        Dir: 输出GPAR的路径


# multimgm使用方法
    简介: 基于MCTS的GPAR并行挖掘算法
    运行方法:
    mpirun -np ProcessNum ./bin/app/multimgm multimgm.yaml
    若不写yaml配置文件，默认配置文件为./multimgm.yaml
    yaml配置文件格式如下：

    Graph:
        Name: 输入图数据的名称
        Dir: 输入图数据路径
        VertexFile: 输入图数据的点文件
        EdgeFile: 输入图数据的边文件

    MGM:
        Supp: 支持度阈值
        Conf: 置信度阈值
        EdgeSize: 边集规模阈值
        TopK: conf Top-K GPAR数量
        SrcNodeLabel: 预测边的源点标签
        DstNodeLabel: 预测边的目的点标签
        EdgeLabel: 预测边标签

    OutputGraph:
        Dir: 输出GPAR的路径
