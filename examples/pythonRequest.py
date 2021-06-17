import requests
key = "ans"

i = 0
while i < 1000:
    #note that lifetime is only used when the lock is never released by the client, for whatever reason. So make sure you take a widerange of your maximum work time,
    #so its not actually just a spike in your workload and the lock releases while you are actually still doing work.
    r = requests.get("http://localhost:8000/getLock?lockname=hello&timeout=10&lifetime=60")
    if r.text != "false":
           key = r.text

    r = requests.get("http://localhost:8000/releaseLock/Hello/"+key)
    i = i + 1
