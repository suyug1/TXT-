#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <time.h>

// 라이브러리를 링크합니다.
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

    // 프로그램 시작 시 시간을 기록
    time(&start_time);

    // 파일 크기를 확인
    start_size = get_file_size("project.txt");

    // iptime.txt 파일 열기 (없으면 생성)
    iptime_file = fopen(iptime_filename, "a+");
    if (iptime_file == NULL) {
        printf("Error opening file %s for appending!\n", iptime_filename);
        return 1;
    }

    // 프로그램 시작 시 IP와 시작 시간을 기록
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
            printf("과당을 너무 많이 먹으면 안되는 이유는?\n");
            printf("길가다 꽈당 할 수 있어서\n");
        }
        else {
            printf("Unknown command.\n");
        }
    }

    return 0;
}

void write_ip_and_time(FILE* file) {
    WSADATA wsaData;//윈도우 소켓 초기화 데이터를 저장하는 구조체.
    char hostname[256];
    struct addrinfo hints, * result, * ptr;//주소 정보 검색에 사용할 구조체 및 포인터.
    char ipstr[INET_ADDRSTRLEN];//IP 주소를 문자열로 저장할 버퍼.
    time_t t;
    struct tm* tm_info;
    char time_str[26];

    // 윈속 초기화
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed with error: %d\n", WSAGetLastError());
        return;
    }

    // 호스트 이름을 가져옴
    if (gethostname(hostname, sizeof(hostname)) == SOCKET_ERROR) {
        printf("Error getting hostname: %d\n", WSAGetLastError());
        WSACleanup();
        return;
    }

    //필드 구조체 안의 구조
    // addrinfo 구조체를 초기화
    ZeroMemory(&hints, sizeof(hints));
    //구조체의 모든 필드가 0으로 설정
    // 나중에 특정 필드를 설정하지 않아도 기본값(0)으로 설정
    // 이는 예상치 못한 동작을 방지합니다.
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM;
    //ai_socktype 필드를 SOCK_STREAM으로 설정
    //getaddrinfo 함수는 스트림 소켓(TCP 소켓)을 반환
    //스트림 소켓은 연결 지향적이며, 데이터의 안정적이고 순차적인 전송
    hints.ai_protocol = IPPROTO_TCP;
    //ai_protocol 필드를 IPPROTO_TCP로 설정
    //getaddrinfo 함수는 TCP 프로토콜을 사용하는 소켓만 반환
    //IPPROTO_TCP는 TCP 프로토콜을 의미
    //ZeroMemory 함수로 hints 구조체를 초기화하고, IPv4 주소와 TCP 소켓을 사용하도록 설정합니다.

    // 호스트 이름으로 addrinfo 구조체를 가져옴
    if (getaddrinfo(hostname, NULL, &hints, &result) != 0) {
        printf("getaddrinfo failed with error: %d\n", WSAGetLastError());
        WSACleanup();
        return;
    }

    // IP 주소를 찾음
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
        struct sockaddr_in* sockaddr_ipv4 = (struct sockaddr_in*)ptr->ai_addr;
        inet_ntop(AF_INET, &sockaddr_ipv4->sin_addr, ipstr, sizeof(ipstr));

        // 현재 시간 가져오기
        time(&t);
        tm_info = localtime(&t);
        strftime(time_str, 26, "%Y-%m-%d %H:%M:%S", tm_info);

        // IP 주소와 시작 시간을 파일에 저장
        fprintf(file, "IP Address: %s\n", ipstr);
        fprintf(file, "Start Time: %s\n", time_str);

        break; // 첫 번째 IP 주소만 사용
    }

    // addrinfo 구조체 해제
    freeaddrinfo(result);
    // 윈속 정리
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
}//project.txt 파일에 입력하는 함수

