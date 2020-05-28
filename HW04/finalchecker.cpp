#include <iostream>
#include <fstream>
#include <string>
#include <map>
#define FILE_PATH "result/"
#define P2_FILE_PATH "p2_result/"
#define PROBLEM_2_CLIENTS_NUMBER 8
using namespace std;
int main()
{
    //  map<int,int> book number , is show or not1  - 1.
    //  map<string,map<int,int>>   kind ticket , to  1.  - 2.
    //  map<string,map<string,map<int,int>>> concert name , to 2. 
    
    map<string,map<string,map<int,int>>> tester;
    map<string,int> ticket_max_number;  // [concertname_ticketName] => max int
    fstream file;
    string concertname,ticketname,ticketnumber,subticketnumber;
    int ticket_time;
    int i;
	bool flag = false;
    bool not_order=false;
    for(i = 0 ; i < PROBLEM_2_CLIENTS_NUMBER ; i++)
    {
		string filename = P2_FILE_PATH;
		filename+= to_string(i+1);
		filename+=".txt";
        file.open(filename.c_str());
        if(!file){
            cerr<<"filename = "<<filename<<" not exist\n";
            exit(-1);
        }
        while(file>>concertname){
            if(concertname == "sorry," || concertname == "exit" || concertname == "input"){
                break;
            }
            file>>ticketname>>ticketnumber>>ticket_time;
			if(ticketname == "not"){
				break;
			}
            if(ticketname == "does"){
                cout<<"format invalid\n"<<endl;
                break;
            }
            subticketnumber = ticketnumber.substr(ticketname.length(),ticketnumber.length());
            if(tester[concertname][ticketname][ atoi(subticketnumber.c_str()) ]){
                cout<<"at file = "<<filename<<" concert name = "<<concertname<<" , ticket class = "<<ticketname<<" , ticket number = "<<subticketnumber<<", is alread exist"<<endl;
                flag = true;
                //break;
            }
            else{
                tester[concertname][ticketname][ atoi(subticketnumber.c_str()) ] = 1;
                string index = concertname+"_"+ticketname;
                if(!ticket_max_number[index]){
                    ticket_max_number[index] = atoi(subticketnumber.c_str());
                    //cout<<index<<" init"<<endl;
                }
                else if( ticket_max_number[index] < atoi(subticketnumber.c_str())){
                    ticket_max_number[index] = atoi(subticketnumber.c_str());
                }
                //cout<<concertname<<" "<<ticketname<<" "<<subticketnumber<<endl;
            }
        }
        cout<<filename<<"load finish"<<endl;
        file.close();
    }
	if(flag){
		cout<<"some tickets have been resold"<<endl;
	}
	else{
		cout<<"p2 ticket not resold \n";
		cout<<"let check whether every kind of ticket is from 0 to N in order\n";
        
        map<string,map<string,map<int,int>>>::iterator it;
        map<string,map<int,int>>:: iterator it2;
        map<int,int>:: iterator it3;
        int walker = 0;
        for( it = tester.begin() ; it != tester.end() ; it++)
        {
            cout<<"for concert name = "<<it->first<<" : "<<endl;
            for(it2 = it->second.begin() ; it2 != it->second.end() ; it2++)
            {
                string index = it->first+"_"+it2->first;
                cout<<"\t ticket : "<<it2->first<<" max number = "<<ticket_max_number[index]<<endl;
                walker = 0;
                for(it3 = it2->second.begin() ; it3 != it2->second.end() ; it3++)
                {
                    if(it3->first != walker){
                        cout<<"concert name = "<<it->first<<" , ticket name = "<<it2->first<<" , number "<<walker<<" is missing"<<endl;
                        break;
                    }
                    else{
                        walker++;
                    }
                }
            }
   
        }
	}
	return 0;
}
