# Graph Convert

## csv_to_rdf

### 简介
    csv_to_rdf是用于将csv格式的图转化为RDF格式(.ttl文件)的程序
### 运行方法
    ./csv_to_rdf 同时在该可执行文件所在目录下放一个csv_to_rdf.yaml 表示该程序的配置文件
### 配置文件格式
    csv_to_rdf.yaml的格式如下：
    Graph:  
        Name: 图名字	
        Dir: 图文件所在目录
        VertexFile: 
            - 点文件
        EdgeFile: 
            - 边文件
        Dict: label对应实际语义的csv


    RDF:
        Name: ttl文件名字（带.ttl后缀)
        Dir: 存储目录