void log_file(FILE* iptime_file, time_t start_time, long start_size) {
    time_t log_time;
    struct tm* tm_info;
    char time_str[26];
    long project_size;
    int hours, minutes, seconds;

    // 현재 시간 가져오기
    time(&log_time);
    tm_info = localtime(&log_time);
    strftime(time_str, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    printf("Log Time: %s\n", time_str);

    // 프로젝트 파일 크기 확인
    project_size = get_file_size("project.txt");

    // 시간 차이 계산
    calculate_time_difference(start_time, log_time, &hours, &minutes, &seconds);

    // 파일에 로그 시간, 사용 시간, 파일 크기 기록
    fprintf(iptime_file, "Log Time: %s\n", time_str);
    fprintf(iptime_file, "Time Used: %d hours, %d minutes, %d seconds\n", hours, minutes, seconds);
    fprintf(iptime_file, "Project File Size: %ld bytes\n", project_size);
    printf("Log written to iptime.txt\n");
}//iptime.txt를 구성하는 함수

void end_program(FILE* iptime_file, time_t start_time, long start_size) {
    time_t end_time;
    struct tm* tm_info;
    char time_str[26];
    long project_size;
    int hours, minutes, seconds;

    // 현재 시간 가져오기
    time(&end_time);
    tm_info = localtime(&end_time);
    strftime(time_str, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    printf("End Time: %s\n", time_str);

    // 시간 차이 계산
    calculate_time_difference(start_time, end_time, &hours, &minutes, &seconds);

    // 프로젝트 파일 크기 확인
    project_size = get_file_size("project.txt");

    // 파일에 종료 시간과 사용한 시간 기록
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
}//iptime.txt 파일을 출력하는 함수

void reinitialize_file(const char* filename) {
    FILE* file = fopen(filename, "w");
    if (file != NULL) {
        fclose(file);
        printf("File %s has been reinitialized.\n", filename);
    }
    else {
        printf("Error reinitializing file %s!\n", filename);
    }
}//파일이 생성이 안됐을 경우 오류를 막는 함수 

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
}//파일 크기 확인을 하는 함수

void calculate_time_difference(time_t start, time_t end, int* hours, int* minutes, int* seconds) {
    double time_diff = difftime(end, start);
    *hours = (int)(time_diff / 3600);
    *minutes = (int)((time_diff - (*hours * 3600)) / 60);
    *seconds = (int)(time_diff - (*hours * 3600) - (*minutes * 60));
}//사용 시간을 계산하는 함수

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
    printf("MENUAL:) *이 프로그램은 시작시 텍스트를 편집할 project.txt 파일과 ip와 시간 용량을 측정할 iptime.txt 파일이 생성이 됩니다.\n");
    printf("read: project.txt 파일을 읽습니다.\n");
    printf("input : project.txt 파일에 글자를 입력합니다.\n");
    printf("log : iptime.txt 파일을 불러옵니다.\n");
    printf("end : 파일을 종료하고 마지막 종료시간과 용량, 사용시간을 iptime.txt 파일에 입력합니다.\n");
    printf("reiptime : iptime.txt 파일을 초기화 합니다.\n");
    printf("reproject : project.txt 파일을 초기화 합니다.\n\n");
    printf("***log파일 읽는 방법***\n");
    printf("IP Address: 사용자 아이피(ip)\n");
    printf("Start Time: 프로그램 시작시간\n");
    printf("Program End Time: 프로그램 종료 시간\n");
    printf("Total Time Used: 프로그램 사용 시간\n");
    printf("Final File Size: 파일의 용량\n");
    printf("====================================================================================================================\n");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
}//매뉴얼을 출력하는 함수






//#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <time.h>

// 라이브러리를 링크합니다.
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

    // 프로그램 시작 시 시간을 기록
    time(&start_time);

    // 파일 크기를 확인
    start_size = get_file_size("project.txt");

    // iptime.txt 파일 열기 (없으면 생성)
    iptime_file = fopen(iptime_filename, "a+");
    if (iptime_file == NULL) {
        printf("Error opening file %s for appending!\n", iptime_filename);
        return 1;
    }

    // 프로그램 시작 시 IP와 시작 시간을 기록
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
            printf("과당을 너무 많이 먹으면 안되는 이유는?\n");
            printf("길가다 꽈당 할 수 있어서\n");
        }
        else {
            printf("Unknown command.\n");
        }
    }

    return 0;
}

