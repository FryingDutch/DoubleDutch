# LockManager: a distributed advisory lock

**LockManager** is a C++ program that coordinates distributed access to shared resources, such as databases or file systems. Clients make requests to LockManager, asking for exclusive usage of a resource (or permission to perform a one-time task). Once the client is finished (or timed-out), the lock is released and ready to be acquired by another client. This project is inspired by MySQL's `GET_LOCK` [function](https://dev.mysql.com/doc/refman/5.7/en/locking-functions.html#function_get-lock). LockManager is a http server, based on [CrowCpp](https://crowcpp.org/). 


## Usage
Any client that can communicate over http(s), can use the server. When using a Python client, a request to access the `database` resource may look like:
```python
# try to acquire the lock on the database
response = requests.get('<hostname>:<port>/getLock/yourLockAsString')
while response == "false":
  response = requests.get('<hostname>:<port>/getLock/yourLockAsString')
  
# if the lock to the resource was granted to this client they will receive a random unique string of 32 chararcters that will be needed to release the lock later.
  
## Installation and set-up
LockManager runs inside a Docker container. To build using the provided _.Dockerfile_:
```bash
docker build -t LockManager .
```
To run and listen for connections on port 80:
```
docker run LockManager -p 80:80
```


## Cluster mode
Distributed locks are used for roughly [two reasons](https://martin.kleppmann.com/2016/02/08/how-to-do-distributed-locking.html):
- **Efficiency**: Taking a lock saves you from unnecessarily doing the same work twice (e.g. some expensive computation).
- **Correctness**: Taking a lock prevents concurrent processes from stepping on each others’ toes and messing up the state of your system.  

When you're using LockManager for the latter reason, you cannot use LockManager in cluster mode. When employing LockerManager for efficiency reasons, though, you can easily spin up multiple instances (on different servers). In that case, you have to ensure that the clients are aware of all the hostnames. When one server is down, clients can try to acquire a lock at the 'next' LockManager instances. 

## Known issues and limitations
- None :)
