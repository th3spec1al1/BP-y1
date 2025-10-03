#include <cstdio>
#include <cstring>
#include <fstream>
#include <ctime>

const char* LongFunctions[10] = {"--output=", "--stats=", "--window=", "--from=", "--to=", "-o=", "-s=", "-w=", "-f=", "-t="};
const size_t log_size = 1000;
const size_t stats_size = 1000;

struct TArgs {  // аргументы консоли        
    const char* input_path = NULL;  // access.log
    const char* output_path = NULL; // result.txt
    bool print = false;
    int stats = 10; // stats
    bool checkStats = false; // флаг, чтобы учитывать ввод -s без значения (по умолчанию берется в этом случае)
    int window = 0; // window 
    time_t from_time = 0;
    time_t to_time = 1800000000;
};

struct LogArgs{ // аргументы логов
    char* remote_addr = NULL;
    time_t local_time = 0;
    char* request = NULL;
    int status = 0;
    char* bytes_send = NULL;
};

time_t TimeTranslation(const char* time){   // перевод в timestamp
    struct tm tm = {};
    int day,year,hour,minute,second,time_zone_hour,time_zone_minute;
    char month[4], time_zone_mark[2];
    sscanf(time, "%d/%3s/%d:%d:%d:%d %1s%2d%2d", &day, month, &year, &hour, &minute, &second, time_zone_mark, &time_zone_hour, &time_zone_minute);

    if (strcmp(month, "Jan") == 0){
        tm.tm_mon = 0;
    }
    if (strcmp(month, "Feb") == 0){
        tm.tm_mon = 1;
    }
    if (strcmp(month, "Mar") == 0){
        tm.tm_mon = 2;
    }
    if (strcmp(month, "Apr") == 0){
        tm.tm_mon = 3;
    }
    if (strcmp(month, "May") == 0){
        tm.tm_mon = 4;
    }
    if (strcmp(month, "Jun") == 0){
        tm.tm_mon = 5;
    }
    if (strcmp(month, "Jul") == 0){
        tm.tm_mon = 6;
    }
    if (strcmp(month, "Aug") == 0){
        tm.tm_mon = 7;
    }
    if (strcmp(month, "Sep") == 0){
        tm.tm_mon = 8;
    }
    if (strcmp(month, "Oct") == 0){
        tm.tm_mon = 9;
    }
    if (strcmp(month, "Nov") == 0){
        tm.tm_mon = 10;
    }
    if (strcmp(month, "Dec") == 0){
        tm.tm_mon = 11;
    }

    tm.tm_mday = day;
    tm.tm_year = year - 1900;
    if (*time_zone_mark == '+'){
        tm.tm_hour = hour-time_zone_hour+3;
        tm.tm_min = minute-time_zone_minute;
    }
    else{
        tm.tm_hour = hour+time_zone_hour+3;
        tm.tm_min = minute+time_zone_minute;
    }
    tm.tm_sec = second;

    time_t new_time = mktime(&tm);
    return new_time;
};

bool ReadLogsArgs(LogArgs* argl, char* log){    // парсинг логов
    char adr[100], data[100], req[1000], status[10], bytes_send[1000];
    std::string Log = log;
    int i = 0, j = 0;
    if (!(Log.find(' ') != Log.npos & Log.find(']') != Log.npos & Log.find('"') != Log.npos)){
        printf("Log Error - %s\n", log);
        return false;
    }

    while (log[i] != ' '){      // adress
        adr[i] = log[i];
        i++;
    }
    adr[i+1] = '\0';

    i += 6;
    while (log[i] != ']'){      // data
        data[j] = log[i];
        j++;
        i++;
    }
    data[j+1] = '\0';

    i += 3;
    j = log_size;
    while (log[j] != '"'){  
        j--;
    }

    int h=0;
    while (i<j){        // request
        req[h]=log[i];
        i++;
        h++;
    }
    req[h] = '\0';

    j += 2;
    i = 0;
    while (log[j] != ' '){      // status
        status[i] = log[j];
        i++;
        j++;
    }
    j++;
    status[i+1]='\0';

    i = 0;
    while (log[j]){         // bytes_send
        bytes_send[i] = log[j];
        j++;
        i++;
    }
    bytes_send[i+1]='\0';

    argl->remote_addr = adr;
    argl->local_time = TimeTranslation(data);
    argl->request = req;
    argl->status = atoi(status);
    argl->bytes_send = bytes_send;
    return true;
};

void QuickSort(char a[], int b[], int l, int r){
    int i = l;
    int j = r;
    int mid = b[(l + r) / 2];

    while (i <= j) {
        while (b[i] > mid) {
            i++;
        }
        while (b[j] < mid) {
            j--;
        }
        if (i <= j) {
            std::swap(a[i], a[j]);
            std::swap(b[i], b[j]);
            
            i++;
            j--;
        }
    }
    if (j > l) {
        QuickSort(a, b, l, j);
    }
    if (r > i) {
        QuickSort(a, b, i, r);
    }
};

