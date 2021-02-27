# Todo List:
1.	Graph的Construct方法中添加对属性的拷贝

2.	考虑是否能将Graph中Vertex与Edge的Attribute提取出来、与Literal共用

3.	右值构造方法（优先在Graph中添加）

4.	补完各种迭代器到指针的转换

5.	Vertex的InEdge和OutEdge的容器整合在一起、提供同时访问InEdge、OutEdge的迭代器

6.	如NAS上Attribute方案所示、完善Attribute

7.	集合拷贝的方法，例如：  
	VertexSetType vertex_set;   
	vertex_ptr->CopyVertexSetTo(vertex_set);   
	vertex_ptr->CopyVertexSetTo(edge_label, vertex_set); 


# Now Problem

1. Large Graph删边时，OutVertexBegin(label),InVertexBegin(label)的索引不会被删除

2. GPointer没有 const ptr = 非const ptr方式

