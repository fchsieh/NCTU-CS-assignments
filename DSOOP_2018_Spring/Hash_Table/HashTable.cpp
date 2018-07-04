#include "HashTable.h"

void HashTable::addItem(string in_key, string in_gender, int in_height, int in_weight)
{
    unsigned int hashValue = hash(in_key);
    HashNode *prev = NULL;
    HashNode *entry = table[hashValue];

    if(entry == NULL)
        table[hashValue] = new HashNode(in_key, in_gender, in_height, in_weight);
    else
    {
        HashNode *new_node = new HashNode(in_key, in_gender, in_height, in_weight);
        new_node->setNext(table[hashValue]);
        table[hashValue] = new_node;
    }
}

HashNode &HashTable::operator[](string in_key)
{
    unsigned int hashValue = hash(in_key);
    HashNode *entry = table[hashValue];
    if(entry->getKey() == in_key)
        return *entry;
    while(entry->getNext() != NULL)
    {
        entry = entry->getNext();
        if(entry->getKey() == in_key)
            return *entry;
    }
}

unsigned int HashTable::hash(string in_key)
{
    // I choose BKDRHash as my hash function
	unsigned int hash = 0;
    unsigned int seed = 131;
	unsigned int len = in_key.length();

	for (unsigned int i = 0; i < len; i++)
		hash = (hash * seed) + (in_key[i]);

	return (hash % TABLE_SIZE);
}
