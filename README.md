#Compiler

Laguage：C

Environment：Code::Blocks

Lexier:

Process：本次作業用到八個Functions，分別是


1. main()			

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

Parser:

to be continue
