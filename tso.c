#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define N     128
#define ABORT 0xf0000000 /* f + 7x0 */

struct value {
	int value;
	int counter;
};

struct memory {
	pthread_mutex_t        lock;
	struct value* volatile values[N];
	volatile int           clock;
};

struct tx {
	int readSet[N];      /* bitset */
	int writeSet[N];     /* bitset */
	int writenValues[N]; /* valeurs */
	int clock;
};

struct memory      memory;
__thread struct tx tx;

struct value* newValue(int value, int counter) {
	struct value* res = malloc(sizeof(struct value));
	res->value = value;
	res->counter = counter;
	return res;
}

void initMemory() {
	pthread_mutex_init(&memory.lock, 0);
	for(int i=0; i<N; i++)
		memory.values[i] = newValue(0, 0);
	memory.clock = 1;
}

void startTX() {
	for(int i=0; i<N; i++) {
		tx.readSet[i] = 0; 
		tx.writeSet[i] = 0;
	}
	tx.clock = memory.clock;
}

/*
 *   ABORT: abort, autre: ok
 */
/*
int writeValue(int idx, int value) {*/
	
	//memory.values[idx] = newValue(value,0); // On écrit en mémoire		
	//memory.clock = 0;		        // L'horloge sert a cadencer les commits, on considère tous ce qui est avant l'horloge
					        // comme cohérent, et tout ce qui est après comme si un autre thread est venu modifier
					        // ERROR dans ce cas.
	
	//return 0;                               /* pas de abort */
//}

/*
 *   ABORT: abort, autre: ok
 */


/*
int readValue(int idx) 
{
	return(memory.values[idx]->value) ;
}
*/
/*
 *   ABORT: abort, autre: ok
 */
int commitTX() {
	
	return 0; /* pas de abort */
}

int writeTX(int idx, int value)
{
	tx.writenValues[idx] = value;
	tx.writeSet[idx] = 1;

	return 0;
}

int readTX(int idx)
{
	if(tx.writeSet[idx]==1)
		return tx.writenValues[idx];
	
	struct value * valeur = memory.values[idx];	

	if(valeur->counter >= tx.clock)
		return ABORT;

	tx.readSet[idx]=1;

	return valeur->value;
}

//struct tx* newTX() 

int main(int argc, char **argv) {
	initMemory();

debut:
	startTX();
	writeTX(0,1000);

	//x = readTX(0,20);
	int x = readTX(0);
	printf(" x = %d\n", x);	
	

	if(x == ABORT)
		goto debut;

	if(writeTX(0, x + 1) == ABORT)
		goto debut;

	if(commitTX() == ABORT)
		goto debut;

	return 0;
}
