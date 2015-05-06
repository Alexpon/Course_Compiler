#Compiler

##Parser:

#####Laguage： C++
#####Environment： Code::Blocks / Macbook terminal
#####Input: 
	* grammer.txt
#####Output: 
	* set.txt
	* LLtable.txt
	* tree.txt
#####Process:
	* grammer.txt --> Grammar Generater --> set.txt(first/follow)
	* set.txt --> LLGenerater --> LLtable.txt
	* main.c --> Simple Lexier --> mainMap[]
	* mainMap[] + LLtable.txt --> tree.txt
#####Analytic:
	* Global Array:
	
		1. string grammerMap[][]
			store grammar.txt into this array

		2. string firstMap[][]
			store basic first products include some nonterminal(Reverse order)

		3. string finalFirstMap[][]
			store first products of each nonterminal(In order)

		4. string followRelative[][]
			store relative between each nonterminal

		5. string followBasic[][]
			store correct follow products after first scanning

		6. string followMerge[][]
			according followBasic,get the first of the same nonterminal and store into this array

		7. string finalFollowMap[][]
			store follow products of race nonterminal(according followRelative and followMerge array)

		8. string llTableMap[][]
			store llTable

		9. string mainMap[]
			store the output of simple lexier(input:main.c)	 
	
	* Other Global Variable:
		1. struct Tree{
			int index	the depth of the node
			string value	the value of the node
			}

		2. class Stack
			the stack store type "Tree"
			implement push and pop only
		
		3. int grammerRow
			the row of grammar
		
		4. int firstRow
			the number of nonterminal

	* Function:
		1. void readGrammer()
			將grammer.txt存入grammerMap array，一個string存一個block

		2. void scanFirst()
			要找出每個nonterminal的First必須知道每個nonterminal指過去product的First	
			如果指過去的product是terminal就太好了！因為這個terminal就一定是nonterminal的其中一個First
			但不會天天過年，常常指過去的是nonterminal，所以必須找這個nonterminal的First
			我利用的方法是從grammer.txt下面往上掃一遍，因為大部分的product都是參考到下面的nonterminal
			所以從下往上掃一次需要參考到下面時就沒問題，因為下面的掃過找出First了
			但不是所有product都是參考到下面的nonterminal，會有少數需要參考到上面
			但是上面的還沒有掃過，沒有自己的First，所以我就先把這些nonterminal保留
			這樣從下掃到上面一次就建立出了firstMap array(裡面包含少數的nonterminal)

		3. void reSetFirst()
			這個function就是為了解決上面function沒解決的問題
			由於firstMap array裡面還有少數的nonterminal
			在這個function裡一樣是從後面往上掃，可是掃的array變成了firstMap array
			因為firstMap array是跟grammerMap上下顛倒存
			所以在從後面往上掃一次就變成正確的順序，而且也會把原本的nonterminal都解決
			從下掃到上一次後，將結果存到finalFirstMap array裡面，這就是每個nonterminal對應到的First

		4. void setFollow(int)
		5. int scanFollow()
		6. void setFollowRelative(int, int, int)
		7. int setFollowBasic(int, int, int, int)
		8. void mergeFollowBasic(int)
		9. void storeMerge(int)
		10. void writeFile()
		11. int setLLTable()
		12. int is_in_First(string, string)
		13. void writeLLTable(int)
		14. void simple_Lexical()
		15. int is_keyword(string)
		16. int is_num(string)
		17. int is_id(string)
		18. void buildTree(int)


##Lexier:

#####Laguage： C
#####Environment： Code::Blocks
#####Process： 本次作業用到八個Functions
	1.main()
		主要函式，開檔讀檔寫檔，流程控制，呼叫函式

	2. init_keyArrray()
		初始化陣列，將所有Keyword存入陣列內，如果要新增Keyword可以直接在這新增

	3. is_operator(char, char, char, char)
		判斷所有Operators並且回傳不同的數字
		return 1: 一個字元的Operator
		return 2: 兩個字元的Operator
		return 3: 字元判斷 'x'
		return 4: 字元判斷 '\n' '\t'
		return 5: 註解//
	
	4. is_symbol(char
		判斷是否為special symble( [, ], {, }... )

	5. store_word(char)
		若皆不是上述提到的字元則將此字元存入word array內
		直到遇到空白或是上述提到的字元就對這個array做處理
		判斷是keyword、num、error或是identifier
	
	6. word_classification(FILE *)
		由第五點提到的內容
		利用global array "word"內所存的字串分別傳入下面兩個函式
		判斷是否為keyword、number或error，若都不是就是identifier  
		判斷完輸出結果並且將word array清空

	7. is_keyword(FILE *)
		判斷是否為keyword並將結果輸出

	8. is_number(FILE *)
		判斷是否為number或是error並將結果輸出


