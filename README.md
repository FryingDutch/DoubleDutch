# DoubleDutch: a distributed advisory lock

**DoubleDutch** is a C++ program that coordinates distributed access to shared resources, such as databases or file systems. Clients make requests to DoubleDutch, asking for exclusive usage of a resource (or permission to perform a one-time task). Once the client is finished (or timed-out), the lock is released and ready to be acquired by another client. This project is inspired by MySQL's `GET_LOCK` [function](https://dev.mysql.com/doc/refman/5.7/en/locking-functions.html#function_get-lock). DoubleDutch is a http(s) server, based on [CrowCpp](https://github.com/CrowCpp/crow/tree/master). 


## Usage

Any client that can communicate over http(s), can use the server. When using a Python client, a request to acquire a lock named `<lockname>` may look like:
```python
# try to acquire the lock:
r = requests.get("https://<host>:<port>/getlock?auth=randomapikey&lockname=<lockname>&timeout=3&lifetime=20")

#OR
r = requests.get("https://<host>:<port>/getlock?auth=randomapikey&lockname=<lockname>")
#-the default lifetime is 30 seconds.
#-the default timeout is 0 seconds.

```
When the lock on `<lockname>` was acquired, the server will return the following JSON:
```json
{
    "servername":    "DoubleDutch/v0.1",
    "lockname" :     "<lockname>",
    "sessiontoken" : "<sessiontoken>",   
    "lockacquired" :  true 
}
```
If a lock could not be acquired, `"lockacquired"` and `"sessiontoken"` will be set to `false` and `""`, respectively. 

The client has to use the `<sessionToken>` (a random string of 32 chararcters) to release the lock.
When using a Python Client, a request to release the lock may look like this:
```python
r = requests.delete("https://<host>:<port>/releaselock?lockname=<lockname>&token="+token)
```
The result would be:
```json
{
    "servername"   : "DoubleDutch/v0.1",
    "lockname"     : "<lockname>",
    "lockreleased" : true
}
```

To query the status of the server (and all of the locks that are currently active), you can request the `status`-endpoint. This will return the following JSON:
```json
{
    "servername" : "DoubleDutch/v0.1",
    "status"     : "ok",
    "locks": [
        {
            "lockname" : "<lockname>",
            "sessiontoken": "<sessiontoken>",
            "remaining": 43.5
        }
    ]
}
```
  
## Installation and set-up
**Requirements:**  
DoubleDutch needs a _.crt_ file named "certificate.crt" and a _.key_ file named "privateKey.key", in order to run on https.  
You will have to place these files in the _SSL directory_.  
The program also uses authentication trough API Key verification. To set the API key, you edit the _config.txt_ file.  
  
DoubleDutch runs inside a Docker container. To build using the provided _Dockerfile_:
```bash
docker build . -t server
```
DoubleDutch needs at least the portnumber from the user.
To run and listen for connections on port 8000:
```bash
docker run -p 8000:8000 server 8000
```
## Customisation
**Besides the default settings, DoubleDutch offers optional customisation.**

- **Name:** Give the server your own name.  
 This is especially useful when you want to use multiple servers for different applications.  
 By default this is "DoubleDutch/v0.1".
```bash
n myServerName
```
- **Precision:** this effects the amount of time a thread sleeps(ms) in between cycles.  
 Specifically when a request is waiting for a lock to be freed and in the dedicated thread that checks the lifetimes of the locks.  
By default this is 333ms.
```bash
p 333
```
- **Threads:** The amount of threads the program will use.  
By default the program makes a hardware check to set the thread amount.
```bash
t 8
```
- **HTTPS:** If you want to disable HTTPS.  
 Only option here is 0 (which makes the program run on http).  
HTTPS is on by default.
```bash
h 0
```
- **.crt & .key:** naming the _.crt_ or _.key_ file.  
 If you want to use a different name for the _.crt_ or _.key_ file, you will have to give notice to DoubleDutch.  
 By default you'll have to use "certificate.crt" and "privateKey.key".
```bash
c newcertificate.crt
#OR
k differentKey.key
```
- **API-key:** If you dont want to use the config file.  
 If you rather not have your API-key in a file, you can also pass it as an argument:
```bash
a myRandomApiKeyString
```

for example:
```bash
docker run -p 8000:8000 server 8000 n myOwnserver h 0
#OR
docker run -p 8000:8000 server 8000 p 250 k differentKey.key
#OR
docker run -p 8000:8000 server 8000 t 12 a myRandomString c newcert.crt

#etc
```


## Cluster mode
Distributed locks are used for roughly [two reasons](https://martin.kleppmann.com/2016/02/08/how-to-do-distributed-locking.html):
- **Efficiency**: Taking a lock saves you from unnecessarily doing the same work twice (e.g. some expensive computation).
- **Correctness**: Taking a lock prevents concurrent processes from stepping on each others’ toes and messing up the state of your system.  

When you're using DoubleDutch for the latter reason, you cannot use DoubleDutch in cluster mode. When employing DoubleDutch for efficiency reasons, though, you can easily spin up multiple instances (on different servers). In that case, you have to ensure that the clients are aware of all the hostnames. When one server is down, clients can try to acquire a lock at the 'next' DoubleDutch instances. 

## Known issues and limitations
- No implementation for a correctness focused backup server yet
