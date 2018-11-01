/*
 * Author: Ehiremen Ekore
 * Started: 10/2018
 *
 * NOTE:    While this project CAN reduce the size of various types of files, it was written with
 *          .js files in mind. As such, OUTPUT IS SAVED to a .js file.
 *          To work around this, simply change: closeBufferFile, bufferFile, bundleFile, and
 *              trimBufferFile to fopen() the appropriate files. The MOST IMPORTANT for this is
 *              bundleFile as that is the one that holds the final output.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

const int QUIT_MENU = 2;

char* getAndCheckFileName();
int readAndTrimFile(char* fileToBeRead);

int main() {
    char* trimThisFile;
    int menuOptionChosen=0, cumulativeSizeOfFiles=0;
    printf("Program by Ehiremen Ekore...\n");
    printf("Welcome,\n\tThis file size optimizer reads files that you input--be careful "
           "with file names--and minifies \nthem in one file...\n");

    while(menuOptionChosen != 2){
        printf("Please type in the appropriate number for your task\n");
        printf("(1) Minify a file for me\n");
        printf("(%i) I'm done; EXIT\n", QUIT_MENU);

        scanf(" %d", &menuOptionChosen);
        //The newline from the scanf is still in the buffer and will be used as the input if not caught
        char junk = getchar();

        if (menuOptionChosen == 1) {
            trimThisFile = getAndCheckFileName();
            FILE* checkSize = fopen(trimThisFile, "r");
            //get the initial size of the file
            fseek(checkSize, 0, SEEK_END);
            int sizeOfCurrentFile = ftell(checkSize);
            fclose(checkSize);
            printf("\nSize of %s is %d bytes\n", trimThisFile, sizeOfCurrentFile);

            cumulativeSizeOfFiles += sizeOfCurrentFile;
            printf("Cumulative size of files is %d bytes\n", cumulativeSizeOfFiles);

            int sizeOfBundle = readAndTrimFile(trimThisFile);
            printf("Size after optimization is %d bytes\n\n", sizeOfBundle);
        } else if (menuOptionChosen == QUIT_MENU) {
            printf("Goodbye :)");

            //clear the content of buffer.js at the end of the program. This way, restarting the program
            //will always have an empty instance of buffer.js
            FILE* closeBufferFile = fopen("buffer.js", "w");
            fclose(closeBufferFile);
        } else {
            printf("Invalid choice, please retry \n");
        }
    }
    return 0;
}

char* getAndCheckFileName(){
    int i, n=0;
    FILE* file;
    char *tempFileName;
    char *inputFileName;

    while (1){
        i = 0;
        //because explicit file names can get long
        int lengthOfFileName = 80;
        printf("Please input the name of your file, including the extension\n");
        printf("If the file is not in the same folder as this program's executable--or if you're not sure--\n"
               "you are free to use an explicit file name");
        //perform the calloc() here so that the memory gets reset each time an invalid fileName is entered
        tempFileName = (char*) calloc(lengthOfFileName, sizeof(char));

        //this is how I like to input char arrays usually
        while (((tempFileName[i] = getchar()) != '\n') && (i < lengthOfFileName)) {
            i++;
            n = i;
        }

        /*
         * inputFileName is dynamically allocated so it can be returned to main.
         * Using tempFileName before inputFileName to fix any issues arising from using calloc with tempFileName
         * The calloc on inputFileName ensures that the array is the perfect size for the data it holds
         */
        inputFileName = (char*) calloc(n, sizeof(char));
        for (i=0; i<n; i++){
            inputFileName[i] = tempFileName[i];
        }

        if ((file = fopen(inputFileName, "r")) == NULL){
            //Get an error message printed to console to help debug
            perror("Error");
            continue;
        } else break;
    }
    fclose(file);
    return inputFileName;
}

int readAndTrimFile(char* fileToBeRead) {
    /*
     * Create a buffer file to allow replacing newlines to be done separately from
     * replacing consecutive whitespace characters.
     * We open buffer.js as an "append" so that previous content isn't deleted from the bundle file
     */
    FILE *bufferFile = fopen("buffer.js", "a");
    FILE *minifyThisFile = fopen(fileToBeRead, "r");

    //bundle file is opened with "w" to avoid having old date duplicated when a new file is added
    FILE *bundleFile = fopen("bundle.js", "w");

    char inputBuffer;
    char* testBuffer;
    int sizeOfTestBundleFile = 0, sizeOfBundleFile = 0;

    /*
     * This block modification of code gotten from stack overflow
     * https://stackoverflow.com/questions/32890627/read-input-from-one-file-and-write-it-to-another-file-in-c
     *
     * I chose to first read the contents of the inputFile to a "buffer" file
     */
    while ((inputBuffer = getc(minifyThisFile)) != EOF) {
        if (inputBuffer == '\n'){
            inputBuffer = '\0';
        }
        putc(inputBuffer, bufferFile);
    }

    //get the size of the bufferFile to know the max memory needed to be allocated to the bundleFile
    fseek(bufferFile, 0, SEEK_END);
    sizeOfTestBundleFile = ftell(bufferFile);
    rewind(bufferFile);

    //this segment works around eliminating consecutive whitespaces
    FILE *trimBufferFile = fopen("buffer.js", "r");
    testBuffer = calloc(sizeOfTestBundleFile, sizeof(char));

    for (int i=0; i<sizeOfTestBundleFile; i++){
        fscanf(trimBufferFile, "%c", &testBuffer[i]);
    }
    /*
     * I realize that I can work this block into the block of code that writes the input file(s)
     * into the bufferFile and avoid having to read an extra file. However, the approach I am taking
     * is a lot easier to follow and understand.
     */
    for (int i=0; i<sizeOfTestBundleFile; i++){
        //Check if testBuffer[i] and the character after it are both whitespace characters
        if (isspace(testBuffer[i]) && isspace(testBuffer[i+1])){
            //this basically skips over i
            //my logic is that i and (i+1) both hold whitespaces, so (i+1) can "represent" them both
            i += 1;
        } else{
            putc(testBuffer[i], bundleFile);
        }
    }
    //get the actual byte count for the minified bundleFile
    fseek(bundleFile, 0, SEEK_END);
    sizeOfBundleFile = ftell(bundleFile);

    fclose(bufferFile);
    fclose(bundleFile);
    fclose(minifyThisFile);
    return sizeOfBundleFile;
}