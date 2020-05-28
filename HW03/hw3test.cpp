#include <iostream>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <sstream>
#define CONCERTS_NUM 3
#define PROBLEM_2_CLIENTS_NUMBER 6
#define SEAT_CLASS_NUMBER 3
#define CLIENTS_NUM 30
#define BUFFER_LENGTH 4096*6
#define RECV_MODE 0   // 0 = recv all , 1 = recv per line
#define FILE_PATH "result/"
#define P2_FILE_PATH "p2_result/"
int onlyrun = -1;
int onlyrun2 = -1;
using namespace std;
int make_connect(char*server_ip,char*server_port)
{
	int sockfd;
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd == -1){
		perror("socket");
		exit(-1);
	}
	struct sockaddr_in info;
	bzero(&info,sizeof(info));
	info.sin_family = PF_INET;
	
	info.sin_addr.s_addr = inet_addr(server_ip);
	info.sin_port = htons(atoi(server_port));
	int err = connect(sockfd,(struct sockaddr*)&info,sizeof(info));
	if(err == -1)
	{
		perror("connect");
		exit(-1);
	}
	return sockfd;
}
string make_show(int order)
{
	string response;
	switch(order%( CONCERTS_NUM + 1) ){
		case 0:
			response = "show con1";
			return response;
		case 1:
			response = "show con2";
			return response;
		case 2:
			response = "show con3";
			return response;
		default:
			response = "show not_exist_concert_name";
			return response;
	}
}
int test_start(char* server_ip , char* server_port)
{
	int repeat_time;
	int i,pid;
	int sockfd;
	int filefd;
	pid_t wpid;
    time_t t1,t2;
	int status;
	for(repeat_time = 0 ; repeat_time < 4 ; repeat_time++){
		for(i = 0 ; i < CLIENTS_NUM ; i++)
		{
			pid = fork();
			if(pid == -1){
				perror("fork()");
				exit(-1);
			}
			else if (pid == 0){
				string filename = FILE_PATH;
				filename+= to_string(repeat_time*CLIENTS_NUM+i+1);
				filename+=".txt";
				filefd = open(filename.c_str(), O_WRONLY  | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
				if(filefd < 0){
					printf("open file %s failed\n",filename.c_str());
					exit(-1);
				}
				dup2(filefd,STDOUT_FILENO);
				dup2(filefd,STDERR_FILENO);
				close(filefd);
				sockfd = make_connect(server_ip,server_port);
				string message = make_show(repeat_time*4+i);
				cout<<message<<endl;
				char recvBuffer[BUFFER_LENGTH] = {};
				write(sockfd,message.c_str(),message.length());
				int nbytes;
				if(RECV_MODE == 0){
					if( (nbytes = recv(sockfd,recvBuffer,sizeof(recvBuffer),0)) <=0){
						if(nbytes ==0){
							cout<<"server leave\n";
						}
						else{
							perror("recv");
						}
						close(sockfd);
						exit(-1);
					}
					else{
						cout<<recvBuffer;
					}
				}
				else
				for(int j = 0 ; j < CONCERTS_NUM ; j++){
					if( (nbytes = recv(sockfd,recvBuffer,sizeof(recvBuffer),0)) <=0){
						if(nbytes ==0){
							cout<<"server leave\n";
						}
						else{
							perror("recv");
						}
						close(sockfd);
						exit(-1);
					}
					else{
						cout<<recvBuffer;
					}
				}
				message = "exit";
				write(sockfd,message.c_str(),message.length());
				nbytes = recv(sockfd,recvBuffer,sizeof(recvBuffer),0);
                if(nbytes < 0){
                    cout<<"error\n";
                }
				if(nbytes == 0){
					cout<<"server leave\n";
				}
				close(sockfd);
				close(filefd);
				exit(1);
			}
			else{
				while( (wpid = wait(&status)) >0);
			}
		}
	}
	return 1;
}
string make_message(int order)
{
	string response;
	switch(order%6){
		case 0:
			response = "con1/A/400";
			return response;
		case 1:
			response = "con1/A/400";
			return response;
		case 2:
			response = "con1/D/77 con1/E/166";
			return response;
		case 3:
			response = "con1/B/42";
			return response;
		case 4:
			response = "con1/C/120 con1/D/123";
			return response;
		case 5:
			response = "con1/B/10 con1/A/10";
			return response;
		default:
			return "LOL";

	}
}
int problem_2(char *server_ip,char*server_port)
{
	int j;
	int i;
	int pid;
	int sockfd;
	int filefd;
	pid_t wpid;
	int status;
    time_t t1,t2;
	for(i = 0 ; i < PROBLEM_2_CLIENTS_NUMBER ; i++)
	{
		pid = fork();
		if(pid == -1){
			perror("fork()");
			exit(-1);
		}
		else if (pid == 0){
			string filename = P2_FILE_PATH;
			filename+= to_string(i+1);
			filename+=".txt";
			filefd = open(filename.c_str(), O_WRONLY  | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
			if(filefd < 0){
				printf("open file %s failed\n",filename.c_str());
				exit(-1);
			}
			dup2(filefd,STDOUT_FILENO);
			dup2(filefd,STDERR_FILENO);
			close(filefd);
            if(onlyrun != -1 ){
                if(i != onlyrun && i != onlyrun2){
                    exit(1);
                }
            }
			sockfd = make_connect(server_ip,server_port);
			char recvBuffer[BUFFER_LENGTH] = {};
			memset(recvBuffer,0,sizeof(recvBuffer));
			int nbytes;
			string message;
            t1 = time(NULL);
			for(j = 0 ; j < 10000000 ; j++){
				message = make_message(i);
				//cout<<message<<endl;
				write(sockfd,message.c_str(),message.length());
                //sleep(1);
                usleep( 20000 );
				memset(recvBuffer,0,sizeof(recvBuffer));
				if(RECV_MODE == 0){
					if( (nbytes = recv(sockfd,recvBuffer,sizeof(recvBuffer),0)) <=0){
						if(nbytes ==0){
							cout<<"server leave\n";
						}
						else{
							perror("recv");
						}
						close(sockfd);
						exit(-1);
					}
					else{
						cout<<recvBuffer;
					}
				}
				else
				for(int j = 0 ; j < CONCERTS_NUM ; j++){
					if( (nbytes = recv(sockfd,recvBuffer,sizeof(recvBuffer),0)) <=0){
						if(nbytes ==0){
							cout<<"server leave\n";
						}
						else{
							perror("recv");
						}
						close(sockfd);
						exit(-1);
					}
					else{
						cout<<recvBuffer;
					}
				}
				stringstream ss(recvBuffer);
				string check;
				ss>>check;
				if(check == "sorry,"){
					break;
				}
			}
			message = "exit";
			cout<<message<<endl;
			memset(recvBuffer,0,sizeof(recvBuffer));
			write(sockfd,message.c_str(),message.length());
			nbytes = recv(sockfd,recvBuffer,sizeof(recvBuffer),0);
			if(nbytes == 0){
				cout<<"server leave\n";
			}
            t2 = time(NULL);
            cout<<"time= "<<t2-t1<<endl;
			close(sockfd);
			close(filefd);
			exit(1);
		}
		else{
			cout<<"client number : "<<i<<"start booking"<<endl;
		}
		
	}
	while( (wpid = wait(&status)) >0);
}
int main(int argc,char*argv[])
{
	if(argc < 3)
	{
		printf("./{program} {server_ip} {server_port}\n");
		exit(-1);
	}
    if(argc >= 4)
        onlyrun = atoi(argv[3]);
    if(argc >= 5)
        onlyrun2 = atoi(argv[4]);
	cout<<"==============START============"<<endl;
	test_start(argv[1],argv[2]);
	problem_2(argv[1],argv[2]);
	pid_t wpid;
	int status;
	while( (wpid = wait(&status)) >0);
	cout<<"===============END=============="<<endl;
	return 0;
}
