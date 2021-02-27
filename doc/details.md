# DminePro实现文档
        本文档主要对于当前的DminePro具体阐述其内部实现
## 数据结构
        在原有的DataGraph以及GraphPattern上，根据实际需求设计了如下的数据结构：
```c++
class PatternWithConf
{
public:
    double Conf; //该pattern的conf值
    GraphPattern Pattern; //对应的pattern
    bool operator<(const PatternWithConf b) const
    {
        //重载小于号原因是该类需要添加进优先队列中来维护top-k
        return Conf > b.Conf;
    }
};
```
```c++
class PatternWithResult
{
public:
    GraphPattern Pattern;  //对应的pattern
    vector<int> MatchResult; //该pattern对应的supp(x)的结果，其中x为pattern的1号节点
    //维护supp(x)的一个好处是在拓展的时候直接从上一次的答案中枚举答案。
};
```
```c++
class IsIsomorphism
{
    //用于判断两个pattern是否同构的一个类
    //单独写出来的原因是GPAR中的子图同构算法针对的是DataGraph和GraphPattern，这里则是两个GraphPattern
public:
    GraphPattern TargetGraph; //目标Pattern
    GraphPattern QueryPattern; //查询Pattern
    void VF2(map<int, int> &MatchState, set<int> &UsedNode, int &FindMatchTag); //VF2 algorithm
    int FeasibilityRules(int QueryID, int TargetID, map<int, int> &MatchState); //VF2中对匹配的节点进行正确性检查
    int CheckIsIsIsomorphism() //检查两个Pattern是否同构
};
```
```c++
class GPAR
{
private:
    DataGraph TargetGraph; //目标的大图
    //q(x,y)的边
    int XNodeLabel;        //q(x,y) of x
    int YNodeLabel;        //q(x,y) of y
    int QLabel;            //q(x,y) of q
    //维护了TargetGraph中所有点的label，所有边的weight，以及label和weight的所有组合可能
    set<int> NodeLabel;
    set<int> EdgeWeight;
    set<pair<pair<int,int>,int > > allow;
public:
    bool LoadNode(string VFileName); //read .v file
    bool LoadEdge(string EFileName); //read .e file
    void SetQEdge(int x, int y, int q);//设置q(x,y)
    int DminePro(int TopK, int UpEdgeNum, vector<GraphPattern> &DiscoveryResult);                //dmine主函数
    int Match(PatternWithResult &Query);                                                         //cal supp
    double CalConf(DataGraph &TargetGraph, PatternWithResult &QueryPattern);                     //cal Conf
    int Expand(vector<PatternWithResult> &PatternList, vector<PatternWithResult> &ExPandResult); //expand a new edge
    int AddNewEdgeInPattern(PatternWithResult &Src, vector<PatternWithResult> &Dst); //在pattern内部拓展一条新边
    int AddNewEdgeOutPattern(PatternWithResult &Src, vector<PatternWithResult> &Dst); //新加入一个节点并在pattern中选择一个点与这个新节点拓展一条边
    int PreMatch(vector<PatternWithResult> &FirstRound);                                                   //cal 0-round supp(x)
    void VF2(GraphPattern &QueryPattern, map<int, int> &MatchState, set<int> &UsedNode, int &FindMatchTag);  //VF2 algorithm
    int FeasibilityRules(GraphPattern &QueryPattern, int QueryID, int TargetID, map<int, int> &MatchState); //VF2 算法中的两个匹配节点的正确性检查
};
```


## 算法

        DminePro的主体算法如下所示：


