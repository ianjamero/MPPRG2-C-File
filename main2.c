#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <windows.h>

#define FIXED_KEY "health_secret"
#define MAX_USERS 100
#define MAX_LOG_ENTRIES 100

typedef struct {
    char username[50];
    char password[50];
} UserCred;

typedef struct {
    char admin_pass[50];
    UserCred users[MAX_USERS];
    int user_count;
} SystemState;

typedef struct {
    int age;
    float weight, height;
    char gender[10];  // NEW: gender
    char water[20], vegetables[20], meat[20], fish[20], exercise[100];
} UserData;

typedef struct {
    char username[50];
    time_t timestamp;
} LogEntry;

SystemState state;
LogEntry logs[MAX_LOG_ENTRIES];
int log_count = 0;

void clrscr() {
    system("@cls||clear");
}

void xor_encrypt_decrypt(const char *input, char *output, size_t len, const char *key) {
    size_t key_len = strlen(key);
    for (size_t i = 0; i < len; i++) {
        output[i] = input[i] ^ key[i % key_len];
    }
}

int read_encrypted_file(const char *filename, char *output, size_t max_len, const char *key) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) return 0;
    size_t n = fread(output, 1, max_len - 1, fp);
    fclose(fp);
    xor_encrypt_decrypt(output, output, n, key);
    output[n] = '\0';
    return (int)n;
}

void write_encrypted_file(const char *filename, const char *data, const char *key) {
    char buffer[4096];
    size_t len = strlen(data);
    size_t keylen = strlen(key);
    for (size_t i = 0; i < len; i++) {
        buffer[i] = data[i] ^ key[i % keylen];
    }
    FILE *fp = fopen(filename, "wb");
    if (!fp) return;
    fwrite(buffer, 1, len, fp);
    fclose(fp);
}

int find_user(UserCred *users, char *username, int start, int end) {
    if (start >= end) return -1;
    if (strcmp(users[start].username, username) == 0) return start;
    return find_user(users, username, start + 1, end);
}

void save_admin_file() {
    char buffer[8192] = {0};
    sprintf(buffer, "admin_pass:%s\n", state.admin_pass);
    for (int i = 0; i < state.user_count; i++) {
        char line[120];
        sprintf(line, "%s:%s\n", state.users[i].username, state.users[i].password);
        strcat(buffer, line);
    }
    write_encrypted_file("admin.bin", buffer, FIXED_KEY);
}


void default_user_data(UserData *d) {
    d->age = 0;
    d->weight = 0.0f;
    d->height = 0.0f;
    strcpy(d->gender, "N/A");  
    strcpy(d->water, "N/A");
    strcpy(d->vegetables, "N/A");
    strcpy(d->meat, "N/A");
    strcpy(d->fish, "N/A");
    strcpy(d->exercise, "N/A");
}

void write_user_file(const char *username, const UserData *data) {
    char filename[128];
    sprintf(filename, "%s.bin", username);
    char buffer[2048];
    sprintf(buffer,
        "age:%d\nweight:%.2f\nheight:%.2f\ngender:%s\nwater:%s\nvegetables:%s\nmeat:%s\nfish:%s\nexercise:%s\n",
        data->age, data->weight, data->height, data->gender,
        data->water, data->vegetables, data->meat, data->fish, data->exercise);

    int idx = find_user(state.users, (char*)username, 0, state.user_count);
    if (idx >= 0) {
        write_encrypted_file(filename, buffer, state.users[idx].password);
    }
}

void read_user_file(const char *username, UserData *data) {
    char filename[128];
    sprintf(filename, "%s.bin", username);
    default_user_data(data);
    char buffer[2048] = {0};
    int idx = find_user(state.users, (char*)username, 0, state.user_count);
    if (idx < 0) return;
    if (!read_encrypted_file(filename, buffer, sizeof(buffer), state.users[idx].password)) return;

    int scanned = sscanf(buffer,
           "age:%d\nweight:%f\nheight:%f\ngender:%9s\nwater:%19s\nvegetables:%19s\nmeat:%19s\nfish:%19s\nexercise:%99[^\n]",
           &data->age, &data->weight, &data->height,
           data->gender, data->water, data->vegetables, data->meat, data->fish, data->exercise);

    if (scanned < 9) {
        strcpy(data->gender, "N/A");
        sscanf(buffer,
           "age:%d\nweight:%f\nheight:%f\nwater:%19s\nvegetables:%19s\nmeat:%19s\nfish:%19s\nexercise:%99[^\n]",
           &data->age, &data->weight, &data->height,
           data->water, data->vegetables, data->meat, data->fish, data->exercise);
    }
}

void view_records(const UserData *data) {
    printf("\n--- Health Records ---\n");
    printf("Age: %d\nWeight: %.2f kg\nHeight: %.2f m\nGender: %s\nWater: %s\nVegetables: %s\nMeat: %s\nFish: %s\nExercise: %s\n",
        data->age, data->weight, data->height, data->gender,
        data->water, data->vegetables, data->meat, data->fish, data->exercise);
}

