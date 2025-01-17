#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>


#define BUFFER_SZ 50

//prototypes
void usage(char *);
void print_buff(char *, int);
int  setup_buff(char *, char *, int);

//prototypes for functions to handle required functionality
int  count_words(char *, int, int);

//add additional prototypes here
void reverse(char *, int, int);
void word_print(char *, int, int);
char replace(char *, char *, char *, int, int);

int setup_buff(char *buff, char *user_str, int len){
    //TODO: #4:  Implement the setup buff as per the directions
    int count;
    int str_len;
    bool space_flg = false;

    while (*user_str){
        if (*user_str == ' ' && !space_flg && *(user_str + 1) != '\0') {
            // Add space to buffer only if it's the first space
            buff[count++] = *user_str;
            str_len++;
            space_flg = true;
        } else if (*user_str != ' ') {
            // Non-space characters
            buff[count++] = *user_str;
            str_len++;
            space_flg = false;  // Reset space flag
        }
        user_str++;  // Move to the next character
    } 

    // user string is > buffer size
    if (count > len){
        printf("The user supplied string is too large\n");
        exit(-1);
    }

    // fill in buffer with '.'
    for (int i=count; i<len; i++){
        buff[i] = '.';
    }

    return str_len;
}

void print_buff(char *buff, int len){
    printf("Buffer:  ");
    for (int i=0; i<len; i++){
        putchar(*(buff+i));
    }
    putchar('\n');
}

void usage(char *exename){
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);

}

int count_words(char *buff, int len, int str_len){
    int word_count = 0;
    bool chara = false; //character

    // every space encountered before '.' AND after a character is another word
    while(*buff != '.'){
        if (*buff == ' ' && (*(buff + 1) != '.')) {
            if (chara){
                word_count++;
                chara = false;
            }
        } else {
            chara = true;
        }
        *buff++;
    }

    if (*buff == '.' & chara){
        word_count++;
    }

    return word_count;
}

//ADD OTHER HELPER FUNCTIONS HERE FOR OTHER REQUIRED PROGRAM OPTIONS
void reverse(char *buff, int len, int str_len){
    // go backwards in the buff, starting at buff+string_len and print out
    for (int i = (str_len-1); i>-1; i--){
        printf("%c", *(buff+i));
    }
    printf("\n");
}

void word_print(char *buff, int wc, int str_len){
    int char_count = 0;
    for (int i = 1; i <= wc && i <= str_len; i++){
        printf("%d. ", i);
        // check if str starts with space
        if (*buff == ' '){
            *buff++;
        }

        // loop through characters in words and print
        while (*buff != ' '){
            if (*buff == '.'){
                break;
            }
            printf("%c", *buff);
            char_count++;
            *buff++;
        }

        // print char count and reset
        printf(" (%d)\n", char_count);
        char_count = 0;
    }
}



int main(int argc, char *argv[]){

    char *buff;             //placehoder for the internal buffer
    char *input_string;     //holds the string provided by the user on cmd line
    char opt;               //used to capture user option from cmd line
    int  rc;                //used for return codes
    int  user_str_len;      //length of user supplied string

    //TODO:  #1. WHY IS THIS SAFE, aka what if arv[1] does not exist?
    //      PLACE A COMMENT BLOCK HERE EXPLAINING
    /*
        If argv[1] doesn't exists then argc will only be 1
        Also, if argv[1] doesn't start with a dash then the code will also exit.
    */
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
    /*
        Purpose: if the user doesn't define a string then the code will exit as the whole point of this code is to do operations on a string.
    */
    if (argc < 3){
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; //capture the user input string

    //TODO:  #3 Allocate space for the buffer using malloc and
    //          handle error if malloc fails by exiting with a 
    //          return code of 99
    // CODE GOES HERE FOR #3
    buff = malloc(BUFFER_SZ);
    if (buff == NULL) {
        exit(2);
    }

    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);     //see todos
    if (user_str_len < 0){
        printf("Error setting up buffer, error = %d", user_str_len);
        exit(2);
    }

    switch (opt){
        case 'c':
            rc = count_words(buff, BUFFER_SZ, user_str_len); 
            if (rc < 0){
                printf("Error counting words, rc = %d", rc);
                exit(2);
            }
            printf("Word Count: %d\n", rc);
            break;

        //TODO:  #5 Implement the other cases for 'r' and 'w' by extending
        //       the case statement options
        case 'r':
            reverse(buff, BUFFER_SZ, user_str_len); 
            break;
        
        case 'w':
            int word_count = count_words(buff, BUFFER_SZ, user_str_len);
            word_print(buff, word_count, user_str_len);
            break;

        case 'x':
            if (argc == 5){
                printf("Not Implemented!");

            } else {
                exit(1);
            }

        default:
            usage(argv[0]);
            exit(1);
    }

    //TODO:  #6 Dont forget to free your buffer before exiting
    print_buff(buff,BUFFER_SZ);
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
/*
    This is good practice because helper functions don't have knowledge
    of the BUFFER_SZ. Passing it as an argument can help prevent undefined
    behavior of the program. This way the functions won't try to access
    memory that it doesn't have access to.
*/