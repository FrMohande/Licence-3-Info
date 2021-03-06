<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
  <meta content="text/html;charset=ISO-8859-1" http-equiv="Content-Type">
  <title>CreerArticle</title>
  <meta content="Cedric dumoulin" name="author">
</head>
<body>
<h1>CreerArticle</h1>
<br>
<form action="ConfirmationCreation.html">
  <table style="text-align: left; width: 100%;" border="1"
 cellpadding="2" cellspacing="2">
    <tbody>
      <tr>
        <td style="vertical-align: top;">Name<br>
        </td>
        <td style="vertical-align: top;"><input name="annee"><br>
        </td>
      </tr>
      <tr>
        <td style="vertical-align: top;">Type<br>
        </td>
        <td style="vertical-align: top;">
        <select>
        <option value="dvd">DVD</option>
        <option value="bluray">Blu Ray</option>
        <option value="cd">CD</option>
        </select>
        </td>
      </tr>
      <tr>
        <td style="vertical-align: top;">Titre<br>
        </td>
        <td style="vertical-align: top;">
        <select>
        <option value="Harry Potter">Harry Potter &agrave; l'Ecole des
Sorciers</option>
        <option value="bond1">James Bond Au service Secret de Sa Majeste</option>
        <option value="Paycheck">Paycheck</option>
        <option value="sw1">Stars Wars 1</option>
&nbsp;&nbsp;
        </select>
        <a href="creerTitre.html">CreerUnNouveauTitre</a><br>
        </td>
      </tr>
      <tr>
        <td style="vertical-align: top;">Prix<br>
        </td>
        <td style="vertical-align: top;"><input name="annee"></td>
      </tr>
    </tbody>
  </table>
  <br>
  <input value="creer" type="submit"> <input value="annuler"
 type="submit"><br>
</form>
<br>
<br>
</body>
</html>
                                                                                                                                                                                                                                                                                   token[j - 1][k - 1][strlen(token[j - 1][k - 1]) - 1] = '\0';
        return 1;
    }

    return 0;                   /* foreground job */
}

/*
 * builtin_cmd - If the user has typed a built-in command then execute
 *    it immediately.
 */
int builtin_cmd(char **argv) {
    char *cmd = argv[0];

    if (!strcmp(cmd, "help")) {
        do_help();
        return 1;
    }

    if (!strcmp(cmd, "stop")) {
        do_stop(argv);
        return 1;
    }

    if (!strcmp(cmd, "exit")) {
        do_exit();
        return 1;
    }

    if (!strcmp(cmd, "jobs")) {
        do_jobs();
        return 1;
    }

    if (!strcmp(cmd, "bg")) {
        do_bg(argv);
        return 1;
    }

    if (!strcmp(cmd, "fg")) {
        do_fg(argv);
        return 1;
    }

    if (!strcmp(cmd, "kill")) {
        do_kill(argv);
        return 1;
    }


    return 0;                   /* not a builtin command */
}

void eval(char *cmdline) {
    char *token[MAXCMDS][MAXARGS];      /* token[i] is a command typed in the command line */
    int nbcmd;                          /* number of commands typed in the command line */
    int bg;                             /* should the job run in bg or fg? */
    pid_t pid;                          /* process id */
    sigset_t mask;                      /* signal mask */
    char **argv;

    /* Parse command line */
    bg = parseline(cmdline, token, &nbcmd);

    if (nbcmd > 1)              /* a command pipeline has been typed in */
        do_pipe(token, nbcmd, bg);
    else {                      /* no pipeline, only one command */
        argv = token[0];
        if (!builtin_cmd(argv)) {
            /*
             * This is a little tricky. Block SIGCHLD, SIGINT, and SIGTSTP
             * signals until we can add the job to the job list. This
             * eliminates some nasty races between adding a job to the job
             * list and the arrival of SIGCHLD, SIGINT, and SIGTSTP signals.
             */

            if (sigemptyset(&mask) < 0)
                unix_error("sigemptyset error");
            if (sigaddset(&mask, SIGCHLD))
                unix_error("sigaddset error");
            if (sigaddset(&mask, SIGINT))
                unix_error("sigaddset error");
            if (sigaddset(&mask, SIGTSTP))
                unix_error("sigaddset error");
            if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0)
                unix_error("sigprocmask error");

            /* Create a child process */
            if ((pid = fork()) < 0)
                unix_error("fork error");

            /*
             * Child  process
             */
            if (pid == 0) {
                /* Child unblocks signals */
                sigprocmask(SIG_UNBLOCK, &mask, NULL);

                /* Each new job must get a new process group ID
                   so that the kernel doesn't send ctrl-c and ctrl-z
                   signals to all of the shell's jobs */
                if (setpgid(0, 0) < 0)
                    unix_error("setpgid error");

                /* Now load and run the program in the new job */
                if (execvp(argv[0], argv) < 0) {
                    printf("%s: Command not found\n", argv[0]);
                    exit(EXIT_FAILURE);
                }
            }

            /*
             * Parent process
             */
            /* Parent adds the job, and then unblocks signals so that
               the signals handlers can run again */
            jobs_addjob(pid, (bg == 1 ? BG : FG), cmdline);
            sigprocmask(SIG_UNBLOCK, &mask, NULL);

            if (!bg)
                waitfg(pid);
            else
                printf("[%d] (%d) %s\n", jobs_pid2jid(pid), (int) pid, cmdline);
        }
    }
    return;
}

/*
 * main - The shell's main routine
 */
int main(int argc, char **argv) {
    int c;
    char cmdline[MAXLINE];

    verbose = 0;

    /* Redirect stderr to stdout (so that driver will get all output
     * on the pipe connected to stdout) */
    dup2(1, 2);

    /* Parse the command line */
    while ((c = getopt(argc, argv, "hv")) != -1) {
        switch (c) {
        case 'h':              /* print help message */
            usage();
            break;
        case 'v':              /* emit additional diagnostic info */
            verbose = 1;
            break;
        default:
            usage();
        }
    }

    /* Install the signal handlers */

    /* These are the ones you will need to implement */
    sigaction_wrapper(SIGINT, sigint_handler);  /* ctrl-c */
    sigaction_wrapper(SIGTSTP, sigtstp_handler);        /* ctrl-z */
    sigaction_wrapper(SIGCHLD, sigchld_handler);        /* Terminated or stopped child */

    /* Initialize the job list */
    jobs_initjobs();

    printf("\nType help to find out all the available commands in mshell\n\n");

    /* Execute the shell's read/eval loop */
    while (1) {
        /* Read command line */
        printf("%s", prompt);

        if ((fgets(cmdline, MAXLINE, stdin) == NULL) && ferror(stdin)) {
            fprintf(stdout, "fgets error\n");
            exit(EXIT_FAILURE);
        }
        if (feof(stdin)) {      /* End of file (ctrl-d) */
            fflush(stdout);
            exit(EXIT_SUCCESS);
        }

        /* Evaluate the command line */
        if (cmdline[0] != '\n') {   /* evaluate the command line only when not empty */
            cmdline[strlen(cmdline) - 1] = '\0';       /* remove the '\n' at the end */
            eval(cmdline);
            fflush(stdout);
            fflush(stdout);
        }
    }

    exit(EXIT_SUCCESS);         /* control never reaches here */
}
