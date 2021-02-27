## GUNDAM done
1.解决了vf2不能匹配多重边的bug
  采用方法：JoinableCheck时维护一个used_edge的set，用于储存匹配过的边
  tips：该方法是针对edge_comp通用情况，如果只是考虑label可以有更高效的方法
2.添加了IDGen,WriteMatchResult的测试程序
3.在VF2.h中增加了用于匹配(query_id,target_id)的VF2

## Dmine done
1.已经重写了dmine相关代码 在dmine_new文件夹下

## GAR done
1.添加了BaseContainer，用于维护LiteralSet的结果
## others
1.dmine_new运行时间:
edge=1:
edge=2:1.17308s 27个pattern进入下一轮     采用缩点:0.92s
edge=3:8.49855s 143个pattern进入下一轮    采用缩点:6.79138s
edge=4:111.153s 822个pattern进入下一轮    采用缩点:103.26s    53.8974s
edge=5:6160.13s 5112个pattern进入下一轮   采用缩点:5012.25s   1652.59s

2.对于目前的DataGraph进行boost_iso中提到的SEC关系缩点之后，新图的点数为3227，约为原来的42% 边数为109886，为原来的96.67%
其中，用户点均未被缩点

pattern5542:660s  采用boost:386.609s


ban_size   edge=4 time  edge=5 time
0          103.26       5012.25
5          55.1976      2702.62
10         53.8974      1652.59
15         52.4975      1393.34
17         50.2231      1928.5
20         50.38        1433.51


pattern size = 3662
time = 115.367

限制user=1时
edge=4  13.8066s 296个pattern进入下一轮
edge=5  51.1452s 933个pattern进入下一轮
edge=6  180.248s 2773个pattern进入下一轮
edge=7  759.046s 7804个pattern进入下一轮


限制user<=2时
edge=4  42.2645s 740个pattern进入下一轮
edge=5  438.082s 3772个pattern进入下一轮
edge=6  超过2h

supp     edge=4   下一轮pattern   edge=5  下一轮pattern
72(0.9)  26.4329  114            1275.44  836  
64(0.8)  35.5522  200            1089.53  1165 
56(0.7)  43.0083  395            1273.3   2274
40(0.5)  49.4875  459            1268.43  2640
0(0)     52.4975  822            1393.34  5112


edge   zero_size
1      0
2      5
3      14
4      57
5      213

pattern size = 143
time = 6.76158
zero size = 14
expand and unique time = 0.044113
total zero time = 0.112699
total learn time = 0.002995

pattern size = 822
time = 48.047
zero size = 57
expand and unique time = 2.07351
total zero time = 0.318349
total learn time = 0.046277


pattern size = 5112
time = 1114.85
zero size = 196
expand and unique time = 144.334
total zero time = 521.653
total learn time = 0.826837


time = 1044.59
zero size = 196
expand and unique time = 138.184
total zero time = 516.083
total learn time = 0.886786
max match time = 51.1422



//new

pattern size = 822
time = 46.2174
zero size = 48
expand and unique time = 1.72886
total zero time = 0.271947
total learn time = 0.300882
max match time = 0.027199
match pattern greater than 30s num is 0
match pattern greater than 30s :

pattern size = 5112
time = 705.994
zero size = 125
expand and unique time = 133.622
total zero time = 132.281
total learn time = 11.1637
max match time = 42.1598
match pattern greater than 10s num is 4
match pattern greater than 10s :

多线程(unique未采用多线程)
edge=4  18.9418
edge=5  406.408

多线程(unique多线程区分 不同线程不合并)
edge=4 45.653
edge=5 617.3

多线程(unique多线程区分 合并后再去重一次)
edge=4 17.8901
edge=5 405.869


大图
平均入度 出度 25.226 25.226
最大入度 出度 133615 70154

25.2059 25.2059
min visit =0
max = 3051 7972
max = 3478 70154
max = 133615 6128


appledeMacBook-Pro-2:build apple$ ./bin/test/test_match 
/Users/apple/Desktop/buaa/data/liantong/liantong_n.csv
/Users/apple/Desktop/buaa/data/liantong/liantong_e.csv
time1 = 15.5304
sz=4779
appledeMacBook-Pro-2:build apple$ ./bin/test/test_match 
/Users/apple/Desktop/buaa/data/liantong/liantong_n.csv
/Users/apple/Desktop/buaa/data/liantong/liantong_e.csv
time1 = 2.08852
sz=3618
appledeMacBook-Pro-2:build apple$ ./bin/test/test_match 
/Users/apple/Desktop/buaa/data/liantong/liantong_n.csv
/Users/apple/Desktop/buaa/data/liantong/liantong_e.csv
time1 = 13.6163
sz=4770
appledeMacBook-Pro-2:build apple$ ./bin/test/test_match 
/Users/apple/Desktop/buaa/data/liantong/liantong_n.csv
/Users/apple/Desktop/buaa/data/liantong/liantong_e.csv
^C
appledeMacBook-Pro-2:build apple$ ./bin/test/test_match 
/Users/apple/Desktop/buaa/data/liantong/liantong_n.csv
/Users/apple/Desktop/buaa/data/liantong/liantong_e.csv
time1 = 1.4291
sz=3978
appledeMacBook-Pro-2:build apple$ ./bin/test/test_match 
/Users/apple/Desktop/buaa/data/liantong/liantong_n.csv
/Users/apple/Desktop/buaa/data/liantong/liantong_e.csv
^C
appledeMacBook-Pro-2:build apple$ ./bin/test/test_match 
/Users/apple/Desktop/buaa/data/liantong/liantong_n.csv
/Users/apple/Desktop/buaa/data/liantong/liantong_e.csv
time1 = 76.8575s


