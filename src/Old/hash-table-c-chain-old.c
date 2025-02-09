#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "../hash-table-chain.h"

Table * init(long maxWords) {
    long primeNumbers[] = { 67, 131, 257, 521, 1031, 2053, 4099, 8209, 16411, 32771, 65537, 131073 };
    long nPrimeNumbers = sizeof(primeNumbers)/sizeof(long);

    if ((maxWords <= 0) || (maxWords > primeNumbers[nPrimeNumbers - 1])) {
        return NULL;
    }

    Table * table = (Table *) malloc(sizeof(Table));
    if (table == NULL) {
        perror("(init) error allocating memory for the table");
        return NULL;
    }

    table->nBuckets = primeNumbers[nPrimeNumbers-1];

    for (int i = 0; i < nPrimeNumbers; i++) {
       if (maxWords < primeNumbers[i]) {
            table->nBuckets =  primeNumbers[i];
            break;
       }
    }

    table->maxWords = maxWords;
    table->nWords = 0;

    table->array = (Node **)
        malloc(table->nBuckets * sizeof(Node *));

    if (table->array == NULL) {
       perror("(init) error allocating memory for table->array");
       return NULL;
    }

    for (int i = 0; i < table->nBuckets; i++) {
       table->array[i] = NULL;
    }

    return table;
}

long hash(Table * table, char * word) {
    if ((table == NULL) || (word == NULL) || (strlen(word) == 0)) {
        return -1;
    }
    
    long hashNum = 1;
    long len = strlen(word);

    for (long i = 0; i < len; i++) {
        hashNum = 31 * hashNum + word[i];
    }

    return hashNum % table->nBuckets;
}

bool lookup(Table * table, char * word) {
    if ((table == NULL) || (word == NULL) || (strlen(word) == 0)) {
        return false;
    }

    long hashNum = hash(table, word);

    Node * elem = table->array[hashNum];

    while (elem != NULL && strcmp(elem->word,word) != 0) {
      elem = elem->next;
    }

    return elem == NULL ? false : true;
}

long get(Table * table, char * word) {
    if ((table == NULL) || (word == NULL) || (strlen(word) == 0)) {
        return -1;
    }

    long targetIdx = hash(table, word);
    Node * head = table->array[targetIdx];

    while (head) {
        if (!strcmp(head->word, word)) {
            return head->value;
        }
        head = head->next;
    }

    return -1;
}

bool insert(Table * table, char * word, long value) {
    if ((table == NULL) || (word == NULL) || (strlen(word) == 0) || (value < 0) || (table->nWords > table->maxWords)) {
        return false;
    }

    long targetIdx = hash(table, word);

    Node * new = (Node *) malloc(sizeof(Node));
    if (new == NULL) {
        perror("(insert) error allocating memory for a new node");
        return false;
    }

    new->word = malloc(strlen(word) + 1);
    if (new->word == NULL) {
        perror("(insert) error allocating memory for the word field");
        return false;
    }

    strcpy(new->word, word);
    new->word[strlen(word)] = '\0';
    new->value = value;
    new->next = NULL;

    Node * head = table->array[targetIdx];

    if (head == NULL) {
        table->array[targetIdx] = new;
    } else {
        while (head) {
            if (strcmp(head->word, word) == 0) {
                if (head->value == value) {
                    return false;
                } else {
                    head->value = value;
                    return true;
                }
            } else if (head->next == NULL) {
                break;
            }
            head = head->next;
        }

        head->next = new;
    }

    table->nWords++;
    return true;
}

bool delete(Table * table, char * word) {
    if ((table == NULL) || (word == NULL) || (strlen(word) == 0)) {
        return false;
    }

    long targetIdx = hash(table, word);

    Node * head = table->array[targetIdx];
    Node * prev = NULL;
    bool found = false;

    while (head) {
        if (strcmp(head->word, word) == 0) {
            found = true;
            Node * temp = head;

            if (prev == NULL) {
                table->array[targetIdx] = head->next;
            } else {
                prev->next = head->next;
            }
            
            free(temp);
            temp = NULL;
            break;
        }

        prev = head;
        head = head->next;
    }

    if (!found) {
        return false;
    }

    table->nWords--;
    return true;
}

bool update(Table * table, char * word, long value) {
    if ((table == NULL) || (word == NULL) || (strlen(word) == 0) || (value < 0)) {
        return false;
    }

    long hashNum = hash(table, word);
    Node * elem = table->array[hashNum];

    while (elem) {
        if (strcmp(elem->word, word) == 0) {
            elem->value = value;
            return true;
        } else if (elem->next == NULL) {
            break;
        }
        elem = elem->next;
    }

    if (table->nWords > table->maxWords) {
        return false;
    }

    Node * e = (Node *) malloc(sizeof(Node));

    if (e == NULL) {
        return false;
    }

    e->word = strdup(word);
    e->value = value;
    e->next = NULL;

    if (elem == NULL) {
        table->array[hashNum] = e;
    } else {
        elem->next = e;
    }

    table->nWords++;
    return false;
}

void print(Table * table) {
    if (table != NULL) {
        for (int i = 0; i < table->nBuckets; i++) {
            Node * head = table->array[i];

            if (head != NULL) {
                printf("Bucket %d\n", (i + 1));

                while (head != NULL) {
                    if (head->next != NULL) {
                        printf("Node(Key=%s, Value=%ld)->", head->word, head->value);
                    } else {
                        printf("Node(Key=%s, Value=%ld)\n", head->word, head->value);
                    }
                    head = head->next;
                }
            }
        }
    }
}

bool clear(Table * table) {
    if (table == NULL) {
        return false;
    }

    for (int i = 0; i < table->nBuckets; i++) {
        Node * head = table->array[i];
        Node * temp;

        while (head) {
            temp = head;
            head = head->next;
            free(temp);
        }

        table->array[i] = NULL;
    }

    table->nWords = 0;
    return true;
}