bool WorkWithErrors5XX(const char* input_path, const char* output_path, bool print, bool CheckStats, int stats, time_t from_time, time_t to_time){  // функции, которые работают с ошибочными логами
    std::ifstream input_file(input_path);
    if (!input_file){   // проверка что файл с логами открыт
        printf("Error: Cannot open file: %s\n", input_path);
        return 1;
    }
    
    if (output_path != NULL){   // проверка что файл с результатом был задан
        std::ofstream output_file(output_path);
        if (!output_file) { // проверка что файл с результатами открыт
            input_file.close();
            printf("Error: Cannot open file: %s\n", output_path);
            return 1;
        };

        char log[log_size];
        char requests[stats_size][log_size];

        int count[stats_size];
        int uniqueRequests = 0;

        while (input_file.getline(log, log_size)) {  // читаю файл построчно
            char log2[log_size];
            strncpy(log2, log, sizeof(log2));
            
            LogArgs argl;
            ReadLogsArgs(&argl, log2);
            // if (!ReadLogsArgs(&argl, log2)){
            //     input_file.close();
            //     return false;
            // }

            if (argl.status > 499 && argl.status < 600){
                if (from_time <= argl.local_time & argl.local_time <= to_time ){
                    output_file << log << std::endl;
                    
                    if (print == true){     // проверка на функцию --print
                        printf("%s\n", log);
                    }

                    if (CheckStats == true){    // проверка на функцию stats
                        bool found = false;
                        for (int i=0; i<uniqueRequests; i++){
                            if (strcmp(requests[i], argl.request)==0){
                                found = true;
                                count[i]++;
                                break;
                            } 
                        }
                        
                        if (!found){
                            strncpy(requests[uniqueRequests], argl.request, log_size);
                            count[uniqueRequests] = 1;
                            uniqueRequests++;
                        }
                    }
                }
            }
        }     

        QuickSort(requests[stats_size], count, 0, uniqueRequests-1);

        for (int i=0; i<stats; i++){
            printf("request: \"%s\"     count: %d\n", requests[i], count[i]);
        }

        input_file.close();
        output_file.close();
        return 0;
    }
    
    else{   // если файл не указан, анализ запросов с ошибками не выполняется (см.условие)
        if(CheckStats == true || print == true){
            printf("Error: output_file missing");
            input_file.close();
        }
        return 1;
    }
};

bool Window(const char* input_path, int window, time_t from_time, time_t to_time){ // --window

    std::ifstream input_file(input_path);
    if (!input_file){   // проверка что файл с логами открыт
        printf("Error: Cannot open file: %s\n", input_path);
        return 1;
    }

    time_t first_request_time = from_time;
    time_t last_request_time = from_time;
    int* time = new int[1800000000];
    char log[log_size];
    
    while (input_file.getline(log, log_size)) {  // читаю файл построчно
        char log2[log_size];
        strncpy(log2, log, sizeof(log2));
        
        LogArgs argl;
        ReadLogsArgs(&argl, log2);
        // if (!ReadLogsArgs(&argl, log2)){
        //     input_file.close();
        //     return false;
        // }

        if (from_time <= argl.local_time & argl.local_time <= to_time){
            if (first_request_time == 0){
                first_request_time = argl.local_time;   // самый первый лог по времени
            }   
            last_request_time = argl.local_time;    // самый послений лог по времени
            
            time[argl.local_time]++;
        }
    }

    int currentSum = 0; // текущее кол-во логов 
    int maxSum = 0; // максимальное кол-во логов
    int start = first_request_time; // начало отрезка, при котором currentSum = maxSum
    int end = first_request_time+window-1;  // конец отрезка, при котором currentSum = maxSum

    for (int i=first_request_time; i<first_request_time+window; ++i){   // первая currentSum - сумма на отрезке от 0 до window
        currentSum = currentSum + time[i];
    }

    maxSum = currentSum;    // максимальная сумма изначально равна самой первой currecntSum
    
    for (int i=first_request_time+1; i<last_request_time-window+1; ++i){
        currentSum = currentSum - time[i-1] + time[i+window-1];

        if (currentSum > maxSum){
            maxSum = currentSum;
            start = i;
            end = i+window-1;
        }
    }

    printf("Max count of requests - %d     Window - %d:%d", maxSum, start, end);
    input_file.close();
    delete []time;
    return 0;

};