void bmi_suggestions(const UserData *data) {
    if (data->height <= 0) {
        printf("\nBMI cannot be calculated. Height not set.\n");
        return;
    }
    float bmi = data->weight / (data->height * data->height);
    printf("\n--- BMI Analysis ---\n");
    printf("Your BMI: %.2f\n", bmi);
    if (bmi < 18.5f) printf("Category: Underweight.\n");
    else if (bmi < 25.0f) printf("Category: Normal.\n");
    else if (bmi < 30.0f) printf("Category: Overweight.\n");
    else printf("Category: Obese.\n");

    if (strcmp(data->gender, "male") == 0) {
        printf("Tip: Consider prioritizing lean protein and strength training.\n");
    } else if (strcmp(data->gender, "female") == 0) {
        printf("Tip: Balance cardio with strength training and monitor iron/calcium intake.\n");
    }
}

void health_suggestions(const UserData *data) {
    printf("\n--- Health Suggestions ---\n");
    if (strcmp(data->water, "1-6") == 0 || strcmp(data->water, "N/A") == 0)
        printf("- Drink at least 6-8 glasses of water.\n");
    if (strcmp(data->vegetables, "low") == 0 || strcmp(data->vegetables, "N/A") == 0)
        printf("- Increase vegetable intake.\n");
    if (strlen(data->exercise) < 3 || strcmp(data->exercise, "N/A") == 0)
        printf("- Add daily exercise.\n");
    bmi_suggestions(data);
}

void update_user_info(const char *username, UserData *data) {
    int newAge;
    float newW, newH;
    char newGender[10];

    printf("\n--- Update Personal Information ---\n");
    printf("Current age: %d\nEnter new age (-1 to keep): ", data->age);
    scanf("%d", &newAge);
    if (newAge >= 0) data->age = newAge;

    printf("Current weight: %.2f\nEnter new weight (-1 to keep): ", data->weight);
    scanf("%f", &newW);
    if (newW >= 0) data->weight = newW;

    printf("Current height: %.2f\nEnter new height (-1 to keep): ", data->height);
    scanf("%f", &newH);
    if (newH >= 0) data->height = newH;

    printf("Current gender: %s\nEnter new gender (male/female or '-' to keep): ", data->gender);
    scanf("%9s", newGender);
    if (strcmp(newGender, "-") != 0) strcpy(data->gender, newGender);

    write_user_file(username, data);
    printf("Personal info updated.\n");
}

void add_health_records(const char *username, UserData *data) {
    int choice;
    printf("\n--- Update/Add Daily Records ---\n");
    printf("Water intake:\n1. 1-6\n2. 6-8\n3. 8+\n> ");
    scanf("%d", &choice);
    if (choice == 1) strcpy(data->water, "1-6");
    else if (choice == 2) strcpy(data->water, "6-8");
    else if (choice == 3) strcpy(data->water, "8+");
    else strcpy(data->water, "N/A");
    printf("Vegetables intake (none/low/moderate/high): ");
    scanf("%19s", data->vegetables);
    printf("Meat intake (none/low/moderate/high): ");
    scanf("%19s", data->meat);
    printf("Fish intake (none/low/moderate/high): ");
    scanf("%19s", data->fish);
    while (getchar() != '\n');
    printf("Exercise habits: ");
    fgets(data->exercise, sizeof(data->exercise), stdin);
    size_t L = strlen(data->exercise);
    if (L > 0 && data->exercise[L - 1] == '\n') data->exercise[L - 1] = '\0';
    write_user_file(username, data);
    printf("Daily records updated.\n");
}

void change_admin_pass() {
    char pass[50];
    printf("Enter new admin password: ");
    scanf("%49s", pass);
    strcpy(state.admin_pass, pass);
    save_admin_file();
    printf("Admin password updated.\n");
}

void add_user_manual() {
    if (state.user_count >= MAX_USERS) return;
    char username[50], password[50];
    printf("Enter new username: ");
    scanf("%49s", username);
    if (find_user(state.users, username, 0, state.user_count) >= 0) return;
    printf("Enter password: ");
    scanf("%49s", password);
    strcpy(state.users[state.user_count].username, username);
    strcpy(state.users[state.user_count].password, password);
    state.user_count++;
    save_admin_file();
    UserData d;
    default_user_data(&d);

    printf("Enter age: ");
    scanf("%d", &d.age);
    printf("Enter weight (kg): ");
    scanf("%f", &d.weight);
    printf("Enter height (m): ");
    scanf("%f", &d.height);
    printf("Enter gender (male/female): ");
    scanf("%9s", d.gender);

    write_user_file(username, &d);
}

void delete_user() {
    char username[50];
    printf("Enter username to delete: ");
    scanf("%49s", username);
    int idx = find_user(state.users, username, 0, state.user_count);
    if (idx < 0) return;
    for (int i = idx; i < state.user_count - 1; i++) {
        state.users[i] = state.users[i + 1];
    }
    state.user_count--;
    save_admin_file();
    char filename[100];
    sprintf(filename, "%s.bin", username);
    remove(filename);
}

