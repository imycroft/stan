#define BUFFER_SIZE 1024
#define BLOCK_SIZE 1024
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
unsigned int* ngram_byte_values(unsigned int block_size, long nsize, FILE* fp, long unsigned int* size_out, long* array_size_out);
/**
 * Compute entropy of the histogram given as a binary tree
 * @param vector  A pointer to the binary tree containing the histogram values.
 * @param length  The lenth of the block
 * @return The entropy of the block
 */
float entropy(struct tnode* vector, unsigned long int length);
/**
 * Inverse the Bytes of a given file
 * @param input_file  The file we want to inverse the bytes.
 * @param output_file  The output file.
 * @return 0 if success, negative value if error
 */
int inverse_bytes(char* input_file, char* output_file);
float simpson_index(struct tnode *vector, long block_size);
float manhaten_distance(struct tnode *vector, long block_size);
long blocks_count(FILE* fp, unsigned int block_size);
long file_size(FILE *fp);
unsigned int* block_ngram(unsigned int block_size, long nsize, unsigned char* buffer);
// ngram
/* ngram container for binary tree */

/* extract tokens from a block */
struct tnode* ngram(unsigned int block_size, long nsize, char* buffer);
/* extract tokens from a file */
struct tnode *fngram(FILE *fp, long ngram_size);
/* add ngram to binary tree */
struct tnode *addtree(struct tnode *, char *);
/* return memory for new node */
struct tnode *talloc(void);
//
struct tnode* ngram_count(struct tnode* vector, unsigned char* value, long size);
/* print the binary tree */
void treeprint(struct tnode * node, FILE *fp, long nsize);