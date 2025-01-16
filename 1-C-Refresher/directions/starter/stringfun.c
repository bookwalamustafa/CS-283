#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define BUFFER_SZ 50

//prototypes
void usage(char *);
void print_buff(char *, int);
int  setup_buff(char *, char *, int);

//prototypes for functions to handle required functionality
int  count_words(char *, int, int);
//add additional prototypes here
void reverse_string(char *, int);
int  print_words(char *, int, int);
int  do_search_replace(char *buff, int *str_len_ptr, int buff_size, char *oldStr, char *newStr);

int setup_buff(char *buff, char *user_str, int len){
    int count = 0;         // number of chars put into buff
    int in_whitespace = 0; // track if we are currently in consecutive whitespace

    while (*user_str != '\0') {
        char c = *user_str;

        // Check for whitespace
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {

            // If we haven't put any characters yet, skip leading whitespace
            if (count == 0) {
                // do nothing
            }
            // Otherwise, if not already in whitespace, insert one space
            else if (!in_whitespace) {
                if (count >= len) {
                    return -1; // user string too large
                }
                *(buff + count) = ' ';
                count++;
                in_whitespace = 1;
            }
        } 
        else {
            // Non-whitespace character
            if (count >= len) {
                return -1; // user string too large
            }
            *(buff + count) = c;
            count++;
            in_whitespace = 0;
        }
        user_str++;
    }

    // <-- NEW LOGIC to remove trailing space if it exists
    if (in_whitespace && count > 0) {
        // The last char we wrote was a space, so remove it
        count--;
    }

    // Fill the remainder of buff with '.'
    for (int i = count; i < len; i++) {
        *(buff + i) = '.';
    }

    return count; // length of the "real" string
}


void print_buff(char *buff, int len) {
    printf("Buffer:  [");
    for (int i = 0; i < len; i++) {
        putchar(*(buff + i));
    }
    printf("]\n");
}

void usage(char *exename){
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);

}

int count_words(char *buff, int len, int str_len){
    //YOU MUST IMPLEMENT
    // We only examine the first str_len chars of buff (ignore trailing dots)
    int count = 0;
    int in_word = 0;

    for (int i = 0; i < str_len; i++) {
        char c = *(buff + i);
        if (c == ' ') {
            // If we were in a word, we just ended it
            if (in_word) {
                in_word = 0;
            }
        } else {
            // Non-space char
            if (!in_word) {
                count++;
                in_word = 1;
            }
        }
    }
    return count;
}

//ADD OTHER HELPER FUNCTIONS HERE FOR OTHER REQUIRED PROGRAM OPTIONS

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

