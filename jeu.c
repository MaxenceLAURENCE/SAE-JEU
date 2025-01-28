#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h> 

/* depending on the operating system : */
#ifdef _WIN32
#include <windows.h> /* For Windows */
#else
#include <unistd.h>  /* For Linux and macos */
#endif

void wait() {
#ifdef _WIN32
    Sleep(1); /* Remark : 1 ms = 0.001 second */
#else
    usleep(10000); /* Remark : Sleep(1) = usleep(10000)*/
#endif
} 

/* struct player information and inventory: */
typedef struct {
    char name[200]; 
    int pv; 
    char inventory[200]; 
    int langage; /* 1 = FR | 2 = GB */
} player_info;  


void print_text_effect(char* text) { /* allows to display text with an effect */
    int size = strlen(text); /* to get the size of string, we can use strlen() */
    int i; 
    for (i = 0 ; i <= size ; i++) { /* print the string letter by letter with a wait of 0.025 second */
        printf("%c", text[i]); 
        fflush(stdout); /* clears the buffer and display immediately (usefull for linux)*/
        wait(); 
    }
} 

void remove_bom(char *str) { /* the bom is a caractere to say the encodage to the file editor. We don't need it */
    if ((unsigned char)str[0] == 0xEF && (unsigned char)str[1] == 0xBB && (unsigned char)str[2] == 0xBF) { 
        memmove(str, str + 3, strlen(str) - 2); /* we move to avoid the character */
    }
}

void buffer_clear(){
    int letter; 
    while (1) {
        letter = getchar(); 
        if (letter == EOF || letter == '\n') {
            break; /* clear a letter until the end of the buffer. we stop if end of file or next line */
        }
    }
}


