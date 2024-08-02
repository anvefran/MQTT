# Real-Time Monitoring Solution for GNU/Linux Systems Metrics

This project provides a real-time monitoring solution for metrics from various GNU/Linux systems, utilizing a lightweight MQTT protocol. The system is composed of a broker, publisher, and subscriber, and incorporates context switching through forks and concurrency using threads.

## Components

1. **Publisher**  
   The Publisher is responsible for entering connection information, such as the host name. This information is sent to the broker to direct the data according to the subscriber's selection. The Publisher process executes a command to obtain the required metric value by creating a child process using `execvp`, which runs the command and redirects the output to the parent process's stdin.

2. **Broker**  
   The broker handles the distribution of messages between the Publisher and Subscriber, ensuring that data is routed correctly based on the subscriber's preferences.

3. **Subscriber**  
   The Subscriber receives data from the broker and processes it according to the metrics it is interested in.

## Key Features

- **Real-Time Monitoring:** Efficiently monitors and reports metrics in real time from multiple GNU/Linux systems.
- **Lightweight MQTT Protocol:** Uses MQTT for efficient communication with minimal overhead.
- **Concurrency and Context Switching:** Implements context switching through forks and manages concurrency using threads to handle multiple processes effectively.

## Complete Flowchart
![project diagram](/resources/flujoSOproject.png)

## Output Examples with Different Inputs
### Host1 o Host1/#
![host1](/resources/host1.png)
### Host1/CPU o Host1/CPU/#
![host1](/resources/host1-cpu.png)
### Host1/CPU/idle o Host1/+/idle
![host1](/resources/host1-idle.png)
### Host1/RAM/free
![host1](/resources/host1-ram-free.png)
