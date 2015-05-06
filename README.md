#Compiler

##Parser:

#####Laguage： C++
#####Environment： Code::Blocks / Macbook terminal
#####Input: 
	* grammar.txt
	* main.c
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
			
		4. int scanFollow()
			前面找完了所有nonterminal的First，這邊是找Follow的起始點
			因為前面的finalFirstMap第一列就是所有的nonterminal，所以這邊直接延用
			nonterminal從上到下掃一次grammerMap的products
			因為是要找Follow，我們focus在右邊的products比對到一樣後做進一步的判斷
			假設現在找nonterminal A 的Follow，比對到會有三種情形：
				* A -> B x	直接忽略，因為我們care的是右邊的products
				* B -> C A	進入setFollowRelative將關係(A的Follow包含B的Follow)存入
				* B -> A C	進入setFollowBasic將C的First存到A的Follow裡面
			從上掃到下面一次就掃過了所有的nonterminal
			不斷的呼叫setFollowRelative及setFollowBasic也將兩個array建好(下面詳述)
			這時就可以呼叫mergeFollowBasic做更進一步的整理

		5. void setFollowRelative(int, int, int)
			由scanFollow判斷呼叫的function
			吃了三個參數row, col, cnt
				* grammerMap[row][col]是當前比對到的nonterminal也就是上面第二列的A
				* cnt是目前followRelative的列數
				* 經過 while可以找到prodect所屬的nonterminal也就是上面第二列的B
					while(grammerMap[row][0]=="\t"){
						row--;
					}
			將直存進followRelative array達成followRelative[n][0]包含followRelative[n][1]的效果

		6. int setFollowBasic(int, int, int, int)
			由scanFollow判斷呼叫的function
			吃了四個參數row, col, cnt, rcnt
				* grammerMap[row][col]是當前比對到的nonterminal也就是上面第三列的A
				* cnt是目前followBasic的列數
				* rcnt是目前followRelative的列數
			grammerMap[row][col+1]是下一個nonterminal或terminal(上面第三列的C)
			如果是nonterminal才會進for loop裡的if做進一步判斷
			在for loop裡面先將C拿來跟finalFirstMap第一列做比較，目的是找出他的first在哪一列
			找到在第n列後，將finalFirstMap第n列的所有terminal存進followBasic裡直到遇到空白停止
			除了遇到空白停止外，遇到epsilon也要做例外處理
				* 上述的B -> A C 遇到epsilon就等同於B -> A
				* 所以這邊要多增加一列followRelative
					followRelative[rcnt][0] = grammerMap[row][col];
					followRelative[rcnt][1] = finalFirstMap[i][0];
					isEps=1;
				* 最後也要回傳告知followRelative已經增加一列了!

		7. void mergeFollowBasic(int)
			由於上面的找法，followBasic會出現很多重複nonterminal但product不同或相同的列
			這個function主要就是把相同nonterminal的products三層while交叉比對存到最上面的且不重複
			ex.
				A , ; [		-->	A , ; [ : int char
				A ; : int	-->	不變
				A int char	-->	不變

		8. void storeMerge(int)
			經過mergeFollowBasic後followBasic array都已經將不同的terminal集中到最上面的nonterminal
			這邊就是從頭掃一次，把每個nonterminal的頭頭都抓起來
			存到一個新的array裡，這就是followMerge array

		9. void setFollow(int)
			經過上面幾個func的判斷轟炸後，終於建好followRelative以及followMerge這兩個重要array
			在第一個迴圈裡面，我先把followMerge裡面有的東西都存進新的array(finalFollow)裡
			如果遇到followMerge裡沒有的nonterminal也不用慌，存一個nonterminal投進去，後面是空的
			第一階段建好後，就可以利用第二個線索followRelative array
			利用一個for loop找出關係X是在finalFollowMap的第refR1列和第refR2列
				for(frow=0; frow<32; frow++){
                			if(followRelative[i][0] == finalFollowMap[frow][0]){
                    				refR1 = frow;
        				}
                			if(followRelative[i][1] == finalFollowMap[frow][0]){
                    				refR2 = frow;
                			}
        			}
			這時就可以利用這兩列兩層while交叉比對，(A包含B)將B中A沒有的存到A裡面
			掃過一次大部分的FinalFollowMap(Follow)就完成了
			再掃一次可以確保FinalFollowMap(Follow)更加正確
			
		10. void writeFile()
			利用前面產生的finalFirstMap及finalFollowMap將結果輸出到set.txt裡
			並且判斷字串長度適當的加上一些tab讓格式更易讀

		11. int setLLTable()
			to be continue...

		12. int is_in_First(string, string)
			to be continue...

		13. void writeLLTable(int)
			將上面辛辛苦苦建好的llTableMap array輸出到LLTable.txt中
			並判斷一些字串長度加上適當的tab讓檔案更易讀

		14. void simple_Lexical()
			十分簡易版的Lexier
			讀取main.c檔，利用空白切出每個string
			把非保留字或符號判斷成id
			數字判斷成num
			並將結果一一存到mainMap裡面

		15. int is_keyword(string)
			在simple_Lexical function裡面會呼叫
			主要判斷所擷取到的string是不是保留字或符號
			如果是保留字或符號return 1 反之 return 0

		16. int is_num(string)
			在simple_Lexical function裡面會呼叫
			判斷如果是數字則return 1 反之 return 0

		17. void buildTree(int)
			to be continue...

歡迎享用及討論
記得標明出處來源並將code open:)

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


