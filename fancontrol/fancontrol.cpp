#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <functional>
#include <vector>
#include <sys/wait.h>
#include <signal.h>
#include <string>
#include <iostream>
#include <cstring>
#include <sys/types.h>


using namespace std;


typedef function<int(vector<int>, vector<int>)> funcx;

int pid = 0;
int buf_size = 1000;
vector<int> fans;
vector<int> temps;
int count_f = 2;
int count_t = 2;


vector<string>nfans = {"/sys/class/hwmon/hwmon0/device/fan1_input",
						 "/sys/class/hwmon/hwmon0/device/fan2_input"}; 

vector<string>ntemps = {"/sys/class/hwmon/hwmon0/temp1_input",
						 "/sys/class/hwmon/hwmon0/temp2_input"};

void itoa(int value, char* buf)
{
    sprintf(buf, "%d", value);
}

int func1(vector<int> fans, std::vector<int> temps)
{
	int ans = 0;
	for (int i = 0; i < temps.size(); i++)
	{
		ans += temps[i];
	}
	ans /= temps.size();
	ans += 10;
	return ans;
}

int func2(vector<int> fans, vector<int> temps)
{
	int ans = 0;
	for (int i = 0; i < temps.size(); i++)
	{
		ans += temps[i];
	}
	ans /= temps.size();
	ans += 20;
	return ans;
}

const funcx funcs[] = {func1, func2};

void foo(int x)
{
    kill(pid, SIGINT);
}

int next(char* buf, int& start)
{
    int cur = start;
    int res;
    char* ans = (char*) malloc(buf_size);
    while (cur < strlen(buf))
    {
        if (buf[cur] == '\n')
        {
            start = cur + 1;
            cout << "andl";
            memcpy(ans, buf + start, cur - start); 
            res = atoi(ans);
            start = cur + 1;
            return res;
        }
        if (buf[cur] < '0' || buf[cur] > '9')
        {
            exit(2);
        }
        cur++;

    }
}

void read_temp(int id)
{
	char* buf = (char*) malloc(buf_size);
	int fd, cur = 0;       
	fd = open(ntemps[id - 1].c_str(), O_RDONLY, S_IRUSR | S_IWUSR);
	read(fd, buf, buf_size);
	temps[id - 1] = atoi(buf);
	close(fd);
	free(buf);

} 

void read_fan(int id)
{
	char* buf = (char*) malloc(buf_size);
	int fd, cur = 0;       
	fd = open(nfans[id - 1].c_str(), O_RDONLY, S_IRUSR | S_IWUSR);
	read(fd, buf, buf_size);
	fans[id - 1] = atoi(buf);
	close(fd);
	free(buf);

} 

void write_fan(int id, int res)
{
	fans[id - 1] = res; 
	int fd = open(nfans[id - 1].c_str(), O_WRONLY, S_IRUSR | S_IWUSR);
	char* buf = (char*) malloc(buf_size);		
	itoa(fans[id - 1], buf);
    write(fd, buf, buf_size);
	free(buf);
    close(fd);

} 

int main(int argc, char* argv[])
{
    pid = fork();
    if (pid != 0) //par
    {
        printf("daemon start. pid = %d\n", pid);
        signal(SIGINT, foo);
        int status;
        waitpid(pid, &status, 0);
        printf("daemon stop\n");
        return 0;
    }
    else
    {

        close(0);
        close(1);
        close(2);
        setsid();
        char* buf = (char*) malloc(buf_size); 

       

        while (true)
        {
			fans.resize(count_f);
        	temps.resize(count_t);
			for (int i = 0; i < count_t; i++)
			{
				read_temp(i + 1);
			}
			for (int i = 0; i < count_f; i++)
			{
				read_fan(i + 1);
			}
			for (int i = 0; i < count_f; i++)
			{
				write_fan(i + 1, funcs[i](fans, temps));
			}
			
			sleep(5);

        }

   
    
        
    return 0;
    }
}