int print_words(char *buff, int len, int str_len) {
    // We'll iterate through the first str_len characters, 
    // collecting words and printing them with an index.
    int word_count = 0;
    int in_word = 0;
    int start_index = 0;

    printf("Word Print\n");
    printf("----------\n");

    for (int i = 0; i < str_len; i++) {
        char c = *(buff + i);

        if (c == ' ') {
            // We hit a space -> end of a word if we were in a word
            if (in_word) {
                in_word = 0;
                // Print the word from start_index to i-1
                word_count++;
                printf("%d. ", word_count);

                // Word length is (i - start_index)
                int word_len = i - start_index;
                // Print characters
                for (int w = 0; w < word_len; w++) {
                    putchar(*(buff + start_index + w));
                }
                printf("(%d)\n", word_len);
            }
        } else {
            // Non-space character
            if (!in_word) {
                // We are starting a new word
                in_word = 1;
                start_index = i;
            }
        }
    }

    // If we ended and the last character(s) form a word,
    // we handle that here.
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

int do_search_replace(char *buff, int *str_len_ptr, int buff_size, 
                     char *oldStr, char *newStr)
{
    // 1) Compute length of oldStr and newStr using pointer-based approach
    int oldLen = 0;
    int newLen = 0;
    char *p = oldStr;
    while (*p != '\0') {
        oldLen++;
        p++;
    }
    p = newStr;
    while (*p != '\0') {
        newLen++;
        p++;
    }

    // 2) Find the FIRST occurrence of oldStr in the first (*str_len_ptr) chars of buff
    //    We'll do a naive substring search.
    int str_len = *str_len_ptr;
    int foundIndex = -1;

    for (int i = 0; i <= str_len - oldLen; i++) {
        // Compare buff[i..(i+oldLen-1)] to oldStr
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

    if (foundIndex == -1) {
        // Not found
        return 1; 
    }

    // 3) If found, we replace in place.
    //    Cases:
    //    a) newLen == oldLen: just copy newLen chars
    //    b) newLen < oldLen: shift the tail left
    //    c) newLen > oldLen: shift the tail right (watch for overflow)

    // The tail is what's after oldStr: buff[foundIndex + oldLen .. str_len - 1]
    int tailLen = str_len - (foundIndex + oldLen);

    // If we are extending the string, check for overflow
    int newTotalLen = str_len - oldLen + newLen; // the new "actual" length
    if (newTotalLen > buff_size) {
        // We'll handle it by truncating the final portion
        newTotalLen = buff_size; // cannot exceed buff_size
    }
    
    // SHIFTING:
    if (newLen > oldLen) {
        // Shift tail to the right by (newLen - oldLen)
        int shift = newLen - oldLen;
        // We'll move from the end to avoid overwriting
        // The tail starts at foundIndex+oldLen, ends at foundIndex+oldLen+tailLen-1
        for (int t = tailLen - 1; t >= 0; t--) {
            int srcPos = foundIndex + oldLen + t;
            int dstPos = srcPos + shift;
            if (dstPos < buff_size) {
                *(buff + dstPos) = *(buff + srcPos);
            }
        }
    }
    else if (newLen < oldLen) {
        // Shift tail to the left by (oldLen - newLen)
        int shift = oldLen - newLen;
        for (int t = 0; t < tailLen; t++) {
            int srcPos = foundIndex + oldLen + t;
            int dstPos = srcPos - shift;
            *(buff + dstPos) = *(buff + srcPos);
        }
    }

    // 4) Now copy newStr into position
    for (int i = 0; i < newLen; i++) {
        // watch out for boundary (in case newLen is huge)
        if (foundIndex + i < buff_size) {
            *(buff + foundIndex + i) = *(newStr + i);
        }
    }

    // 5) Update *str_len_ptr if newLen != oldLen
    //    but also cap it if it exceeds buff_size
    if (newTotalLen < buff_size) {
        *str_len_ptr = newTotalLen;
    } else {
        *str_len_ptr = buff_size;
    }

    // 6) Fill trailing space with '.' if we shortened or truncated
    //    So from index = *str_len_ptr up to buff_size-1, fill with '.'
    for (int i = *str_len_ptr; i < buff_size; i++) {
        *(buff + i) = '.';
    }

    // 7) Return success
    return 0;
}


int main(int argc, char *argv[]){

    char *buff;             //placehoder for the internal buffer
    char *input_string;     //holds the string provided by the user on cmd line
    char opt;               //used to capture user option from cmd line
    int  rc;                //used for return codes
    int  user_str_len;      //length of user supplied string

    //TODO:  #1. WHY IS THIS SAFE, aka what if arv[1] does not exist?
    //      PLACE A COMMENT BLOCK HERE EXPLAINING
    // We check (argc < 2) first, ensuring argv[1] is valid before we dereference *argv[1].
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
    //      PLACE A COMMENT BLOCK HERE EXPLAINING
    // We require at least 3 args. ex: ./stringfun -c "some string". If user only typed ./stringfun -c with no quoted string, it's invalid.

    if (argc < 3){
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; //capture the user input string

    //TODO:  #3 Allocate space for the buffer using malloc and
    //          handle error if malloc fails by exiting with a 
    //          return code of 99
    // CODE GOES HERE FOR #3

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
        case 'c': {
            rc = count_words(buff, BUFFER_SZ, user_str_len);
            if (rc < 0) {
                printf("Error counting words, rc = %d\n", rc);
                free(buff);
                exit(2);
            }
            printf("Word Count: %d\n", rc);
            break;
        }

        case 'r': {
            // Reverse the first user_str_len characters
            reverse_string(buff, user_str_len);
            // The tests only rely on final Buffer output, but if you 
            // want to explicitly print something, you could do:
            // printf("Reversed String: ");
            // for (int i=0; i<user_str_len; i++){
            //     putchar(*(buff+i));
            // }
            // putchar('\n');
            break;
        }

        case 'w': {
            rc = print_words(buff, BUFFER_SZ, user_str_len);
            if (rc < 0) {
                printf("Error printing words, rc = %d\n", rc);
                free(buff);
                exit(2);
            }
            break;
        }

        case 'x': {
            // Extra credit: fully do search-and-replace
            if (argc < 5) {
                // we need at least 5 args: 
                //    argv[0] -> program
                //    argv[1] -> -x
                //    argv[2] -> "some string"
                //    argv[3] -> oldStr
                //    argv[4] -> newStr
                usage(argv[0]);
                free(buff);
                exit(1);
            }
            rc = do_search_replace(buff, &user_str_len, BUFFER_SZ, argv[3], argv[4]);
            if (rc == 1) {
                // oldStr not found
                printf("Substring not found!\n");
                // Some tests might expect a non-zero exit if not found
                free(buff);
                exit(3);
            } else if (rc == 2) {
                // Overflow or other issue
                printf("Buffer overflow on replace!\n");
                free(buff);
                exit(3);
            }
            // rc == 0 -> success, do nothing special
            break;
        }

        default:
            usage(argv[0]);
            free(buff);
            exit(1);
    }

    //----------------------------------------------------------------
    //  #6: Don't forget to free your buffer before exiting
    //----------------------------------------------------------------
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
//          PLACE YOUR ANSWER HERE
//  It's good practice because the function won't rely on global 
//  or hidden assumptions about the buffer size. Also, it helps 
//  make the functions more generic and safer, preventing reading 
//  or writing out of bounds.