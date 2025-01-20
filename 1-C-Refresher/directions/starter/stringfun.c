#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define BUFFER_SZ 50

//prototypes
void usage(char *);
void print_buff(char *, int);
int  setup_buff(char *, char *, int);

//prototypes for functions to handle required functionality
int  count_words(char *, int);
//add additional prototypes here
void reverse_string(char *, int);
int  print_words(char *, int);
int  do_search_replace(char *buff, int *str_len_ptr, int buff_size, char *oldStr, char *newStr);


// Fills 'buff' with the user string, replacing runs of whitespace with a single space.
// Returns the number of characters copied, or -1 if it wouldn't fiut.
int setup_buff(char *buff, char *user_str, int len){
    int count = 0;         
    int in_whitespace = 0; 

    while (*user_str != '\0') {
        char c = *user_str;

        // Check for whitespace characters
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {

            // Only add one space if we aren't at the start or already in whitespace
            if (count != 0 && !in_whitespace) {
                if (count >= len) {
                    return -1;
                }
                *(buff + count) = ' ';
                count++;
                in_whitespace = 1;
            }
        } 
        else {
            if (count >= len) {
                return -1;
            }
            *(buff + count) = c;
            count++;
            in_whitespace = 0;
        }
        user_str++;
    }

    // Remove trailing space if the last character was whitespace.
    if (in_whitespace && count > 0) {
        count--;
    }

    // Fill the rest of the buffer with dots
    for (int i = count; i < len; i++) {
        *(buff + i) = '.';
    }

    return count;
}

// Prints the buffer contents
void print_buff(char *buff, int len) {
    printf("Buffer:  [");
    for (int i = 0; i < len; i++) {
        putchar(*(buff + i));
    }
    printf("]\n");
}

void usage(char *exename){
    // Print how the program should be run
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);

}

// Counts how many words are in the buffer by checking spaces
int count_words(char *buff, int str_len) {
    int count = 0;
    int in_word = 0;

    for (int i = 0; i < str_len; i++) {
        char c = *(buff + i);
        if (c == ' ') {
            if (in_word) {
                in_word = 0;
            }
        } else {
            if (!in_word) {
                count++;
                in_word = 1;
            }
        }
    }
    return count;
}

// Reverses the entire string in place
void reverse_string(char *buff, int str_len) {
    int i = 0;
    int j = str_len - 1;
    while (i < j) {
        char temp = *(buff + i);
        *(buff + i) = *(buff + j);
        *(buff + j) = temp;
        i++;
        j--;
    }
}

// Prints each word on its own line, and returns the total number of words
int print_words(char *buff, int str_len) {
    int word_count = 0;
    int in_word = 0;
    int start_index = 0;

    printf("Word Print\n");
    printf("----------\n");

    for (int i = 0; i < str_len; i++) {
        char c = *(buff + i);

        if (c == ' ') {
            // End of word
            if (in_word) {
                in_word = 0;
                word_count++;
                printf("%d. ", word_count);

                int word_len = i - start_index;
                for (int w = 0; w < word_len; w++) {
                    putchar(*(buff + start_index + w));
                }
                printf("(%d)\n", word_len);
            }
        } else {
            // Start of new word
            if (!in_word) {
                in_word = 1;
                start_index = i;
            }
        }
    }

    // Check for trailing word
    if (in_word) {
        word_count++;
        printf("%d. ", word_count);
        int word_len = str_len - start_index;
        for (int w = 0; w < word_len; w++) {
            putchar(*(buff + start_index + w));
        }
        printf("(%d)\n", word_len);
    }

    printf("\nNumber of words returned: %d\n", word_count);
    return word_count;
}

