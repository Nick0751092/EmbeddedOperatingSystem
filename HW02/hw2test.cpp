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
#define BUFFER_LENGTH 1024
#define RECV_MODE 0   // 0 = recv all , 1 = recv per line
#define FILE_PATH "result/"
#define P2_FILE_PATH "p2_result/"
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
	int i,pid;
	int sockfd;
	int filefd;
	pid_t wpid;
	int status;
	for(i = 0 ; i < CLIENTS_NUM ; i++)
	{
		pid = fork();
		if(pid == -1){
			perror("fork()");
			exit(-1);
		}
		else if (pid == 0){
			string filename = FILE_PATH;
			filename+= to_string(i+1);
			filename+=".txt";
			filefd = open(filename.c_str(), O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
			if(filefd < 0){
				printf("open file %s failed\n",filename.c_str());
				exit(-1);
			}
			dup2(filefd,STDOUT_FILENO);
			dup2(filefd,STDERR_FILENO);
			close(filefd);
			sockfd = make_connect(server_ip,server_port);
			string message = "showall";
			char recvBuffer[BUFFER_LENGTH] = {};
			write(sockfd,message.c_str(),message.length());/*write to server*/
			int nbytes;
			if(RECV_MODE == 0){
				if( (nbytes = recv(sockfd,recvBuffer,sizeof(recvBuffer),0)/*recieve from server*/) <=0){
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
	int new_comer = i+10;
	for( ;i < new_comer ; i++)
	{
		pid = fork();
		if(pid == -1){
			perror("fork()");
			exit(-1);
		}
		else if (pid == 0){	
			//child process
			string filename = FILE_PATH;
			filename+= to_string(i+1);
			filename+=".txt";
			filefd = open(filename.c_str(), O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
			if(filefd < 0){
				printf("open file %s failed\n",filename.c_str());
				exit(-1);
			}
			dup2(filefd,STDOUT_FILENO);
			dup2(filefd,STDERR_FILENO);
			close(filefd);
			sockfd = make_connect(server_ip,server_port);
			string message = make_show(i);
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
			message = "exit"; //Command:exit
			write(sockfd,message.c_str(),message.length());
			nbytes = recv(sockfd,recvBuffer,sizeof(recvBuffer),0);
			if(nbytes == 0){
				cout<<"server leave\n";
			}
			close(sockfd);
			close(filefd);
			exit(1);
		}
		else{	
			//parent process
			/*
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
			string message = "showall";
			char recvBuffer[BUFFER_LENGTH] = {};
			//send(sockfd,string.c_str(),sizeof(message),0);
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
			stringstream ss(recvBuffer);
			string temp_string;
			for(int j = 0 ; j < CONCERTS_NUM ;j++){
				getline(ss,temp_string);
				stringstream sss(temp_string);
				
			}
			message = "exit";
			write(sockfd,message.c_str(),message.length());
			*/
			
			while( (wpid = wait(&status)) >0);
		}

	}
	return 1;
}
string make_message(int order)
{
	string response;
	switch(order%6){
		case 0:
			response = "con1/A/1 con1/A/1 con1/A/1 con1/A/1 con1/A/1 con1/A/1 con1/A/1 con1/A/1";
			return response;
		case 1:
			response = "con1/E/10 con2/E/10 con3/E/10";
			return response;
		case 2:
			response = "con3/A/3 con3/A/3 con3/A/5";
			return response;
		case 3:
			response = "con2/F/20";
			return response;
		case 4:
			response = "con2/D/31";
			return response;
		case 5:
			response = "OuO";
			return response;
		default:
			return "LOL";

	}
}
int problem_2(char *server_ip,char*server_port)
{
	int i;
	int pid;
	int sockfd;
	int filefd;
	pid_t wpid;
	int status;
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
			filefd = open(filename.c_str(), O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
			if(filefd < 0){
				printf("open file %s failed\n",filename.c_str());
				exit(-1);
			}
			dup2(filefd,STDOUT_FILENO);
			dup2(filefd,STDERR_FILENO);
			close(filefd);
			sockfd = make_connect(server_ip,server_port);
			string message = make_message(i);
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
int main(int argc,char*argv[])
{
	if(argc < 3)
	{
		printf("./{program} {server_ip} {server_port}\n");
		exit(-1);
	}
	cout<<"==============START============"<<endl;
	test_start(argv[1],argv[2]);
	problem_2(argv[1],argv[2]);
	pid_t wpid;
	int status;
	while( (wpid = wait(&status)) >0);
	cout<<"===============END=============="<<endl;
	return 0;
}
