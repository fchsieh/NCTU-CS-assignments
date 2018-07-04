#include <iostream>
#include <cstring>
using namespace std;

#define TABLE_SIZE 9973

class HashNode
{
public:
  HashNode(string in_key, string in_gender, int in_height, int in_weight)
  {
    key = in_key;
    gender = in_gender;
    height = in_height;
    weight = in_weight;
    next = NULL;
  }
  string getKey() { return key; }
  string getGender() { return gender; }
  int getHeight() { return height; }
  int getWeight() { return weight; }
  HashNode *getNext() { return next; }
  void setNext(HashNode *next) { HashNode::next = next; }

private:
  string key;
  string gender;
  int height;
  int weight;
  HashNode *next;
};

class HashTable
{
public:
  HashTable() { table = new HashNode *[TABLE_SIZE](); }
  void addItem(string in_key, string in_gender, int in_height, int in_weight);
  HashNode &operator[](string in_key);
  unsigned int hash(string in_key);
private:
  HashNode **table;
};
