//wait_proc[第幾行要接上]=pid
//pid_pipe[pid]=(vector)p[2]
//做到 "!"
#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <string>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <algorithm>
#include <unordered_map>
#define MAX_WORDS_IN_LINE 500000
#define MAX_PIPE 10000
using namespace std;
int set_env(string name ,string value)
{
    int p=setenv((char*)name.data(),(char*)value.data(),1);
    if(p<0) cout<<"set error\n";
    return 0;
}

void init()
{
    setenv("PATH","/bin:./bin",1);
}
void print_env(string name)
{
    char*p=getenv((char*)name.data());
    if(p==NULL) return ;
    cout<<p<<endl;
}

int main()
{
    stringstream ss;
    vector<string>cmd;
    string temp;
    vector<int>bar_pos;
    unordered_map<int,int>wait_num;
    int p_num_idx=0;
    int cur=0;
    init();
    int p[MAX_PIPE][2]={0};
    int p_num[MAX_PIPE][2]={0};
    while(1){
        vector<int>wait_proc;
        cur++;
        string c_in="";
        string temp="";
        int now_bar=0;
        int last_bar=0;

        bar_pos.clear();
        bar_pos.push_back(-1);
        int pipe_create=0;
        int std_in=dup(STDIN_FILENO);
        int std_out=dup(STDOUT_FILENO);
        ss.clear();
        cmd.clear();
        cout<<"% ";
        if(!std::getline(cin,c_in,'\n')) break;
        ss.str(c_in);
        for(int i=0;ss>>temp;i++){
            cmd.push_back(temp);
            if(temp=="|"){
                bar_pos.push_back(i);
            }
        }
        if(cmd[0]=="setenv"){
            set_env(cmd[1],cmd[2]);
            continue;
        }
        if(cmd[0]=="printenv"){
            print_env(cmd[1]);
            continue;
        }
        if(cmd[0]=="exit") break;
        int need_catch=0;
        for(auto &i:wait_num){
            int p=i.first,col=i.second;
            if(col==cur){
                wait_proc.push_back(i.first);
                need_catch=1;
            }
        }
        for(int i=0;i<cmd.size();i++){
            if(cmd[i]=="|"||cmd[i]==">"||cmd[i][0]=='|'){
                now_bar=std::find(bar_pos.begin(),bar_pos.end(),i)-bar_pos.begin();
                last_bar=now_bar-1;
            }
            
            if(cmd[i]=="|"){
                pipe_create++;
                int now_pipe=pipe_create-1;
                int last_pipe=pipe_create-2;
                if(pipe(p[now_pipe])<0) std::cout<<"create pipe error\n";
                if(fork()==0){
                    if(need_catch){
                        for(int i=0;i<wait_proc.size();i++){
                            dup2(p_num[wait_proc[i]][0],STDIN_FILENO);
                            close(p_num[wait_proc[i]][0]);
                            close(p_num[wait_proc[i]][1]);
                        }
                        need_catch=0;
                    }
                    if(last_pipe>=0){
                        dup2(p[last_pipe][0],STDIN_FILENO);
                        close(p[last_pipe][0]);
                        close(p[last_pipe][1]);
                    }
                    close(p[now_pipe][0]);
                    dup2(p[now_pipe][1],STDOUT_FILENO);
                    close(p[now_pipe][1]);
                    char*argv[MAX_WORDS_IN_LINE]={0};
                    for(int j=0;j<i-bar_pos.at(last_bar)-1;j++){
                        argv[j]=(char*)cmd.at(bar_pos.at(last_bar)+j+1).data();
                        //fprintf(stderr,"argv %s\n",argv[j]);
                    }
                    argv[i-bar_pos.at(last_bar)-1]=NULL;
                    
                    if(execvp(argv[0],argv)<0){
                        close(1);
                        fprintf(stderr,"Unknown command: [%s]\n",argv[0]);
                    }
                    exit(0);
                }else{
                    if(last_pipe>=0){
                        close(p[last_pipe][0]);
                        close(p[last_pipe][1]);
                    }
                    if(!wait_proc.empty()){
                        for(int i=0;i<wait_proc.size();i++){
                            close(p_num[wait_proc[i]][0]);
                            close(p_num[wait_proc[i]][1]);
                        }
                    }
                }
            }else if(cmd[i]==">"){
                //std::cout<<cmd.at(bar_pos.at(last_bar)+1).data()<<endl;
                int last_pipe=pipe_create-1;
                int status;
                const char*filename=cmd.at(i+1).data();
                int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);

                // 檢查檔案是否成功開啟
                if (fd == -1) {
                    std::cerr << "Error opening file tmp.txt" << std::endl;
                    return 1;
                }
                int pid=fork();
                if(pid==0){
                    if(need_catch){
                        for(int i=0;i<wait_proc.size();i++){
                            dup2(p_num[wait_proc[i]][0],STDIN_FILENO);
                            close(p_num[wait_proc[i]][0]);
                            close(p_num[wait_proc[i]][1]);
                        }
                        need_catch=0;
                    }
                    if(last_pipe>=0){
                        dup2(p[last_pipe][0],STDIN_FILENO);
                        close(p[last_pipe][0]);
                        close(p[last_pipe][1]);
                    }   
                    dup2(fd,STDOUT_FILENO);
                    close(fd);
                    //int arr_size=i-bar_pos.at(last_bar);

                    char*argv[MAX_WORDS_IN_LINE]={0};
                    
                    for(int j=0;j<i-bar_pos.at(last_bar)-1;j++){
                        argv[j]=(char*)cmd.at(bar_pos.at(last_bar)+j+1).data();
                        //fprintf(stderr,"argv %s\n",argv[j]);
                    }
                    argv[i-bar_pos.at(last_bar)-1]=NULL;
                    
                    execvp(argv[0],argv);
                }else{
                    if(last_pipe>=0){
                        close(p[last_pipe][0]);
                        close(p[last_pipe][1]);
                    }
                    if(!wait_proc.empty()){
                        for(int i=0;i<wait_proc.size();i++){
                            close(p_num[wait_proc[i]][0]);
                            close(p_num[wait_proc[i]][1]);
                        }
                    }
                    int wtr=waitpid(pid,&status,WUNTRACED | WCONTINUED);
                    if (wtr == -1) {
                        perror("waitpid");
                        exit(EXIT_FAILURE);
                    }
                }
                
                i++;
                
            }else if(cmd[i][0]=='|'||cmd[i][0]=='!'){
                int num=cmd[i][1]-'0';
                int wait_now=-1;
                for(auto i:wait_num){
                    if(cur+num==i.second){
                        wait_now=i.first;
                    }
                }
                if(wait_now==-1){
                    p_num_idx++;
                    wait_now=p_num_idx-1;
                    wait_num[wait_now]=cur+num;
                    if(pipe(p_num[wait_now])<0) std::cout<<"create pipe error\n";
                }
                
                int last_pipe=pipe_create-1;
                
                int status;
                int pid=fork();
                if(pid==0){
                    if(need_catch){
                        for(int i=0;i<wait_proc.size();i++){
                            dup2(p_num[wait_proc[i]][0],STDIN_FILENO);
                            close(p_num[wait_proc[i]][0]);
                            close(p_num[wait_proc[i]][1]);
                        }
                        need_catch=0;
                    }
                    if(last_pipe>=0){
                        dup2(p[last_pipe][0],STDIN_FILENO);
                        close(p[last_pipe][0]);
                        close(p[last_pipe][1]);
                    }
                    if(cmd[i][0]=='!'){
                        dup2(p_num[wait_now][1],STDERR_FILENO);
                    }
                    dup2(p_num[wait_now][1],STDOUT_FILENO);
                    close(p_num[wait_now][0]);
                    close(p_num[wait_now][1]);

                    //close(p[now_pipe][0]);
                    //dup2(p[now_pipe][1],STDOUT_FILENO);
                    //close(p[now_pipe][1]);
                    char*argv[MAX_WORDS_IN_LINE]={0};
                    for(int j=0;j<i-bar_pos.at(last_bar)-1;j++){
                        argv[j]=(char*)cmd.at(bar_pos.at(last_bar)+j+1).data();
                        //fprintf(stderr,"argv %s\n",argv[j]);
                    }
                    argv[i-bar_pos.at(last_bar)-1]=NULL;

                    if(execvp(argv[0],argv)<0){
                        close(1);
                        fprintf(stderr,"Unknown command: [%s]\n",argv[0]);
                    }
                    exit(0);
                }else{
                    if(last_pipe>=0){
                        close(p[last_pipe][0]);
                        close(p[last_pipe][1]);
                    }
                    if(!wait_proc.empty()){
                        for(int i=0;i<wait_proc.size();i++){
                            close(p_num[wait_proc[i]][0]);
                            close(p_num[wait_proc[i]][1]);
                        }
                    }
                    int wtr=waitpid(pid,&status,WUNTRACED | WCONTINUED);
                    if (wtr == -1) {
                        perror("waitpid");
                        exit(EXIT_FAILURE);
                    }
                }

            }
            else if(i==cmd.size()-1){
                pipe_create++;
                int now_pipe=pipe_create-1;
                int last_pipe=pipe_create-2;
                //last_bar=bar_pos.size()-1;
                //fprintf(stderr,"bar_pos size-1 %d \n",last_bar);
                //if(last_pipe<0) last_pipe=0;
                if(pipe(p[now_pipe])<0) std::cout<<"create pipe error\n";
                int status;
                int pid=fork();
                if(pid==0){
                    if(last_pipe>=0){
                        dup2(p[last_pipe][0],STDIN_FILENO);
                        close(p[last_pipe][0]);
                        close(p[last_pipe][1]);
                    }
                    if(need_catch){
                        if(need_catch){
                            for(int i=0;i<wait_proc.size();i++){
                                dup2(p_num[wait_proc[i]][0],STDIN_FILENO);
                                close(p_num[wait_proc[i]][0]);
                                close(p_num[wait_proc[i]][1]);
                            }
                            need_catch=0;
                        }
                    }
                    //std::cout<<"last pipe "<<last_pipe<<"now_pipe "<<now_pipe<<std::endl;
                    close(p[now_pipe][0]);
                    close(p[now_pipe][1]);
                    char*argv[MAX_WORDS_IN_LINE]={0};
                    for(int j=0;j<i-bar_pos.at(now_bar);j++){
                        argv[j]=(char*)cmd.at(bar_pos.at(now_bar)+j+1).data();
                        //fprintf(stderr,"argv last %s %d\n",argv[j],bar_pos.at(last_bar));
                    }
                    argv[i-bar_pos.at(now_bar)]=NULL;
                    if(execvp(argv[0],argv)<0){
                        fprintf(stderr,"Unknown command: [%s]\n",argv[0]);
                    }
                    exit(0);
                }else{
                    close(p[now_pipe][0]);
                    close(p[now_pipe][1]);
                    if(last_pipe>=0){
                        close(p[last_pipe][0]);
                        close(p[last_pipe][1]);
                    }
                    if(!wait_proc.empty()){
                        for(int i=0;i<wait_proc.size();i++){
                            close(p_num[wait_proc[i]][0]);
                            close(p_num[wait_proc[i]][1]);
                        }
                    }
                    int wtr=waitpid(pid,&status,WUNTRACED | WCONTINUED);
                    if (wtr == -1) {
                        perror("waitpid");
                        exit(EXIT_FAILURE);
                    }
                    
                    //std::cout<<"parent status %d"<<status<<endl;
                }   
            }
        }
    }
    return 0;
    
}