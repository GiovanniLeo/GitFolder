Dati p processi di cui il master è processo 0 e i restanti p-1 sono slave, che comunicano in ring ovvero lo slave i
riceve un messaggio e lo invia allo slave i+1, lo slave p-1 invia al master.
Inizio: Il master invia un intero con valore zero allo slave
Slave:Incrementa l'intero ricevuto(random) con un valore piccolo e lo reinvia al master
Master: riceve lo slave e controlla se ha raggiunto la soglia prefissata se è cosi termina altrimenti lo rinvia 
allo slave