#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "static.h"

int main(int argc, char *argv[])
{
    // Parsing the arguments
    int debugFlag = 0;
    int csvFlag = 0;
    int verboseFlag = 0;
    int entropyFlag = 0;
    int inverseFlag = 0;
    int ngramFlag = 0;
    int simpsonFlag = 0;
    int manhatenFlag = 0;
    int concentrationFlag = 0;
    long nsize = -1;
    long block_size = -1;
    int helpFlag = 0;
    char *inputFile = NULL;
    char *outputFile = NULL;
    char *hexString = NULL;
    char *decision = NULL;
    int index;
    int c;

    opterr = 0;

    while ((c = getopt(argc, argv, "DXvhernsmci:o:b:B:H:d:")) != -1) // e entropy, r invert, n ngram
    {
        switch (c)
        {
        case 'D':
            debugFlag = 1;
            break;
        case 'X': // csvfile
            csvFlag = 1;
            break;
        case 'c':
            concentrationFlag = 1;
            break;
        case 'v':
            verboseFlag = 1;
            break;
        case 'h':
            helpFlag = 1;
            break;
        case 'e':
            entropyFlag = 1;
            break;
        case 'r':
            inverseFlag = 1;
            break;
        case 'n':
            ngramFlag = 1;
            break;
        case 's':
            simpsonFlag = 1;
            break;
        case 'm':
            manhatenFlag = 1;
            break;
        case 'b':
            nsize = strtol(optarg, NULL, 10);
            break;
        case 'B': // Block size in bytes
            block_size = strtol(optarg, NULL, 10);
            break;
        case 'i':
            inputFile = optarg;
            break;
        case 'o':
            outputFile = optarg;
            break;
        case 'd':
            decision = optarg;
            break;
        case 'H':
            hexString = optarg;
            break;
        case '?':
            if (optopt == 'i' || optopt == 'o' || optopt == 'b' || optopt == 'B' || optopt == 'H' || optopt == 'd')
                fprintf(stderr, "Option -%c requires an argument.\n", optopt);
            else if (isprint(optopt))
                fprintf(stderr, "Unknown option `-%c'.\n", optopt);
            else
                fprintf(stderr,
                        "Unknown option character `\\x%x'.\n",
                        optopt);
            return 1;
        default:
            abort();
        }
    }
    // setting the default value for nsize and block_size
    nsize = (nsize == -1) ? 1 : nsize;
    block_size = (block_size == -1) ? BUFFER_SIZE : block_size;
    //
    if (debugFlag == 1)
    {
        printf("DEBUG MODE\n");
        FILE *fp = fopen("debug.txt", "r");
        //FILE *fpout = fopen("output.out", "w");
        // treeprint(root, fpout);
        
        //
        long blockCount = blocks_count(fp, block_size);
        struct tnode *root = NULL;
        size_t byte_read = 0;
        unsigned char buffer_in[block_size];
        for (long i = 0; i < blockCount; i++)
        {
            // get the block ngram
            byte_read = fread(buffer_in, sizeof(char), block_size, fp);
            // block_byte_values = block_ngram(byte_read, 1, buffer_in);
            root = ngram(byte_read, nsize, buffer_in);
           printf("mandist:%f\n", manhaten_distance(root, byte_read)) ;
        }
        //
        return -2;
    }
    if (entropyFlag + inverseFlag + ngramFlag + simpsonFlag + concentrationFlag + manhatenFlag + csvFlag != 1 || helpFlag == 1)
    {
        help();
        return -1;
    }
    // if ALL
    if (csvFlag == 1)
    {

        if (inputFile)
        {
            FILE *fp = NULL;
            unsigned long int size = 0;
            //
            fp = fopen(inputFile, "r");
            if (!fp)
            {
                printf("File not found\n");
                return -1;
            }
            // get the block count
            long blockCount = blocks_count(fp, block_size);
            // float *entropy_block = (float *)malloc(blockCount * sizeof(float));
            struct tnode *vector = NULL;
            unsigned char buffer_in[block_size];
            size_t byte_read = 0;
            printf("E1,S1,M1,E2,S2,M2,E3,S3,M3,E4,S4,M4,Decision\n");
            for (long i = 0; i < blockCount; i++)
            {
                // get the block ngram
                byte_read = fread(buffer_in, sizeof(char), block_size, fp);
                // block_byte_values = block_ngram(byte_read, 1, buffer_in);
                // calculate the four ngram
               
                for (size_t i = 0; i < MAXNGRAM; i++)
                {
                    vector = ngram(byte_read, i + 1, buffer_in);
                    printf("%f,", entropy(vector, byte_read));
                    printf("%f,", simpson_index(vector, byte_read));
                    printf("%f,", manhaten_distance(vector, byte_read));
                }
                printf("%s\n", decision);
            }
        }
    }
    // if it is entropy calculation
    if (entropyFlag == 1)
    {
        if (verboseFlag == 1)
            printf("Entropy calculation\n");
        // check for input file
        if (inputFile)
        {
            FILE *fp = NULL;
            unsigned long int size = 0;
            //
            fp = fopen(inputFile, "r");
            if (!fp)
            {
                printf("File not found\n");
                return -1;
            }
            // variables
            struct tnode *vector = NULL;
            unsigned char buffer_in[block_size];
            size_t byte_read = 0;
            // if blocksize = 0 => get the entropy of the whole file
            if (block_size == 0)
            {
               vector = fngram(fp, nsize);
               printf("%f\n", entropy(vector, file_size(fp)));    
               return 0;        
            }
            
            // get the block count
            long blockCount = blocks_count(fp, block_size);
            float *entropy_block = (float *)malloc(blockCount * sizeof(float));
            
            for (long i = 0; i < blockCount; i++)
            {
                // get the block ngram
                byte_read = fread(buffer_in, sizeof(char), block_size, fp);
                // block_byte_values = block_ngram(byte_read, 1, buffer_in);
                vector = ngram(byte_read, nsize, buffer_in);
                entropy_block[i] = entropy(vector, byte_read);
                printf("%f\n", entropy_block[i]);
            }
        }
        else
        {
            entropyHelp();
            return -1;
        }
        return 0;
    }
    // if byte inverse
    if (inverseFlag == 1)
    {

        if (verboseFlag == 1)
            printf("Inversing bytes\n");

        return inverse_bytes(inputFile, outputFile);
    }
    // if ngram extraction
    if (ngramFlag == 1)
    {
        if (verboseFlag == 1)
            printf("N-gram extraction\n");
        if (!inputFile)
        {
            ngramHelp();
            return -1;
        }
        FILE *fp = NULL;
        fp = fopen(inputFile, "r");

        if (!fp)
        {
            printf("File not found\n");
            return -1;
        }
        long blockCount = blocks_count(fp, block_size);
        struct tnode *root = NULL;
        size_t byte_read = 0;
        unsigned char buffer_in[block_size];
        for (long i = 0; i < blockCount; i++)
        {
            // get the block ngram
            byte_read = fread(buffer_in, sizeof(char), block_size, fp);
            // block_byte_values = block_ngram(byte_read, 1, buffer_in);
            root = ngram(byte_read, nsize, buffer_in);
            fprintf(stdout, "block #%ld\n", i);
            treeprint(root, stdout, nsize);
            fprintf(stdout, "\n");
        }
        return 0;
    }
    // if simpson index
    if (simpsonFlag == 1)
    {
        if (verboseFlag == 1)
            printf("Simpson Index Calculator\n");
        if (!inputFile)
        {
            simpsonHelp();
            return -1;
        }
        FILE *fp = NULL;
        fp = fopen(inputFile, "r");
        if (!fp)
        {
            printf("File not found\n");
            return -1;
        }
        unsigned long size = 0;
        long array_size = 0;
        // unsigned int *n_gram = ngram_byte_values(BUFFER_SIZE, nsize, fp, &size, &array_size);
        // printf("Simpson index: %f\n", simpson_index(n_gram, size, array_size));
        long blocksCount = blocks_count(fp, block_size);
        struct tnode *root = NULL;
        long byte_read = 0;
        char buffer_in[block_size];
        for (int i = 0; i < blocksCount; i++)
        {
            // get the block ngram
            byte_read = fread(buffer_in, sizeof(char), block_size, fp);
            printf("byte read %ld\n", sizeof(buffer_in));
            root = ngram(byte_read, nsize, buffer_in);
            printf("%f\n", simpson_index(root, byte_read));
        }
    }
    // if manhaten distance
    if (manhatenFlag == 1)
    {
        if (verboseFlag == 1)
            printf("Manhaten distance Calculator\n");
        if (!inputFile)
        {
            help(); // TODO add manhatenHelp
            return -1;
        }
        FILE *fp = NULL;
        fp = fopen(inputFile, "r");
        if (!fp)
        {
            printf("File not found\n");
            return -1;
        }
        
        
        long blocksCount = blocks_count(fp, block_size);
        struct tnode *root = NULL;
        long byte_read = 0;
        char buffer_in[block_size];
        for (int i = 0; i < blocksCount; i++)
        {
            // get the block ngram
            byte_read = fread(buffer_in, sizeof(char), block_size, fp);
            printf("byte read %ld\n", byte_read);
            root = ngram(byte_read, nsize, buffer_in);
            printf("%f\n", manhaten_distance(root, byte_read));
        }
    }
    // if concentration calculation
    if (concentrationFlag == 1)
    {
        if (verboseFlag == 1)
            printf("Concentration Calculator\n");
        if (!inputFile)
        {
            help();
            return -1;
        }
        FILE *fp = NULL;
        fp = fopen(inputFile, "r");
        if (!fp)
        {
            printf("File not found\n");
            return -1;
        }
        unsigned long size = 0;
        long array_size = 0;
        long blocksCount = blocks_count(fp, block_size);
        struct tnode *root = NULL;
        long byte_read = 0;
        char buffer_in[block_size];
        struct tnode *node = NULL;
        unsigned char hextemp[2];
        nsize = strlen(hexString) / 2; // to do if nsize is not 2n
        unsigned char hexvalue[nsize]; // malloc
        // convert string to hex
        // 4142 nsize=2
        for (int i = 0; i < nsize; i++)
        {
            strncpy(hextemp, hexString + (i * 2), 2);
            hexvalue[i] = (char)strtol(hextemp, NULL, 16);
        }
        //    for (int i = 0; i < nsize; i++)
        //    {
        //         printf("%02x", hexvalue[i]);
        //    }

        for (int i = 0; i < blocksCount; i++)
        {
            // get the block ngram
            byte_read = fread(buffer_in, sizeof(char), block_size, fp);
            root = ngram(byte_read, nsize, buffer_in);
            if (node = ngram_count(root, hexvalue, nsize))
                printf("%ld => %f%%\n", node->count, 100 * (float)node->count / (float)byte_read);
            else
                printf("0\n");
        }
    }
    // End parsing

    return 0;
}
