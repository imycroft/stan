// Created by walid hafid
#define BUFFER_SIZE 1024
#define NGRAM 1
#define MAXNGRAM 4
#define PACKAGE "stan"
#define VERSION "1.0"
void help();
void entropyHelp();
void inverseHelp();
void ngramHelp();
void simpsonHelp();

struct tnode {
 unsigned char *ngrm;
 long count;
 struct tnode *left;
 struct tnode *right;
};
unsigned int* ngram_byte_values(unsigned int BLOCK_SIZE, long nsize, FILE* fp, long unsigned int* size_out, long* array_size_out);
float entropy(struct tnode* vector, unsigned long int length);
//float entropy(unsigned int *byte_count, unsigned long int length);
int inverse_bytes(char* inputFile, char* outputFile);
float simpson_index(struct tnode *vector, long block_size);
float manhaten_distance(struct tnode *vector, long block_size);
long blocks_count(FILE* fp, unsigned int BLOCK_SIZE);
unsigned int* block_ngram(unsigned int BLOCK_SIZE, long nsize, unsigned char* buffer);
// ngram
/* ngram container for binary tree */

/* extract tokens from word */
struct tnode* ngram(unsigned int BLOCK_SIZE, long nsize, char* buffer);
/* add ngram to binary tree */
struct tnode *addtree(struct tnode *, char *);
/* return memory for new node */
struct tnode *talloc(void);
//
struct tnode* ngram_count(struct tnode* vector, unsigned char* value, long size);
/* print the binary tree */
void treeprint(struct tnode * node, FILE *fp, long nsize);