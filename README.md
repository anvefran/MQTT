# MQTT
En esta solución, el Publisher ingresa la información de conexión, es decir, el nombre del host.
Esta será enviada al bróker para sesgar el envío dependiendo de lo elegido por el suscriptor. El
proceso Publisher necesita ejecutar un comando para obtener el valor de la métrica requerida por
lo que crea un proceso hijo que mediante execvp, ejecuta el comando y redirige la salida al stdin
del proceso padre.
## Diagrama de flujo completo 
![project diagram](/resources/flujoSOproject.png)

## Ejemplos de salida con diferentes entradas
### Host1 o Host1/#
![host1](/resources/host1.png)
### Host1/CPU o Host1/CPU/#
![host1](/resources/host1-cpu.png)
### Host1/CPU/idle o Host1/+/idle
![host1](/resources/host1-idle.png)
### Host1/RAM/free
![host1](/resources/host1-ram-free.png)