# Graph With Attribute

## 表示形式
$$ (V,E,A)$$
$V$ :点集
$E$ :边集
$A$ :属性literal集合，每个属性literal的表示形式为$x.A=c$，其中$x$为节点编号，$A$为属性名，$c$为其值
    
## 例子
![Graph With Attribute](/Users/apple/Desktop/buaa/code/dmine-pro/doc/AD958166-C0B5-47FC-BC6D-7926FC4F6C0D.png)


# Graph Pattern With Attribute

## 表示形式
$$ (V,E,A)$$
$V$ :点集
$E$ :边集
$A$ :属性literal集合，每个属性literal的表示形式为$x.A=c,x.A=y.B,x.A$
    
## 例子
![Graph Pattern With Attribute](/Users/apple/Desktop/buaa/code/dmine-pro/doc/5662E903-5029-408B-8E96-26E214C0E3D6.png)

# Attribute Expansion Graph

## 表示形式
$$ (V,E,V_{A},V_{V},V_{C},E_{A},E_{V},E_{C})$$

$V$ :原图中的点集
$E$ :原图中的边集
$V_{A}$ :Attribute点
$V_{V}$ :Value点
$V_{C}$ :Constant点
$E_{A}$ :$E_{A} \in V*V_{A}$，其中$(u,v)$表示一条从$u$到$v$的有向边
$E_{V}$ :$E_{V} \in V_{A}*V_{V}$，其中$(u,v)$表示一条从$u$到$v$的有向边
$E_{C}$ :$E_{C} \in V_{V}*V_{C}$，其中$(u,v)$表示一条从$u$到$v$的有向边

## 限制条件
1.若$(u,v) \in E_{C}$,则不存在$u'\not=u$使得$(u',v)\in E_{C}$
2.若$(u,v) \in E_{C}$,则不存在$v'\not=v$使得$(u,v')\in E_{C}$
3.若$(u,v) \in E_{V}$,则不存在$v'\not=v$使得$(u,v')\in E_{C}$
4.若$(u,v) \in E_{A}$,则不存在$u'\not=u$使得$(u',v)\in E_{C}$
5.对于$v \in V_{A}$,一定有$indegree(v)\geq1,outdegree(v)=1$
6.对于$v \in V_{V}$,一定有$indegree(v)\geq1,outdegree(v)=1$
7.对于$v \in V_{C}$,一定有$indegree(v)=1,outdegree(v)= 0 $

## 说明
1.value点与constant点一一对应，有value点一定就要有对应的constant点
2.一个attribute点只能连向一个value点，一个value点可与多个Attribute点一一对应
3.一个实体点可与多个Attribute点对应，一个Attribute点只能对应一个实体点

## 例子
![Data Graph](/Users/apple/Desktop/buaa/code/dmine-pro/doc/DataGraph.png)

# Graph With Attribute转化为Attribute Expansion Graph
对于Graph With Attribute.A中的每个literal，进行如下操作：
1.在x节点后添加一个Attribute节点，表示其属性A，并将x节点与Attribute节点脸边
2.若图中存在表示值为c的Value节点，则将该Attribute节点与其连边，否则在该Attribute节点后添加一个Value节点并添加从Attribute节点与Value节点的有向边
3.若操作2中的Value节点为新加入节点，则在该Value节点后添加一个label为c的constant节点，表示其值为constant，并将该Value节点与Constant节点相连

##例子
对于![Graph With Attribute](/Users/apple/Desktop/buaa/code/dmine-pro/doc/AD958166-C0B5-47FC-BC6D-7926FC4F6C0D.png)
首先，展开literal $phone_1.head=130$，得到如下结果
![Result1](/Users/apple/Desktop/buaa/code/dmine-pro/doc/E0052C59-07B0-4619-B340-DAF4F4A3C96A.png)
接着，展开literal $phone_3.head=130$，由于Value为130的Value节点已经存在，不单独加点，得到如下结果
![Result2](/Users/apple/Desktop/buaa/code/dmine-pro/doc/E1CFC643-445A-419D-8C63-E7F52614C068.png)
最后，展开literal $phone_2.length=11$,得到如下结果
![Data Graph](/Users/apple/Desktop/buaa/code/dmine-pro/doc/DataGraph.png)