|Alogorithm Name : DminePro|
|--|
|input : DataGraph **TargetGraph** , int **TopK** , int **UpEdgeNum** , Edge **q(x,y)**|
|output : Top-k Pattern vector **DiscoverResult**|
|**Algorithm**:|
|Init **DiscoverResult** ,PatternWithResult List **LastPattern** and Priority Queue **L**|
|Cal supp(x) of a Pattern with single node x and add (x,supp(x) ) to **LastPattern**|
|For i in [1 , UpEdgeNum]:|
|&emsp;&emsp;**NowPattern** = Expand(**LastPattern**)|
|&emsp;&emsp;For j in [0 , NowPattern.size()-1]: |
|&emsp;&emsp;&emsp;&emsp;Cal conf of **NowPattern[j]**|
|&emsp;&emsp;&emsp;&emsp;Update(**L** , (PatternWithConf( **NowPattern[j]** , Conf ) ) |
|&emsp;&emsp;**LastPattern** = **NowPattern**|
|&emsp;&emsp;End For|
|End For|
|While ( !**L**.empty() ):
|&emsp;&emsp;Add L.top().Pattern to **DiscoveryResult**|
|&emsp;&emsp;L.pop()|
|End While|
|Return **DiscoveryResult**|

        DminePro的主体算法与之前的Dmine算法基本一致，包括了初始化，拓展，维护Top-k三个过程。
        一开始，先初始化用于维护Top-k Pattern的堆L以及最终结果DiscoverResult，并提前计算只包含一个x节点时的Pattern的supp(x)。之后对Pattern进行拓展，每一轮拓展用Expand函数表示，代表对于上一轮生成的Pattern都拓展出一条新的边。对于新生成的pattern计算其在TargetGraph下的conf值，并放进L中进行维护（用update表示）。所有的拓展结束之后，取出L的所有Pattern加入到DiscoverResult中。
        在计算只包含一个x节点时的Pattern的supp(x)时，采用的方法是直接枚举TargetGraph的每个节点，确认该节点的label是否是x节点的Label，如果是，就将该节点id加入到答案中。
        对于Expand过程，其算法如下所示：


|Alogorithm Name : Expand|
|:--|
|input : PatternWithResult vector **PatternList**|
|output : PatternWithResult vector **ExpandResult**|
|**Algorithm**:|
|Init **ExpandResult**|
|For i in [0 , PatternList.size()-1]:|
|&emsp;&emsp;**ExpandResult** += AddNewEdgeInPattern(**PatternList**[i])|
|&emsp;&emsp;**ExpandResult** += AddNewEdgeOutPattern(**PatternList**[i])|
|End For|
|Return **ExpandResult**|
&nbsp;</br> 



|Alogorithm Name : AddNewEdgeInPattern|
|:--|
|input:PatternWithResult **Src**|
|output:PatternWithResult vector **Dst**|
|**Algorithm**:|
|Init **Dst**|
|For i in [1,Src.Pattern.Graphsize()]:|
|&emsp;&emsp;For j in [1,Src.Pattern.Graphsize()]:|
|&emsp;&emsp;&emsp;&emsp;if (i==j) continue|
|&emsp;&emsp;&emsp;&emsp;if (i==1&&Src.Pattern.getnode(j).label==YLabel) continue|
|&emsp;&emsp;&emsp;&emsp;For edgeit in **edgeweight**:|
|&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;if (**allow**.count(i,j,edgeit)==0) continue|
|&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;**SrcCopy**=**Src**|
|&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;**SrcCopy**.Addedge(i,j,edgeit)|
|&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;check **SrcCopy** is Isomorphism|
|&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;Match(**SrcCopy**)| 
|&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;if (**SrcCopy**.MatchResult.size()>0) **Dst**.add(**SrcCopy**)|
|&emsp;&emsp;&emsp;&emsp;End For|
|&emsp;&emsp;End For|
|End For|
|Return **Dst**|
&nbsp;</br>

|Alogorithm Name : AddNewEdgeOutPattern|
|:--|
|input:PatternWithResult **Src**|
|output:PatternWithResult vector **Dst**|
|**Algorithm**:|
|Init **Dst**|
|For i in [1,Src.Pattern.Graphsize()]:|
|&emsp;&emsp;For edgeit in **edgeweight**:|
|&emsp;&emsp;&emsp;&emsp;For nodeit in **nodelabel**:|
|&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;if (allow.count(i,nodeit,edgeit)==0) continue|
|&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;if(i==1&&(nodeit==YLabel or edgeit==QLabel)) continue|
|&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;**SrcCopy**=**Src**|
|&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;**SrcCopy**.Addedge(i,nodeit,edgeit)|
|&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;check **SrcCopy** is Isomorphism|
|&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;Match(**SrcCopy**)| 
|&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;if (**SrcCopy**.MatchResult.size()>0) **Dst**.add(**SrcCopy**)|
|&emsp;&emsp;&emsp;&emsp;End For|
|&emsp;&emsp;End For|
|End For|
|Return **Dst**|

    对于拓展的过程，可以分为两种情况：
    (1).不增加新的点
    (2).增加了一个新的点
    对于情况(1),拓展的时候只需要枚举新边的起点跟终点，并加上这两个点可能连的边的label。
    对于情况(2),先枚举与新的点连的pattern内部的点，再枚举新的点的label以及边的weight,最后枚举是新的点指向pattern内部的点还是pattern内部的点指向新的点
    在载入大图的过程中，维护了一个可行边的可能集合allow，表示label(u)->label(v) weight 为w的边在原图是出现过的，在加入边的时候，检查该边是否在allow里，如果不在，就不需要进行match
    新的pattern拓展完之后，会先与之前拓展的pattern逐一检查，检查是否与之前的某一个pattern同构，若同构，则该pattern之前已经计算过，不需要再进行match。

    Match的过程如下所示：

|Alogorithm Name : Match|
|:--|
|input:PatternWithResult **Query**|
|output:PatternWithResult **Dst**|
|**Algorithm**:|
|Init **MatchAns**|
|For i in [0,Query.MatchResult.size()-1]:|
|&emsp;&emsp;Init **MatchState**,**UsedNode**|
|&emsp;&emsp;if (FeasibilityRules(**Query**.Pattern,1,**Query**.MatchResult[i],**MatchState**)==0)|
|&emsp;&emsp;&emsp;&emsp;continue|
|&emsp;&emsp;**MatchState**[1]=**Query**.MatchResult[i]|
|&emsp;&emsp;**UsedNode**.insert(**Query**.MatchResult[i])|
|&emsp;&emsp;**Find**=VF2(**Query**.Pattern,**MatchState**,**UsedNode**)|
|&emsp;&emsp;if (**Find**==1) **MatchAns**.add(**Query**.MatchResult[i])|
|End For|
|**Query**.MatchResult=**MatchAns**|
|Return Query|
&nbsp;</br> 


|Alogorithm Name : VF2|
|:--|
|input: map **MatchState** set **UsedNode** GraphPattern **Query**|
|output: int **FindMatchTag**|
|**Algorithm**:|
|if (**MatchState**.size()==**Query**.GetGraphsize())|
|&emsp;&emsp;return 1|
|Get All 1-hop ancestors of **Query** named $Q_{in}$ and **TargetGraph** named $T_{in}$|
|Get All 1-hop descendant of **Query** named $Q_{out}$ and **TargetGraph** named $T_{out}$|
|**FindMatchTag**=0|
|For i in $Q_{in}$:|
|&emsp;&emsp;if (**MatchState**.count(i)) continue|
|&emsp;&emsp;For j in $T_{in}$:|
|&emsp;&emsp;&emsp;&emsp;if (**UsedNode**.count(j)) continue|
|&emsp;&emsp;&emsp;&emsp;if (FeasibilityRules(**Query**,i,j,**MatchState**)==0)|
|&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;continue|
|&emsp;&emsp;&emsp;&emsp;**MatchState**[i]=j|
|&emsp;&emsp;&emsp;&emsp;**UsedNode**.insert(j)|
|&emsp;&emsp;&emsp;&emsp;**FindMatchTag**=(**FindMatchTag** or (VF2(**MatchState**,**UsedNode**,**Query**)))|
|&emsp;&emsp;&emsp;&emsp;**MatchState**.delete(i,j)|
|&emsp;&emsp;&emsp;&emsp;**UsedNode**.delete(j)|
|&emsp;&emsp;End For|
|&emsp;&emsp;Return **FindMatchTag**|
|End For|
|For i in $Q_{out}$:|
|&emsp;&emsp;if (**MatchState**.count(i)) continue|
|&emsp;&emsp;For j in $T_{out}$:|
|&emsp;&emsp;&emsp;&emsp;if (**UsedNode**.count(j)) continue|
|&emsp;&emsp;&emsp;&emsp;if (FeasibilityRules(**Query**,i,j,**MatchState**)==0)|
|&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;continue|
|&emsp;&emsp;&emsp;&emsp;**MatchState**[i]=j|
|&emsp;&emsp;&emsp;&emsp;**UsedNode**.insert(j)|
|&emsp;&emsp;&emsp;&emsp;**FindMatchTag**=(**FindMatchTag** or (VF2(**MatchState**,**UsedNode**,**Query**)))|
|&emsp;&emsp;&emsp;&emsp;**MatchState**.delete(i,j)|
|&emsp;&emsp;&emsp;&emsp;**UsedNode**.delete(j)|
|&emsp;&emsp;End For|
|&emsp;&emsp;Return **FindMatchTag**|
|End For|
|Return **FindMatchTag**|
&emsp;</br>

|Alogorithm Name : FeasibilityRules|
|:--|
|input：GraphPattern **Query**,int **Queryid**,int **Targetid**,map MatchState|
|output:int IsVaild|
|**Algorithm**:|
|if (label[**Queryid**]!=label[**Targetid**]) return 0|
|if (indegree[**Queryid**]>indegree[**Targetid**]) return 0|
|if (outdegree[**Queryid**]>outdegree[**Targetid**]) return 0|
|if (**MatchState**.size()==0) return 1|
|for e in **Query**[**Queryid**].outedge：|
|&emsp;&emsp;if (**MatchState**.count(e.Toid))|
|&emsp;&emsp;&emsp;&emsp;if (**TargetGraph**.count(**Targetid**,e.label,**MatchState**[e.Toid])==0)|
|&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;return 0|
|End For|
|for e in **Query**[**Queryid**].inedge：|
|&emsp;&emsp;if (**MatchState**.count(e.Fromid))|
|&emsp;&emsp;&emsp;&emsp;if (**TargetGraph**.count(**MatchState**[e.Fromid],e.label,**TargetID**)==0)|
|&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;return 0|
|End For|
|return 1|

        对于Match过程而言，会枚举该Pattern拓展前的Pattern的supp(x),并逐个与Pattern的1节点进行配对并进行子图同构的检查，如果发现了一个可行的匹配，则将该点加入到答案中。
        子图同构的检查采用的是VF2算法，该算法维护了当前匹配的结果集合MatchState,每一次都对于MatchState内的点的父节点进行两两配对并进行可行性检查，并且对于MatchState内的点的子节点进行两两配对并进行可行性检查，如果可行，就将该点对加入到MatchState中并进行递归调用。直到配对的数量刚好为Pattern的大小。
        可行性检查分为如下的几个步骤：1.查看两个点的Label是否相同。2.查看Targetid的出入度是否大于等于Queryid的出入度。3.对于Pattern中与Queryid有关的边，若边的两端点均被匹配上，查看TargetGraph中是否存在满足匹配条件的边。
        在VF2算法的过程中，采用了如下的剪枝：对于Query的匹配节点的父节点/子节点集合只检查一个点的匹配结果。因为如果这个点被匹配上了，最后肯定会返回1,若这个点从一开始到结束都不会被匹配上，那么肯定不存在匹配。

        计算Pattern的conf方法则是对于当前的supp(x),查看一下该点是否有qLabel边即可。该方法成立的原因是联通的数据特性决定了边只有isfake和isnotfake两种。


## 效果
|边数|运行时间|
|:--|:--|
|3| 0.67s|
|4| 13.955s|
|5| 4234.89s|

## Todo List
1.整理代码</br>
2.尝试优化子图同构部分的代码</br>
3.尝试找到可行的拓展边的方式</br>