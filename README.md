# DoubleDutch: a distributed advisory lock

**DoubleDutch** is a C++ program that coordinates distributed access to shared resources, such as databases or file systems. Clients make requests to DoubleDutch, asking for exclusive usage of a resource (or permission to perform a one-time task). Once the client is finished (or timed-out), the lock is released and ready to be acquired by another client. This project is inspired by MySQL's `GET_LOCK` [function](https://dev.mysql.com/doc/refman/5.7/en/locking-functions.html#function_get-lock). DoubleDutch is a https server, based on [CrowCpp](https://github.com/CrowCpp/crow/tree/master). 


## Usage
Any client that can communicate over http(s), can use the server. When using a Python client, a request to access the `database` resource may look like:
```python
# try to acquire the lock on the database:
r = requests.get("https://<host>:<port>/getLock?auth=randomapikey&lockname=lock_as_string&timeout=3&lifetime=20")

#OR
r = requests.get("https://<host>:<port>/getLock?auth=randomapikey&lockname=lock_as_string")
#-the default lifetime is 30 seconds.
#-the default timeout is 0 seconds.

```
 If the lock to the resource was granted, the client will receive a random string of 32 chararcters that will be needed to release the lock later.
 When using a Python Client, a request to release the lock may look like this:
```
r = requests.get("https://<host>:<port>/releaseLock?lockname=lock_as_string&key="+key)
```
  
## Installation and set-up
DoubleDutch needs a .crt file named "certificate.crt" and a .key file named "privateKey.key", in order to run on https.
You will have to place these files in the SSL directory.

The program also uses authentication trough API Key verification. To set the API key, you just edit the config.txt file.

DoubleDutch runs inside a Docker container. To build using the provided _.Dockerfile_:
```bash
docker build . -t server
```
DoubleDutch needs at least the portnumber from the user.
To run and listen for connections on port 8000:
```
docker run -p 8000:8000 server 8000
```
**Besides the default settings, DoubleDutch offers optional customisation.**

- **Precision:** this effects the amount of time a thread sleeps(ms) in between cycles. Specifically where a request is waiting for a lock to be possibly freed. And also in a  the dedicated thread that checks the lifetimes of the current locks. By default this is 333ms.

```
p 333
```

- **Threads:** The amount of threads the program will use. By default this is 8

```
t 8
```

- **HTTPS:** If the user wants to disable HTTPS. Only option here is 0 (which makes the program run on http). HTTPS is on by default.

```
h 0
```

for example:
```
docker run -p 8000:8000 server 8000 t 12 p 100 h 0
#OR
docker run -p 8000:8000 server 8000 t 4
#OR
docker run -p 8000:8000 server 8000 p 250 h 0
#OR
docker run -p 8000:8000 server 8000 h 0 t 11
#OR
docker run -p 8000:8000 server 8000 h 0

#etc
```


## Cluster mode
Distributed locks are used for roughly [two reasons](https://martin.kleppmann.com/2016/02/08/how-to-do-distributed-locking.html):
- **Efficiency**: Taking a lock saves you from unnecessarily doing the same work twice (e.g. some expensive computation).
- **Correctness**: Taking a lock prevents concurrent processes from stepping on each others’ toes and messing up the state of your system.  

When you're using DoubleDutch for the latter reason, you cannot use DoubleDutch in cluster mode. When employing DoubleDutch for efficiency reasons, though, you can easily spin up multiple instances (on different servers). In that case, you have to ensure that the clients are aware of all the hostnames. When one server is down, clients can try to acquire a lock at the 'next' DoubleDutch instances. 

## Known issues and limitations
- No way to graciously exit the server. Terminal has to be closed.