void write_ip_and_time(FILE* file) {
    WSADATA wsaData;
    char hostname[256];
    struct addrinfo hints, * result, * ptr;
    char ipstr[INET_ADDRSTRLEN];
    char username[256];
    time_t t;
    struct tm* tm_info;
    char time_str[26];
    FILE* user_file;

    // 윈속 초기화
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed with error: %d\n", WSAGetLastError());
        return;
    }

    // 호스트 이름을 가져옴
    if (gethostname(hostname, sizeof(hostname)) == SOCKET_ERROR) {
        printf("Error getting hostname: %d\n", WSAGetLastError());
        WSACleanup();
        return;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    if (getaddrinfo(hostname, NULL, &hints, &result) != 0) {
        printf("getaddrinfo failed with error: %d\n", WSAGetLastError());
        WSACleanup();
        return;
    }

    // IP 주소를 찾음
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
        struct sockaddr_in* sockaddr_ipv4 = (struct sockaddr_in*)ptr->ai_addr;
        inet_ntop(AF_INET, &sockaddr_ipv4->sin_addr, ipstr, sizeof(ipstr));

        // 사용자명 입력
        printf("Enter your username: ");
        scanf("%s", username);

        // 현재 시간 가져오기
        time(&t);
        tm_info = localtime(&t);
        strftime(time_str, 26, "%Y-%m-%d %H:%M:%S", tm_info);

        // 사용자명과 시작 시간을 파일에 저장
        fprintf(file, "Username: %s\n", username);
        fprintf(file, "Start Time: %s\n", time_str);

        // user.txt 파일에 IP와 사용자명 저장
        user_file = fopen("user.txt", "a");
        if (user_file != NULL) {
            fprintf(user_file, "IP Address: %s, Username: %s\n", ipstr, username);
            fclose(user_file);
        }

        break;
    }

    freeaddrinfo(result);
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
}

void log_file(FILE* iptime_file, time_t start_time, long start_size) {
    time_t log_time;
    struct tm* tm_info;
    char time_str[26];
    long project_size;
    int hours, minutes, seconds;

    time(&log_time);
    tm_info = localtime(&log_time);
    strftime(time_str, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    printf("Log Time: %s\n", time_str);

    project_size = get_file_size("project.txt");

    calculate_time_difference(start_time, log_time, &hours, &minutes, &seconds);

    fprintf(iptime_file, "Log Time: %s\n", time_str);
    fprintf(iptime_file, "Time Used: %d hours, %d minutes, %d seconds\n", hours, minutes, seconds);
    fprintf(iptime_file, "Project File Size: %ld bytes\n", project_size);
    printf("Log written to iptime.txt\n");
}

void end_program(FILE* iptime_file, time_t start_time, long start_size) {
    time_t end_time;
    struct tm* tm_info;
    char time_str[26];
    long project_size;
    int hours, minutes, seconds;

    time(&end_time);
    tm_info = localtime(&end_time);
    strftime(time_str, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    printf("End Time: %s\n", time_str);

    calculate_time_difference(start_time, end_time, &hours, &minutes, &seconds);

    project_size = get_file_size("project.txt");

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
}

void reinitialize_file(const char* filename) {
    FILE* file = fopen(filename, "w");
    if (file != NULL) {
        fclose(file);
        printf("File %s has been reinitialized.\n", filename);
    }
    else {
        printf("Error reinitializing file %s!\n", filename);
    }
}

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
}

void calculate_time_difference(time_t start, time_t end, int* hours, int* minutes, int* seconds) {
    double time_diff = difftime(end, start);
    *hours = (int)(time_diff / 3600);
    *minutes = (int)((time_diff - (*hours * 3600)) / 60);
    *seconds = (int)(time_diff - (*hours * 3600) - (*minutes * 60));
}

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
    printf("MENUAL:) *이 프로그램은 시작시 텍스트를 편집할 project.txt 파일과 ip와 시간 용량을 측정할 iptime.txt 파일이 생성이 됩니다.\n");
    printf("read: project.txt 파일을 읽습니다.\n");
    printf("input : project.txt 파일에 글자를 입력합니다.\n");
    printf("log : iptime.txt 파일을 불러옵니다.\n");
    printf("end : 파일을 종료하고 마지막 종료시간과 용량, 사용시간을 iptime.txt 파일에 입력합니다.\n");
    printf("reiptime : iptime.txt 파일을 초기화 합니다.\n");
    printf("reproject : project.txt 파일을 초기화 합니다.\n\n");
    printf("***log파일 읽는 방법***\n");
    printf("Username: 사용자명\n");
    printf("Start Time: 프로그램 시작시간\n");
    printf("Program End Time: 프로그램 종료 시간\n");
    printf("Total Time Used: 프로그램 사용 시간\n");
    printf("Final File Size: 파일의 용량\n");
    printf("====================================================================================================================\n");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
}

