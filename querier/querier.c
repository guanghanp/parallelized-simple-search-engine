/* querier.c --- 
1;95;0c * 
 * Author: Yu Chen
 * Created: Thu Oct 24 17:21:39 2019 (-0400)
 * Version: 
 * 
 * Description: 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <queue.h>

void printfcn(void* wordp){
	char* wp = (char*)wordp;
	printf("%s ",wp);
}

int main(void){
	char input[100];
	const char s[2] = " ";
	char *word;
	char *output;
	bool valid = true;
	
	while(1) {
		printf("> ");
		queue_t *qp = qopen();
		valid = true;
		output = fgets(input,100,stdin);
		if (output == NULL) {
			qclose(qp);
			break;
		}
		input[strlen(input)-1] = '\0'; // to replace the newline with \0
		//printf("Reads %s\n",input);
		//printf("output: %s\n",output);
		word = strtok(input, s);

		while (word != NULL ){
			//	printf("beginning of the loop: %s\n",word);
			for(int i=0; word[i]!= '\0'; i++){
				if(isalpha(word[i]) == 0 && valid){
					printf("[Invalid input]\n");
			
					valid = false;
					break;
				}
				if (valid)
					word[i] = tolower(word[i]);
			} // to lower letters
			//	printf("after tolower: %s,%ld\n",word,strlen(word));
			if (valid)
				qput(qp,word);
			
			word = strtok(NULL, s);
		}

		if (valid){
			qapply(qp,printfcn);
			printf("\n");
			qclose(qp);
		}
		// char *c = strchr(input,4);
		//printf("%s",c); //did not manage to check EOF sig. 
	}
	return 0;
}

