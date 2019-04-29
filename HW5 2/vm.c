/**
 * Demonstration C program illustrating how to perform file I/O for vm assignment.
 *
 * Input file contains logical addresses
 * 
 * Backing Store represents the file being read from disk (the backing store.)
 *
 * We need to perform random input from the backing store using fseek() and fread()
 *
 * This program performs nothing of meaning, rather it is intended to illustrate the basics
 * of I/O for the vm assignment. Using this I/O, you will need to make the necessary adjustments
 * that implement the virtual memory manager.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// number of characters to read for each line from input file
#define BUFFER_SIZE         8

// number of bytes to read
#define CHUNK               99

// the size of page table (2^8)
#define PAGE_SIZE           256

// the size of the virtual memory (2^8)
#define VM_SIZE             256  

// the size of the memory
#define MEMORY_SIZE         65536

// the entries allowed in the TLB
#define TLB_SIZE            16




FILE    *address_file;
FILE    *backing_store;

// how we store reads from input file
char    address[BUFFER_SIZE];

int     logical_address;
int     offset;
int     pageNumber;
int     counter = 0;
int     pageFault = 0;
int     tlbCounter = 0;
int     frontOfQueue = -1; 
int     backOfQueue = -1; 
int     frameNumber;
int     emptyFrame =0;
int     physical;
int     tlb[TLB_SIZE][2];
int     pageTable[PAGE_SIZE];

// the buffer containing reads from backing store
signed char     buffer[CHUNK];
// the value of the byte (signed char) in memory
signed char     value;

// methods
void update(int pageNumber, int frameNumber);
int getFrame(int pageNumber);
int pageFrame(int pageNumber);


int main(int argc, char *argv[])
{
    int i = 0;

    // array for physical memory 


    // filling TLB
    for(int i = 0; i < TLB_SIZE; i++)
    {
        if (tlb[i][0] == (i > TLB_SIZE - 1))
        {
            tlb[i][0] = -1;

        }

        else
        {
            tlb[i][0] = i;
        }

        if (tlb[i][1] == (i > TLB_SIZE - 1))
        {
            tlb[i][1] = -1;
        }
        else 
        {
            tlb[i][1] = TLB_SIZE - i;
        }
    }

    // filling pagetable
    for (int i = 0; i < PAGE_SIZE; i++) 
    {
        pageTable[i] = -1;
    }

    // perform basic error checking
    if (argc != 3) {
        fprintf(stderr,"Usage: ./vm [backing store] [input file]\n");
        return -1;
    }

    // open the file containing the backing store
    backing_store = fopen(argv[1], "rb");

    if (backing_store == NULL) { 
        fprintf(stderr, "Error opening %s\n",argv[1]);
        return -1;
    }

    // open the file containing the logical addresses
    address_file = fopen(argv[2], "r");

    if (address_file == NULL) {
        fprintf(stderr, "Error opening %s\n",argv[2]);
        return -1;
    }

    // read through the input file and output each logical address
    while ( fgets(address, BUFFER_SIZE, address_file) != NULL) {
        logical_address = atoi(address);

        counter++;

        pageNumber = logical_address >> BUFFER_SIZE;
        offset = logical_address & (PAGE_SIZE - 1); //applies mask of 255

        frameNumber = getFrame(pageNumber);

        physical = pageNumber + offset;



        if(frameNumber != -1)
        {
            physical = frameNumber + offset; 
        }

        else
        {
            frameNumber = pageFrame(pageNumber);

            if(frameNumber != -1)
            {
                physical = frameNumber + offset;
                update(pageNumber, frameNumber);


            }

            else
            {
                pageFault++;
                //int pageAddress = pageNumber * PAGE_SIZE

                if(emptyFrame != -1)
                {
                    //store the page

                    frameNumber = emptyFrame;

                    physical = frameNumber + offset;



                    pageTable[pageNumber] = emptyFrame;
                    update(pageNumber, frameNumber);

                    if(emptyFrame < MEMORY_SIZE - PAGE_SIZE)
                    {
                        emptyFrame += PAGE_SIZE;
                    }

                    else
                    {
                        emptyFrame = -1;
                    }
                }

            }

        }



        // first seek to byte CHUNK in the backing store
        // SEEK_SET in fseek() seeks from the beginning of the file
        if (fseek(backing_store, CHUNK * i, SEEK_SET) != 0) {
            fprintf(stderr, "Error seeking in backing store\n");
            return -1;
        }

        // now read CHUNK bytes from the backing store to the buffer
        if (fread(buffer, sizeof(signed char), CHUNK, backing_store) == 0) {
            fprintf(stderr, "Error reading from backing store\n");
            return -1;
        }

        // arbitrarily retrieve the 50th byte from the buffer 
       // value = buffer[50];


        printf("Virtual address: %d ", logical_address); 
        printf("Physical address: %d ", physical);
       // printf("Value: %d\n", value);

    }

    /* Print the statistics to the end of the output file. */
    printf("Number of Translated Addresses = %d\n", counter); 
    printf("Page Faults = %d\n", pageFault/2);
    printf("TLB Hits = %d\n", tlbCounter);


    fclose(address_file);
    fclose(backing_store);

    return 0;
}

void update(int pageNumber, int frameNumber)
{
    // Use FIFO for TLB

    if(frontOfQueue == -1)
    {
        frontOfQueue = 0;
        backOfQueue = 0;

        tlb[backOfQueue][0] = pageNumber;
        tlb[backOfQueue][1] = frameNumber;
    }

    else
    {
        frontOfQueue = (frontOfQueue + 1) % TLB_SIZE;
        backOfQueue = (backOfQueue + 1) % TLB_SIZE;

        tlb[backOfQueue][0] = pageNumber;
        tlb[backOfQueue][1] = frameNumber;
    }
}

int getFrame(int pageNumber)
{
    for(int i = 0; i < TLB_SIZE; i++)
    {
        if(tlb[i][0] == pageNumber)
        {
            tlbCounter++;


            return tlb[i][1];
        }
    }

    return -1;

}

int pageFrame(int pageNumber)
{
    if (pageTable[pageNumber] == -1) {
        pageFault++;
    }

    return pageTable[pageNumber];

}
