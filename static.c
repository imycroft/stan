#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "static.h"

void help(void)
{
    // Dvhernsi:o:b:B:
    printf("%s %s static analyser \n", PACKAGE, VERSION);
    printf("%s [-e] [-r] [-n] [-s] [-m] [-c [-H HEX]] [-v] [-X [-d DECISION]] [-i IN_FILE] [-o OUT_FILE] [-b NGRAM] [-B BLOCKSIZE] \n\n", PACKAGE);

    printf("  -h                Print this help\n");
    printf("  -v                Verbose mode\n");
    printf("  -c                Compute concentration of HEX\n");
    printf("  -X                Calculate all the features, use with -d M or B\n");
    printf("                    the output will be a CSV file\n");
    printf("  -n                Build ngrams of length NGRAM\n");
    printf("  -e                Calculate the entropy of IN_FILE\n");
    printf("  -r                Inverte the bytes of IN_FILE\n");
    printf("  -s                Calculate the simpson index\n");
    printf("  -m                Calculate the manhattan distance\n");
    printf("  -i [IN_FILE]      inpute file\n");
    printf("  -o [OUT_FILE]     output file\n");
    printf("  -b [NGRAM]        NGRAM size\n");
    printf("  -B [BLOCKSIZE]    Devide the file in input to blocks of BLOCKSIZE\n");
    printf("                    if BLOCKSIZE = 0 the file will not be divided \n\n");
    printf(" Default parametres: BLOCKSIZE = %d, NGRAM = %d\n", BLOCK_SIZE, NGRAM);
    printf(" Example: %s -e -i FILE.EXE -b 1 -B 256 > FILE.OUT\n\n", PACKAGE);
}
void entropyHelp()
{
    printf("usage: -e -i <file_name> [-b ngram-size] [-B block-size]\n");
    printf("by default ngram-size is 1, and block-size is 1024\n");
}
void inverseHelp()
{
    printf("file names missing\n");
    printf("usage: -r -i <input_file> -o <output_file>\n");
}
void ngramHelp()
{
    printf("file name or n-gram size missing\n");
    printf("usage: -n -i <input_file> -b <n-gram_size>\n");
}
void simpsonHelp()
{
    printf("file name or n-gram size missing\n");
    printf("usage: -s -i <input_file> -b <n-gram_size>\n");
}

// float entropy(unsigned int *byte_count, unsigned long int length)
// {
//     double entropy = 0;
//     double count = 0;

//     for (int i = 0; i < BUFFER_SIZE; i++)
//     {

//         if (byte_count[i] != 0)
//         {
//             count = (double)byte_count[i] / (double)length;
//             entropy += -count * log2(count);
//         }
//     }
//     return entropy;
// }
long file_size(FILE *fp)
{
    fseek(fp, 0L, SEEK_END);
    long size = ftell(fp);
    rewind(fp);
    return size;
}
long blocks_count(FILE *fp, unsigned int block_size)
{
    return (file_size(fp) / block_size) + 1;
}