void reset_user_pass() {
    char username[50], newpass[50];
    printf("Enter username: ");
    scanf("%49s", username);
    int idx = find_user(state.users, username, 0, state.user_count);
    if (idx < 0) return;
    printf("Enter new password: ");
    scanf("%49s", newpass);
    strcpy(state.users[idx].password, newpass);
    save_admin_file();
}

void authenticate_admin() {
    char pass[50];
    printf("Enter admin password: ");
    scanf("%49s", pass);
    if (strcmp(pass, state.admin_pass) == 0) {
        char choice;
        while (1) {
            clrscr();  
            printf("\nAdmin Menu:\na) Change Admin Password\nb) Add User\nc) Delete User\nd) Reset User Password\ne) Exit\n> ");
            scanf(" %c", &choice);
            clrscr();
            if (choice == 'a') change_admin_pass();
            else if (choice == 'b') add_user_manual();
            else if (choice == 'c') delete_user();
            else if (choice == 'd') reset_user_pass();
            else if (choice == 'e') break;
        }
    } else {
        printf("Wrong admin password.\n");
        Sleep(10001);
    }
}

void exit_program() {
    char filename[50];
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(filename, sizeof(filename), "log_%Y-%m-%d.txt", tm_info);
    FILE *fp = fopen(filename, "w");
    if (fp) {
        for (int i = 0; i < log_count; i++) {
            char time_str[20];
            strftime(time_str, sizeof(time_str), "%H:%M:%S", localtime(&logs[i].timestamp));
            fprintf(fp, "%s: %s\n", logs[i].username, time_str);
        }
        fclose(fp);
    }
    printf("Log saved. Exiting...\n");
}

void user_menu(char *username) {
    UserData data;
    read_user_file(username, &data);
    char choice;
    while (1) {
        clrscr();
        printf("\nUser Menu:\n"
               "a) View Records\nb) Update Personal Information\nc) Add / Update Daily Records\n"
               "d) Health Tips\ne) BMI\nf) Exit\n> ");
        scanf(" %c", &choice);
        clrscr();
        if (choice == 'a') view_records(&data);
        else if (choice == 'b') update_user_info(username, &data);
        else if (choice == 'c') add_health_records(username, &data);
        else if (choice == 'd') health_suggestions(&data);
        else if (choice == 'e') bmi_suggestions(&data);
        else if (choice == 'f') { exit_program(); return; }
    }
}

void register_new_user(const char *username) {
    if (state.user_count >= MAX_USERS) {
        printf("Cannot register, user limit reached.\n");
        return;
    }
    char password[50];
    UserData d;
    default_user_data(&d);

    printf("No account found for '%s'. Creating new account.\n", username);
    printf("Set your password: ");
    scanf("%49s", password);
    strcpy(state.users[state.user_count].username, username);
    strcpy(state.users[state.user_count].password, password);
    state.user_count++;

    printf("Enter age: ");
    scanf("%d", &d.age);
    printf("Enter weight (kg): ");
    scanf("%f", &d.weight);
    printf("Enter height (m): ");
    scanf("%f", &d.height);
    printf("Enter gender (male/female): ");
    scanf("%9s", d.gender);

    save_admin_file();
    write_user_file(username, &d);
    printf("User '%s' registered successfully.\n", username);
}

void handle_user_login() {
    char username[50], password[50];
    printf("Enter username: ");
    scanf("%49s", username);
    int idx = find_user(state.users, username, 0, state.user_count);
    if (idx < 0) {
        register_new_user(username);
        idx = find_user(state.users, username, 0, state.user_count);
    }
    printf("Enter password: ");
    scanf("%49s", password);
    if (idx >= 0 && strcmp(password, state.users[idx].password) == 0) {
        if (log_count < MAX_LOG_ENTRIES) {
            strcpy(logs[log_count].username, username);
            logs[log_count].timestamp = time(NULL);
            log_count++;
        }
        user_menu(username);
    } else {
        printf("Wrong password.\n");
    }
}

void main_menu() {
    int choice;
    while (1) {
        clrscr();  
        printf("\nMain Menu:\n1. Admin\n2. User\n3. Exit\n> ");
        scanf("%d", &choice);
        if (choice == 1) {
            clrscr();
            authenticate_admin();
        }
        else if (choice == 2) {
            clrscr();
            handle_user_login();
        }
        else if (choice == 3) { 
            exit_program(); 
            return; 
        }
    }
}

void init() {
    state.user_count = 0;
    char buffer[8192];
    if (!read_encrypted_file("admin.bin", buffer, sizeof(buffer), FIXED_KEY)) {
        strcpy(state.admin_pass, "admin");
        save_admin_file();
        return;
    }
    sscanf(buffer, "admin_pass:%49s", state.admin_pass);
    char *ptr = strchr(buffer, '\n');
    if (!ptr) return;
    ptr++;
    while (*ptr && state.user_count < MAX_USERS) {
        char u[50], p[50];
        if (sscanf(ptr, "%49[^:]:%49s", u, p) == 2) {
            strcpy(state.users[state.user_count].username, u);
            strcpy(state.users[state.user_count].password, p);
            state.user_count++;
        }
        ptr = strchr(ptr, '\n');
        if (!ptr) break;
        ptr++;
    }
}

int main() {
    init();
    main_menu();
    return 0;
}
