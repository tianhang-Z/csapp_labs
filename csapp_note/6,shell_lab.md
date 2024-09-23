### 实验要求

完善tsh.c  完成一个tiny shell，其具有以下功能：识别是内置命令还是可执行文件，实现ctrl-c和ctrl-z，以‘&’结尾的命令后台运行，实现quit、jobs、bg <job>、 fg <job>、kill <job>这些内置命令，等。

具体而言 需要完善以下函数：

• eval: Main routine that parses and interprets the command line. [70 lines]

• builtin cmd: Recognizes and interprets the built-in commands: quit, fg, bg, and jobs. [25 lines]

• do bgfg: Implements the bg and fg built-in commands. [50 lines]

• waitfg: Waits for a foreground job to complete. [20 lines]

• sigchld handler: Catches SIGCHILD signals. 80 lines]

• sigint handler: Catches SIGINT (ctrl-c) signals. [15 lines]

• sigtstp handler: Catches SIGTSTP (ctrl-z) signals. [15 lines]



实验给了参考的shell：tshref和辅助测试文件sdriver.pl



### eval

* 这里用到两处竞争消除

一个是fork子程序前 屏蔽SIGCHLD；addjob后才允许处理SIGCHLG

一个是显式等待前台进程，在waitfg中使用sigsuspend函数

* 由于jobs是全局变量 读写时要阻塞所有信号

```c
void eval(char *cmdline)   //参考教材p525
{
    char *argv[MAXARGS];
    int bg;
    pid_t pid;

    bg=parseline(cmdline,argv);   //解析命令行 
    int job_state=bg?BG:FG;
    if(argv[0]==NULL)
        return ;  // 跳过空行命令
    
    sigset_t mask_all,prev,mask_one;
    sigfillset(&mask_all);
    sigemptyset(&mask_one);
    sigaddset(&mask_one,SIGCHLD);
    if(!builtin_cmd(argv)){   //非内置命令
        //fork前阻塞SIGCHLD,参考教材p543，消除竞争 先addjob才能deletejob
        sigprocmask(SIG_BLOCK,&mask_one,&prev);   
        pid=fork();
        if(pid==0){      //子进程
            sigprocmask(SIG_SETMASK,&prev,NULL);  //消除子进程的阻塞  
            setpgid(0,0);
            if(execve(argv[0],argv,environ)<0){   //execve会替换原来内存空间的上下文信息
                printf("%s:command not found.\n",argv[0]);
                exit(0);
            }
            exit(0);
        }
        //父进程
        sigprocmask(SIG_BLOCK,&mask_all,&prev);   //阻塞所有信号，保证全局变量jobs的修改是安全的
        addjob(jobs,pid,job_state,cmdline);
        sigprocmask(SIG_SETMASK,&mask_one,NULL);  //注意，这里要恢复成mask_one。必须先进入waitfg，之后才接收sigchld,													//不然可能会造成阻塞在waitfg
        if(!bg){   //前台job   参考教材p545 消除竞争  显式等待进程
            waitfg(pid);
        }
        else{    //后台job    
            printf("[%d] (%d) %s",pid2jid(pid),pid,cmdline);
        }    
        sigprocmask(SIG_SETMASK,&prev,NULL);    
    }
    return;
}

```



### builtin_cmd

```c
int builtin_cmd(char **argv) 
{
    if(!strcmp(argv[0],"quit"))
        exit(0);
    else if(!strcmp(argv[0],"jobs")){
        listjobs(jobs);
        return 1;
    }
    else if (!strcmp(argv[0], "bg") || !strcmp(argv[0], "fg"))
    {
    	do_bgfg(argv);
    	return 1;
    }
    // 对单独的&不处理
    else if (!strcmp(argv[0], "&"))
    {
    	return 1;
    }
    return 0;     /* not a builtin command */
}
```



### dobg_fg

使用sscanf格式化读取 对错误格式进行说明即可

利用kill发送信号 并修改jobs的state

