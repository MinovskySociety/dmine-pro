#include <sstream>
#include "gpar.h"
/*
ReturnType gmine_new::ReadGraph(
    INOUT GraphType &target_graph,
    IN const char *v_file,IN const char *e_file){
    //load .v file
    std::ifstream node_file(v_file);
    if (!node_file){
        std::cout << "node file is not opened" << std::endl;
        return E_ERROR_NOT_FOUND;
    }
    //csv read
    std::string linestr;
    getline(node_file,linestr);
    while (getline(node_file,linestr)){
        std::stringstream ss(linestr);
        std::string str;
        std::vector<VertexIDType> node_information;
        while (getline(ss, str, ',')){
            node_information.push_back(atoi(str.c_str()));
        }
        VertexIDType node_id=node_information[0];
        VertexLabelUnderlieType node_label=node_information[1];
        target_graph.AddVertex(node_id,VertexLabelType(node_label));
    }
    //.e read
    node_file.close();
    //load .e file
    std::ifstream edge_file(e_file);
    if (!edge_file){
        std::cout << "edge file is not opened" << std::endl;
        return E_ERROR_NOT_FOUND;
    }
    getline(edge_file,linestr);
    //解决重边的set 如果添加重边就把相关处理删掉就可以了
    std::set<std::pair
            <std::pair<VertexLabelUnderlieType,VertexLabelUnderlieType>,
             EdgeLabelUnderlieType> > multi_edge;
    while (getline(edge_file,linestr)){
        std::stringstream ss(linestr);
        std::string str;
        std::vector<EdgeLabelUnderlieType> edge_information;
        while (getline(ss, str, ',')){
            edge_information.push_back(atoi(str.c_str()));
        }
        VertexIDType from_id=edge_information[1];
        VertexIDType to_id=edge_information[2];
        VertexLabelUnderlieType edge_label=edge_information[3];
        //const Edge edge_type(edge_label);
        if (multi_edge.count(std::make_pair(
                            std::make_pair(from_id,to_id),
                            edge_label))) continue;
        target_graph.AddEdge(from_id,to_id,EdgeLabelType(edge_label));
        multi_edge.insert((std::make_pair(
                           std::make_pair(from_id,to_id),
                           edge_label)));
    }
    edge_file.close();
    return E_OK;
}
*/