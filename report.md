# Project 2 Report: Simlarity Join



## 系统与源码理解

需修改的有如下三个文件

- `/src/include/catalog/pg_proc.dat`: add the prototype of user-defined functions, which will generate `fmgroids.h`, `fmgrprotos.h`, and `fmgrtab.c` by `Gen_fmgrtab.pl` when compiling.
- `/src/backend/utils/adt/similarity_join.c`: our user-defined functions
- `/src/backend/utils/adt/Makefile`: add support to compile similarity_join.c 

我写了一个`patch.py`以便一键完成以上修改或去修改，详见`README.md`



## 思路和算法分析



### Levenshtein Distance

令字符串A的前i个字符为字符串A'，字符串B的前j个字符为字符串B'

则从A'「编辑」成B'的距离为
$$
d(i,j)=\min(d(i-1, j)+1, d(i, j-1)+1, d(i-1, j-1)+[a_i\ne b_j])
$$
其中$[a_i\ne b_j]=\cases{0, & i-th character of string A is equal to j-th character of string B\\1 &otherwise}​$

用动态规划的方法求解，复杂度$O(m\cdot n)$，m和n分别是A和B所含的字符个数

当已知要求「编辑距离」小于某一值时，算法可以提前终止，因$\cases {d(i, j)\ge d(i-1, j)\\d(i, j)\ge d(i, j-1)\\ d(i, j)\ge d(i-1, j-1)}$，在此数据集和测试SQL语句上，时间减半



### Jaccard Index

令字符串A和字符串B的Jaccard指数定义如下
$$
J(A, B)=\frac{|A\cap B|}{|A\cup B|}=\frac{|A\cap B|}{|A| + |B| - |A\cap B|}
$$
交集的计算可用hash实现，计算Jaccard指数的复杂度为$O(m+n)$，m和n分别是A和B所含的字符个数



## 关键代码说明





## 实验结果

复现结果的方法见`README.md`

|      | 1(opt-lev) | 2(opt-lev) | 3(opt-lev) | 4(jaccard) | 5(jaccard) | 6(jaccard) |
| ---- | ---------- | ---------- | ---------- | ---------- | ---------- | ---------- |
| Time | 2.183      | 2.354      | 4.373      | 2.040      | 3.236      | 3.824      |



## 优化



### gcc -O2

效率提高一倍以上



### Levenshntein Distance: Early Stop

在已知条件是「编辑距离」小于某个阈值的时候，DP计算时可以提前知道条件不符合，函数可以提前终止



### Jaccard Index: Hashing

在计算交集的时候使用Hash，如下

- 对A的每个2-gram，插入哈希表
- 对于B每个2-gram，若哈希表中存在此元素，删之，交集个数加一