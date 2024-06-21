#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <time.h>

// ���̺귯���� ��ũ�մϴ�.
#pragma comment(lib, "Ws2_32.lib")

void write_ip_and_time(FILE* file);
void read_file();
void input_to_file();
void log_file(FILE* iptime_file, time_t start_time, long start_size);
void end_program(FILE* iptime_file, time_t start_time, long start_size);
void read_iptime_file();
void reinitialize_file(const char* filename);
long get_file_size(const char* filename);
void calculate_time_difference(time_t start, time_t end, int* hours, int* minutes, int* seconds);
void menual();

int main() {
    char command[10];
    time_t start_time;
    FILE* iptime_file;
    char iptime_filename[] = "iptime.txt";
    long start_size;

    // ���α׷� ���� �� �ð��� ���
    time(&start_time);

    // ���� ũ�⸦ Ȯ��
    start_size = get_file_size("project.txt");

    // iptime.txt ���� ���� (������ ����)
    iptime_file = fopen(iptime_filename, "a+");
    if (iptime_file == NULL) {
        printf("Error opening file %s for appending!\n", iptime_filename);
        return 1;
    }

    // ���α׷� ���� �� IP�� ���� �ð��� ���
    write_ip_and_time(iptime_file);
    menual();

    while (1) {
        printf("Enter command (read, input, log, end, reiptime, reproject): ");
        scanf("%s", command);

        if (strcmp(command, "read") == 0) {
            read_file();
        }
        else if (strcmp(command, "input") == 0) {
            input_to_file();
        }
        else if (strcmp(command, "log") == 0) {
            log_file(iptime_file, start_time, start_size);
            read_iptime_file();
        }
        else if (strcmp(command, "end") == 0) {
            end_program(iptime_file, start_time, start_size);
            fclose(iptime_file);
            break;
        }
        else if (strcmp(command, "reiptime") == 0) {
            fclose(iptime_file);
            reinitialize_file(iptime_filename);
            iptime_file = fopen(iptime_filename, "a+");
            if (iptime_file == NULL) {
                printf("Error opening file %s for appending!\n", iptime_filename);
                return 1;
            }
            write_ip_and_time(iptime_file);
        }
        else if (strcmp(command, "reproject") == 0) {
            reinitialize_file("project.txt");
        }
        else if (strcmp(command, "joke") == 0) {
            printf("������ �ʹ� ���� ������ �ȵǴ� ������?\n");
            printf("�氡�� �ʴ� �� �� �־\n");
        }
        else {
            printf("Unknown command.\n");
        }
    }

    return 0;
}