char* read_file(char* file_path, char* folder_path, player_info player){ 
    printf("\033[H\033[J"); /* = clear the terminal */

    /* open the file */
    FILE *text_origine = fopen(file_path, "r");  

    /* get the size of the file (after that, the size is on sb.st_size) */
    struct stat sb;
    stat(file_path, &sb); 
    
    /* create the variable with size of the file */ 
    char all_text[sb.st_size]; 

    /* take the text of the file and put it on "text" */
    fread(all_text, 1, sb.st_size, text_origine); 

    /* some information is on the first line. */ 

    remove_bom(all_text); /* the bom is a caractere to say the encodage to the file editor. We don't need it */

    char first_line[2000]; 
    char text[9999]; 

    int i = 0;  
    while (all_text[i] != '\n') { /* copy all_text until we have '\n' = the second line */
        first_line[i] = all_text[i]; 
        i++; 
    }
    first_line[i] = '\0'; 

    i++; /* for past the '\n' */

    int j = 0; 
    while (all_text[i] != '\0') { /* copy all_text from i to the end */
        text[j] = all_text[i]; 
        i++; 
        j++; 
    }
    text[j] = '\0'; 

    /* close the file */ 
    fclose(text_origine); 
    
    /* extract information of the first line */ 
    /* the information is in the form : 0| (= none)   or   1| (= win)   or   2| (= game_over)  */ 

    int finish; 
    sscanf(first_line, "%d|", &finish); 

    /* print the text */
    print_text_effect(text); 
    printf("\n\n"); 

    switch (finish) {
        case 0: /* not a win and not a game over */
            /* now, the choice */ 
            int choice; 

            while (1) { 
                if (player.langage == 1) { /* if langage is FR */
                    print_text_effect("\n\nVotre choix : "); 
                } else if (player.langage == 2) { /* if langage is GB */
                    print_text_effect("\n\nYour choice : "); 
                }
                
                scanf("%d", &choice); 
                
                switch (choice) {
                    case 0: /* save & exit */
                        char save_info[2000]; 

                        sprintf(save_info, "%s|%s|%s|%d|%s\n", file_path, folder_path, player.name, player.pv, player.inventory);  
                        printf("save_info : \n%s", save_info); 

                        FILE *save_file = fopen("./save.txt", "a"); 
                        fprintf(save_file, save_info); 
                        fclose(save_file); 

                        if (player.langage == 1) { /* if langage is FR */
                            print_text_effect("\n\nPartie sauvegarder !"); 
                        } else if (player.langage == 2) { /* if langage is GB */
                            print_text_effect("\n\nGame Saved !"); 
                        }
                        break; 
                    case 1: 
                        /* new file_path : folder_path + choice1.txt */
                        sprintf(file_path, "%s%s", folder_path, "choice1.txt"); /* folder_path + choice1.txt*/
                        
                        /* new folder_path : old folder + choice2/ */
                        sprintf(folder_path, "%s%s", folder_path, "choice1/");
                        
                        /* now, we have new folder_path and new file_path : we can read */
                        return read_file(file_path, folder_path, player); /* nom fichier1 après */
                        break; 

                    case 2: 
                        /* new file_path : folder_path + choice2.txt */
                        sprintf(file_path, "%s%s", folder_path, "choice2.txt"); /* folder_path + choice1.txt*/

                        /* new folder_path : old folder + choice2/ */
                        sprintf(folder_path, "%s%s", folder_path, "choice2/");
                        
                        /* now, we have new folder_path and new file_path : we can read */
                        return read_file(file_path, folder_path, player); 
                        break; 
                    
                    case 3: 
                        /* new file_path : folder_path + choice2.txt */
                        sprintf(file_path, "%s%s", folder_path, "choice3.txt"); /* folder_path + choice1.txt*/

                        /* new folder_path : old folder + choice2/ */
                        sprintf(folder_path, "%s%s", folder_path, "choice3/");
                        
                        /* now, we have new folder_path and new file_path : we can read */
                        read_file(file_path, folder_path, player); 
                        break; 
                    
                    default: 
                        print_text_effect("Error, try again\n"); 

                        /* we need to clear the buffer */
                        buffer_clear(); 
                        break; 
                }
            }
            break; 
        
        case 1: /* win */
            printf("                                                                                                         \n                 ,----..                                                          ,--.,`--.' |  \n                /   /   \\                                   .---.   ,---,       ,--.'||   :  :  \n        ,---,  /   .     :          ,--,                   /. ./|,`--.' |   ,--,:  : |'   '  ;  \n       /_ ./| .   /   ;.  \\       ,'_ /|               .--'.  ' ;|   :  :,`--.'`|  ' :|   |  |  \n ,---, |  ' :.   ;   /  ` ;  .--. |  | :              /__./ \\ : |:   |  '|   :  :  | |'   :  ;  \n/___/ \\.  : |;   |  ; \\ ; |,'_ /| :  . |          .--'.  '   \\' .|   :  |:   |   \\ | :|   |  '  \n .  \\  \\ ,' '|   :  | ; | '|  ' | |  . .         /___/ \\ |    ' ''   '  ;|   : '  '; |'   :  |  \n  \\  ;  `  ,'.   |  ' ' ' :|  | ' |  | |         ;   \\  \\;      :|   |  |'   ' ;.    ;;   |  ;  \n   \\  \\    ' '   ;  \\; /  |:  | | :  ' ;          \\   ;  `      |'   :  ;|   | | \\   |`---'. |  \n    '  \\   |  \\   \\  ',  / |  ; ' |  | '           .   \\    .\\  ;|   |  ''   : |  ; .' `--..`;  \n     \\  ;  ;   ;   :    /  :  | : ;  ; |            \\   \\   ' \\ |'   :  ||   | '`--'  .--,_     \n      :  \\  \\   \\   \\ .'   '  :  `--'   \\            :   '  |--\" ;   |.' '   : |      |    |`.  \n       \\  ' ;    `---`     :  ,      .-./             \\   \\ ;    '---'   ;   |.'      `-- -`, ; \n        `--`                `--`----'                  '---\"             '---'          '---`\"  \n                                                                                                \n");
            if (player.langage == 1) { /* if langage is FR */
                print_text_effect("\nAppuyer sur entrer pour quitter"); 
            } else if (player.langage == 2) { /* if langage is GB */
                print_text_effect("\nPress enter to exit"); 
            }

            buffer_clear(); /* clear the buffer */
            getchar(); /* wait until a caracter was taped */
            break; 
        
        case 2: /* game over */ 
            printf("┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼\n███▀▀▀██┼███▀▀▀███┼███▀█▄█▀███┼██▀▀▀\n██┼┼┼┼██┼██┼┼┼┼┼██┼██┼┼┼█┼┼┼██┼██┼┼┼\n██┼┼┼▄▄▄┼██▄▄▄▄▄██┼██┼┼┼▀┼┼┼██┼██▀▀▀\n██┼┼┼┼██┼██┼┼┼┼┼██┼██┼┼┼┼┼┼┼██┼██┼┼┼\n███▄▄▄██┼██┼┼┼┼┼██┼██┼┼┼┼┼┼┼██┼██▄▄▄\n┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼\n███▀▀▀███┼▀███┼┼██▀┼██▀▀▀┼██▀▀▀▀██▄┼\n██┼┼┼┼┼██┼┼┼██┼┼██┼┼██┼┼┼┼██┼┼┼┼┼██┼\n██┼┼┼┼┼██┼┼┼██┼┼██┼┼██▀▀▀┼██▄▄▄▄▄▀▀┼\n██┼┼┼┼┼██┼┼┼██┼┼█▀┼┼██┼┼┼┼██┼┼┼┼┼██┼\n███▄▄▄███┼┼┼─▀█▀┼┼─┼██▄▄▄┼██┼┼┼┼┼██▄\n┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼\n┼┼┼┼┼┼┼┼██┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼██┼┼┼┼┼┼┼┼┼\n┼┼┼┼┼┼████▄┼┼┼▄▄▄▄▄▄▄┼┼┼▄████┼┼┼┼┼┼┼\n┼┼┼┼┼┼┼┼┼▀▀█▄█████████▄█▀▀┼┼┼┼┼┼┼┼┼┼\n┼┼┼┼┼┼┼┼┼┼┼█████████████┼┼┼┼┼┼┼┼┼┼┼┼\n┼┼┼┼┼┼┼┼┼┼┼██▀▀▀███▀▀▀██┼┼┼┼┼┼┼┼┼┼┼┼\n┼┼┼┼┼┼┼┼┼┼┼██┼┼┼███┼┼┼██┼┼┼┼┼┼┼┼┼┼┼┼\n┼┼┼┼┼┼┼┼┼┼┼█████▀▄▀█████┼┼┼┼┼┼┼┼┼┼┼┼\n┼┼┼┼┼┼┼┼┼┼┼┼███████████┼┼┼┼┼┼┼┼┼┼┼┼┼\n┼┼┼┼┼┼┼┼▄▄▄██┼┼█▀█▀█┼┼██▄▄▄┼┼┼┼┼┼┼┼┼\n┼┼┼┼┼┼┼┼▀▀██┼┼┼┼┼┼┼┼┼┼┼██▀▀┼┼┼┼┼┼┼┼┼\n┼┼┼┼┼┼┼┼┼┼▀▀┼┼┼┼┼┼┼┼┼┼┼▀▀┼┼┼┼┼┼┼┼┼┼┼\n┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼┼\n");
            if (player.langage == 1) { /* if langage is FR */
                print_text_effect("\nAppuyer sur entrer pour quitter"); 
            } else if (player.langage == 2) { /* if langage is GB */
                print_text_effect("\nPress enter to exit"); 
            }

            buffer_clear(); /* clear the buffer */
            getchar(); /* wait until a caracter was taped */
            break; 
    }
}


