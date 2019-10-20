/* test_index.c --- 
 * 
 * Author: Yu Chen
 * Created: Sat Oct 19 21:27:13 2019 (-0400)
 * Version: 
 * 
 * Description: 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <indexio.h>

int main(void){

	char dirnm[15];
	strcpy(dirnm,"../pages/");
	char indexnm[10];
	strcpy(indexnm, "result1");

	hashtable_t *index = indexload(indexnm, dirnm);
	
	indexsave(index,"result2",dirnm);

	hashtable_t *index_new = indexload("result2", dirnm);

	word_t *wordp1 = hsearch(index, searchWord, "these",5);
	word_t *wordp2 = hsearch(index_new, searchWord, "these",5);
	int id =7;
	doc_t *docp1 = qsearch(wordp1->docq,searchDoc,&id);
	doc_t *docp2 = qsearch(wordp2->docq,searchDoc,&id);

	bool doc_check = (docp1->document == docp2->document);
	bool count_check = (docp1->count == docp2->count);

	happly(index,freeWords);
	happly(index_new,freeWords);
	hclose(index);
	hclose(index_new);
	
	if(doc_check == true && count_check == true) 
		exit(EXIT_SUCCESS);

	exit(EXIT_FAILURE);

}
