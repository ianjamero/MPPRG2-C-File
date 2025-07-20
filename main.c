#include <stdio.h>
#include <string.h>

#define MAX_USER 64
#define MAX_CHAR 12

FILE *fp;

typedef char String60[60];

struct user{
    String60 name[60];
    int age;
    float weight;
    float height;
};

void recordUser(String60 *stringsName, int *age, float *weight, float *height, int *users){

        printf("Please Input Your Name: ");
        scanf(" %[^\n]", stringsName[*users]);

        printf("\nPlease Input Your Age: ");
        scanf("%d", &age[*users]);

        printf("\nPlease Input Your Weight: ");
        scanf("%f", &weight[*users]);

        printf("\nPlease Input Your Height: ");
        scanf("%f", &height[*users]);
    
        (*users)++;

        
}

void init(String60 *stringsName, int *age, float *weight, float *height, int *users){
    int i;

    for (i = 0; i < MAX_USER; i++){
        strcpy(stringsName[i], "");
        age[i] = 0;
        weight[i] = 0;
        height[i] = 0;
    }
    *users = 0; 
}

int mainMenu(){
    int temp = 0;

    while (temp < 1 || temp > 4){
    printf("What are we going to do today?\n");
    printf("  1. Check Health Suggestions\n");
    printf("  2. Register a user\n");
    printf("  3. Check User BMI\n");
    printf("  4. Exit Program (All Current Users Will Be Deleted)\n");

        scanf("%d", &temp);
    }
    return temp;
}

void mainMenuOp(String60 *stringsName, int *age, float *weight, float *height, int *users){
    int choice;
    
    do {
        choice = mainMenu();

        switch (choice)
        {
            case 1:
                //Health Suggestions
                break;
            case 2:
                recordUser(stringsName, age, weight, height, users);
                break;
            case 3:
                //Check User BMI
                break;
            case 4: 
                printf("Exiting program. All current users will be deleted.\n");
                break;
            default: 
                printf("Please Input A Valid Option:");
                break;
        }
    } while (choice != 4);
}

int main()
{
    String60 strName[MAX_USER];
    int age[MAX_USER];
    float weight[MAX_USER];
    float height[MAX_USER];
    int users = 0; 

    init(strName, age, weight, height, &users);
    mainMenuOp(strName, age, weight, height, &users);

    printf("%s\n", strName[0]);
    printf("%d\n", age[0]);
    printf("%.2f\n", weight[0]);
    printf("%.2f\n", height[0]);
    printf("%d\n", users);

    return 0;
}