bool ReadArgs(TArgs* args, int argc, char** argv){  // чтение аргументов
    bool LastArgumentIsFunction;
    for (int i = 1; i<argc; i++){
        
        if (LastArgumentIsFunction == true){    // проверка, что прошлый аргумент - функция
            LastArgumentIsFunction = false;
            continue;
        }
        
        if (strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--from") == 0){        // -f n
            if (atoi(argv[i+1]) != 0){
                args->from_time = atoi(argv[i+1]);
                LastArgumentIsFunction = true;
            }
            continue;
        }

        if (strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--to") == 0){        // -t n
            if (atoi(argv[i+1]) != 0){
                args->to_time = atoi(argv[i+1]);
                LastArgumentIsFunction = true;
            }
            continue;
        }

        if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0){        // -o path
            if (i < argc - 1){
                args->output_path = argv[i+1];
                LastArgumentIsFunction = true;
                continue;
            }
            printf("You used the '-o path' command and didn't write path\n");
            return false;
        }

        if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--print") == 0){     // -p and --print
            args->print = true;
            continue;
        }

        if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--stats") == 0){        // -s n
            args->checkStats = true;
            if (atoi(argv[i+1]) != 0){
                args->stats = atoi(argv[i+1]);
                LastArgumentIsFunction = true;
            }
            continue;
        }

        if (strcmp(argv[i], "-w") == 0 || strcmp(argv[i], "--window") == 0){        // -w n
            if (atoi(argv[i+1]) == 0 || i >= argc - 1){
                printf("You used the '-w t' command and didn't write t or t isn't number\n");
                return false;
            }
            args->window = atoi(argv[i+1]);
            LastArgumentIsFunction = true;
            continue;
        }
        int k=0;    // счетчик
        for (int j=0; j<10; j++){        // long functions type: --[function]=n
            if (strstr(argv[i], LongFunctions[j]) != NULL){
                char* p = strtok(argv[i], "=");
                p = strtok(NULL, "=");

                if (LongFunctions[j] == "--output="){       // --output=n
                    args->output_path=p;
                    break;
                }

                if (LongFunctions[j] == "-o="){       // -o=path
                    args->output_path=p;
                    break;
                }

                if (LongFunctions[j] == "--stats="){        // --stats=n
                    if (atoi(p) == 0){
                        printf("You used the '--stats=n' command and didn't write n or n isn't number\n");
                        return false;
                    }
                    args->stats = atoi(p);
                    args->checkStats = true;
                    break;
                }

                if (LongFunctions[j] == "-s="){        // -s=n
                    if (atoi(p) == 0){
                        printf("You used the '-s=n' command and didn't write n or n isn't number\n");
                        return false;
                    }
                    args->stats = atoi(p);
                    args->checkStats = true;
                    break;
                }

                if (LongFunctions[j] == "--window="){       // --window=n
                    if (atoi(p) == 0){
                        printf("You used the '--window=t' command and didn't write t or t isn't number\n");
                        return false;
                    }
                    args->window = atoi(p);
                    break;
                }

                if (LongFunctions[j] == "-w="){       // -w=n
                    if (atoi(p) == 0){
                        printf("You used the '-w=t' command and didn't write t or t isn't number\n");
                        return false;
                    }
                    args->window = atoi(p);
                    break;
                }

                if (LongFunctions[j] == "--from="){         // --from=time
                    if (atoi(p) == 0){
                        printf("You used the '--from=time' command and didn't write time or time isn't number\n");
                        return false;
                    }
                    args->from_time = atoi(p);
                    break;
                }

                if (LongFunctions[j] == "-f="){         // -f=time
                    if (atoi(p) == 0){
                        printf("You used the '--from=time' command and didn't write time or time isn't number\n");
                        return false;
                    }
                    args->from_time = atoi(p);
                    break;
                }

                if (LongFunctions[j] == "--to="){           // --to=time
                    if (atoi(p) == 0){
                        printf("You used the '--to=time' command and didn't write time or time isn't number\n");
                        return false;
                    }
                    args->to_time = atoi(p);
                    break;
                }
                
                if (LongFunctions[j] == "-t="){           // --to=time
                    if (atoi(p) == 0){
                        printf("You used the '-t=time' command and didn't write time or time isn't number\n");
                        return false;
                    }
                    args->to_time = atoi(p);
                    break;
                }
            }
            else{
                k++;
            }
        }
        
        if (k==10){  // если это не аргумент и не значение, то это либо путь к файлу, либо несуществующая функция
            if (args->input_path != NULL){
                printf("You used unknown function %s or %s\n", argv[i], args->input_path);
                return false;
            }
            args->input_path=argv[i];
        }
    }
    
    return true;
};    

int main(int argc, char** argv) {

    if (argc < 3){
        printf("Error: Few arguments\n");
        return 1;
    }
    
    TArgs args;
    if (!ReadArgs(&args, argc, argv)){
        return 1;
    }
    
    if (args.input_path == NULL){
        printf("Error: Log file not entered");
        return 1;
    }

    WorkWithErrors5XX(args.input_path, args.output_path, args.print, args.checkStats, args.stats, args.from_time, args.to_time);

    if (args.window > 0){
        Window(args.input_path, args.window, args.from_time, args.to_time);
    }

    return 0;
}

