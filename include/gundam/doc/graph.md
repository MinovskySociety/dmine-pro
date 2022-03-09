# Comment 
- [1.GUNDAM::Graph使用说明](#1-gundamgraph-%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E)
   - [1.1相关规定](#11-%E7%9B%B8%E5%85%B3%E8%A7%84%E5%AE%9A)
   - [1.2定义](#12-%E5%AE%9A%E4%B9%89)
   - [1.3接口](#13-%E6%8E%A5%E5%8F%A3)
   - [1.4示例程序](#14-%E7%A4%BA%E4%BE%8B%E7%A8%8B%E5%BA%8F)
   - [1.5相关类型定义](#15-%E7%9B%B8%E5%85%B3%E7%B1%BB%E5%9E%8B%E5%AE%9A%E4%B9%89)
## 1. GUNDAM::Graph 使用说明

本篇文档的目的主要是针对GUNDAM::Graph进行一些接口和使用上的说明，并提供相应的示例程序。
### 1.1 相关规定

1.图的节点与边均有ID以及Label，且节点的ID需要两两不同，边的ID需要两两不同<br>
2.图的节点和边可以选择是否存在Attribute<br>
3.属性由(key,value)二元组构成。以联通数据为例，phone节点的phonenumber:123456789 属性中，key是phonenumber,value是13456789<br>
4.GUNDAM中有数据类型到枚举类型的映射，用BasicDataType表示，其结构为:<br>
```c++
enum class BasicDataType {
  int_,
  double_,
  string_,
  unknown_type_
};
```
其中前三个枚举表示全部整型，全部浮点型以及字符串类型，最后一个表示其他类型

### 1.2 定义

Graph的一般性定义如下所示:

```c++
//when no "using namespace GUNDAM;"
GUNDAM::Graph<configure...> g;
//using namespace GUNDAM;
using namespace GUNDAM;
Graph<configures...> g;
```
其中，configures...表示对于图中一些类型的配置项。配置项均有默认类型。配置项先后次序没有要求。<br>
配置类型可以是任意类型。若需要将图写入到csv中，则数据类型只支持基础数据类型。<br>
configures...中可配置项如下表所示：<br>


|configure|说明|type配置类型|type默认类型/值|
|:--|:--|:--|:--|
|SetVertexIDType\<type\>|设置节点的ID类型|任意数据类型|unsigned int|
|SetVertexLabelType\<type\>|设置节点的Label类型|任意数据类型|unsigned int|
|SetVertexHasAttribute\<type\>|设置节点是否拥有属性|true/false|true|
|SetVertexAttributeKeyType\<type\>|设置节点的属性的key的类型|任意数据类型|unsigned int|
|SetEdgeIDType\<type\>|设置边的ID的类型|任意数据类型|unsigned int|
|SetEdgeLabelType\<type\>|设置边的Label类型|任意数据类型|unsigned int|
|SetEdgeHasAttribute\<type\>|设置边是否拥有属性|true/false|true|
|SetEdgeAttributeKeyType\<type\>|设置边的属性的key的类型|任意数据类型|unsigned int|
|SetAllowMultipleEdge\<type\>|设置图是否有重边|true/false|false|

一些图的配置样例如下所示:
```c++
//只配置节点id和label,其他采用默认配置
//其他部分配置情况类似
GUNDAM::Graph<SetVertexIDType<int>,SetVertexLabelType<std::string>> g;
//全部采用默认配置
GUNDAM::Graph<> g;
//全配置的结果
GUNDAM::Graph<SetVertexIDType<uint32_t>, SetVertexLabelType<uint16_t>,
SetEdgeIDType<uint32_t>, SetEdgeLabelType<std::string>,SetAllowMultipleEdge<true>,
SetVertexHasAttribute<true>, SetEdgeHasAttribute<true>,
SetVertexAttributeKeyType<std::string>,
SetEdgeAttributeKeyType<uint32_t>> g;
```

### 1.3 接口

Graph的可调用接口表如下所示:<br>



|接口|接口格式|接口说明|
|:--|:--|:--|
|AddVertex|pair\<VertexPtr,bool\> AddVertex(vertex_id,vertex_label)<br>vertex_id:点的id<br>vertex_label:点的label<br>返回值:若当前Graph中已有ID为vertex_id的vertex，则返回的pair为：<指向已有节点的指针, false>；否则加入新的节点，并返回<指向新加入节点的指针, true>。|往Graph中新加入一个ID为vertex_id,Label为vertex_label的节点|
|AddEdge|pair\<EdgePtr, bool\> AddEdge(src_id,dst_id,edge_label,edge_id)<br>src_id:起点的id<br>dst_id:终点的id<br>edge_label:边的label<br>edge_id:边的id<br>返回值：若当前Graph中已有相同ID的edge，则返回的pair为：<指向已有边的指针, false>；否则加入新的边，并返回<指向新加入边的指针, true>；若Graph中不存在ID为src_id的vertex、或Graph中不存在ID为dst_id的vertex，则返回<Null指针, false>。|往Graph中加入一条从src_id到dst_id,EdgeLabel为edge_label,EdgeID为edge_id的有向边|
|CountVertex|size_t CountVertex()<br>size_t CountVertex(vertex_label)<br>vertex_label:点的label|计算这个图的节点数量/满足VertexLabel为vertex_label的节点数量|
|FindVertex|VertexPtr FindVertex(vertex_id)<br>VertexConstPtr FindVertex(vertex_id) const<br>vertex_id:点的id<br>返回值：若图中存在vertex_id对应的节点，则返回其指针，否则返回Null指针|找到Graph中VertexID为vertex_id的节点的指针|
|FindConstVertex|VertexConstPtr FindConstVedrtex(vertex_id)<br>vertex_id:点的id<br>返回值：若图中存在vertex_id对应的节点，则返回其Const指针，否则返回Null指针|找到Graph中VertexID为vertex_id的节点的Const指针|
|FindEdge|EdgePtr FindEdge(edge_id)<br>edge_id:边的id<br>返回值：若图中存在edge_id对应的边，则返回其指针，否则返回Null指针|找到Graph中EdgeID为edge_id的边的指针|
|EraseEdge|bool EraseEdge(edge_id)<br>edge_id:边的id<br>返回值：若成功删除返回true,否则返回false|删除Graph中EdgeID为edge_id的边的指针|
|VertexBegin/VertexCBegin|VertexIterator VertexBegin()<br>VertexIteratorSpecifiedLabel VertexBegin(vertex_label)<br>VertexConstIterator VertexCBegin()<br>VertexConstIteratorSpecifiedLabel VertexCBegin(vertex_label)<br><br>vertex_label:点的label|得到Vertex/Label为vertex_label的Vertex的迭代器/常量迭代器|
|vertex_id_type|string vertex_id_type()<br>返回值:若类型是整型(int,long long,unsigned int等等)，返回"int",若类型是浮点类型(float,double,long double),返回"double"，若是std::string类型，返回"string"，其他类型返回"unknown type"|返回VertexIDType的字符串|
|vertex_label_type|string vertex_label_type()<br>返回值:若类型是整型(int,long long,unsigned int等等)，返回"int",若类型是浮点类型(float,double,long double),返回"double"，若是std::string类型，返回"string"，其他类型返回"unknown type"|返回VertexLabelType的字符串|
|edge_id_type|string edge_id_type()<br>返回值:若类型是整型(int,long long,unsigned int等等)，返回"int",若类型是浮点类型(float,double,long double),返回"double"，若是std::string类型，返回"string"，其他类型返回"unknown type"|返回EdgeIDType的字符串|
|edge_label_type|string edge_label_type()<br>返回值:若类型是整型(int,long long,unsigned int等等)，返回"int",若类型是浮点类型(float,double,long double),返回"double"，若是std::string类型，返回"string"，其他类型返回"unknown type"|返回EdgeLabelType的字符串|

### 1.4 示例程序

Graph的相关实例程序如下所示：

```c++
GUNDAM::Graph<> g;
for (int i=1;i<=10;i++){
    //Add Vertex
    g.AddVertex(i,1);
}
for (int i=11;i<=20;i++){
    g.AddVertex(i,2);
}
int edge_id =1;
for (int i=1;i<=10;i++){
    for (int j=1;j<=10;j++){
        if (i==j) continue;
        //add edge
        g.AddEdge(i,j,2,edge_id++);
    }
}
//visit all vertex
for (auto it = g.VertexBegin();!it.IsDone();it++{
    std::cout<<it->id()<<" "<< it->label()<<std::endl;
}
//visit all 2-label vertex
for (auto it = g.VertexBegin(2);!it.IsDone();it++{
    std::cout<<it->id()<<" "<< it->label()<<std::endl;
}
//vertex num
int node_num = g.CountVertex();
//count 1-label vertex
int label_is_1_num = g.CountVertex(1);
//Find Vertex
typename GUNDAM::Graph<>::VertexPtr node_ptr = g.FindVertex(2);
//Find Edge
typename GUNDAN::Graph<>::EdgePtr edge_ptr = g.FindEdge(10);
//Erase Edge
bool erase_flag = g.EraseEdge(11);
//print id and label type
std::cout<<"vertex id type = "<<g.vertex_id_type()<<std::endl;
std::cout<<"vertex label type = "<<g.vertex_label_type()<<std::endl;
std::cout<<"edge id type = "<<g.edge_id_type()<<std::endl;
std::cout<<"edge label type = "<<g.edge_label_type()<<std::endl;

```

### 1.5 相关类型定义

GUNDAM中常见的类型定义如下所示：
```c++
using GraphType = GUNDAM::Graph<>;
using VertexType = typename GraphType::VertexType;
using EdgeType = typename GraphType::EdgeType;
using VertexIDType = typename VertexType::IDType;
using VertexLabelType = typename VertexType::LabelType;
using EdgeIDType = typename GraphType::EdgeType::IDType;
using EdgeLabelType = typename EdgeType::LabelType;
using VertexPtr = typename GraphType::VertexPtr;
using VertexConstPtr = typename GraphType::VertexConstPtr;
using EdgePtr = typename GraphType::EdgePtr;
using EdgeConstPtr = typename GraphType::EdgeConstPtr;
using VertexAttributeKeyType = typename GraphType::VertexType::AttributeKeyType;
using EdgeAttributeKeyType = typename GraphType::EdgeType::AttributeKeyType;
```