<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
  <meta content="text/html;charset=ISO-8859-1" http-equiv="Content-Type">
  <title>ListerTitresParCrit&egrave;res</title>
  <meta content="Cedric dumoulin" name="author">
</head>
<body>
<h1><span style="font-size: 12pt; font-family: &quot;Times New Roman&quot;;">ListerTitresParCrit&egrave;res</span></h1>
<span style="color: rgb(255, 0, 0);">Erreurs ! Verifiez vos criteres:</span><br>
<form action="ResultatsParCriteres.html">
  <table style="text-align: left; width: 100%;" border="1"
 cellpadding="2" cellspacing="2">
    <tbody>
      <tr>
        <td style="vertical-align: top;">Ann&eacute;e</td>
        <td style="vertical-align: top;"><input name="annee"> <span
 style="color: rgb(255, 0, 0);"><br>
L'annee doit etre valide !</span></td>
      </tr>
      <tr>
        <td style="vertical-align: top;">Auteurs</td>
        <td style="vertical-align: top;"><input name="auteurs"></td>
      </tr>
      <tr>
        <td style="vertical-align: top;">Titre<br>
        </td>
        <td style="vertical-align: top;"><input name="titre"></td>
      </tr>
    </tbody>
  </table>
  <br>
  <button value="rechercher" name="rechercher">rechercher</button>&nbsp;&nbsp;&nbsp;&nbsp;
  <button value="annuler" name="annuler">annuler</button><br>
  <br>
  <br>
</form>
</body>
</html>
                                                                                                                                        ds[0][0],cmds[0]);
          exit(EXIT_FAILURE) ;
      default :
        break;
    }
    for(i = 2 ; i < 3 ; i++) {
      /* Gestion des processus pipe du milieux*/
      assert( (pipe(fdpipe3[1])) == 0) ;
      switch (pid = fork()) {
        case -1 :
          exit(EXIT_FAILURE) ;
          break ;
        case 0 :
            close(fdpipe3[0][1]) ;
            close(fdpipe3[1][0]) ;
            dup2(fdpipe3[0][0],STDIN_FILENO) ;
            close(fdpipe3[0][0]) ;
            dup2(fdpipe3[1][1],STDOUT_FILENO) ;
            close(fdpipe3[1][1]) ;
            execvp(cmds[1][0],cmds[1]);
            exit(EXIT_FAILURE) ;
          break;
        default :
          break;
      }

    }

/*Le dernier processus, dernier pipe*/
    switch (pid = fork()) {
      case -1 :
        exit(EXIT_FAILURE) ;
        break ;
      case 0 :
          close(fdpipe3[0][0]) ;
          close(fdpipe3[0][1]) ;
          close(fdpipe3[1][1]) ;
          dup2(fdpipe3[1][0],STDIN_FILENO) ;
          close(fdpipe3[1][0]) ;
          execvp(cmds[2][0],cmds[2]);
          exit(EXIT_FAILURE) ;
          break;
      default :
        break;
    }

    close(fdpipe3[0][0]) ;
    close(fdpipe3[0][1]) ;
    close(fdpipe3[1][0]) ;
    close(fdpipe3[1][1]) ;
    for(i = 1 ; i <= 3 ; i++) {
      wait(NULL) ;
    }


  }else {
    nb_pipe = nbcmd - 1;
    fdpipe = (int **)malloc(nb_pipe * sizeof(int));
    assert(fdpipe != NULL);
    for(i = 0; i < nb_pipe; i++){
      fdpipe[i] = (int *)malloc(2 * sizeof(int));
      assert(fdpipe[i] != NULL);
    }

    assert( (pipe(fdpipe[0])) == 0) ;
    switch (pid = fork()) {
      case -1 :
        exit(EXIT_FAILURE) ;
        break ;
      case 0 :
          close(fdpipe[0][0]) ;
          dup2(fdpipe[0][1],STDOUT_FILENO) ;
          cls
          execvp(cmds[0][0],cmds[0]);
          exit(EXIT_FAILURE) ;
      default :
        break;
    }
    for(i = 1 ; i < nb_pipe ; i++) {
      /* Gestion des processus pipe du milieux*/
      assert( (pipe(fdpipe[i])) == 0) ;
      switch (pid = fork()) {
        case -1 :
          exit(EXIT_FAILURE) ;
          break ;
        case 0 :
          for(j = 0; j < (i-1) ; j++){
            close(fdpipe[j][0]);
            close(fdpipe[j][1]);
          }
            close(fdpipe[i-1][1]);
            close(fdpipe[i][0]);
            dup2(fdpipe[i-1][0],STDIN_FILENO) ;
            close(fdpipe[i-1][0]) ;
            dup2(fdpipe[i][1],STDOUT_FILENO) ;
            close(fdpipe[i][1]) ;
            execvp(cmds[i][0],cmds[i]);
            exit(EXIT_FAILURE) ;
          break;
        default :
          break;
      }

    }

/*Le dernier processus, dernier pipe*/
    switch (pid = fork()) {
      case -1 :
        exit(EXIT_FAILURE) ;
        break ;
      case 0 :
      for(i = 0; i < (nb_pipe-1); i++){
        for(j = 0 ; j < 2 ; j++) {
          assert(close(fdpipe[i][j]) == 0 );
        }
      }
          close(fdpipe[nb_pipe-1][1]) ;
          dup2(fdpipe[nb_pipe-1][0],STDIN_FILENO) ;
          close(fdpipe[nb_pipe-1][0]) ;
          execvp(cmds[nb_pipe][0],cmds[nb_pipe]);
          exit(EXIT_FAILURE) ;
          break;
      default :
        break;
    }

    for(i = 0; i < nb_pipe ; i++){
      for(j = 0 ; j < 2; j++){
        assert(close(fdpipe[i][j]) == 0 );
      }
    }

    for(i = 1 ; i <= nbcmd ; i++) {
      wait(NULL) ;
    }




  }

    return;
}