# Attribute Expansion Graph Pattern

## 表示形式
$$ (V,E,V_{A},V_{V},V_{C},E_{A},E_{V},E_{C})$$

$V$ :原图中的点集
$E$ :原图中的边集
$V_{A}$ :Attribute点
$V_{V}$ :Value点
$V_{C}$ :Constant点
$E_{A}$ :$E_{A} \in V*V_{A}$，其中$(u,v)$表示一条从$u$到$v$的有向边
$E_{V}$ :$E_{V} \in V_{A}*V_{V}$，其中$(u,v)$表示一条从$u$到$v$的有向边
$E_{C}$ :$E_{C} \in V_{V}*V_{C}$，其中$(u,v)$表示一条从$u$到$v$的有向边

## 限制条件
1.若$(u,v) \in E_{C}$,则不存在$u'\not=u$使得$(u',v)\in E_{C}$
2.若$(u,v) \in E_{C}$,则不存在$v'\not=v$使得$(u,v')\in E_{C}$
3.若$(u,v) \in E_{V}$,则不存在$v'\not=v$使得$(u,v')\in E_{C}$
4.若$(u,v) \in E_{A}$,则不存在$u'\not=u$使得$(u',v)\in E_{C}$
5.对于$v \in V_{A}$,一定有$indegree(v)=1,outdegree(v)=1$
6.对于$v \in V_{V}$,一定有$indegree(v)\geq1,outdegree(v)\leq1,degree(v)\geq2$
7.对于$v \in V_{C}$,一定有$indegree(v)=1,outdegree(v)= 0 $

## 说明
1.value点与constant点一一对应,若value点没有对应的constant点，需要有多个attribute点对应该value点
2.一个attribute点只能连向一个value点，一个value点可与多个Attribute点一一对应
3.一个实体点可与多个Attribute点对应，一个Attribute点只能对应一个实体点

##例子
![pattern](/Users/apple/Desktop/buaa/code/dmine-pro/doc/pattern.png)

# 性质
每一个Attribute Expansion Graph Pattern均可对应一个Graph Pattern+Literal Set，且每一个Graph Pattern+Literal Set的结合体均可表示为一个Attribute Expansion Graph Pattern。其中LiteralSet支持的语义有：
1.x.A
2.x.A=c
3.x.A=y.B

## 证明
先证明：每一个Attribute Expansion Graph Pattern均可对应一个Graph Pattern+Literal Set。
若$V_{A}=V_{V}=V_{C}=\emptyset$，则对应Graph Pattern+$\emptyset$
若$V_{A}\not =\emptyset，V_{V}=V_{C}=\emptyset$，则对应Graph Pattern+$x.A$,其中x为Attribute点对应的实体点
若$V_{A}\not =\emptyset，V_{V}\not =\emptyset，V_{C}=\emptyset$，此时的value点一定有多个Attribute点对应，此时可以对应的结构有：Graph Pattern+$x.A=y.B$,Graph Pattern+$x.A,x.A=y.B$
若$V_{A}\not =\emptyset，V_{V}\not =\emptyset，V_{C}\not=\emptyset$，此时的value点要么有多个Attribute点对应，要么有一个constant点对应。此时可以对应的结构有：Graph Pattern+$x.A=y.B,x.A=c$,Graph Pattern+$x.A,x.A=y.B,x.A=c$,Graph Pattern+$x.A,x.A=c$,Graph Pattern+$x.A=c$
此时，Graph Pattern+Literal Set的全部情况均对应上

再证明：对于任意的一种raph Pattern+Literal Set,都可以转化为一个Attribute Expansion Graph Pattern
对于x.A，在x节点处拓展一个Attribute节点即可
对于x.A=y.B，在x节点拓展一个Attribute节点A,在y节点处拓展一个Attribute B，将A,B节点与同一个value节点相连
对于x.A=c,在x节点拓展一个Attribute节点A，在A节点拓展一个value节点V,在V节点拓展一个constant节点C即可