unsigned int *block_ngram(unsigned int block_size, long nsize, unsigned char *buffer)
{
    long array_size = pow(2, 8 * nsize);
    unsigned int *ngram_byte_values = (unsigned int *)malloc(array_size * sizeof(int));
    long ngram_value = 0;
    for (int i = 0; i + nsize <= block_size; i++)
    {
        for (int j = 0; j < nsize; j++)
        {
            ngram_value = buffer[i + j] << 8 * j | ngram_value;
        }

        ngram_byte_values[ngram_value]++;
        ngram_value = 0;
    }
    return ngram_byte_values;
}
unsigned int *block_ngram_proto(unsigned int block_size, long nsize, unsigned char *buffer)
{
    long array_size = block_size - nsize + 1;
    unsigned int *ngram_byte_values = (unsigned int *)malloc(array_size * sizeof(int));
    long ngram_value = 0;
    for (int i = 0; i + nsize <= block_size; i++)
    {
        for (int j = 0; j < nsize; j++)
        {
            ngram_value = buffer[i + j] << 8 * j | ngram_value;
        }

        ngram_byte_values[ngram_value]++;
        ngram_value = 0;
    }
    return ngram_byte_values;
}
unsigned int *ngram_byte_values(unsigned int block_size, long nsize, FILE *fp, long unsigned int *size, long *array_size_out)
{
    long array_size = pow(2, 8 * nsize);
    if (array_size_out)
        *array_size_out = array_size;
    unsigned int *ngram_byte_values = (unsigned int *)malloc(array_size * sizeof(int));
    size_t byte_read = block_size;

    unsigned char buffer_in[block_size];
    long ngram_value = 0;
    while (byte_read == block_size)
    {
        byte_read = fread(buffer_in, sizeof(char), block_size, fp);
        for (int i = 0; i + nsize <= byte_read; i++)
        {
            for (int j = 0; j < nsize; j++)
            {
                ngram_value = buffer_in[i + j] << 8 * j | ngram_value;
            }

            ngram_byte_values[ngram_value]++;
            ngram_value = 0;
        }
    }
    return ngram_byte_values;
}
int inverse_bytes(char *inputFile, char *outputFile)
{

    // checking for arguments
    if (!inputFile || !outputFile)
    {
        inverseHelp();
        return -1;
    }
    FILE *fp = NULL;
    FILE *fpout = NULL;
    fp = fopen(inputFile, "r");
    if (!fp)
    {
        printf("file not found!\n");
        return -1;
    }
    fpout = fopen(outputFile, "w");
    if (!fpout)
    {
        printf("file not created!\n");
        return -1;
    }
    size_t byte_read = BUFFER_SIZE;
    unsigned char buffer_in[BUFFER_SIZE];
    unsigned char buffer_out[BUFFER_SIZE];
    while (byte_read == BUFFER_SIZE)
    {

        byte_read = fread(buffer_in, sizeof(unsigned char), BUFFER_SIZE, fp);
        for (int i = 0; i < byte_read; i++)
        {
            buffer_out[i] = buffer_in[i] ^ 0xFF;
        }
        fwrite(buffer_out, sizeof(unsigned char), byte_read, fpout);
    }
    return 0;
}
float entropy(struct tnode *vector, unsigned long int length)
{
    if (vector == NULL)
        return 0;
    return (-((float)vector->count / (float)length) *
                log2((float)vector->count / (float)length) +
            entropy(vector->left, length) +
            entropy(vector->right, length));
}
float sum_n_1(struct tnode *vector)
{
    if (vector == NULL)
        return 0;
    return (
        (float)(vector->count * (vector->count - 1) +
                sum_n_1(vector->left) +
                sum_n_1(vector->right)));
}
float simpson_index(struct tnode *vector, long block_size)
{
    return sum_n_1(vector) / (float)(block_size * (block_size - 1));
}
int count(struct tnode *vector)
{
    int c = 1;

    if (vector == NULL)
        return 0;
    else
    {
        c += count(vector->left);
        c += count(vector->right);
        return c;
    }
}
int AddToArray(struct tnode *vector, long arr[], int i)
{
    if (vector == NULL)
        return i;

    arr[i] = vector->count;
    i++;
    if (vector->left != NULL)
        i = AddToArray(vector->left, arr, i);
    if (vector->right != NULL)
        i = AddToArray(vector->right, arr, i);

    return i;
}
float manhaten_distance(struct tnode *vector, long block_size)
{
    float md = 0.0;
    int size = count(vector);
    long *arr = calloc(size, sizeof(long));
    AddToArray(vector, arr, 0);

    for (int i = 0; i < size - 1; i++)
    {
        md += fabs(
            (float)(arr[i] - arr[i + 1]) / (float)block_size);
    }
    return md;
}
// ngram
struct tnode *fngram(FILE *fp, long ngram_size)
{
    struct tnode *root = NULL;
    unsigned char buffer_in[BUFFER_SIZE];
    unsigned char ptr[ngram_size];
    unsigned char *buff = NULL;
    size_t byte_read = 0;
    long size = file_size(fp);
    do
    {
        byte_read = fread(buffer_in, sizeof(char), BUFFER_SIZE, fp);
        for (int i = 0; i < byte_read - ngram_size + 1; i++)
        {
            buff = buffer_in + i;
            strncpy(ptr, buff, ngram_size);
            root = addtree(root, ptr);
        }

    } while (byte_read == BUFFER_SIZE);
    return root;
}
struct tnode *ngram(unsigned int block_size, long ngram_size, char *buffer)
{
    struct tnode *root = NULL;
    unsigned char ptr[ngram_size];
    unsigned char *buff = NULL;

    for (int i = 0; i < block_size - ngram_size + 1; i++)
    {
        buff = buffer + i;
        strncpy(ptr, buff, ngram_size);
        root = addtree(root, ptr);
    }
    return root;
    //////
}

struct tnode *talloc(void)
{
    return (struct tnode *)malloc(sizeof(struct tnode));
}
struct tnode *addtree(struct tnode *p, char *w)
{
    int cond;

    if (p == NULL)
    {
        p = talloc();
        p->ngrm = strdup(w); // -> potential
        p->count = 1;
        p->left = p->right = NULL;
    }
    else if ((cond = strcmp(w, p->ngrm)) == 0)
        p->count++;
    else if (cond < 0)
        p->left = addtree(p->left, w);
    else
        p->right = addtree(p->right, w);

    return p;
}
int hexcmp(unsigned char *str1, unsigned char *str2, long len)
{
    for (int i = 0; i < len; i++)
    {
        if (str1[i] < str2[i])
            return -1;
        if (str1[i] > str2[i])
            return 1;
    }
    return 0;
}
struct tnode *ngram_count(struct tnode *vector, unsigned char *value, long size)
{

    // Base Cases: root is null or key is present at root
    if (vector == NULL || hexcmp(vector->ngrm, value, size) == 0)
        return vector;
    // value is greater than root's key
    if (hexcmp(vector->ngrm, value, size) < 0)
        return ngram_count(vector->right, value, size);

    // Key is smaller than root's key
    return ngram_count(vector->left, value, size);
}
void treeprint(struct tnode *p, FILE *fp, long nsize)
{
    if (p != NULL)
    {
        treeprint(p->left, fp, nsize);
        fprintf(fp, "%4ld ", p->count);
        for (int i = 0; i < nsize; i++)
            fprintf(fp, "%02x", p->ngrm[i]);
        fprintf(fp, "\n");
        treeprint(p->right, fp, nsize);
    }
}