void write_ip_and_time(FILE* file) {
    WSADATA wsaData;//������ ���� �ʱ�ȭ �����͸� �����ϴ� ����ü.
    char hostname[256];
    struct addrinfo hints, * result, * ptr;//�ּ� ���� �˻��� ����� ����ü �� ������.
    char ipstr[INET_ADDRSTRLEN];//IP �ּҸ� ���ڿ��� ������ ����.
    time_t t;
    struct tm* tm_info;
    char time_str[26];

    // ���� �ʱ�ȭ
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed with error: %d\n", WSAGetLastError());
        return;
    }

    // ȣ��Ʈ �̸��� ������
    if (gethostname(hostname, sizeof(hostname)) == SOCKET_ERROR) {
        printf("Error getting hostname: %d\n", WSAGetLastError());
        WSACleanup();
        return;
    }

    //�ʵ� ����ü ���� ����
    // addrinfo ����ü�� �ʱ�ȭ
    ZeroMemory(&hints, sizeof(hints));
    //����ü�� ��� �ʵ尡 0���� ����
    // ���߿� Ư�� �ʵ带 �������� �ʾƵ� �⺻��(0)���� ����
    // �̴� ����ġ ���� ������ �����մϴ�.
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM;
    //ai_socktype �ʵ带 SOCK_STREAM���� ����
    //getaddrinfo �Լ��� ��Ʈ�� ����(TCP ����)�� ��ȯ
    //��Ʈ�� ������ ���� �������̸�, �������� �������̰� �������� ����
    hints.ai_protocol = IPPROTO_TCP;
    //ai_protocol �ʵ带 IPPROTO_TCP�� ����
    //getaddrinfo �Լ��� TCP ���������� ����ϴ� ���ϸ� ��ȯ
    //IPPROTO_TCP�� TCP ���������� �ǹ�
    //ZeroMemory �Լ��� hints ����ü�� �ʱ�ȭ�ϰ�, IPv4 �ּҿ� TCP ������ ����ϵ��� �����մϴ�.

    // ȣ��Ʈ �̸����� addrinfo ����ü�� ������
    if (getaddrinfo(hostname, NULL, &hints, &result) != 0) {
        printf("getaddrinfo failed with error: %d\n", WSAGetLastError());
        WSACleanup();
        return;
    }

    // IP �ּҸ� ã��
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
        struct sockaddr_in* sockaddr_ipv4 = (struct sockaddr_in*)ptr->ai_addr;
        inet_ntop(AF_INET, &sockaddr_ipv4->sin_addr, ipstr, sizeof(ipstr));

        // ���� �ð� ��������
        time(&t);
        tm_info = localtime(&t);
        strftime(time_str, 26, "%Y-%m-%d %H:%M:%S", tm_info);

        // IP �ּҿ� ���� �ð��� ���Ͽ� ����
        fprintf(file, "IP Address: %s\n", ipstr);
        fprintf(file, "Start Time: %s\n", time_str);

        break; // ù ��° IP �ּҸ� ���
    }

    // addrinfo ����ü ����
    freeaddrinfo(result);
    // ���� ����
    WSACleanup();
}

void read_file() {
    FILE* file;
    char filename[] = "project.txt";
    char buffer[256];

    file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file %s!\n", filename);
        return;
    }

    printf("Contents of %s:\n", filename);
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        printf("%s", buffer);
    }
    fclose(file);
}

void input_to_file() {
    FILE* file;
    char filename[] = "project.txt";
    char input[256];

    printf("Enter text to write to the file: ");
    getchar();
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;

    file = fopen(filename, "a");
    if (file == NULL) {
        printf("Error opening file %s for appending!\n", filename);
        return;
    }

    fprintf(file, "%s\n", input);
    fclose(file);
    printf("Text written to %s\n", filename);
}//project.txt ���Ͽ� �Է��ϴ� �Լ�

void log_file(FILE* iptime_file, time_t start_time, long start_size) {
    time_t log_time;
    struct tm* tm_info;
    char time_str[26];
    long project_size;
    int hours, minutes, seconds;

    // ���� �ð� ��������
    time(&log_time);
    tm_info = localtime(&log_time);
    strftime(time_str, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    printf("Log Time: %s\n", time_str);

    // ������Ʈ ���� ũ�� Ȯ��
    project_size = get_file_size("project.txt");

    // �ð� ���� ���
    calculate_time_difference(start_time, log_time, &hours, &minutes, &seconds);

    // ���Ͽ� �α� �ð�, ��� �ð�, ���� ũ�� ���
    fprintf(iptime_file, "Log Time: %s\n", time_str);
    fprintf(iptime_file, "Time Used: %d hours, %d minutes, %d seconds\n", hours, minutes, seconds);
    fprintf(iptime_file, "Project File Size: %ld bytes\n", project_size);
    printf("Log written to iptime.txt\n");
}//iptime.txt�� �����ϴ� �Լ�

void end_program(FILE* iptime_file, time_t start_time, long start_size) {
    time_t end_time;
    struct tm* tm_info;
    char time_str[26];
    long project_size;
    int hours, minutes, seconds;

    // ���� �ð� ��������
    time(&end_time);
    tm_info = localtime(&end_time);
    strftime(time_str, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    printf("End Time: %s\n", time_str);

    // �ð� ���� ���
    calculate_time_difference(start_time, end_time, &hours, &minutes, &seconds);

    // ������Ʈ ���� ũ�� Ȯ��
    project_size = get_file_size("project.txt");

    // ���Ͽ� ���� �ð��� ����� �ð� ���
    fprintf(iptime_file, "Program End Time: %s\n", time_str);
    fprintf(iptime_file, "Total Time Used: %d hours, %d minutes, %d seconds\n", hours, minutes, seconds);
    fprintf(iptime_file, "Final Project File Size: %ld bytes\n", project_size);
    printf("Program End Time and Total Time Used written to iptime.txt\n");
}

void read_iptime_file() {
    FILE* file;
    char filename[] = "iptime.txt";
    char buffer[256];

    file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file %s!\n", filename);
        return;
    }

    printf("Contents of %s:\n", filename);
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        printf("%s", buffer);
    }
    fclose(file);
}//iptime.txt ������ ����ϴ� �Լ�

void reinitialize_file(const char* filename) {
    FILE* file = fopen(filename, "w");
    if (file != NULL) {
        fclose(file);
        printf("File %s has been reinitialized.\n", filename);
    }
    else {
        printf("Error reinitializing file %s!\n", filename);
    }
}//������ ������ �ȵ��� ��� ������ ���� �Լ� 

long get_file_size(const char* filename) {
    FILE* file = fopen(filename, "r");
    long size;

    if (file == NULL) {
        return 0;
    }

    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fclose(file);

    return size;
}//���� ũ�� Ȯ���� �ϴ� �Լ�

void calculate_time_difference(time_t start, time_t end, int* hours, int* minutes, int* seconds) {
    double time_diff = difftime(end, start);
    *hours = (int)(time_diff / 3600);
    *minutes = (int)((time_diff - (*hours * 3600)) / 60);
    *seconds = (int)(time_diff - (*hours * 3600) - (*minutes * 60));
}//��� �ð��� ����ϴ� �Լ�

void menual() {
    printf(" _______  __   __  _______    _______  ______   ___   _______  _______  ______ \n"  
        "|       ||  |_|  ||       |  |       ||      | |   | |       ||       ||    _ | \n"
        "|_     _||       ||_     _|  |    ___||  _    ||   | |_     _||   _   ||   | ||\n"
        "  |   |  |       |  |   |    |   |___ | | |   ||   |   |   |  |  | |  ||   |_||_\n"
        "  |   |   |     |   |   |    |    ___|| |_|   ||   |   |   |  |  |_|  ||    __  |\n"
        "  |   |  |   _   |  |   |    |   |___ |       ||   |   |   |  |       ||   |  | |\n"
        "  |___|  |__| |__|  |___|    |_______|| ______||___|   |___|  |_______||___|  |_| \n");

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
    printf("====================================================================================================================\n");
    printf("MENUAL:) *�� ���α׷��� ���۽� �ؽ�Ʈ�� ������ project.txt ���ϰ� ip�� �ð� �뷮�� ������ iptime.txt ������ ������ �˴ϴ�.\n");
    printf("read: project.txt ������ �н��ϴ�.\n");
    printf("input : project.txt ���Ͽ� ���ڸ� �Է��մϴ�.\n");
    printf("log : iptime.txt ������ �ҷ��ɴϴ�.\n");
    printf("end : ������ �����ϰ� ������ ����ð��� �뷮, ���ð��� iptime.txt ���Ͽ� �Է��մϴ�.\n");
    printf("reiptime : iptime.txt ������ �ʱ�ȭ �մϴ�.\n");
    printf("reproject : project.txt ������ �ʱ�ȭ �մϴ�.\n\n");
    printf("***log���� �д� ���***\n");
    printf("IP Address: ����� ������(ip)\n");
    printf("Start Time: ���α׷� ���۽ð�\n");
    printf("Program End Time: ���α׷� ���� �ð�\n");
    printf("Total Time Used: ���α׷� ��� �ð�\n");
    printf("Final File Size: ������ �뷮\n");
    printf("====================================================================================================================\n");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
}//�Ŵ����� ����ϴ� �Լ