int main(){
    printf("    _                          _             _             _____  ___   ___   ___  \n   / \\   _ __   ___   ___ __ _| |_   _ _ __ | |_ ___      |___ / / _ \\ / _ \\ / _ \\ \n  / _ \\ | '_ \\ / _ \\ / __/ _` | | | | | '_ \\| __/ _ \\ _____ |_ \\| | | | | | | | | |\n / ___ \\| |_) | (_) | (_| (_| | | |_| | |_) | || (_) |_____|__) | |_| | |_| | |_| |\n/_/   \\_\\ .__/ \\___/ \\___\\__,_|_|\\__, | .__/ \\__\\___/     |____/ \\___/ \\___/ \\___/ \n        |_|                      |___/|_|                                          \n");
    printf("\n\n\n\n"); 
    
    player_info player; 
    char file_path[260]; 
    char folder_path[260];

    print_text_effect("Select langage : \n1 : Français \n2 : English\n"); 
    int choice;
    
    while (1) {
        scanf("%d", &choice); 
        switch (choice) {
                case 1: /* Français */
                    print_text_effect("Nouvelle partie : 1 \nCharger une partie : 2\n");
                    while (1) {
                        scanf("%d", &choice); 
                        switch (choice) {
                            case 1: /* new game */
                                print_text_effect("Entrez votre nom : ");
                                scanf("%s", player.name);

                                player.pv = 100;  

                                strcpy(player.inventory, "shotgun;bullet"); 

                                player.langage = 1; 

                                strcpy(file_path, "./lobby-FR/intro.txt"); 
                                strcpy(folder_path, "./lobby-FR/");

                                read_file(file_path, folder_path, player);
                                return 0; 
                                break; 

                            case 2: /* load a game */ 
                                print_text_effect("work in progres !"); 
                                // FILE *save_file = fopen("./save.txt", "r"); 

                                // struct stat sb;
                                // stat("./save.txt", &sb); 
                                
                                // char save_info[sb.st_size]; 
                                // fread(save_info, 1, sb.st_size, save_file); 

                                // fclose(save_file); 
                                // /* now, the content of the file "./save.txt" is on save_info */

                                // printf("save_info : \n%s\n\n", save_info); 

                                // game_info all_game[10]; /* list of all game saved */ 
                                // game_info a_game; 
                                
                                // char line[2000]; 
                                // int line_number = 0; /* 0 to ... */
                                // int i = 0; 
                                // int y = 0; 

                                // while (save_info[i] != '\0') { 
                                //     while (save_info[i] != '\n') { /* copy all_text until we have '\n' = the next line */
                                //         printf("jsp N°%d : %c\n", i, line[y]); 
                                //         line[y] = save_info[i]; 
                                //         i++; 
                                //         y++; 
                                //     }
                                //     line[y] = '\0'; 
                                //     line_number++; 
                                //     printf("\nline %d : %s\n", line_number, line); 
                                    
                                //     i++; /* for past the '\0' */
                                //     strcpy(line, ""); /* reset the line */
                                //     y = 0; 

                                //     /* we have a line. now extract information of this line. */

                                //     // sscanf(line, "%[^|]|%[^|]|%d|%[^|]|%[^|]",); 
                                    
                                //     /* %[^|] = scan until we have a | */
                                //     //sscanf(line, "%[^|]|%[^|]|%d|%[^|]|%[^|]", &inventory, &name, &pv, &folder, &file); 
                                print_text_effect("\n\nChoisissez autre chose\n\n"); 
                                buffer_clear();
                                break;  

                            default: 
                                print_text_effect("\n\nErreur, essayer encore\n\n"); 
                                buffer_clear();
                                break; 
                        }
                    }
                    break; 
                
                case 2: /* English */ 
                    print_text_effect("New game : 1 \nLoad a game : 2\n");
                    while (1) {
                        scanf("%d", &choice); 
                        switch (choice) {
                            case 1: /* new game */
                                print_text_effect("Enter your name : "); 
                                scanf("%s", player.name);

                                player.pv = 100; 

                                player.langage = 2; 

                                strcpy(player.inventory, "shotgun;bullet"); 

                                strcpy(file_path, "./lobby-GB/intro.txt"); 
                                strcpy(folder_path, "./lobby-GB/"); 

                                read_file(file_path, folder_path, player);
                                return 0; 
                                break; 

                            case 2: /* load a game */
                                print_text_effect("work in progres !"); 
                                print_text_effect("\n\nChoisissez autre chose\n\n"); 
                                buffer_clear();
                                break;

                            default: 
                                print_text_effect("\n\nError, try again\n\n"); 
                                buffer_clear();
                                break; 
                        }
                    }
                    break; 
                
                default: 
                    print_text_effect("\n\nError, try again\n\n"); 
                    buffer_clear();
                    break; 
                }
    }
}