// Searches for oldStr in buff and replaces it with newStr, adjusting buffer as need be
int do_search_replace(char *buff, int *str_len_ptr, int buff_size, char *oldStr, char *newStr) {
    int oldLen = 0;
    int newLen = 0;
    char *p = oldStr;

    // Compute lengths of oldStr
    while (*p != '\0') {
        oldLen++;
        p++;
    }

    // Compute lengths of newStr
    p = newStr;
    while (*p != '\0') {
        newLen++;
        p++;
    }

    int str_len = *str_len_ptr;
    int foundIndex = -1;

    // Finds the first occurrance of oldStr in buff
    for (int i = 0; i <= str_len - oldLen; i++) {
        int match = 1;
        for (int j = 0; j < oldLen; j++) {
            if (*(buff + i + j) != *(oldStr + j)) {
                match = 0;
                break;
            }
        }
        if (match) {
            foundIndex = i;
            break;
        }
    }

    // If not found, return 1
    if (foundIndex == -1) {
        return 1; 
    }

    // Calculate new total length after replacement
    int tailLen = str_len - (foundIndex + oldLen);
    int newTotalLen = str_len - oldLen + newLen;
    if (newTotalLen > buff_size) {
        newTotalLen = buff_size;
    }
    
    // Shift the tail of the buffer if newStr is longer or shorter
    if (newLen > oldLen) {
        int shift = newLen - oldLen;
        for (int t = tailLen - 1; t >= 0; t--) {
            int srcPos = foundIndex + oldLen + t;
            int dstPos = srcPos + shift;
            if (dstPos < buff_size) {
                *(buff + dstPos) = *(buff + srcPos);
            }
        }
    }
    else if (newLen < oldLen) {
        int shift = oldLen - newLen;
        for (int t = 0; t < tailLen; t++) {
            int srcPos = foundIndex + oldLen + t;
            int dstPos = srcPos - shift;
            *(buff + dstPos) = *(buff + srcPos);
        }
    }

    // Copy newStr into the foundIndex position
    for (int i = 0; i < newLen; i++) {
        if (foundIndex + i < buff_size) {
            *(buff + foundIndex + i) = *(newStr + i);
        }
    }

    // Update str_len_ptr and fill extra space witj dots
    if (newTotalLen < buff_size) {
        *str_len_ptr = newTotalLen;
    } else {
        *str_len_ptr = buff_size;
    }

    for (int i = *str_len_ptr; i < buff_size; i++) {
        *(buff + i) = '.';
    }

    return 0;
}

int main(int argc, char *argv[]){

    char *buff;             //placehoder for the internal buffer
    char *input_string;     //holds the string provided by the user on cmd line
    char opt;               //used to capture user option from cmd line
    int  rc;                //used for return codes
    int  user_str_len;      //length of user supplied string

    //TODO:  #1. WHY IS THIS SAFE, aka what if argv[1] does not exist?
    //TODO ANSWER #1: We check if (argc < 2) before accessing argv[1], 
    //                ensuring there’s actually a second argument. 
    //                This avoids reading memory that isn’t there.
    if ((argc < 2) || (*argv[1] != '-')){
        usage(argv[0]);
        exit(1);
    }

    opt = (char)*(argv[1]+1);   //get the option flag

    //handle the help flag and then exit normally
    if (opt == 'h'){
        usage(argv[0]);
        exit(0);
    }

    //WE NOW WILL HANDLE THE REQUIRED OPERATIONS

    //TODO:  #2 Document the purpose of the if statement below
    //TODO ANSWER #2: We need at least 3 arguments (argv[0] for the 
    //                program name, argv[1] for the option, argv[2] 
    //                for the string). If fewer, we print usage and exit.
    if (argc < 3){
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; //capture the user input string

    //TODO:  #3 Allocate space for the buffer using malloc and
    //          handle error if malloc fails by exiting with a 
    //          return code of 99

    buff = malloc(BUFFER_SZ * sizeof(char));
    if (!buff) {
        printf("Memory allocation failed.\n");
        exit(99);
    }

    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);     //see todos
    if (user_str_len < 0){
        printf("Error setting up buffer, error = %d", user_str_len);
        free(buff);
        exit(2);
    }

    switch (opt) {
        // counts of the number of words
        case 'c': {
            rc = count_words(buff, user_str_len);
            if (rc < 0) {
                printf("Error counting words, rc = %d\n", rc);
                free(buff);
                exit(2);
            }
            printf("Word Count: %d\n", rc);
            break;
        }

        // reverses the characters (in place)
        case 'r': {
            reverse_string(buff, user_str_len);
            break;
        }

        // prints the individual words and their length
        case 'w': {
            rc = print_words(buff, user_str_len);
            if (rc < 0) {
                printf("Error printing words, rc = %d\n", rc);
                free(buff);
                exit(2);
            }
            break;
        }

        // takes sample string and 2 other strings, replaces the first
        case 'x': {
            if (argc < 5) {
                usage(argv[0]);
                free(buff);
                exit(1);
            }
            rc = do_search_replace(buff, &user_str_len, BUFFER_SZ, argv[3], argv[4]);
            if (rc == 1) {
                printf("Substring not found!\n");
                free(buff);
                exit(3);
            } else if (rc == 2) {
                printf("Buffer overflow on replace!\n");
                free(buff);
                exit(3);
            }
            break;
        }

        default:
            usage(argv[0]);
            free(buff);
            exit(1);
    }

    print_buff(buff, BUFFER_SZ);

    free(buff);
    exit(0);
}

//TODO:  #7  Notice all of the helper functions provided in the 
//          starter take both the buffer as well as the length.  Why
//          do you think providing both the pointer and the length
//          is a good practice, after all we know from main() that 
//          the buff variable will have exactly 50 bytes?
//  
//TODO ANSWER #7: Passing both char *buff and int len lets each function safely 
//                handle data without assuming a fixed size. It prevents accidental
//                out-of-bounds errors and makes the code more reusable.