min visit =0
total = 560605 590350
max = 3051 7972
total = 227204 75772
max = 3478 1181
total = 0 4.80834e+06
max = 0 6128


目前的5000人数据图转化后的相关信息：
点：248438
边: 6267096
平均入度 出度 25.226 25.226
最大入度 出度 133615 70154
统计所有用户每种label出入边总和，最大值，连接的vertex总和
call 入总和：560605 出总和：590350
call 入最大值：3051 出最大值：7972
call 入vertex：156178 出vertex： 170053
边/vertex: 3.58  3.47
send 入总和：227204 出总和：75772
send 入最大值：3478 出最大值：1181
send 入vertex：11236  出vertex:21710
边/vertex: 20.22 3.49
visit 入总和:0 出总和:4808343
visit 入最大值：0 出最大值：6128
visit 入总和：0   出总和：825736 
边/vertex: 0 5.82

特点：
1.重边很多
2.2/3的边都与visit web有关

edge=1的匹配时间
采用策略：直接匹配，中间增加supp剪枝
user call phone :15.5304s
user send phone :2.08852s
phone call user :13.6163s
phone send user :1.4291s
user visit web  :76.8575s

时间与图情况基本吻合
可优化的点：
1.UpdateCandidateSet时若直接InVertexBegin(edge_label)，速度可提升大约三倍
2.待补充


edge=3 469号pattern

edge=3运行时间：
6172s
edge=3挖掘结果特点：
1.visit web这种边在Top30种就出现一条 表示visit web这个信息其实无法有效区分
2.conf=1的pattern就两个 筛出的fake user个数分别为1与15，对应的pattern分别为"若user1与phone3互相发短信，且phone3给user4打电话，则user1为fake"以及"若phone3给user1打电话，且phone3与user4互相发过短信，那么user1一定是fake"
3.其他pattern的conf都在0.94以上，筛出的user(fake+not fake)个数大概在100-150这个区间，且大多数pattern筛出的user数量相同，推测大概率有很大重合
4.Top30的pattern中出现了"若user1 xxx,user2 xxx user2为fake，那么user1为fake"的pattern,分别为“若phone3给user1发短信，且user2给phone3打电话，若user2为fake，则user1为fake”以及“若phone3给user1发短信，且phone3给user2打电话，若user2为fake，则user1为fake”，筛出的fake user分别为135以及136，推测重合度很大
5.观察了中间过程中生成conf=0的pattern,发现以下特点：
5.1  存在pattern"若phone3给user1打电话以及发短信，那么user1一定不是fake"以及"若phone3给user1发短信且user1给phone3打电话，那么user1一定不是fake"
5.2  存在pattern"若user1给phone3打电话/发短信，phone3给user4发过短信并且user4也给phone3打电话（根据5.1说明user4肯定不是fake），则user1一定不是fake"，"若user1给phone3打电话，phone3给user4以及user5发过短信，则user1一定不是fake"

6.观察了中间过程中没match的pattern,发现以下特点：
6.1 存在pattern"若phone3给user1发短信且user1给phone3打了两次电话"，"若phone3给user1打电话以及发短信，且user4给phone3打电话"，"若phone3给user4发短信且user4给phone3打电话，并且phone3给user1打电话"，"若phone3给user1打电话以及发短信，且user1给phone3打电话"



Release下运行时间
edge=1 3.33041
edge=2 16.9673
edge=3 148.166
edge=4 4361.74

加入DAGDP之后的时间
edge=1 3.5539
edge=2 17.461
edge=3 144.587
edge=4 1560.42


edge n2去重留下的GPAR数量    dfs_code去重留下的GPAR数量
1    5                      5
2    43                     43
3    438                    438
4    4943                   4967
5    59430                  60048

edge brute force time      dfs code time
1    0.000404              0.000119
2    0.004748              0.000964
3    0.147535              0.013809
4    29.1479               0.238831
5                          6.90127


edge dfs-code time     brute time
1    0.000346          0.000358
2    0.003774          0.003979
3    0.046327          0.12802
4    0.633858          23.0615

total time = match time + unique time
dfs code
edge total time match time unique time  ratio           GPAR num 
1    3.55388    3.5518     0.00208      0.06%           5
2    16.988874  16.9858    0.003074     0.02%           43
3    141.654848 141.616    0.038848     0.03%           438
4    1510.66064 1510.1     0.56064      0.04%           4967
5    20738.7769 20715.3    23.4769      0.11%           60104

iso
edge total time match time unique time  ratio           GPAR num
1    3.454042   3.45369    0.000352     0.01%           5
2    18.529092  18.5246    0.004492     0.02%           43
3    149.30242  149.158    0.14442      0.1%            438
4    1561.8391  1534.76    27.0791      1.73%           4943
5    27440.01   20461.5    6978.51      25.43%          59430



using checksupp
supp_r_limit   time            max match time
0              20738.7769
(180)0.2             
(360)0.4
(540)0.6       >28800s         
(720)0.8       >28800s         11298
(810)0.9       >28800s


采用配置文件格式：json
库:RapidJson