```c
void do_bgfg(char **argv) 
{   
    //bg fg 没有参数
    if(argv[1]==NULL){
        printf("%s command requires PID or %%jobid argument\n",argv[0]);
        return ;
    } 

    struct job_t *job=NULL;
    int id;

    //含百分号的是jid
    if(sscanf(argv[1],"%%%d",&id)>0){
        job=getjobjid(jobs,id);
        if(job==NULL) {
            printf("(%d):No such process\n",id);
            return;
        }
    }
    //pid
    else if(sscanf(argv[1],"%d",&id)>0){
        job=getjobpid(jobs,id);
        if(job==NULL) {
            printf("(%d):No such process\n",id);
            return;
        }
    }
    //格式错误
    else{
        printf("%s:aryment must be a PID or %%jobid\n",argv[0]);
        return;
    };

    //bg stsp->cont
    if(!strcmp(argv[0],"bg")) {
        kill(-(job->pid),SIGCONT);
        job->state=BG;
        printf("[%d] (%d) %s",job->jid,job->pid,job->cmdline);
    }
    //fg
    else {
        kill(-(job->pid),SIGCONT);
        job->state=FG;
        waitfg(job->pid);
    };

    return;
}

```



### waitfg

sigsuspend函数 显式等待信号

```c
//参考教材p543 显式等待信号
//子进程是由处理函数waitpid回收的，此处利用fgpid检查前台进程是否结束，未结束则利用sigsuspend休眠shell
void waitfg(pid_t pid)  
{   
    sigset_t mask;
    sigemptyset(&mask);
    while(fgpid(jobs)!=0) {   //前台程序未结束 则挂起 直到收到sigchld；
        sigsuspend(&mask);
    }       
    return;
}
```

###  sigchld_handler

这里的waitpid需要设置option参数 不可以使用0  原因参考注释

```c
//子进程停止或终止会触发sigchld
void sigchld_handler(int sig)  //参考教材p543    该函数回收子进程 且删除job
{
    int olderrno=errno;
    sigset_t mask_all,prev;
    pid_t pid;
    int status;

    sigfillset(&mask_all);
    /*  这里pid=waitpid(-1,&status,0))不能使用默认的0参数，否则waitpid会出现难以排查的bug，可以测试看一下。
        原因是/bin/echo是前台进程，父进程会进入waitfg，并进入sigsuspend挂起，直到父程序捕获并从处理函数返回后，它才会继续。
        但是echo子进程终止后，父进程调用sigchld_handler,并进入这个while循环回收子进程，之后，在下次循环时，会在waitpid阻塞；
        我们不希望waitpid阻塞，而是希望其立即返回,之后waitfg才能正常返回，父进程才能继续。*/
    // no child process时 waitpid返回-1
    while((pid=waitpid(-1,&status,WNOHANG|WUNTRACED))>0){ 

        sigprocmask(SIG_BLOCK,&mask_all,&prev);       //全局变量读写保护
        //正常退出
        if(WIFEXITED(status)){ 
            // printf("pid :%d,exit :%s",pid,getjobpid(jobs,pid)->cmdline);
            // fflush(stdout);
            deletejob(jobs,pid);  
        }
        //ctrl-c 产生SIGINT , 处理函数利用kill向前台进程发送信号  信号导致前台进程终止 
        else if(WIFSIGNALED(status)){   
            printf("Job [%d] (%d) terminated by signal %d\n",pid2jid(pid),pid,WTERMSIG(status));
            deletejob(jobs,pid);
        }
        //ctrl-z 产生SIGTSTP ,其处理函数利用kill向前台进程发送信号 导致前台进程停止
        else if(WIFSTOPPED(status)){
            printf("Job [%d] (%d) stopped by signal %d\n",pid2jid(pid),pid,WSTOPSIG(status));
            struct job_t *job=getjobpid(jobs,pid);
            job->state=ST;
        }
        sigprocmask(SIG_SETMASK,&prev,NULL);
    }
    errno=olderrno;
    return;
}

```

### sigint_handler

```c
//由于子进程执行execve，其内存空间上下文被替换了，是全新的程序，因此和父进程就没关系了。
//新程序不会接受键盘的信号，并且其信号处理都是默认的；
//键盘的ctrl-c 和ctrl-z信号发送到shell主程序，主程序调用信号处理函数，利用kill向新程序发送信号。
void sigint_handler(int sig) 
{
    int olderrno=errno;
    pid_t fg_pid;
    fg_pid=fgpid(jobs);
    if(fg_pid!=0)
        kill(-fg_pid,sig);   //利用kill发送信号  这里要加负号 向子进程的进程组中的所有进程发送信号
    errno=olderrno;
    return;
}
```

### sigtstp_handler

```c
void sigtstp_handler(int sig) 
{
    int olderrno=errno;
    pid_t fg_pid;
    fg_pid=fgpid(jobs);
    if(fg_pid!=0)
        kill(-fg_pid,sig);   //利用kill发送信号
    errno=olderrno;
    return;
}
```



### 参考

[CSAPP | Lab7-Shell Lab 深入解析 - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/492645370)

[CSAPP实验之shell lab - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/89224358)