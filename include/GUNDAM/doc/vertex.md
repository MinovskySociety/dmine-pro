# Comment 
- [1.Vertex使用说明](#1-vertex-%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E)
   - [1.1相关规定](#11-%E7%9B%B8%E5%85%B3%E8%A7%84%E5%AE%9A)
   - [1.2接口](#12-%E6%8E%A5%E5%8F%A3)
   - [1.3示例程序](#13-%E7%A4%BA%E4%BE%8B%E7%A8%8B%E5%BA%8F)
## 1. Vertex 使用说明

本篇文档的目的主要是针对Vertex进行一些接口和使用上的说明，并提供相应的示例程序。

### 1.1 相关规定

1.本库无法访问具体的Vertex对象，只能通过VertexPtr/迭代器来得到相关内容，因此调用接口时应该写->而不是.<br>
2.VertexPtr与VertexIterator两者的接口除了IsNull(判断VertexPtr是否为空)和IsDone(判断VertexIterator是否到end)不同，其余接口均相同。<br>
3.其他规定请参考 [点击此处](./graph.md)。

### 1.2 接口
VertexPtr及VertexIterator可调用接口表如下所示:


|接口|接口格式|接口说明|
|:--|:--|:--|
|AddAttribute|pair\<AtributePtr,bool\> AddAttribute(key,value)<br>key:属性的键值<br>value:属性的值<br>返回值:若该节点中已存在键为key的属性则返回<指向已有属性的指针, false>；否则，加入新的属性并返回<指向新属性的指针, true>|往节点中加入属性key值为key,value值为value的属性|
|attribute<br>const_attribute|template\<ValueType\>attribute(key)<br>key:属性的键值<br>ValueType:value值的数据类型<br>返回值:返回key对应的value|得到该节点键值为key的value/常量value|
|attribute_value_type|BasicDataType attribute_value_type(key)<br>key:属性的键值|得到该节点键值为key的value的类型的id|
|attribute_value_type_name|string attribute_value_type(key)<br>key:属性的键值<br>返回值:若类型是整型(int,long long,unsigned int等等)，返回"int",若类型是浮点类型(float,double,long double),返回"double"，若是std::string类型，返回"string"，其他类型返回"unknown type"|得到该节点键值为key的value的类型的名字|
|AttributeBegin<br>AttributeBegin|AttributeIterator AttributeBegin()<br>AttributeConstIterator AttributeCBegin()|得到这个节点属性的迭代器/常量迭代器|
|CountInEdge|size_t CountInEdge()|得到指向这个节点的边的数量|
|CountOutEdge|size_t CountOutEdge()|得到指出这个节点的边的数量|
|CountInEdgeLabel|size_t CountInEdgeLabel()|得到指向该节点的所有边的不同Label数量|
|CountOutEdgeLabel|size_t CountOutEdgeLabel()|得到指出该节点的所有边的不同Label数量|
|CountInVertex|size_t CountInVertex()<br><br>size_t CountInVertex(vertex_label)<br><br>vertex_label:节点的label|得到指向该节点所有边的起点的数量/VertexLabel为vertex_label的起点的数量|
|CountOutVertex|size_t CountOutVertex()<br>size_t CountOutVertex(vertex_label)<br>vertex_label:节点的label|得到指出该节点所有边的起点的数量/VertexLabel为vertex_label的起点的数量|
|EraseAttribute|bool EraseAttribute(key)<br>key:属性的键值<br>返回值：是否成功删除<br><br>AttributeIterator EraseAttribute(attribute_iterator)<br>attribute_iterator:属性的迭代器<br>返回值：返回删除的迭代器的下一个迭代器|删除该节点键值为key的属性或删除属性迭代器为attribute_iterator的属性|
|EraseEdge|bool EraseEdge(edge_id)<br>edge_id:边的id<br>返回值:返回是否成功被删除<br><br>EdgeIterator EraseEdge(edge_iterator)<br>edge_iterator:边的迭代器<br>返回值:若则返回被删除的边的下一条边的迭代器|删除EdgeID为edge_id的边或者删除迭代器为edge_iterator的边|
|FindAttributePtr<br><br>FindConstAttributePtr|AttributePtr FindAttributePtr(key)<br><br>AttributeConstPtr FindConstAttributePtr(key)<br><br>key:属性的键值<br>返回值:若找到了该属性则返回该属性的指针/常量指针，否则返回Null指针|找到该节点键值为key对应的Attribute指针/常量指针|
|FindInEdge|EdgePtr FindInEdge(edge_id)<br><br>EdgePtr FindInEdge (edge_label,edge_id)<br><br>EdgePtr FindInEdge(edge_label,src_ptr,edge_id);<br><br>edge_id:边的id<br>edge_label:边的label<br>src_ptr:边的起点的指针<br>返回值:若存在则返回该边的指针、若不存在则返回Null指针。|找到该节点入边的id为edge_id的边<br><br>找到该节点的Label为edge_label的入边集中查找ID为edge_id的边<br><br>在该节点的Label为edge_label的入边集中查找指向src_ptr、ID为edge_id的边|
|FindInVertex|VertexPtr FindInVertex(src_ptr)<br><br>VertexPtr FindInVertex(src_id)<br><br>VertexPtr FindInVertex(edge_label,src_ptr)<br><br>VertexPtr FindInVertex(edge_label,src_id)<br><br>src_ptr:边的起点对应的指针<br>src_id:边的起点对应的ID<br>edge_label:边的label<br>返回值：若该节点存在、则返回指向该节点的指针/常量指针，否则返回Null指针。|在该节点的入边指向的节点集合中查找指针src_ptr指向的vertex。<br><br>在该节点的入边指向的节点集合中查找ID为src_id的vertex。<br><br>在该节点Label为edge_label的入边指向的节点集合中查找ID为src_id的vertex。<br><br>在该节点Label为edge_label的入边指向的节点集合中查找ID为src_id的vertex。|
|FindOutEdge<br>FindConstOutEdge|EdgePtr FindOutEdge/FindConstOutEdge(edge_id)<br><br>EdgePtr FindOutEdge/FindConstOutEdge(edge_label,edge_id)<br><br>EdgePtr FindOutEdge/FindConstOutEdge(edge_label,dst_ptr,edge_id);<br><br>edge_id:边的id<br>edge_label:边的label<br>dst_ptr:边的终点的指针<br>返回值:若存在则返回该边的指针、若不存在则返回Null指针。|找到该节点出边的id为edge_id的边<br><br>找到该节点的Label为edge_label的出边集中查找ID为edge_id的边<br><br>在该节点的Label为edge_label的出边集中查找指向dst_ptr、ID为edge_id的边|
|FindOutVertex<br>FindConstOutVertex|VertexPtr FindOutVertex/FindConstOutVertex(dst_ptr)<br><br>VertexPtr FindOutVertex/FindConstOutVertex(dst_id)<br><br>VertexPtr FindOutVertex/FindConstOutVertex(edge_label,dst_ptr)<br><br>VertexPtr FindOutVertex/FindConstOutVertex(edge_label,dst_id)<br><br>dst_ptr:边的终点对应的指针<br>dst_id:边的终点对应的ID<br>edge_label:边的label<br>返回值：若该节点存在、则返回指向该节点的指针/常量指针，否则返回Null指针。|在该节点的出边指向的节点集合中查找指针dst_ptr指向的vertex。<br><br>在该节点的出边指向的节点集合中查找ID为dst_id的vertex。<br><br>在该节点Label为edge_label的出边指向的节点集合中查找指针为dst_ptr的vertex。<br><br>在该节点Label为edge_label的出边指向的节点集合中查找ID为dst_id的vertex。|
|id|VertexIDType id()|得到该节点的id|
|label|VertexLabelType label()|得到该节点的label|
|InEdgeBegin<br>InEdgeCBegin|EdgeIterator InEdgeBegin/InEdgeCBegin()<br><br>EdgeIteratorSpecifiedLabel InEdgeBegin/InEdgeCBegin(edge_label)<br><br>edge_label:边的label<br>返回值:边的迭代器|得到该点所有入边的迭代器<br><br>得到该点所有边Label为edge_label的入边的迭代器|
|InVertexBegin<br>InVertexCBegin|VertexIterator InVertexBegin/InVertexCBegin()<br><br>VertexIterator InVertexBegin/InVertexCBegin(edge_label)<br><br>edge_label:边的label<br>返回值:点的迭代器|得到该点所有入边的起点的迭代器<br><br>得到该点所有边Label为edge_label的入边起点的迭代器|
|InEdgeLabelBegin|EdgeLabelIterator InEdgeLabelBegin()<br><br>返回值:边label的迭代器|得到该点所有入边label的迭代器|
|OutEdgeBegin<br>OutEdgeCBegin|EdgeIterator OutEdgeBegin/OutEdgeCBegin()<br><br>EdgeIteratorSpecifiedLabel OutEdgeBegin/OutEdgeCBegin(edge_label)<br><br>edge_label:边的label<br>返回值:边的迭代器|得到该点所有出边的迭代器<br><br>得到该点所有边Label为edge_label的出边的迭代器|
|OutVertexBegin<br>OutVertexCBegin|VertexIterator OutVertexBegin/OutVertexCBegin()<br><br>VertexIterator OutVertexBegin/OutVertexCBegin(edge_label)<br><br>edge_label:边的label<br>返回值:点的迭代器|得到该点所有出边的终点的迭代器<br><br>得到该点所有边Label为edge_label的出边终点的迭代器|
|OutEdgeLabelBegin|EdgeLabelIterator OutEdgeLabelBegin()<br><br>返回值:边label的迭代器|得到该点所有出边label的迭代器|
|SetAttribute|pair<AttributePtr, bool> SetAttribute(key,value)<br><br>key:属性的key<br>value:属性的value<br>返回值:若该节点中存在键为key的属性则返回<指向修改后属性的指针, true>；否则，返回<NULL指针, false>|将该节点中键为key的属性的值设为value|
|IsNull|bool IsNull()|判断指针是否非空|

### 1.3 示例程序


```c++
GUNDAM::graph<> g;
VertexPtr node_ptr = g.FindVertex(1);
std::cout<<it->id()<<" "<<it->label()<<std::endl;
//visit all out edge of vertex1
for (auto it = node_ptr->OutEdgeBegin();!it.IsDone();it++){
    ......
}
//visit all out edge with label 1 of vertex1
for (auto it = node_ptr->OutEdgeBegin(1);!it.IsDone();it++){
    ......
}
//add attribute
node_ptr->AddAttribute(key,value);
//visit attribute of this vertex
for (auto it = node_ptr->AttributeBegin();!it.IsDone();it++){
    std::cout<<it->key()<<" "<<it->template<ValueType>value()<<std::endl;
}
//erase edge
auto it = node_ptr->OutEdgeBegin();
while (!it.IsDone()){
    it = node_ptr->EraseEdge(it);
}
```