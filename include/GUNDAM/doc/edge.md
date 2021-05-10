# Comment 
- [1.Edge使用说明](#1-edge-%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E)
   - [1.1相关规定](#11-%E7%9B%B8%E5%85%B3%E8%A7%84%E5%AE%9A)
   - [1.2接口](#12-%E6%8E%A5%E5%8F%A3)
   - [1.3示例程序](#13-%E7%A4%BA%E4%BE%8B%E7%A8%8B%E5%BA%8F)
## 1. Edge 使用说明

本篇文档的目的主要是针对Edge进行一些接口和使用上的说明，并提供相应的示例程序。

### 1.1 相关规定

1.本库无法访问具体的Edge对象，只能通过EdgePtr/迭代器来得到相关内容，因此调用接口时应该写->而不是.<br>
2.EdgePtr与EdgeIterator两者的接口除了IsNull(判断EdgePtr是否为空)和IsDone(判断EdgeIterator是否到end)不同，其余接口均相同。<br>
3.其他规定请参考 [点击此处](./graph.md)。

### 1.2 接口
EdgePtr及Edge迭代器可调用接口表如下所示:


|接口|接口格式|接口说明|
|:--|:--|:--|
|AddAttribute|pair\<AtributePtr,bool\> AddAttribute(key,value)<br>key:属性的键值<br>value:属性的值<br>返回值:若该节点中已存在键为key的属性则返回<指向已有属性的指针, false>；否则，加入新的属性并返回<指向新属性的指针, true>|往节点中加入属性key值为key,value值为value的属性|
|attribute<br>const_attribute|template\<ValueType\>attribute(key)<br>key:属性的键值<br>ValueType:value值的数据类型<br>返回值:返回key对应的value|得到该节点键值为key的value/常量value|
|attribute_value_type|BasicDataType attribute_value_type(key)<br>key:属性的键值|得到该节点键值为key的value的类型的id|
|attribute_value_type_name|string attribute_value_type(key)<br>key:属性的键值<br>返回值:若类型是整型(int,long long,unsigned int等等)，返回"int",若类型是浮点类型(float,double,long double),返回"double"，若是std::string类型，返回"string"，其他类型返回"unknown type"|得到该节点键值为key的value的类型的名字|
|AttributeBegin<br>AttributeBegin|AttributeIterator AttributeBegin()<br>AttributeConstIterator AttributeBegin()|得到这个节点属性的迭代器/常量迭代器|
|EraseAttribute|bool EraseAttribute(key)<br>key:属性的键值<br>返回值：是否成功删除<br><br>AttributeIterator EraseAttribute(attribute_iterator)<br>attribute_iterator:属性的迭代器<br>返回值：返回删除的迭代器的下一个迭代器|删除该节点键值为key的属性或删除属性迭代器为attribute_iterator的属性|
|FindAttribute<br><br>FindAttribute|AttributePtr FindAttribute(key)<br><br>AttributeConstPtr FindAttribute(key)<br><br>key:属性的键值<br>返回值:若找到了该属性则返回该属性的指针/常量指针，否则返回Null指针|找到该节点键值为key对应的Attribute指针/常量指针|
|id|VertexIDType id()|得到该节点的id|
|label|VertexLabelType label()|得到该节点的label|
|SetAttribute|pair<AttributePtr, bool> SetAttribute(key,value)<br><br>key:属性的key<br>value:属性的value<br>返回值:若该节点中存在键为key的属性则返回<指向修改后属性的指针, true>；否则，返回<NULL指针, false>|将该节点中键为key的属性的值设为value|
|IsNull|bool IsNull()|判断指针是否非空|
|src_ptr<br>const_src_ptr|VertexPtr src_handle()<br><br>VertexConstPtr const_src_handle()|得到这个边的起点的指针/常量指针|
|dst_ptr<br>const_dst_ptr|VertexPtr dst_handle()<br><br>VertexConstPtr const_dst_handle()|得到这个边的终点的指针|


### 1.3 示例程序
```c++
GUNDAM::Graph<> g;
EdgePtr edge_ptr = g.FindEdge(1);
//print edge
std::cout<<edge_ptr->src_handle()->id()<<" "<<edge_ptr->dst_handle()->id()<<" "<<edge_ptr->label()<<" "<<edge_ptr->id()<<std::endl;
//add attribute
edge_ptr->AddAttribute(key,value);
//visit attribute
for (auto it = edge_ptr->AttributeBegin();!it.IsDone();it++){
    ......
}
```