# 挖掘算法
基于传统方法的挖掘算法（一条一条加边）做一定变动，即将加边操作变为实体拓展或属性拓展，挖掘的是在一定的操作步数内能够得到的Attribute Expansion Graph Pattern
实体拓展：正常加边
属性拓展：分为以下几种情况
1.在某个实体节点拓展一个A节点
2.选择>=2个Attribute节点，新建一个Value节点且让选择的Attribute节点与该节点连边
3.选择一个Value节点，拓展一个constant节点


## 分类讨论literal展开情况

若仅有x.A，则对于每个x，添加一个Label为A的attribute节点
展开数：1
添加的节点数:全图出现的属性类别数量（一个属性一个点）
添加的label数：全图出现的属性类别数量（一个属性一个label）

若仅有x.A=c，则对于每个x，添加一个Label为(A,c)的Attribute_Constant节点
展开数：1
添加的节点数:全图出现的(A,c)数量（一个（属性，值）的pair对应一个点）
添加的label数：全图出现的(A,c)数量（一个（属性，值）的pair对应一个点label）

若仅有x.A=y.B，则对于每个x，添加一个Label为(A)的attribute点，在attribute点后添加一个Label为Value_C的Value点
展开数：2
添加的节点数:全图出现的(A,c)数量+全图出现的C数量
添加的label数：全图出现的(A)数量+全图出现的C数量（虽然每个x都展开一个A节点，但是不同的节点的A的label实际上是相同的）

若有x.A以及x.A=c，则对于每个x，添加一个Label为A的attribute节点，在attribute节点后添加一个label为c的constant节点
展开数：2
添加的节点数:全图出现的(x.A)数量+全图出现的C数量
添加的label数：全图出现的(A)数量+全图出现的C数量(每个x.A展开一个节点，但是label是一样的）

若有x.A以及x.A=y.B,则对于每个x，添加一个Label为(A)的attribute点，在attribute点后添加一个Label为Value_C的Value点
展开数：2
添加的节点数:全图出现的(x.A)数量+全图出现的属性值数量
添加的label数：全图出现的(A)数量+全图出现的C数量（虽然每个x都展开一个A节点，但是不同的节点的A的label实际上是相同的）

若有x.A=c以及x.A=y.B，则对于每个x，添加一个Label为(A)的attribute点，在attribute点后添加一个Label为Value_C的Value点，在Value_C的Value点后添加一个Label为C的constant点
展开数：3
添加的节点数：全图出现的(A,c)数量+全图出现的C数量+全图出现的C数量
添加的label数：全图出现的A数量+全图出现的C数量+全图出现的C数量

若有x.A,x.A=c以及x.A=y.B，则对于每个x，添加一个Label为(A)的attribute点，在attribute点后添加一个Label为Value_C的Value点，在Value_C的Value点后添加一个Label为C的constant点
展开数：3
添加的节点数：全图出现的(x.A)数量+全图出现的C数量+全图出现的C数量
添加的label数：全图出现的A数量+全图出现的C数量+全图出现的C数量


| LiteralSet | 最小展开级数 | 增加的点种类 | 增加点数 | 增加label数 |
| --- | --- | --- | --- | --- |
| x.A | 1 | Attribute | A | A |
| x.A=c | 1 | Attribute-constant | (A,c) | (A,c) |
| x.A=y.B | 2 | Attribute,Value | (A,c)+c | A+c |
| x.A x.A=c | 2 | Attribute,Constant | (x.A)+c | A+c |
| x.A x.A=y.B | 2 | Attribute,Value | (x.A)+c | A+c |
| x.A=c x.A=y.B | 3 | Attribute,Value,Constant | (A,c)+c+c | A+c+c |
| x.A x.A=c x.A=y.B | 3 | Attribute,Value,Constant | (x.A)+c+c | A+c+c |
