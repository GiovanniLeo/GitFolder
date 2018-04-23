2 Processori: master e slave
Inizio: Il master invia un intero con valore zero allo slave
Slave:Incrementa l'intero ricevuto(random) con un valore piccolo e lo reinvia al master
Master: riceve lo slave e controlla se ha raggiunto la soglia prefissata se è cosi termina altrimenti lo rinvia 
allo slave